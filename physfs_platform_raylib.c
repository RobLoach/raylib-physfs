/*
 * Raylib platform implementation for PhysFS.
 *
 * Implements the PhysFS platform abstraction layer using raylib's file I/O
 * API. This allows PhysFS to operate on any platform raylib supports without
 * requiring any platform-specific system calls.
 *
 * File handles are fully buffered in memory (using raylib's LoadFileData /
 * SaveFileData) so that seek and tell are supported despite raylib having only
 * load-all-at-once file I/O. Mutex operations are no-ops; this platform is
 * single-threaded by design, matching raylib's own threading model.
 *
 * Usage — define PHYSFS_PLATFORM_RAYLIB before including raylib-physfs.h:
 *
 *   #define PHYSFS_PLATFORM_RAYLIB
 *   #define RAYLIB_PHYSFS_IMPLEMENTATION
 *   #include "raylib-physfs.h"
 */

#include <string.h>  /* memcpy, memset */
#include <stdio.h>   /* remove() */

/* -------------------------------------------------------------------------
 * Threading — single-threaded no-ops
 * ---------------------------------------------------------------------- */

void *__PHYSFS_platformCreateMutex(void)  { return (void *)0x1; }
void  __PHYSFS_platformDestroyMutex(void *mutex) { (void)mutex; }
int   __PHYSFS_platformGrabMutex(void *mutex)    { (void)mutex; return 1; }
void  __PHYSFS_platformReleaseMutex(void *mutex) { (void)mutex; }
void *__PHYSFS_platformGetThreadID(void)  { return (void *)0x1; }

/* -------------------------------------------------------------------------
 * Lifecycle
 * ---------------------------------------------------------------------- */

int __PHYSFS_platformInit(const char *argv0)
{
    (void)argv0;
    return 1;
}

void __PHYSFS_platformDeinit(void) { /* no-op */ }

/* -------------------------------------------------------------------------
 * CD-ROM detection — not supported
 * ---------------------------------------------------------------------- */

void __PHYSFS_platformDetectAvailableCDs(PHYSFS_StringCallback cb, void *data)
{
    (void)cb;
    (void)data;
}

/* -------------------------------------------------------------------------
 * Path resolution
 * ---------------------------------------------------------------------- */

/* Returns an allocator-owned copy of a path, ensuring it ends with '/'. */
static char *platformDupWithSep(const char *path)
{
    size_t len = strlen(path);
    int needSep = (len == 0 || path[len - 1] != '/');
    char *result = (char *)allocator.Malloc(len + needSep + 1);
    if (result == NULL) {
        PHYSFS_setErrorCode(PHYSFS_ERR_OUT_OF_MEMORY);
        return NULL;
    }
    memcpy(result, path, len);
    if (needSep) result[len++] = '/';
    result[len] = '\0';
    return result;
}

char *__PHYSFS_platformCalcBaseDir(const char *argv0)
{
    (void)argv0;
    return platformDupWithSep(GetApplicationDirectory());
}

char *__PHYSFS_platformCalcUserDir(void)
{
    return platformDupWithSep(GetApplicationDirectory());
}

char *__PHYSFS_platformCalcPrefDir(const char *org, const char *app)
{
    const char *base = GetApplicationDirectory();
    size_t baselen = strlen(base);
    size_t orglen  = strlen(org);
    size_t applen  = strlen(app);
    /* base [/] org / app / \0 */
    size_t total = baselen + 1 + orglen + 1 + applen + 2;
    char *result = (char *)allocator.Malloc(total);
    if (result == NULL) {
        PHYSFS_setErrorCode(PHYSFS_ERR_OUT_OF_MEMORY);
        return NULL;
    }
    size_t pos = 0;
    memcpy(result + pos, base, baselen);
    pos += baselen;
    if (baselen == 0 || base[baselen - 1] != '/')
        result[pos++] = '/';
    memcpy(result + pos, org, orglen);
    pos += orglen;
    result[pos++] = '/';
    memcpy(result + pos, app, applen);
    pos += applen;
    result[pos++] = '/';
    result[pos]   = '\0';
    return result;
}

/* -------------------------------------------------------------------------
 * Directory operations
 * ---------------------------------------------------------------------- */

