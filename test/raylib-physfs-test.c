#include "raylib.h"
#include "raylib-assert.h"

#define RAYLIB_PHYSFS_IMPLEMENTATION
#include "raylib-physfs.h"

int main(int argc, char *argv[]) {
    // Initialization
    SetTraceLogLevel(LOG_ALL);
    TraceLog(LOG_INFO, "================================");
    TraceLog(LOG_INFO, "raylib-physfs-test");
    TraceLog(LOG_INFO, "================================");

    // Make sure we're running in the correct directory.
    if (argc > 0) {
        const char* dir = GetDirectoryPath(argv[0]);
        Assert(ChangeDirectory(dir) == true);
    }

    // IsPhysFSReady()
    AssertNot(IsPhysFSReady());

    // InitPhysFS()
    Assert(InitPhysFS());
    Assert(IsPhysFSReady());

    // MountPhysFS()
    Assert(MountPhysFS("resources", "assets"));

    // FileExistsInPhysFS()
    Assert(FileExistsInPhysFS("assets/text.txt"));
    AssertNot(FileExistsInPhysFS("MissingFile.txt"));

    // DirectoryExistsInPhysFS()
    Assert(DirectoryExistsInPhysFS("assets"));
    AssertNot(DirectoryExistsInPhysFS("MissingDirectory"));

    // LoadFileDataFromPhysFS()
    {
        int bytesRead = 0;
        unsigned char* fileData = LoadFileDataFromPhysFS("assets/text.txt", &bytesRead);
        Assert(fileData);
        Assert(bytesRead > 0);
        UnloadFileData(fileData);

        unsigned char* missingFileData = LoadFileDataFromPhysFS("MissingFile.txt", &bytesRead);
        AssertEqual(missingFileData, NULL);
    }

    // SaveFileDataToPhysFS()
    {
        Assert(SaveFileDataToPhysFS("resources/SaveFileDataToPhysFS.txt", "Hello", 5));
        int bytesRead = 0;
        unsigned char* output = LoadFileData("resources/SaveFileDataToPhysFS.txt", &bytesRead);
        AssertEqual(bytesRead, 5);
        Assert(TextIsEqual(TextSubtext((const char*)output, 0, 5), "Hello"));
        UnloadFileData(output);
    }

    // SaveFileTextToPhysFS()
    {
        Assert(SaveFileTextToPhysFS("resources/SaveFileTextToPhysFS.txt", "Hello World"));
        char* fileText = LoadFileText("resources/SaveFileTextToPhysFS.txt");
        Assert(TextIsEqual(fileText, "Hello World"));
        UnloadFileText(fileText);
    }

    // LoadDirectoryFilesFromPhysFS()
    {
        FilePathList files = LoadDirectoryFilesFromPhysFS("assets");
        bool textFileFound = false;
        Assert(files.count > 4);
        TraceLog(LOG_INFO, "LoadDirectoryFilesFromPhysFS: Files in assets: %i", files.count);
        for (unsigned int i = 0; i < files.count; i++) {
            if (TextIsEqual(GetFileName(files.paths[i]), "text.txt")) {
                textFileFound = true;
            }
        }
        UnloadDirectoryFiles(files);
        Assert(textFileFound, "LoadDirectoryFilesFromPhysFS() could not find text.txt");
    }

    // LoadDirectoryFilesExFromPhysFS()
    {
        // Scan all files recursively (should include subdir/text2.txt)
        FilePathList allFiles = LoadDirectoryFilesExFromPhysFS("assets", NULL, true);
        Assert(allFiles.count > 4, "LoadDirectoryFilesExFromPhysFS() should find files recursively");
        bool subdirFileFound = false;
        for (unsigned int i = 0; i < allFiles.count; i++) {
            if (TextIsEqual(GetFileName(allFiles.paths[i]), "text2.txt")) {
                subdirFileFound = true;
            }
        }
        UnloadDirectoryFiles(allFiles);
        Assert(subdirFileFound, "LoadDirectoryFilesExFromPhysFS() should find text2.txt in subdir");

        // Filter by extension
        FilePathList pngFiles = LoadDirectoryFilesExFromPhysFS("assets", ".png", true);
        Assert(pngFiles.count >= 1, "LoadDirectoryFilesExFromPhysFS() should find .png files");
        for (unsigned int i = 0; i < pngFiles.count; i++) {
            Assert(TextIsEqual(GetFileExtension(pngFiles.paths[i]), ".png"), "LoadDirectoryFilesExFromPhysFS() filter should only return .png files");
        }
        UnloadDirectoryFiles(pngFiles);

        // No subdirs scan should match LoadDirectoryFilesFromPhysFS behaviour
        FilePathList flatFiles = LoadDirectoryFilesExFromPhysFS("assets", NULL, false);
        FilePathList flatRef = LoadDirectoryFilesFromPhysFS("assets");
        AssertEqual(flatFiles.count, flatRef.count);
        UnloadDirectoryFiles(flatFiles);
        UnloadDirectoryFiles(flatRef);

        // Missing directory returns empty list
        FilePathList missing = LoadDirectoryFilesExFromPhysFS("MissingDirectory", NULL, true);
        AssertEqual(missing.count, 0);
    }

    // LoadFileTextFromPhysFS()
    {
        char* fileText = LoadFileTextFromPhysFS("assets/text.txt");
        AssertNotEqual(fileText, 0);
        Assert(TextIsEqual(TextSubtext(fileText, 7, 5), "World")); // Hello, World!
        UnloadFileText(fileText);

        char* missingText = LoadFileTextFromPhysFS("MissingText.txt");
        AssertEqual(missingText, 0);
    }

    // LoadImageFromPhysFS()
    {
        Image image = LoadImageFromPhysFS("assets/image.png");
        AssertImage(image);
        Assert(image.width > 100);

        Image loadedImage = LoadImage("resources/image.png");
        AssertImage(loadedImage);
        AssertImageSame(image, loadedImage);
        UnloadImage(image);
        UnloadImage(loadedImage);

        Image missingImage = LoadImageFromPhysFS("MissingFile.png");
        AssertEqual(missingImage.data, 0);
    }

    // LoadWaveFromPhysFS()
    {
        Wave wave = LoadWaveFromPhysFS("assets/sound.wav");
        AssertNotEqual(wave.data, 0);
        UnloadWave(wave);

        Wave missingWave = LoadWaveFromPhysFS("MissingFile.wav");
        AssertEqual(missingWave.data, 0);
    }

    // LoadShaderFromPhysFS()
    {
        Shader missingShader = LoadShaderFromPhysFS("MissingFile.txt", "MissingFile.txt");
        AssertEqual(missingShader.locs, 0);
    }

    // GetFileModTimeFromPhysFS()
    Assert(GetFileModTimeFromPhysFS("assets/text.txt") > 1000);
    AssertEqual(GetFileModTimeFromPhysFS("MissingFile.txt"), -1);

    // SetPhysFSWriteDirectory()
    Assert(SetPhysFSWriteDirectory("resources"));
    AssertNot(SetPhysFSWriteDirectory("MissingDirectory"));

    // UnmountPhysFS()
    Assert(UnmountPhysFS("resources"));
    AssertNot(UnmountPhysFS("MissingDirectory"));

    // SetPhysFSCallbacks()
    SetPhysFSCallbacks();

    // GetPrefDirectory
    const char* perfDir = GetPrefDirectory("RobLoach", "raylib-physfs-test");
    AssertNotEqual(perfDir, 0);

    // ClosePhysFS()
    Assert(ClosePhysFS());

    TraceLog(LOG_INFO, "================================");
    TraceLog(LOG_INFO, "raylib-physfs-test successful");
    TraceLog(LOG_INFO, "================================");

    return 0;
}
