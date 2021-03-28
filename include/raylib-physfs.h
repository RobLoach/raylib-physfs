/**********************************************************************************************
*
*   raylib-physfs - Integrate PhysFS with raylib, allowing to load images, audio and fonts from data archives.
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
*   Copyright (c) 2021 Rob Loach (@RobLoach)
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

#ifndef RAYLIB_PHYSFS_H__
#define RAYLIB_PHYSFS_H__

#include "raylib.h"

#ifdef __cplusplus
extern "C" {
#endif

bool InitPhysFS();                                              // Initialize the PhysFS file system
bool ClosePhysFS();                                             // Close the PhysFS file system
bool MountPhysFS(const char* newDir, const char* mountPoint);   // Mount the given directory at a mount point
bool UnmountPhysFS(const char* oldDir);                         // Unmounts the given directory
bool FileExistsInPhysFS(const char* fileName);                  // Check if the given file exists in PhysFS
bool DirectoryExistsInPhysFS(const char* dirPath);              // Check if the given directory exists in PhysFS
unsigned char* LoadFileDataFromPhysFS(const char* fileName, unsigned int* bytesRead); // Load a data buffer from PhysFS (memory should be freed)
char* LoadFileTextFromPhysFS(const char* fileName);             // Load text from a file (memory should be freed)
void SaveFileDataToPhysFS(const char* fileName, void* data, unsigned int bytesToWrite); // Save the given file data in PhysFS
void SaveFileTextToPhysFS(const char* fileName, char* text);    // Save the given file text in PhysFS
void SetPhysFSDataCallbacks();                                  // Register all the PhysFS load/save file callbacks
char** GetDirectoryFilesFromPhysFS(const char* dirPath, int* count); // Get filenames in a directory path (memory should be freed)
void ClearDirectoryFilesFromPhysFS(char** files);               // Clear directory files paths buffers (free memory)
Image LoadImageFromPhysFS(const char* file);                    // Load an image from PhysFS
Wave LoadWaveFromPhysFS(const char* fileName);                  // Load wave data from PhysFS
Music LoadMusicStreamFromPhysFS(const char* fileName);          // Load music data from PhysFS
Font LoadFontFromPhysFS(const char* fileName, int fontSize, int *fontChars, int charsCount); // Load a font from PhysFS

#ifdef RAYLIB_PHYSFS_IMPLEMENTATION

#include "physfs.h"

/**
 * Reports the last PhysFS error to the raylib TraceLog.
 */
void PhysFSReportError(const char* detail) {
    int errorCode = PHYSFS_getLastErrorCode();
    if (errorCode == PHYSFS_ERR_OK) {
        TraceLog(LOG_DEBUG, TextFormat("PHYSFS: No error reported - %s", detail));
    } else {
        TraceLog(LOG_ERROR, TextFormat("PHYSFS: %s %s", PHYSFS_getErrorByCode(errorCode), detail));
    }
}

unsigned char* LoadFileDataFromPhysFS(const char* fileName, unsigned int* bytesRead) {
    if (!FileExistsInPhysFS(fileName)) {
        TraceLog(LOG_ERROR, TextFormat("PHYSFS: The file to load doesn't exist: %s.", fileName));
        return 0;
    }

    // Open up the file.
    void* handle = PHYSFS_openRead(fileName);
    if (handle == 0) {
        PhysFSReportError(fileName);
        return 0;
    }

    // Check to see how large the file is.
    int size = PHYSFS_fileLength(handle);
    if (size < 0) {
        TraceLog(LOG_ERROR, TextFormat("PHYSFS: Cannot determine size of file %s.", fileName));
        return 0;
    }

    // Read the file.
    void* buffer = MemAlloc(size);
    int read = PHYSFS_readBytes(handle, buffer, size);
    if (read <= 0) {
        PHYSFS_close(handle);
        MemFree(buffer);
        PhysFSReportError(fileName);
        return 0;
    }

    PHYSFS_close(handle);
    *bytesRead = read;
    return buffer;
}

bool InitPhysFS() {
    if (PHYSFS_init(0) == 0) {
        PhysFSReportError("Occured when initiatlizing");
        return false;
    }
    TraceLog(LOG_DEBUG, "PHYSFS: Initialized PhysFS");
    return true;
}

bool MountPhysFS(const char* newDir, const char* mountPoint) {
    if (PHYSFS_mount(newDir, mountPoint, 1) == 0) {
        PhysFSReportError(mountPoint);
        return false;
    }
    TraceLog(LOG_DEBUG, "PHYSFS: Mounted %s at %s", newDir, mountPoint);
    return true;
}

