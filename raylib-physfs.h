/**********************************************************************************************
*
*   raylib-physfs 6.0.0 - Integrate PhysFS with raylib, allowing to load images, audio and fonts from data archives.
*
*   Copyright 2026 Rob Loach (@RobLoach)
*
*   DEPENDENCIES:
*       raylib 6.0+ https://www.raylib.com/
*       physfs https://github.com/icculus/physfs
*
*   LICENSE: zlib/libpng
*
*   raylib-physfs is licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software:
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/

#ifndef INCLUDE_RAYLIB_PHYSFS_H_
#define INCLUDE_RAYLIB_PHYSFS_H_

#ifndef RAYLIB_PHYSFS_DEF
#define RAYLIB_PHYSFS_DEF
#endif

#ifdef __cplusplus
extern "C" {
#endif

RAYLIB_PHYSFS_DEF bool InitPhysFS();                                              // Initialize the PhysFS file system
RAYLIB_PHYSFS_DEF bool InitPhysFSEx(const char* newDir, const char* mountPoint);  // Initialize the PhysFS file system with a mount point.
RAYLIB_PHYSFS_DEF bool ClosePhysFS();                                             // Close the PhysFS file system
RAYLIB_PHYSFS_DEF bool IsPhysFSReady();                                           // Check if PhysFS has been initialized successfully
RAYLIB_PHYSFS_DEF bool MountPhysFS(const char* newDir, const char* mountPoint);   // Mount the given directory or archive as a mount point
RAYLIB_PHYSFS_DEF bool MountPhysFSFromMemory(const unsigned char *fileData, int dataSize, const char* newDir, const char* mountPoint);  // Mount the given file data as a mount point
RAYLIB_PHYSFS_DEF bool UnmountPhysFS(const char* oldDir);                         // Unmounts the given directory
RAYLIB_PHYSFS_DEF bool FileExistsInPhysFS(const char* fileName);                  // Check if the given file exists in PhysFS
RAYLIB_PHYSFS_DEF bool DirectoryExistsInPhysFS(const char* dirPath);              // Check if the given directory exists in PhysFS
RAYLIB_PHYSFS_DEF unsigned char* LoadFileDataFromPhysFS(const char* fileName, int* bytesRead);  // Load a data buffer from PhysFS (memory should be freed)
RAYLIB_PHYSFS_DEF char* LoadFileTextFromPhysFS(const char* fileName);             // Load text from a file (memory should be freed)
RAYLIB_PHYSFS_DEF bool SetPhysFSWriteDirectory(const char* newDir);               // Set the base directory where PhysFS should write files to (defaults to the current working directory)
RAYLIB_PHYSFS_DEF bool SaveFileDataToPhysFS(const char* fileName, void* data, int bytesToWrite);  // Save the given file data in PhysFS
RAYLIB_PHYSFS_DEF bool SaveFileTextToPhysFS(const char* fileName, const char* text);    // Save the given file text in PhysFS
RAYLIB_PHYSFS_DEF FilePathList LoadDirectoryFilesFromPhysFS(const char* dirPath);  // Get filenames in a directory path (memory should be freed)
RAYLIB_PHYSFS_DEF FilePathList LoadDirectoryFilesExFromPhysFS(const char *basePath, const char *filter, bool scanSubdirs);  // Get filenames in a directory path with optional extension filter and recursive scanning (memory should be freed)
RAYLIB_PHYSFS_DEF long GetFileModTimeFromPhysFS(const char* fileName);            // Get file modification time (last write time) from PhysFS
RAYLIB_PHYSFS_DEF Image LoadImageFromPhysFS(const char* fileName);                // Load an image from PhysFS
RAYLIB_PHYSFS_DEF Texture2D LoadTextureFromPhysFS(const char* fileName);          // Load a texture from PhysFS
RAYLIB_PHYSFS_DEF Wave LoadWaveFromPhysFS(const char* fileName);                  // Load wave data from PhysFS
RAYLIB_PHYSFS_DEF Music LoadMusicStreamFromPhysFS(const char* fileName);          // Load music data from PhysFS
RAYLIB_PHYSFS_DEF Font LoadFontFromPhysFS(const char* fileName, int fontSize, int *fontChars, int charsCount);  // Load a font from PhysFS
RAYLIB_PHYSFS_DEF Shader LoadShaderFromPhysFS(const char* vsFileName, const char* fsFileName);  // Load shader from PhysFS
RAYLIB_PHYSFS_DEF void SetPhysFSCallbacks();                                      // Set the raylib file loader/saver callbacks to use PhysFS
RAYLIB_PHYSFS_DEF const char* GetPrefDirectory(const char *organization, const char *application); // Get the user's current config directory for the application.

#ifdef __cplusplus
}
#endif

#endif  // INCLUDE_RAYLIB_PHYSFS_H_

#ifdef RAYLIB_PHYSFS_IMPLEMENTATION
#ifndef RAYLIB_PHYSFS_IMPLEMENTATION_ONCE
#define RAYLIB_PHYSFS_IMPLEMENTATION_ONCE

#ifndef RAYLIB_PHYSFS_MEMCPY
#include <string.h>
#define RAYLIB_PHYSFS_MEMCPY memcpy
#endif

// PhysFS
#define PHYSFS_IMPL
#define PHYSFS_PLATFORM_IMPL
#define PHYSFS_DECL RAYLIB_PHYSFS_DEF
#include "physfs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Reports the last PhysFS error to raylib's TraceLog.
 *
 * @param detail Any additional detail to append to the reported error.
 *
 * @see PHYSFS_getLastErrorCode()
 *
 * @internal
 */
