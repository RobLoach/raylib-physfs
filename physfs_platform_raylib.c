/**
 * raylib platform implementation for PhysFS.
 *
 * Define PHYSFS_PLATFORM_RAYLIB before including raylib-physfs.h:
 *
 *   #define PHYSFS_PLATFORM_RAYLIB
 *   #define RAYLIB_PHYSFS_IMPLEMENTATION
 *   #include "raylib-physfs.h"
 */

#include <limits.h>  /* UINT_MAX */
#include <string.h>  /* memset */

typedef struct {
    unsigned char  *data; /* file contents */
    PHYSFS_uint64   size; /* current logical size of the buffer */
    PHYSFS_uint64   pos; /* current read/write position */
    char           *filename; /* non-NULL for writable handles */
} PhysFSRaylibHandle;

void* __PHYSFS_platformCreateMutex(void) { return (void *)0x1; }
void __PHYSFS_platformDestroyMutex(void *mutex) { (void)mutex; }
int __PHYSFS_platformGrabMutex(void *mutex) { (void)mutex; return 1; }
void __PHYSFS_platformReleaseMutex(void *mutex) { (void)mutex; }
void* __PHYSFS_platformGetThreadID(void) { return (void *)0x1; }
void __PHYSFS_platformDetectAvailableCDs(PHYSFS_StringCallback cb, void *data) { (void)cb; (void)data; }

int __PHYSFS_platformInit(const char *argv0)
{
    (void)argv0;
    TraceLog(LOG_DEBUG, "PHYSFS: platformInit");
    return 1;
}

void __PHYSFS_platformDeinit(void)
{
    TraceLog(LOG_DEBUG, "PHYSFS: platformDeinit");
}

/**
 * Returns a MemAlloc copy of a path, ensuring it ends with '/'.
 */
static char *platformDupWithSep(const char *path)
{
    unsigned int len = TextLength(path);
    int needSep = (len == 0 || path[len - 1] != '/');
    char *result = (char *)MemAlloc(len + needSep + 1);
    if (result == NULL) {
        PHYSFS_setErrorCode(PHYSFS_ERR_OUT_OF_MEMORY);
        return NULL;
    }
    TextCopy(result, path);
    if (needSep) {
        result[len]     = '/';
        result[len + 1] = '\0';
    }
    return result;
}

/**
 * Gets the application directory.
 */
char *__PHYSFS_platformCalcBaseDir(const char *argv0)
{
    (void)argv0;
    char *result = platformDupWithSep(GetApplicationDirectory());
    TraceLog(LOG_DEBUG, "PHYSFS: platformCalcBaseDir: %s", result ? result : "(null)");
    return result;
}

/**
 * Gets the application directory.
 *
 * TODO: Add a HOME Directory function to raylib?
 */
char *__PHYSFS_platformCalcUserDir(void)
{
    char *result = platformDupWithSep(GetApplicationDirectory());
    TraceLog(LOG_DEBUG, "PHYSFS: platformCalcUserDir: %s", result ? result : "(null)");
    return result;
}

/**
 * Gets a subdirectory from the application directory.
 *
 * TODO: Add a user pref directory to raylib?
 */
char *__PHYSFS_platformCalcPrefDir(const char *org, const char *app)
{
    const char *base     = GetApplicationDirectory();
    unsigned int baselen = TextLength(base);
    unsigned int orglen  = TextLength(org);
    unsigned int applen  = TextLength(app);
    /* base [/] org / app / \0 */
    unsigned int total = baselen + 1 + orglen + 1 + applen + 2;
    char *result = (char *)MemAlloc(total);
    if (result == NULL) {
        PHYSFS_setErrorCode(PHYSFS_ERR_OUT_OF_MEMORY);
        return NULL;
    }
    int pos = TextCopy(result, base);
    if (baselen == 0 || base[baselen - 1] != '/')
        result[pos++] = '/';
    pos += TextCopy(result + pos, org);
    result[pos++] = '/';
    pos += TextCopy(result + pos, app);
    result[pos++] = '/';
    result[pos]   = '\0';
    TraceLog(LOG_DEBUG, "PHYSFS: platformCalcPrefDir: %s", result);
    return result;
}

PHYSFS_EnumerateCallbackResult __PHYSFS_platformEnumerate(const char *dirname, PHYSFS_EnumerateCallback callback, const char *origdir, void *callbackdata)
{
    TraceLog(LOG_DEBUG, "PHYSFS: platformEnumerate: %s", dirname);
    FilePathList list = LoadDirectoryFiles(dirname);
    PHYSFS_EnumerateCallbackResult rc = PHYSFS_ENUM_OK;

    for (unsigned int i = 0; i < list.count && rc == PHYSFS_ENUM_OK; i++) {
        if (list.paths[i] == NULL) continue;
        const char *name = GetFileName(list.paths[i]);
        if (name == NULL || name[0] == '\0') continue;
        if (name[0] == '.' && name[1] == '\0') continue;
        if (name[0] == '.' && name[1] == '.' && name[2] == '\0') continue;
        rc = callback(callbackdata, origdir, name);
        /* The return value is a PHYSFS_EnumerateCallbackResult */
        if (rc != PHYSFS_ENUM_OK) {
            break;
        }
    }

    UnloadDirectoryFiles(list);
    return rc;
}