bool UnmountPhysFS(const char* oldDir) {
    TraceLog(LOG_DEBUG, "PHYSFS: Unmounting %s", oldDir);
    if (PHYSFS_unmount(oldDir) == 0) {
        return false;
    }
    return true;
}

bool FileExistsInPhysFS(const char* fileName) {
    return PHYSFS_exists(fileName) != 0;
}

bool DirectoryExistsInPhysFS(const char* dirPath) {
    PHYSFS_Stat stat;
    if (PHYSFS_stat(dirPath, &stat) == 0) {
        return false;
    }
    return stat.filetype == PHYSFS_FILETYPE_DIRECTORY;
}

Image LoadImageFromPhysFS(const char* fileName) {
    unsigned int bytesRead;
    const unsigned char* fileData = LoadFileDataFromPhysFS(fileName, &bytesRead);
    if (fileData == 0) {
        struct Image output;
        return output;
    }

    // Load from the memory.
    const char* extension = GetFileExtension(fileName);
    Image image = LoadImageFromMemory(extension, fileData, bytesRead);
    MemFree((void*)fileData);
    return image;
}

char *LoadFileTextFromPhysFS(const char *fileName) {
    unsigned int bytesRead;
    return (char*)LoadFileDataFromPhysFS(fileName, &bytesRead);
}

Wave LoadWaveFromPhysFS(const char* fileName) {
    unsigned int bytesRead;
    unsigned char* fileData = LoadFileDataFromPhysFS(fileName, &bytesRead);
    if (fileData == 0) {
        struct Wave output;
        return output;
    }

    // Load from the memory.
    const char* extension = GetFileExtension(fileName);
    Wave wave = LoadWaveFromMemory(extension, fileData, bytesRead);
    MemFree((void*)fileData);
    return wave;
}

Music LoadMusicStreamFromPhysFS(const char* fileName) {
    unsigned int bytesRead;
    unsigned char* fileData = LoadFileDataFromPhysFS(fileName, &bytesRead);
    if (fileData == 0) {
        struct Music output;
        return output;
    }

    // Load from the memory.
    const char* extension = GetFileExtension(fileName);
    Music music = LoadMusicStreamFromMemory(extension, fileData, bytesRead);
    MemFree((void*)fileData);
    return music;
}

Font LoadFontFromPhysFS(const char* fileName, int fontSize, int *fontChars, int charsCount) {
    unsigned int bytesRead;
    unsigned char* fileData = LoadFileDataFromPhysFS(fileName, &bytesRead);
    if (fileData == 0) {
        struct Font output;
        return output;
    }

    // Load from the memory.
    const char* extension = GetFileExtension(fileName);
    Font font = LoadFontFromMemory(extension, fileData, bytesRead, fontSize, fontChars, charsCount);
    MemFree((void*)fileData);
    return font;
}

void SaveFileDataToPhysFS(const char* fileName, void* data, unsigned int bytesToWrite) {
    void* handle = PHYSFS_openWrite(fileName);
    if (handle == 0) {
        PhysFSReportError(fileName);
        return;
    }

    if (PHYSFS_writeBytes(handle, data, bytesToWrite) < 0) {
        PHYSFS_close(handle);
        PhysFSReportError(fileName);
    } else {
        PHYSFS_close(handle);
    }
}

void SaveFileTextToPhysFS(const char* fileName, char* text) {
    SaveFileDataToPhysFS(fileName, text, TextLength(text));
}

char** GetDirectoryFilesFromPhysFS(const char* dirPath, int *count) {
    if (!DirectoryExistsInPhysFS(dirPath)) {
        TraceLog(LOG_ERROR, "PHYSFS: Can't get directory files from non-existant directory %s", dirPath);
        return 0;
    }
    char** list = PHYSFS_enumerateFiles(dirPath);
    int number = 0;
    for (char** i = list; *i != 0; i++) {
        number++;
    }
    *count = number;
    return list;
}

void ClearDirectoryFilesFromPhysFS(char** files) {
    PHYSFS_freeList(files);
}

void SetPhysFSDataCallbacks() {
    SetLoadFileDataCallback(LoadFileDataFromPhysFS);
    SetSaveFileDataCallback(SaveFileDataToPhysFS);
    SetLoadFileTextCallback(LoadFileTextFromPhysFS);
    SetSaveFileTextCallback(SaveFileTextToPhysFS);
    TraceLog(LOG_DEBUG, "PHYSFS: Set all PhysFS load/save callbacks");
}

bool ClosePhysFS() {
    if (PHYSFS_deinit() == 0) {
        PhysFSReportError("ClosePhysFS() unsuccessful");
        return false;
    }
    TraceLog(LOG_DEBUG, "PHYSFS: Closed successfully");
    return true;
}

#endif

#ifdef __cplusplus
}
#endif

#endif