void TracePhysFSError(const char* detail) {
    PHYSFS_ErrorCode errorCode = PHYSFS_getLastErrorCode();
    if (errorCode == PHYSFS_ERR_OK) {
        TraceLog(LOG_WARNING, TextFormat("PHYSFS: %s", detail));
    } else {
        const char* errorMessage = PHYSFS_getErrorByCode(errorCode);
        TraceLog(LOG_WARNING, TextFormat("PHYSFS: %s (%s)", errorMessage, detail));
    }
}

/**
 * Loads the given file as a byte array from PhysFS (read).
 *
 * @param fileName The file to load.
 * @param bytesRead An integer to save the bytes that were read.
 *
 * @return The file data as a pointer. Make sure to use UnloadFileData() when finished using the file data.
 *
 * @see UnloadFileData()
 */
unsigned char* LoadFileDataFromPhysFS(const char* fileName, int* bytesRead) {
    if (!FileExistsInPhysFS(fileName)) {
        TraceLog(LOG_WARNING, TextFormat("PHYSFS: Tried to load non-existent file '%s'", fileName));
        *bytesRead = 0;
        return 0;
    }

    // Open up the file.
    PHYSFS_File* handle = PHYSFS_openRead(fileName);
    if (handle == 0) {
        TracePhysFSError(fileName);
        *bytesRead = 0;
        return 0;
    }

    // Check to see how large the file is.
    PHYSFS_sint64 size = PHYSFS_fileLength(handle);
    if (size == -1) {
        *bytesRead = 0;
        PHYSFS_close(handle);
        TraceLog(LOG_WARNING, TextFormat("PHYSFS: Cannot determine size of file '%s'", fileName));
        return 0;
    }

    // Close safely when it's empty.
    if (size == 0) {
        PHYSFS_close(handle);
        *bytesRead = 0;
        return 0;
    }

    // Read the file, return if it's empty.
    void* buffer = MemAlloc((unsigned int)size);
    PHYSFS_sint64 read = PHYSFS_readBytes(handle, buffer, (PHYSFS_uint64)size);
    if (read != size) {
        *bytesRead = 0;
        MemFree(buffer);
        PHYSFS_close(handle);
        TracePhysFSError(fileName);
        return 0;
    }

    // Close the file handle, and return the bytes read and the buffer.
    PHYSFS_close(handle);
    *bytesRead = (int)read;
    return (unsigned char*) buffer;
}

/**
 * Initialize the PhysFS virtual file system.
 *
 * @return True on success, false on failure.
 *
 * @see ClosePhysFS()
 */
bool InitPhysFS() {
    // Initialize PhysFS.
    if (PHYSFS_init(0) == 0) {
        TracePhysFSError("InitPhysFS() failed");
        return false;
    }

    // Set the default write directory, and report success.
    SetPhysFSWriteDirectory(GetWorkingDirectory());
    TraceLog(LOG_DEBUG, "PHYSFS: Initialized PhysFS");
    return true;
}