int __PHYSFS_platformMkDir(const char *path)
{
    if (MakeDirectory(path) != 0) {
        TraceLog(LOG_WARNING, "PHYSFS: platformMkDir failed: %s", path);
        PHYSFS_setErrorCode(PHYSFS_ERR_OS_ERROR);
        return 0;
    }
    TraceLog(LOG_DEBUG, "PHYSFS: platformMkDir: %s", path);
    return 1;
}

int __PHYSFS_platformDelete(const char *path)
{
    if (!FileRemove(path)) {
        TraceLog(LOG_WARNING, "PHYSFS: platformDelete failed: %s", path);
        PHYSFS_setErrorCode(PHYSFS_ERR_OS_ERROR);
        return 0;
    }
    TraceLog(LOG_DEBUG, "PHYSFS: platformDelete: %s", path);
    return 1;
}

int __PHYSFS_platformStat(const char *fn, PHYSFS_Stat *stat, const int follow)
{
    (void)follow;

    if (DirectoryExists(fn)) {
        TraceLog(LOG_DEBUG, "PHYSFS: platformStat directory: %s", fn);
        stat->filesize   = 0;
        stat->modtime    = -1;
        stat->createtime = -1;
        stat->accesstime = -1;
        stat->filetype   = PHYSFS_FILETYPE_DIRECTORY;
        stat->readonly   = 0;
        return 1;
    }

    if (FileExists(fn)) {
        TraceLog(LOG_DEBUG, "PHYSFS: platformStat file: %s", fn);
        stat->filesize   = (PHYSFS_sint64)GetFileLength(fn);
        stat->modtime    = (PHYSFS_sint64)GetFileModTime(fn);
        stat->createtime = stat->modtime;
        stat->accesstime = -1;
        stat->filetype   = PHYSFS_FILETYPE_REGULAR;
        stat->readonly   = 0;
        return 1;
    }

    TraceLog(LOG_DEBUG, "PHYSFS: platformStat not found: %s", fn);
    PHYSFS_setErrorCode(PHYSFS_ERR_NOT_FOUND);
    return 0;
}

void *__PHYSFS_platformOpenRead(const char *filename)
{
    if (DirectoryExists(filename)) {
        TraceLog(LOG_DEBUG, "PHYSFS: platformOpenRead directory: %s", filename);
        PhysFSRaylibHandle *h = (PhysFSRaylibHandle *)MemAlloc(sizeof(*h));
        if (h == NULL) {
            PHYSFS_setErrorCode(PHYSFS_ERR_OUT_OF_MEMORY);
            return NULL;
        }
        h->data     = NULL;
        h->size     = 0;
        h->pos      = 0;
        h->filename = NULL;
        return h;
    }

    int bytesRead = 0;
    unsigned char *raw = LoadFileData(filename, &bytesRead);
    if (raw == NULL) {
        TraceLog(LOG_WARNING, "PHYSFS: platformOpenRead failed: %s", filename);
        PHYSFS_setErrorCode(PHYSFS_ERR_NOT_FOUND);
        return NULL;
    }

    PhysFSRaylibHandle *h = (PhysFSRaylibHandle *)MemAlloc(sizeof(*h));
    if (h == NULL) {
        UnloadFileData(raw);
        PHYSFS_setErrorCode(PHYSFS_ERR_OUT_OF_MEMORY);
        return NULL;
    }

    if (bytesRead > 0) {
        h->data = (unsigned char *)MemAlloc((unsigned int)bytesRead);
        if (h->data == NULL) {
            UnloadFileData(raw);
            MemFree(h);
            PHYSFS_setErrorCode(PHYSFS_ERR_OUT_OF_MEMORY);
            return NULL;
        }
        RAYLIB_PHYSFS_MEMCPY(h->data, raw, (size_t)bytesRead);
    }
    UnloadFileData(raw);
    h->size     = (PHYSFS_uint64)bytesRead;
    h->pos      = 0;
    h->filename = NULL;
    TraceLog(LOG_DEBUG, "PHYSFS: platformOpenRead: %s (%i bytes)", filename, bytesRead);
    return h;
}

