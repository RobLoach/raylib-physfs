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

    // LoadDirectoryFilesFromPhysFSEx()
    {
        // Build the nested tests
        AssertEqual(MakeDirectory("resources/nested"), 0);
        AssertEqual(MakeDirectory("resources/nested/deeper"), 0);
        Assert(SaveFileText("resources/nested/nested.txt", "Nested file"));
        Assert(SaveFileText("resources/nested/deeper/deeper.txt", "Deeper file"));

        // Load all the text files, without subdirectories
        FilePathList flatFiles = LoadDirectoryFilesFromPhysFSEx("assets", ".txt", false);
        bool rootTextFound = false;
        bool nestedTextFound = false;
        for (unsigned int i = 0; i < flatFiles.count; i++) {
            if (TextIsEqual(flatFiles.paths[i], "assets/text.txt")) {
                rootTextFound = true;
            }
            if (TextIsEqual(flatFiles.paths[i], "assets/nested/nested.txt")) {
                nestedTextFound = true;
            }
        }
        UnloadDirectoryFiles(flatFiles);
        Assert(rootTextFound, "LoadDirectoryFilesFromPhysFSEx() did not include assets/text.txt");
        AssertNot(nestedTextFound, "LoadDirectoryFilesFromPhysFSEx() should not recurse when scanSubdirs is false");

        // Load all the files and directories, with subdirectories.
        FilePathList recursiveFiles = LoadDirectoryFilesFromPhysFSEx("assets", ".txt;DIR", true);
        bool nestedDirectoryFound = false;
        bool nestedFileFound = false;
        bool deeperFileFound = false;
        for (unsigned int i = 0; i < recursiveFiles.count; i++) {
            if (TextIsEqual(recursiveFiles.paths[i], "assets/nested")) {
                nestedDirectoryFound = true;
            }
            if (TextIsEqual(recursiveFiles.paths[i], "assets/nested/nested.txt")) {
                nestedFileFound = true;
            }
            if (TextIsEqual(recursiveFiles.paths[i], "assets/nested/deeper/deeper.txt")) {
                deeperFileFound = true;
            }
        }
        UnloadDirectoryFiles(recursiveFiles);
        Assert(nestedDirectoryFound, "LoadDirectoryFilesFromPhysFSEx() did not include directories when requested");
        Assert(nestedFileFound, "LoadDirectoryFilesFromPhysFSEx() did not include nested files");
        Assert(deeperFileFound, "LoadDirectoryFilesFromPhysFSEx() did not include deeper nested files");
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
