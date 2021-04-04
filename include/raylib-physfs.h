/**********************************************************************************************
*
*   raylib-physfs - Integrate PhysFS with raylib, allowing to load images, audio and fonts from data archives.
*
*   Copyright 2021 Rob Loach (@RobLoach)
*
*   DEPENDENCIES:
*       raylib https://www.raylib.com/
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

bool InitPhysFS();                                             // Initialize the PhysFS file system
bool ClosePhysFS();                                            // Close the PhysFS file system
bool IsPhysFSReady();                                          // Check if PhysFS has been initialized successfully
bool MountPhysFS(const char* newDir, const char* mountPoint);  // Mount the given directory at a mount point
bool MountPhysFSFromMemory(const unsigned char *fileData, int dataSize, const char* newDir, const char* mountPoint);  // Mount the given file data as a mount point.
bool UnmountPhysFS(const char* oldDir);                        // Unmounts the given directory
bool FileExistsInPhysFS(const char* fileName);                 // Check if the given file exists in PhysFS
bool DirectoryExistsInPhysFS(const char* dirPath);             // Check if the given directory exists in PhysFS
unsigned char* LoadFileDataFromPhysFS(const char* fileName, unsigned int* bytesRead);  // Load a data buffer from PhysFS (memory should be freed)
char* LoadFileTextFromPhysFS(const char* fileName);            // Load text from a file (memory should be freed)
bool SetPhysFSWriteDirectory(const char* newDir);              // Set the base directory where PhysFS should write files to.
bool SaveFileDataToPhysFS(const char* fileName, void* data, unsigned int bytesToWrite);  // Save the given file data in PhysFS
bool SaveFileTextToPhysFS(const char* fileName, char* text);   // Save the given file text in PhysFS
char** GetDirectoryFilesFromPhysFS(const char* dirPath, int* count); // Get filenames in a directory path (memory should be freed)
void ClearDirectoryFilesFromPhysFS(char** files);              // Clear directory files paths buffers (free memory)
long GetFileModTimeFromPhysFS(const char* fileName);           // Get file modification time (last write time) from PhysFS.
Image LoadImageFromPhysFS(const char* file);                   // Load an image from PhysFS
Wave LoadWaveFromPhysFS(const char* fileName);                 // Load wave data from PhysFS
Music LoadMusicStreamFromPhysFS(const char* fileName);         // Load music data from PhysFS
Font LoadFontFromPhysFS(const char* fileName, int fontSize, int *fontChars, int charsCount);  // Load a font from PhysFS

#ifdef RAYLIB_PHYSFS_IMPLEMENTATION
#ifndef RAYLIB_PHYSFS_IMPLEMENTATION_ONCE
#define RAYLIB_PHYSFS_IMPLEMENTATION_ONCE

#include "physfs.h" // NOLINT

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
unsigned char* LoadFileDataFromPhysFS(const char* fileName, unsigned int* bytesRead) {
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

    // Read the file.
    void* buffer = MemAlloc(size);
    int read = PHYSFS_readBytes(handle, buffer, size);
    if (read == -1) {
        MemFree(buffer);
        PHYSFS_close(handle);
        TracePhysFSError(fileName);
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
bool MountPhysFSFromMemory(const unsigned char *fileData, int dataSize, const char* newDir, const char* mountPoint) {
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
bool UnmountPhysFS(const char* oldDir) {
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
 * Load image from PhysFS.
 *
 * @param fileName The filename to load from the search paths.
 *
 * @return The loaded image on success. An empty Image otherwise.
 */
Image LoadImageFromPhysFS(const char* fileName) {
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
 * Load text data from file (read). Make sure to call UnloadFileText() when done.
 *
 * @param fileName The file name to load from the PhysFS mount paths.
 *
 * @return A '\0' terminated string.
 *
 * @see UnloadFileText()
 */
char* LoadFileTextFromPhysFS(const char *fileName) {
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
Wave LoadWaveFromPhysFS(const char* fileName) {
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
Music LoadMusicStreamFromPhysFS(const char* fileName) {
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
Font LoadFontFromPhysFS(const char* fileName, int fontSize, int *fontChars, int charsCount) {
    unsigned int bytesRead;
    unsigned char* fileData = LoadFileDataFromPhysFS(fileName, &bytesRead);
    if (fileData == 0) {
        struct Font output;
        output.baseSize = 0;
        output.chars = 0;
        output.charsCount = 0;
        output.charsPadding = 0;
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
bool SaveFileDataToPhysFS(const char* fileName, void* data, unsigned int bytesToWrite) {
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
bool SaveFileTextToPhysFS(const char* fileName, char* text) {
    return SaveFileDataToPhysFS(fileName, text, TextLength(text));
}

/**
 * Gets a list of files in the given directory in PhysFS.
 *
 * Make sure to clear the loaded list by using ClearDirectoryFilesFromPhysFS().
 *
 * @see ClearDirectoryFilesFromPhysFS()
 */
char** GetDirectoryFilesFromPhysFS(const char* dirPath, int *count) {
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
 * @see GetDirectoryFilesFromPhysFS()
 */
void ClearDirectoryFilesFromPhysFS(char** files) {
    PHYSFS_freeList(files);
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

#endif  // RAYLIB_PHYSFS_IMPLEMENTATION_ONCE
#endif  // RAYLIB_PHYSFS_IMPLEMENTATION

#ifdef __cplusplus
}
#endif

#endif  // INCLUDE_RAYLIB_PHYSFS_H_