/**
 * Initialize the PhysFS virtual file system with the given mount point.
 *
 * @return True on success, false on failure.
 *
 * @see ClosePhysFS()
 */
bool InitPhysFSEx(const char* newDir, const char* mountPoint) {
    if (InitPhysFS()) {
        return MountPhysFS(newDir, mountPoint);
    }
    return false;
}

/**
 * Check if PhysFS has been initialized successfully.
 *
 * @return True if PhysFS is initialized, false otherwise.
 *
 * @see InitPhysFS()
 */
bool IsPhysFSReady() {
    return PHYSFS_isInit() != 0;
}

/**
 * Mounts the given directory, at the given mount point.
 *
 * @param newDir Directory or archive to add to the path, in platform-dependent notation.
 * @param mountPoint Location in the interpolated tree that this archive will be "mounted", in platform-independent notation. NULL or "" is equivalent to "/".
 *
 * @return True on success, false on failure.
 *
 * @see UnmountPhysFS()
 */
bool MountPhysFS(const char* newDir, const char* mountPoint) {
    if (PHYSFS_mount(newDir, mountPoint, 1) == 0) {
        TracePhysFSError(mountPoint);
        return false;
    }

    TraceLog(LOG_DEBUG, "PHYSFS: Mounted '%s' at '%s'", newDir, mountPoint);
    return true;
}

/**
 * Mounts the given file data as a mount point in PhysFS.
 *
 * @param fileData The archive data as a file buffer.
 * @param dataSize The size of the file buffer.
 * @param newDir A filename that can represent the file data. Has to be unique. For example: data.zip
 * @param mountPoint The location in the tree that the archive will be mounted.
 *
 * @return True on success, false on failure.
 *
 * @see MountPhysFS()
 */
bool MountPhysFSFromMemory(const unsigned char *fileData, int dataSize, const char* newDir, const char* mountPoint) {
    if (dataSize <= 0) {
        TraceLog(LOG_WARNING, "PHYSFS: Cannot mount a data size of 0");
        return false;
    }

    if (PHYSFS_mountMemory(fileData, (PHYSFS_uint64)dataSize, 0, newDir, mountPoint, 1) == 0) {
        TracePhysFSError(TextFormat("Failed to mount '%s' at '%s'", newDir, mountPoint));
        return false;
    }

    TraceLog(LOG_DEBUG, "PHYSFS: Mounted memory '%s' at '%s'", newDir, mountPoint);
    return true;
}

/**
 * Unmounts the given directory or archive.
 *
 * @param oldDir The directory that was supplied to MountPhysFS's newDir.
 *
 * @return True on success, false on failure.
 *
 * @see MountPhysFS()
 */
bool UnmountPhysFS(const char* oldDir) {
    if (PHYSFS_unmount(oldDir) == 0) {
        TraceLog(LOG_WARNING, "PHYSFS: Failed to unmount directory '%s'", oldDir);
        return false;
    }

    TraceLog(LOG_DEBUG, "PHYSFS: Unmounted '%s'", oldDir);
    return true;
}

/**
 * Determine if a file exists in the search path.
 *
 * @param fileName Filename in platform-independent notation.
 *
 * @return True if the file exists, false otherwise.
 *
 * @see DirectoryExistsInPhysFS()
 */
bool FileExistsInPhysFS(const char* fileName) {
    PHYSFS_Stat stat;
    if (PHYSFS_stat(fileName, &stat) == 0) {
        return false;
    }
    return stat.filetype == PHYSFS_FILETYPE_REGULAR;
}

/**
 * Determine if a directory exists in the search path.
 *
 * @param dirPath Directory in platform-independent notation.
 *
 * @return True if the directory exists, false otherwise.
 *
 * @see FileExistsInPhysFS()
 */
bool DirectoryExistsInPhysFS(const char* dirPath) {
    PHYSFS_Stat stat;
    if (PHYSFS_stat(dirPath, &stat) == 0) {
        return false;
    }
    return stat.filetype == PHYSFS_FILETYPE_DIRECTORY;
}

/**
 * Load an image from PhysFS.
 *
 * @param fileName The filename to load from the search paths.
 *
 * @return The loaded image on success. An empty Image otherwise.
 */
