/**********************************************************************************************
*
*   raylib-physfs - Integrate PhysFS with raylib, allowing to load images, audio and fonts from data archives.
*
*   Copyright 2021 Rob Loach (@RobLoach)
*
*   DEPENDENCIES:
*       raylib 4 https://www.raylib.com/
*       physfs https://www.icculus.org/physfs/
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

#include "raylib.h" // NOLINT

#ifdef __cplusplus
extern "C" {
#endif

#ifndef RAYLIB_PHYSFS_DEF
#ifdef RAYLIB_PHYSFS_STATIC
#define RAYLIB_PHYSFS_DEF static
#else
#define RAYLIB_PHYSFS_DEF extern
#endif
#endif

RAYLIB_PHYSFS_DEF bool InitPhysFS();                                              // Initialize the PhysFS file system
RAYLIB_PHYSFS_DEF bool ClosePhysFS();                                             // Close the PhysFS file system
RAYLIB_PHYSFS_DEF bool IsPhysFSReady();                                           // Check if PhysFS has been initialized successfully
RAYLIB_PHYSFS_DEF bool MountPhysFS(const char* newDir, const char* mountPoint);   // Mount the given directory or archive as a mount point
RAYLIB_PHYSFS_DEF bool MountPhysFSFromMemory(const unsigned char *fileData, int dataSize, const char* newDir, const char* mountPoint);  // Mount the given file data as a mount point
RAYLIB_PHYSFS_DEF bool UnmountPhysFS(const char* oldDir);                         // Unmounts the given directory
RAYLIB_PHYSFS_DEF bool FileExistsInPhysFS(const char* fileName);                  // Check if the given file exists in PhysFS
RAYLIB_PHYSFS_DEF bool DirectoryExistsInPhysFS(const char* dirPath);              // Check if the given directory exists in PhysFS
RAYLIB_PHYSFS_DEF unsigned char* LoadFileDataFromPhysFS(const char* fileName, unsigned int* bytesRead);  // Load a data buffer from PhysFS (memory should be freed)
RAYLIB_PHYSFS_DEF char* LoadFileTextFromPhysFS(const char* fileName);             // Load text from a file (memory should be freed)
RAYLIB_PHYSFS_DEF bool SetPhysFSWriteDirectory(const char* newDir);               // Set the base directory where PhysFS should write files to (defaults to the current working directory)
RAYLIB_PHYSFS_DEF bool SaveFileDataToPhysFS(const char* fileName, void* data, unsigned int bytesToWrite);  // Save the given file data in PhysFS
RAYLIB_PHYSFS_DEF bool SaveFileTextToPhysFS(const char* fileName, char* text);    // Save the given file text in PhysFS
RAYLIB_PHYSFS_DEF char** GetDirectoryFilesFromPhysFS(const char* dirPath, int* count);  // Get filenames in a directory path (memory should be freed)
RAYLIB_PHYSFS_DEF void ClearDirectoryFilesFromPhysFS(char** filesList);           // Clear directory files paths buffers (free memory)
RAYLIB_PHYSFS_DEF long GetFileModTimeFromPhysFS(const char* fileName);            // Get file modification time (last write time) from PhysFS
RAYLIB_PHYSFS_DEF Image LoadImageFromPhysFS(const char* fileName);                // Load an image from PhysFS
RAYLIB_PHYSFS_DEF Texture2D LoadTextureFromPhysFS(const char* fileName);          // Load a texture from PhysFS
RAYLIB_PHYSFS_DEF Wave LoadWaveFromPhysFS(const char* fileName);                  // Load wave data from PhysFS
RAYLIB_PHYSFS_DEF Music LoadMusicStreamFromPhysFS(const char* fileName);          // Load music data from PhysFS
RAYLIB_PHYSFS_DEF Font LoadFontFromPhysFS(const char* fileName, int fontSize, int *fontChars, int charsCount);  // Load a font from PhysFS
RAYLIB_PHYSFS_DEF Shader LoadShaderFromPhysFS(const char *vsFileName, const char *fsFileName);  // Load shader from PhysFS
RAYLIB_PHYSFS_DEF void SetPhysFSCallbacks();                                      // Set the raylib file loader/saver callbacks to use PhysFS

#ifdef __cplusplus
}
#endif

#endif  // INCLUDE_RAYLIB_PHYSFS_H_

#ifdef RAYLIB_PHYSFS_IMPLEMENTATION
#ifndef RAYLIB_PHYSFS_IMPLEMENTATION_ONCE
#define RAYLIB_PHYSFS_IMPLEMENTATION_ONCE

#include "physfs.h" // NOLINT

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
    int errorCode = PHYSFS_getLastErrorCode();
    if (errorCode == PHYSFS_ERR_OK) {
        TraceLog(LOG_DEBUG, TextFormat("PHYSFS: No error (%s)", detail));
    } else {
        const char* errorMessage = PHYSFS_getErrorByCode(errorCode);
        TraceLog(LOG_WARNING, TextFormat("PHYSFS: %s (%s)", errorMessage, detail));
    }
}

/**
 * Loads the given file as a byte array from PhysFS (read).
 *
 * @param fileName The file to load.
 * @param bytesRead An unsigned integer to save the bytes that were read.
 *
 * @return The file data as a pointer. Make sure to use UnloadFileData() when finished using the file data.
 *
 * @see UnloadFileData()
 */