PHYSFS_EnumerateCallbackResult __PHYSFS_platformEnumerate(
    const char *dirname,
    PHYSFS_EnumerateCallback callback,
    const char *origdir,
    void *callbackdata)
{
    FilePathList list = LoadDirectoryFiles(dirname);
    PHYSFS_EnumerateCallbackResult rc = PHYSFS_ENUM_OK;

    for (unsigned int i = 0; i < list.count && rc == PHYSFS_ENUM_OK; i++) {
        const char *name = GetFileName(list.paths[i]);
        if (name == NULL || name[0] == '\0') continue;
        if (name[0] == '.' && name[1] == '\0') continue;
        if (name[0] == '.' && name[1] == '.' && name[2] == '\0') continue;
        rc = callback(callbackdata, origdir, name);
    }

    UnloadDirectoryFiles(list);
    return rc;
}

int __PHYSFS_platformMkDir(const char *path)
{
    if (MakeDirectory(path) != 0) {
        PHYSFS_setErrorCode(PHYSFS_ERR_OS_ERROR);
        return 0;
    }
    return 1;
}

int __PHYSFS_platformDelete(const char *path)
{
    if (remove(path) != 0) {
        PHYSFS_setErrorCode(PHYSFS_ERR_OS_ERROR);
        return 0;
    }
    return 1;
}

int __PHYSFS_platformStat(const char *fn, PHYSFS_Stat *stat, const int follow)
{
    (void)follow;

    if (FileExists(fn)) {
        stat->filesize   = (PHYSFS_sint64)GetFileLength(fn);
        stat->modtime    = (PHYSFS_sint64)GetFileModTime(fn);
        stat->createtime = stat->modtime;
        stat->accesstime = -1;
        stat->filetype   = PHYSFS_FILETYPE_REGULAR;
        stat->readonly   = 0;
        return 1;
    }

    if (DirectoryExists(fn)) {
        stat->filesize   = 0;
        stat->modtime    = -1;
        stat->createtime = -1;
        stat->accesstime = -1;
        stat->filetype   = PHYSFS_FILETYPE_DIRECTORY;
        stat->readonly   = 0;
        return 1;
    }

    PHYSFS_setErrorCode(PHYSFS_ERR_NOT_FOUND);
    return 0;
}

/* -------------------------------------------------------------------------
 * File I/O — fully buffered so seek / tell work on top of raylib's API
 * ---------------------------------------------------------------------- */

typedef struct {
    unsigned char  *data;      /* file contents buffer                    */
    PHYSFS_uint64   size;      /* current logical size of the buffer      */
    PHYSFS_uint64   pos;       /* current read/write position             */
    char           *filename;  /* non-NULL for writable handles           */
} PhysFSRaylibHandle;

void *__PHYSFS_platformOpenRead(const char *filename)
{
    int bytesRead = 0;
    unsigned char *raw = LoadFileData(filename, &bytesRead);
    if (raw == NULL) {
        PHYSFS_setErrorCode(PHYSFS_ERR_NOT_FOUND);
        return NULL;
    }

    PhysFSRaylibHandle *h = (PhysFSRaylibHandle *)allocator.Malloc(sizeof(*h));
    if (h == NULL) {
        UnloadFileData(raw);
        PHYSFS_setErrorCode(PHYSFS_ERR_OUT_OF_MEMORY);
        return NULL;
    }

    /* Copy into allocator-owned memory for consistent lifetime management. */
    h->data = (unsigned char *)allocator.Malloc((size_t)bytesRead);
    if (h->data == NULL) {
        UnloadFileData(raw);
        allocator.Free(h);
        PHYSFS_setErrorCode(PHYSFS_ERR_OUT_OF_MEMORY);
        return NULL;
    }

    memcpy(h->data, raw, (size_t)bytesRead);
    UnloadFileData(raw);
    h->size     = (PHYSFS_uint64)bytesRead;
    h->pos      = 0;
    h->filename = NULL;
    return h;
}

static void *openWritable(const char *filename, int append)
{
    PhysFSRaylibHandle *h = (PhysFSRaylibHandle *)allocator.Malloc(sizeof(*h));
    if (h == NULL) {
        PHYSFS_setErrorCode(PHYSFS_ERR_OUT_OF_MEMORY);
        return NULL;
    }

    size_t fnlen = strlen(filename) + 1;
    h->filename = (char *)allocator.Malloc(fnlen);
    if (h->filename == NULL) {
        allocator.Free(h);
        PHYSFS_setErrorCode(PHYSFS_ERR_OUT_OF_MEMORY);
        return NULL;
    }
    memcpy(h->filename, filename, fnlen);

    h->data = NULL;
    h->size = 0;
    h->pos  = 0;

    if (append && FileExists(filename)) {
        int bytesRead = 0;
        unsigned char *raw = LoadFileData(filename, &bytesRead);
        if (raw != NULL && bytesRead > 0) {
            h->data = (unsigned char *)allocator.Malloc((size_t)bytesRead);
            if (h->data == NULL) {
                UnloadFileData(raw);
                allocator.Free(h->filename);
                allocator.Free(h);
                PHYSFS_setErrorCode(PHYSFS_ERR_OUT_OF_MEMORY);
                return NULL;
            }
            memcpy(h->data, raw, (size_t)bytesRead);
            UnloadFileData(raw);
            h->size = (PHYSFS_uint64)bytesRead;
            h->pos  = h->size;
        } else if (raw != NULL) {
            UnloadFileData(raw);
        }
    }

    return h;
}