Image LoadImageFromPhysFS(const char* fileName) {
    int bytesRead;
    unsigned char* fileData = LoadFileDataFromPhysFS(fileName, &bytesRead);
    if (bytesRead == 0) {
        Image output = { 0 };
        return output;
    }

    // Load from the memory.
    const char* extension = GetFileExtension(fileName);
    Image image = LoadImageFromMemory(extension, fileData, bytesRead);
    UnloadFileData(fileData);
    return image;
}

/**
 * Load a texture from PhysFS.
 *
 * @param fileName The filename to load from the search paths.
 *
 * @return The loaded texture on success. An empty texture otherwise.
 *
 * @see LoadImageFromPhysFS()
 */
Texture2D LoadTextureFromPhysFS(const char* fileName) {
    Image image = LoadImageFromPhysFS(fileName);
    if (image.data == 0) {
        Texture2D output = { 0 };
        return output;
    }
    Texture2D texture = LoadTextureFromImage(image);
    UnloadImage(image);
    return texture;
}

/**
 * Load text data from file (read). Make sure to call UnloadFileText() when done.
 *
 * @param fileName The file name to load from the PhysFS mount paths.
 *
 * @return A '\0' terminated string.
 *
 * @see UnloadFileText()
 */
char* LoadFileTextFromPhysFS(const char *fileName) {
    int bytesRead;
    unsigned char* data = LoadFileDataFromPhysFS(fileName, &bytesRead);
    if (bytesRead == 0) {
        return 0;
    }

    // Copy the data, and append a null terminator.
    char* text = (char*)MemAlloc(bytesRead + 1);
    RAYLIB_PHYSFS_MEMCPY(text, data, bytesRead);
    text[bytesRead] = '\0';

    // Free the original data, and return the string.
    MemFree(data);

    return text;
}

/**
 * Load wave data from PhysFS.
 *
 * @param fileName The file name to load from the PhysFS mount paths.
 *
 * @return The Wave object, or an empty Wave object on failure.
 *
 * @see UnloadWave()
 */
Wave LoadWaveFromPhysFS(const char* fileName) {
    int bytesRead;
    unsigned char* fileData = LoadFileDataFromPhysFS(fileName, &bytesRead);
    if (bytesRead == 0) {
        Wave output = { 0 };
        return output;
    }

    // Load from the memory.
    const char* extension = GetFileExtension(fileName);
    Wave wave = LoadWaveFromMemory(extension, fileData, bytesRead);
    UnloadFileData(fileData);
    return wave;
}

/**
 * Load module music from PhysFS.
 *
 * @param fileName The file name to load from the PhysFS mount paths.
 *
 * @return The Music object, or an empty Music object on failure.
 *
 * @see UnloadMusic()
 */
Music LoadMusicStreamFromPhysFS(const char* fileName) {
    int bytesRead;
    unsigned char* fileData = LoadFileDataFromPhysFS(fileName, &bytesRead);
    if (bytesRead == 0) {
        Music output = { 0 };
        return output;
    }

    // Load from the memory.
    const char* extension = GetFileExtension(fileName);
    Music music = LoadMusicStreamFromMemory(extension, fileData, bytesRead);

    // Unload the file data if the music failed to load.
    if (music.ctxData == (void*)0) {
        UnloadFileData(fileData);
    }

    return music;
}

/**
 * Load font from PhysFS.
 *
 * @param fileName The file name to load from the PhysFS mount paths.
 *
 * @return The Font object, or an empty Font object on failure.
 *
 * @see UnloadFont()
 */
Font LoadFontFromPhysFS(const char* fileName, int fontSize, int *fontChars, int charsCount) {
    int bytesRead;
    unsigned char* fileData = LoadFileDataFromPhysFS(fileName, &bytesRead);
    if (bytesRead == 0) {
        Font output = { 0 };
        return output;
    }

    // Load from the memory.
    const char* extension = GetFileExtension(fileName);
    Font font = LoadFontFromMemory(extension, fileData, bytesRead, fontSize, fontChars, charsCount);
    UnloadFileData(fileData);
    return font;
}

/**
 * Load a Shader from PhysFS.
 *
 * @param vsFileName The name of the vs file to load.
 * @param fsFileName The name of the fs file to load.
 *
 * @return The Shader object.
 *
 * @see UnloadShader()
 */
