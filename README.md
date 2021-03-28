# raylib-physfs

Integrate the virtual file system [PhysicsFS](https://icculus.org/physfs/) with [raylib](https://www.raylib.com/), allowing to load images, audio and fonts from data archives.

## Features

- Load the following data from archives
    - Font with `LoadFontFromPhysFS()`
    - Music with `LoadMusicStreamFromPhysFS()`
    - Wave with `LoadWaveFromPhysFS()`
    - Image with `LoadImageFromPhysFS()`
    - Text with `LoadFileTextFromPhysFS()`
- Enumerate across multiple archives and mount paths
- Check if directories and files exist
- Register the file loading API callbacks with PhysFS

## Usage

This is a single-file header. To use it, define `RAYLIB_PHYSFS_IMPLEMENTATION` in one `.c` source files before including [`raylib-physfs.h`](include/raylib-physfs.h). You will also have to link the PhysFS and raylib dependencies.

### Example

``` c
#define RAYLIB_PHYSFS_IMPLEMENTATION
#include "raylib-physfs.h"

int main() {
    // Initiatize the file system.
    InitPhysFS();

    // Mount a directory or archive.
    MountPhysFS("assets.zip", "assets");

    // Load an image through PhysFS.
    Image dog = LoadImageFromPhysFS("assets/dog.png");

    // Close the file system.
    ClosePhysFS();
}
```

### Cheatsheet

``` c
bool InitPhysFS();                                              // Initialize the PhysFS file system
bool ClosePhysFS();                                             // Close the PhysFS file system
bool MountPhysFS(const char* newDir, const char* mountPoint);   // Mount the given directory at a mount point
bool UnmountPhysFS(const char* oldDir);                         // Unmounts the given directory
bool FileExistsFromPhysFS(const char* fileName);                // Check if the given file exists in PhysFS
bool DirectoryExistsFromPhysFS(const char* dirPath);            // Check if the given directory exists in PhysFS
unsigned char* LoadFileDataFromPhysFS(const char* fileName, unsigned int* bytesRead); // Load a data buffer from PhysFS (memory should be freed)
char* LoadFileTextFromPhysFS(const char* fileName);             // Load text from a file (memory should be freed)
void SaveFileDataInPhysFS(const char* fileName, void* data, unsigned int bytesToWrite); // Save the given file data in PhysFS
void SaveFileTextInPhysFS(const char* fileName, char* text);    // Save the given file text in PhysFS
void SetPhysFSDataCallbacks();                                  // Register all the PhysFS load/save file callbacks
char** GetDirectoryFilesFromPhysFS(const char* dirPath, int* count); // Get filenames in a directory path (memory should be freed)
void ClearDirectoryFilesFromPhysFS(char** files);               // Clear directory files paths buffers (free memory)
Image LoadImageFromPhysFS(const char* file);                    // Load an image from PhysFS
Wave LoadWaveFromPhysFS(const char* fileName);                  // Load wave data from PhysFS
Music LoadMusicStreamFromPhysFS(const char* fileName);          // Load music data from PhysFS
Font LoadFontFromPhysFS(const char* fileName, int fontSize, int *fontChars, int charsCount); // Load a font from PhysFS
```

## Development

To build the examples locally, use [cmake](https://cmake.org/).

``` bash
git submodule update --init
mkdir build
cd build
cmake ..
make
cd examples
./textures_image_loading
```

## License

raylib-physfs is licensed under an unmodified zlib/libpng license, which is an OSI-certified, BSD-like license that allows static linking with closed source software. Check [LICENSE](LICENSE) for further details.