void *__PHYSFS_platformOpenWrite(const char *filename)
{
    return openWritable(filename, 0);
}

void *__PHYSFS_platformOpenAppend(const char *filename)
{
    return openWritable(filename, 1);
}

PHYSFS_sint64 __PHYSFS_platformRead(void *opaque, void *buf, PHYSFS_uint64 len)
{
    PhysFSRaylibHandle *h = (PhysFSRaylibHandle *)opaque;
    PHYSFS_uint64 remaining = h->size - h->pos;
    PHYSFS_uint64 toRead    = (len < remaining) ? len : remaining;
    if (toRead > 0) {
        memcpy(buf, h->data + h->pos, (size_t)toRead);
        h->pos += toRead;
    }
    return (PHYSFS_sint64)toRead;
}

PHYSFS_sint64 __PHYSFS_platformWrite(void *opaque, const void *buf, PHYSFS_uint64 len)
{
    PhysFSRaylibHandle *h = (PhysFSRaylibHandle *)opaque;
    PHYSFS_uint64 needed = h->pos + len;

    if (needed > h->size) {
        unsigned char *newdata = (unsigned char *)allocator.Realloc(h->data, (size_t)needed);
        if (newdata == NULL) {
            PHYSFS_setErrorCode(PHYSFS_ERR_OUT_OF_MEMORY);
            return -1;
        }
        h->data = newdata;
        h->size = needed;
    }

    memcpy(h->data + h->pos, buf, (size_t)len);
    h->pos += len;
    return (PHYSFS_sint64)len;
}

int __PHYSFS_platformSeek(void *opaque, PHYSFS_uint64 pos)
{
    PhysFSRaylibHandle *h = (PhysFSRaylibHandle *)opaque;

    /* Writable handles may seek past EOF; extend buffer with zeros. */
    if (pos > h->size) {
        if (h->filename != NULL) {
            unsigned char *newdata = (unsigned char *)allocator.Realloc(h->data, (size_t)pos);
            if (newdata == NULL) {
                PHYSFS_setErrorCode(PHYSFS_ERR_OUT_OF_MEMORY);
                return 0;
            }
            memset(newdata + h->size, 0, (size_t)(pos - h->size));
            h->data = newdata;
            h->size = pos;
        } else {
            PHYSFS_setErrorCode(PHYSFS_ERR_PAST_EOF);
            return 0;
        }
    }

    h->pos = pos;
    return 1;
}

PHYSFS_sint64 __PHYSFS_platformTell(void *opaque)
{
    return (PHYSFS_sint64)((PhysFSRaylibHandle *)opaque)->pos;
}

PHYSFS_sint64 __PHYSFS_platformFileLength(void *opaque)
{
    return (PHYSFS_sint64)((PhysFSRaylibHandle *)opaque)->size;
}

int __PHYSFS_platformFlush(void *opaque)
{
    PhysFSRaylibHandle *h = (PhysFSRaylibHandle *)opaque;
    if (h->filename == NULL) return 1;  /* read-only handle */
    if (h->data == NULL) return 1;      /* nothing written yet */

    if (!SaveFileData(h->filename, h->data, (int)h->size)) {
        PHYSFS_setErrorCode(PHYSFS_ERR_OS_ERROR);
        return 0;
    }
    return 1;
}

void __PHYSFS_platformClose(void *opaque)
{
    PhysFSRaylibHandle *h = (PhysFSRaylibHandle *)opaque;
    if (h->filename != NULL) {
        if (h->data != NULL)
            SaveFileData(h->filename, h->data, (int)h->size);
        allocator.Free(h->filename);
    }
    if (h->data != NULL)
        allocator.Free(h->data);
    allocator.Free(h);
}