Shader LoadShaderFromPhysFS(const char *vsFileName, const char *fsFileName) {
    char* vsFile = LoadFileTextFromPhysFS(vsFileName);
    char* fsFile = LoadFileTextFromPhysFS(fsFileName);
    if (vsFile == 0 && fsFile == 0) {
        Shader output = { 0 };
        return output;
    }
    Shader output = LoadShaderFromMemory(vsFile, fsFile);
    if (vsFile != 0) {
        UnloadFileText(vsFile);
    }
    if (fsFile != 0) {
        UnloadFileText(fsFile);
    }
    return output;
}

/**
 * Sets where PhysFS will attempt to write files. Defaults to the current working directory.
 *
 * @param newDir The new directory to be the root for writing files.
 *
 * @return True on success, false on failure.
 */
bool SetPhysFSWriteDirectory(const char* newDir) {
    if (PHYSFS_setWriteDir(newDir) == 0) {
        TracePhysFSError(newDir);
        return false;
    }

    return true;
}

/**
 * Save file data to file (write).
 *
 * @param fileName The name of the file to save.
 * @param data The data to be saved.
 * @param bytesToWrite The amount of bytes that are to be written.
 *
 * @return True on success, false on failure.
 */
bool SaveFileDataToPhysFS(const char* fileName, void* data, int bytesToWrite) {
    // Protect against empty writes.
    if (bytesToWrite == 0) {
        return true;
    }

    // Open the file.
    PHYSFS_File* handle = PHYSFS_openWrite(fileName);
    if (handle == 0) {
        TracePhysFSError(fileName);
        return false;
    }

    // Write the data to the file handle.
    if (PHYSFS_writeBytes(handle, data, (PHYSFS_uint64)bytesToWrite) < (PHYSFS_sint64)bytesToWrite) {
        PHYSFS_close(handle);
        TracePhysFSError(fileName);
        return false;
    }

    PHYSFS_close(handle);
    return true;
}

/**
 * Save text data to file (write).
 *
 * @param fileName The name of the file to save.
 * @param text A '\0' terminated string.
 *
 * @return True on success, false on failure.
 */
bool SaveFileTextToPhysFS(const char* fileName, const char* text) {
    return SaveFileDataToPhysFS(fileName, (void*)text, TextLength(text) + 1); // +1 for the Null Terminator
}

/**
 * Gets a list of files in the given directory in PhysFS.
 *
 * Make sure to clear the loaded list by using UnloadDirectoryFiles().
 *
 * @see UnloadDirectoryFiles()
 */
FilePathList LoadDirectoryFilesFromPhysFS(const char* dirPath) {
    // Make sure the directory exists.
    if (!DirectoryExistsInPhysFS(dirPath)) {
        TraceLog(LOG_WARNING, "PHYSFS: Can't get files from non-existent directory (%s)", dirPath);
        FilePathList out = { 0 };
        return out;
    }

    // Load the list of files from PhysFS into a temporary PhysFS-owned list.
    char** physfsList = PHYSFS_enumerateFiles(dirPath);
    if (physfsList == NULL) {
        TracePhysFSError(dirPath);
        FilePathList out = { 0 };
        return out;
    }

    // Count the files.
    int count = 0;
    for (char** i = physfsList; *i != NULL; i++) {
        count++;
    }

    // Copy into raylib-allocated memory so UnloadDirectoryFiles() can free it correctly.
    FilePathList output = { 0 };
    output.capacity = (unsigned int)count;
    output.count = count;
    output.paths = count > 0 ? (char**)MemAlloc(count * sizeof(char*)) : NULL;
    for (int i = 0; i < count; i++) {
        output.paths[i] = (char*)MemAlloc(TextLength(physfsList[i]) + 1);
        TextCopy(output.paths[i], physfsList[i]);
    }

    PHYSFS_freeList(physfsList);
    return output;
}

/**
 * Enumerate matching files in a PhysFS directory, recursively if requested.
 * When files is NULL, returns the count only (counting pass).
 * When files is non-NULL, fills files->paths and increments files->count (filling pass).
 */