static void *openWritable(const char *filename, int append)
{
    PhysFSRaylibHandle *h = (PhysFSRaylibHandle *)MemAlloc(sizeof(*h));
    if (h == NULL) {
        PHYSFS_setErrorCode(PHYSFS_ERR_OUT_OF_MEMORY);
        return NULL;
    }

    h->filename = (char *)MemAlloc(TextLength(filename) + 1);
    if (h->filename == NULL) {
        MemFree(h);
        PHYSFS_setErrorCode(PHYSFS_ERR_OUT_OF_MEMORY);
        return NULL;
    }
    TextCopy(h->filename, filename);

    h->data = NULL;
    h->size = 0;
    h->pos  = 0;

    if (append && FileExists(filename)) {
        int bytesRead = 0;
        unsigned char *raw = LoadFileData(filename, &bytesRead);
        if (raw != NULL && bytesRead > 0) {
            h->data = (unsigned char *)MemAlloc((unsigned int)bytesRead);
            if (h->data == NULL) {
                UnloadFileData(raw);
                MemFree(h->filename);
                MemFree(h);
                PHYSFS_setErrorCode(PHYSFS_ERR_OUT_OF_MEMORY);
                return NULL;
            }
            RAYLIB_PHYSFS_MEMCPY(h->data, raw, (size_t)bytesRead);
            UnloadFileData(raw);
            h->size = (PHYSFS_uint64)bytesRead;
            h->pos  = h->size;
        } else if (raw != NULL) {
            UnloadFileData(raw);
        }
    }

    TraceLog(LOG_DEBUG, "PHYSFS: platformOpen%s: %s", append ? "Append" : "Write", filename);
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
        RAYLIB_PHYSFS_MEMCPY(buf, h->data + h->pos, (size_t)toRead);
        h->pos += toRead;
    }
    return (PHYSFS_sint64)toRead;
}

PHYSFS_sint64 __PHYSFS_platformWrite(void *opaque, const void *buf, PHYSFS_uint64 len)
{
    PhysFSRaylibHandle *h = (PhysFSRaylibHandle *)opaque;
    PHYSFS_uint64 needed = h->pos + len;

    if (needed > h->size) {
        if (needed > UINT_MAX) {
            PHYSFS_setErrorCode(PHYSFS_ERR_OUT_OF_MEMORY);
            return -1;
        }
        unsigned char *newdata = (unsigned char *)MemRealloc(h->data, (unsigned int)needed);
        if (newdata == NULL) {
            PHYSFS_setErrorCode(PHYSFS_ERR_OUT_OF_MEMORY);
            return -1;
        }
        h->data = newdata;
        h->size = needed;
    }

    RAYLIB_PHYSFS_MEMCPY(h->data + h->pos, buf, (size_t)len);
    h->pos += len;
    return (PHYSFS_sint64)len;
}

int __PHYSFS_platformSeek(void *opaque, PHYSFS_uint64 pos)
{
    PhysFSRaylibHandle *h = (PhysFSRaylibHandle *)opaque;

    /* Expand the buffer if it goes beyond the memory size. */
    if (pos > h->size) {
        if (h->filename != NULL) {
            if (pos > UINT_MAX) {
                PHYSFS_setErrorCode(PHYSFS_ERR_OUT_OF_MEMORY);
                return 0;
            }
            unsigned char *newdata = (unsigned char *)MemRealloc(h->data, (unsigned int)pos);
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

    if (h->size > (PHYSFS_uint64)INT_MAX) {
        TraceLog(LOG_ERROR, "PHYSFS: platformFlush file too large: %s", h->filename);
        PHYSFS_setErrorCode(PHYSFS_ERR_OS_ERROR);
        return 0;
    }
    if (!SaveFileData(h->filename, h->data, (int)h->size)) {
        TraceLog(LOG_ERROR, "PHYSFS: platformFlush failed: %s", h->filename);
        PHYSFS_setErrorCode(PHYSFS_ERR_OS_ERROR);
        return 0;
    }
    TraceLog(LOG_DEBUG, "PHYSFS: platformFlush: %s (%i bytes)", h->filename, (int)h->size);
    return 1;
}

void __PHYSFS_platformClose(void *opaque)
{
    PhysFSRaylibHandle *h = (PhysFSRaylibHandle *)opaque;
    if (h->filename != NULL) {
        TraceLog(LOG_DEBUG, "PHYSFS: platformClose: %s (%i bytes)", h->filename, (int)h->size);
        if (h->data != NULL) {
            if (h->size > (PHYSFS_uint64)INT_MAX)
                TraceLog(LOG_ERROR, "PHYSFS: platformClose file too large to save: %s", h->filename);
            else if (!SaveFileData(h->filename, h->data, (int)h->size))
                TraceLog(LOG_ERROR, "PHYSFS: platformClose failed to save: %s", h->filename);
        }
        MemFree(h->filename);
    }
    if (h->data != NULL)
        MemFree(h->data);
    MemFree(h);
}