RAYLIB_PHYSFS_DEF unsigned char* LoadFileDataFromPhysFS(const char* fileName, unsigned int* bytesRead) {
    if (!FileExistsInPhysFS(fileName)) {
        TraceLog(LOG_WARNING, TextFormat("PHYSFS: Tried to load unexisting file %s", fileName));
        return 0;
    }

    // Open up the file.
    void* handle = PHYSFS_openRead(fileName);
    if (handle == 0) {
        TracePhysFSError(fileName);
        return 0;
    }

    // Check to see how large the file is.
    int size = PHYSFS_fileLength(handle);
    if (size == -1) {
        PHYSFS_close(handle);
        TraceLog(LOG_WARNING, TextFormat("PHYSFS: Cannot determine size of file %s", fileName));
        return 0;
    }

    // Read the file, return if it's empty.
    void* buffer = MemAlloc(size);
    int read = PHYSFS_readBytes(handle, buffer, size);
    if (read <= 0) {
        MemFree(buffer);
        PHYSFS_close(handle);
        // Only report the error if PhysFS failed to read.
        if (read == -1) {
            TracePhysFSError(fileName);
        }
        return 0;
    }

    // Close the file handle, and return the bytes read and the buffer.
    PHYSFS_close(handle);
    *bytesRead = read;
    return buffer;
}

/**
 * Initialize the PhysFS virtual file system.
 *
 * @return True on success, false on failure.
 *
 * @see ClosePhysFS()
 */