static int EnumerateFilesFromPhysFS(const char* dirPath, const char* filter, bool scanSubdirs, FilePathList* files) {
    char** physfsList = PHYSFS_enumerateFiles(dirPath);
    if (physfsList == NULL) return 0;
    int count = 0;

    for (char** i = physfsList; *i != NULL; i++) {
        char path[4096];
        snprintf(path, sizeof(path), "%s/%s", dirPath, *i);

        PHYSFS_Stat stat;
        if (PHYSFS_stat(path, &stat) == 0) continue;

        if (stat.filetype == PHYSFS_FILETYPE_DIRECTORY) {
            if (scanSubdirs) {
                count += EnumerateFilesFromPhysFS(path, filter, scanSubdirs, files);
            }
        } else if (stat.filetype == PHYSFS_FILETYPE_REGULAR) {
            if (filter == NULL || TextIsEqual(GetFileExtension(path), filter)) {
                if (files != NULL) {
                    files->paths[files->count] = (char*)MemAlloc(TextLength(path) + 1);
                    TextCopy(files->paths[files->count], path);
                    files->count++;
                }
                count++;
            }
        }
    }

    PHYSFS_freeList(physfsList);
    return count;
}

/**
 * Gets a list of files in the given directory in PhysFS, with optional extension filter and subdirectory scanning.
 *
 * Make sure to clear the loaded list by using UnloadDirectoryFiles().
 *
 * @param basePath The base directory to scan.
 * @param filter Optional file extension filter (e.g. ".png"). Pass NULL to include all files.
 * @param scanSubdirs When true, recursively scans subdirectories.
 *
 * @see UnloadDirectoryFiles()
 * @see LoadDirectoryFilesFromPhysFS()
 */
FilePathList LoadDirectoryFilesExFromPhysFS(const char* basePath, const char* filter, bool scanSubdirs) {
    FilePathList out = { 0 };

    if (!DirectoryExistsInPhysFS(basePath)) {
        TraceLog(LOG_WARNING, "PHYSFS: Can't get files from non-existent directory (%s)", basePath);
        return out;
    }

    int count = EnumerateFilesFromPhysFS(basePath, filter, scanSubdirs, NULL);

    out.capacity = (unsigned int)count;
    out.paths = count > 0 ? (char**)MemAlloc(count * sizeof(char*)) : NULL;

    if (count > 0) {
        EnumerateFilesFromPhysFS(basePath, filter, scanSubdirs, &out);
    }

    return out;
}

/**
 * Get file modification time (last write time) from a file in PhysFS.
 *
 * @param fileName The file to retrieve the mod time for.
 *
 * @return The modification time (last write time) of the given file. -1 on failure.
 *
 * @see GetFileModTime()
 */
long GetFileModTimeFromPhysFS(const char* fileName) {
    PHYSFS_Stat stat;
    if (PHYSFS_stat(fileName, &stat) == 0) {
        TraceLog(LOG_WARNING, "PHYSFS: Cannot get mod time of file (%s)", fileName);
        return -1;
    }

    return stat.modtime;
}

/**
 * Close the PhysFS virtual file system.
 *
 * @return True on success, false on failure.
 */
bool ClosePhysFS() {
    if (PHYSFS_deinit() == 0) {
        TracePhysFSError("ClosePhysFS() unsuccessful");
        return false;
    }
    TraceLog(LOG_DEBUG, "PHYSFS: Closed successfully");
    return true;
}

/**
 * Sets the raylib file saver/loader callbacks to use PhysFS.
 *
 * @see SetLoadFileDataCallback()
 * @see SetSaveFileDataCallback()
 * @see SetLoadFileTextCallback()
 * @see SetSaveFileTextCallback()
 */
void SetPhysFSCallbacks() {
    SetLoadFileDataCallback(LoadFileDataFromPhysFS);
    SetSaveFileDataCallback(SaveFileDataToPhysFS);
    SetLoadFileTextCallback(LoadFileTextFromPhysFS);
    SetSaveFileTextCallback(SaveFileTextToPhysFS);
}

/**
 * Get the user's configuration directory for the application.
 *
 * @param organization The name of your organization.
 * @param application The name of your application.
 *
 * @return string of user directory in platform-dependent notation.
 *         NULL if there's a problem (creating directory failed, etc)
 */
const char* GetPrefDirectory(const char *organization, const char *application) {
    const char* output = PHYSFS_getPrefDir(organization, application);
    if (output == 0) {
        TracePhysFSError("Failed to get pref directory");
        return 0;
    }
    TraceLog(LOG_DEBUG, "PHYSFS: Pref Directory: %s", output);
    return output;
}

#ifdef __cplusplus
}
#endif

#endif  // RAYLIB_PHYSFS_IMPLEMENTATION_ONCE
#endif  // RAYLIB_PHYSFS_IMPLEMENTATION
