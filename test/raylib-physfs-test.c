#include <assert.h>
#include "raylib.h"

#define RAYLIB_PHYSFS_IMPLEMENTATION
#include "raylib-physfs.h"

int main(int argc, char *argv[]) {
    // Initialization
    SetTraceLogLevel(LOG_ALL);
    TraceLog(LOG_INFO, "================================");
    TraceLog(LOG_INFO, "raylib-physfs-test");
    TraceLog(LOG_INFO, "================================");

    // Make sure we're running in the correct directory.
    const char* dir = GetDirectoryPath(argv[0]);
    assert(ChangeDirectory(dir) == true);

    // IsPhysFSReady()
    assert(!IsPhysFSReady());

    // InitPhysFS()
    assert(InitPhysFS());
    assert(IsPhysFSReady());

    // MountPhysFS()
    assert(MountPhysFS("resources", "assets"));

    // FileExistsInPhysFS()
    assert(FileExistsInPhysFS("assets/text.txt"));
    assert(!FileExistsInPhysFS("MissingFile.txt"));

    // DirectoryExistsInPhysFS()
    assert(DirectoryExistsInPhysFS("assets"));
    assert(!DirectoryExistsInPhysFS("MissingDirectory"));

    // LoadFileDataFromPhysFS()
    {
        unsigned int bytesRead = 0;
        unsigned char* fileData = LoadFileDataFromPhysFS("assets/text.txt", &bytesRead);
        assert(fileData);
        assert(bytesRead > 0);
        UnloadFileData(fileData);

        unsigned char* missingFileData = LoadFileDataFromPhysFS("MissingFile.txt", bytesRead);
        assert(missingFileData == 0);
    }

    // SaveFileDataToPhysFS()
    {
        assert(SaveFileDataToPhysFS("resources/SaveFileDataToPhysFS.txt", "Hello", 5));
        unsigned int bytesRead = 0;
        unsigned char* fileData = LoadFileData("resources/SaveFileDataToPhysFS.txt", &bytesRead);
        assert(bytesRead == 5);
    }

    // SaveFileTextToPhysFS()
    {
        assert(SaveFileTextToPhysFS("resources/SaveFileTextToPhysFS.txt", "Hello World"));
        char* fileText = LoadFileText("resources/SaveFileTextToPhysFS.txt");
        assert(TextIsEqual(fileText, "Hello World"));
        UnloadFileText(fileText);
    }

    // GetDirectoryFilesFromPhysFS()
    {
        int count = 0;
        char** files = GetDirectoryFilesFromPhysFS("assets", &count);
        bool textFileFound = false;
        assert(count > 4);
        for (int i = 0; i < count; i++) {
            if (TextIsEqual(GetFileName(files[i]), "text.txt")) {
                textFileFound = true;
            }
        }
        ClearDirectoryFilesFromPhysFS(files);
        assert(textFileFound);
    }

    // LoadFileTextFromPhysFS()
    {
        char* fileText = LoadFileTextFromPhysFS("assets/text.txt");
        assert(fileText != 0);
        assert(TextIsEqual(fileText, "Hello, World!\n"));
        UnloadFileText(fileText);

        char* missingText = LoadFileTextFromPhysFS("MissingText.txt");
        assert(missingText == 0);
    }

    // LoadImageFromPhysFS()
    {
        Image image = LoadImageFromPhysFS("assets/image.png");
        assert(image.width > 100);
        UnloadImage(image);

        Image missingImage = LoadImageFromPhysFS("MissingFile.png");
        assert(missingImage.data == 0);
    }

    // LoadWaveFromPhysFS()
    {
        Wave wave = LoadWaveFromPhysFS("assets/sound.wav");
        assert(wave.data != 0);
        UnloadWave(wave);

        Wave missingWave = LoadWaveFromPhysFS("MissingFile.wav");
        assert(missingWave.data == 0);
    }

    // LoadShaderFromPhysFS()
    {
        Shader missingShader = LoadShaderFromPhysFS("MissingFile.txt", "MissingFile.txt");
        assert(missingShader.locs == 0);
    }

    // GetFileModTimeFromPhysFS()
    assert(GetFileModTimeFromPhysFS("assets/text.txt") > 1000);
    assert(GetFileModTimeFromPhysFS("MissingFile.txt") == -1);

    // SetPhysFSWriteDirectory()
    assert(SetPhysFSWriteDirectory("resources"));
    assert(!SetPhysFSWriteDirectory("MissingDirectory"));

    // UnmountPhysFS()
    assert(UnmountPhysFS("resources"));
    assert(!UnmountPhysFS("MissingDirectory"));

    // ClosePhysFS()
    assert(ClosePhysFS());

    TraceLog(LOG_INFO, "================================");
    TraceLog(LOG_INFO, "raylib-physfs tests succesful");
    TraceLog(LOG_INFO, "================================");

    return 0;
}