RAYLIB_PHYSFS_DEF bool InitPhysFS() {
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
 * Check if PhysFS has been initialized successfully.
 *
 * @return True if PhysFS is initialized, false otherwise.
 *
 * @see InitPhysFS()
 */
RAYLIB_PHYSFS_DEF bool IsPhysFSReady() {
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
RAYLIB_PHYSFS_DEF bool MountPhysFS(const char* newDir, const char* mountPoint) {
    if (PHYSFS_mount(newDir, mountPoint, 1) == 0) {
        TracePhysFSError(mountPoint);
        return false;
    }

    TraceLog(LOG_DEBUG, "PHYSFS: Mounted %s at %s", newDir, mountPoint);
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
RAYLIB_PHYSFS_DEF bool MountPhysFSFromMemory(const unsigned char *fileData, int dataSize, const char* newDir, const char* mountPoint) {
    if (dataSize <= 0) {
        TraceLog(LOG_WARNING, "PHYSFS: Cannot mount a data size of 0");
        return false;
    }

    if (PHYSFS_mountMemory(fileData, dataSize, 0, newDir, mountPoint, 1) == 0) {
        TracePhysFSError(TextFormat("Failed to mount %s at %s", newDir, mountPoint));
        return false;
    }

    TraceLog(LOG_DEBUG, "PHYSFS: Mounted memory %s at %s", newDir, mountPoint);
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
RAYLIB_PHYSFS_DEF bool UnmountPhysFS(const char* oldDir) {
    if (PHYSFS_unmount(oldDir) == 0) {
        TraceLog(LOG_WARNING, "PHYSFS: Failed to unmount directory %s", oldDir);
        return false;
    }

    TraceLog(LOG_DEBUG, "PHYSFS: Unmounted %s", oldDir);
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
RAYLIB_PHYSFS_DEF bool FileExistsInPhysFS(const char* fileName) {
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
RAYLIB_PHYSFS_DEF bool DirectoryExistsInPhysFS(const char* dirPath) {
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
RAYLIB_PHYSFS_DEF Image LoadImageFromPhysFS(const char* fileName) {
    unsigned int bytesRead;
    unsigned char* fileData = LoadFileDataFromPhysFS(fileName, &bytesRead);
    if (fileData == 0) {
        struct Image output;
        output.data = 0;
        output.width = 0;
        output.height = 0;
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
RAYLIB_PHYSFS_DEF Texture2D LoadTextureFromPhysFS(const char* fileName) {
    Image image = LoadImageFromPhysFS(fileName);
    if (image.data == 0) {
        Texture2D output = { 0 };
        output.id = 0;
        output.format = 0;
        output.width = 0;
        output.height = 0;
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
RAYLIB_PHYSFS_DEF char* LoadFileTextFromPhysFS(const char *fileName) {
    unsigned int bytesRead;
    return (char*)LoadFileDataFromPhysFS(fileName, &bytesRead);
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
RAYLIB_PHYSFS_DEF Wave LoadWaveFromPhysFS(const char* fileName) {
    unsigned int bytesRead;
    unsigned char* fileData = LoadFileDataFromPhysFS(fileName, &bytesRead);
    if (fileData == 0) {
        struct Wave output;
        output.data = 0;
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
RAYLIB_PHYSFS_DEF Music LoadMusicStreamFromPhysFS(const char* fileName) {
    unsigned int bytesRead;
    unsigned char* fileData = LoadFileDataFromPhysFS(fileName, &bytesRead);
    if (fileData == 0) {
        struct Music output;
        output.ctxData = 0;
        output.stream.buffer = 0;
        return output;
    }

    // Load from the memory.
    const char* extension = GetFileExtension(fileName);
    Music music = LoadMusicStreamFromMemory(extension, fileData, bytesRead);
    UnloadFileData(fileData);
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
RAYLIB_PHYSFS_DEF Font LoadFontFromPhysFS(const char* fileName, int fontSize, int *fontChars, int charsCount) {
    unsigned int bytesRead;
    unsigned char* fileData = LoadFileDataFromPhysFS(fileName, &bytesRead);
    if (fileData == 0) {
        struct Font output;
        output.baseSize = 0;
        output.glyphCount = 0;
        output.glyphPadding = 0;
        output.glyphs = 0;
        output.recs = 0;
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
RAYLIB_PHYSFS_DEF Shader LoadShaderFromPhysFS(const char *vsFileName, const char *fsFileName) {
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
RAYLIB_PHYSFS_DEF bool SetPhysFSWriteDirectory(const char* newDir) {
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
RAYLIB_PHYSFS_DEF bool SaveFileDataToPhysFS(const char* fileName, void* data, unsigned int bytesToWrite) {
    // Protect against empty writes.
    if (bytesToWrite == 0) {
        return true;
    }

    // Open the file.
    void* handle = PHYSFS_openWrite(fileName);
    if (handle == 0) {
        TracePhysFSError(fileName);
        return false;
    }

    // Write the data to the file handle.
    if (PHYSFS_writeBytes(handle, data, bytesToWrite) < 0) {
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
RAYLIB_PHYSFS_DEF bool SaveFileTextToPhysFS(const char* fileName, char* text) {
    return SaveFileDataToPhysFS(fileName, text, TextLength(text));
}

/**
 * Gets a list of files in the given directory in PhysFS.
 *
 * Make sure to clear the loaded list by using ClearDirectoryFilesFromPhysFS().
 *
 * @see ClearDirectoryFilesFromPhysFS()
 */
RAYLIB_PHYSFS_DEF char** GetDirectoryFilesFromPhysFS(const char* dirPath, int *count) {
    // Make sure the directory exists.
    if (!DirectoryExistsInPhysFS(dirPath)) {
        TraceLog(LOG_WARNING, "PHYSFS: Can't get files from non-existant directory (%s)", dirPath);
        return 0;
    }

    // Load the list of files from PhysFS.
    char** list = PHYSFS_enumerateFiles(dirPath);

    // Find out how many files there were.
    int number = 0;
    for (char** i = list; *i != 0; i++) {
        number++;
    }

    // Output the count and the list.
    *count = number;
    return list;
}

/**
 * Clears the loaded list of directory files from GetDirectoryFilesFromPhysFS().
 *
 * @param filesList The list of files to clear that was provided from GetDirectoryFilesFromPhysFS().
 *
 * @see GetDirectoryFilesFromPhysFS()
 */
RAYLIB_PHYSFS_DEF void ClearDirectoryFilesFromPhysFS(char** filesList) {
    PHYSFS_freeList(filesList);
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
RAYLIB_PHYSFS_DEF long GetFileModTimeFromPhysFS(const char* fileName) {
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
RAYLIB_PHYSFS_DEF bool ClosePhysFS() {
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
RAYLIB_PHYSFS_DEF void SetPhysFSCallbacks() {
    SetLoadFileDataCallback(LoadFileDataFromPhysFS);
    SetSaveFileDataCallback(SaveFileDataToPhysFS);
    SetLoadFileTextCallback(LoadFileTextFromPhysFS);
    SetSaveFileTextCallback(SaveFileTextToPhysFS);
}

#ifdef __cplusplus
}
#endif

#endif  // RAYLIB_PHYSFS_IMPLEMENTATION_ONCE
#endif  // RAYLIB_PHYSFS_IMPLEMENTATION
