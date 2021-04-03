/*******************************************************************************************
*
*   raylib [audio] example - Sound loading and playing
*
*   This example has been created using raylib-physfs 0.0.2 (https://github.com/RobLoach/raylib-physfs)
*   raylib-physfs is licensed under an unmodified zlib/libpng license (View raylib-physfs.h for details)
*
*   Copyright (c) 2021 Rob Loach (@RobLoach)
*
********************************************************************************************/

#include "raylib.h"

#define RAYLIB_PHYSFS_IMPLEMENTATION
#include "raylib-physfs.h"

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [audio] example - sound loading and playing");

    InitAudioDevice();      // Initialize audio device

    InitPhysFS();           // Initialize PhysFS
    MountPhysFS("resources", ""); // Mount the resources directory.

    Wave wav = LoadWaveFromPhysFS("sound.wav");
    Wave ogg = LoadWaveFromPhysFS("target.ogg");

    Sound fxWav = LoadSoundFromWave(wav);         // Load WAV audio file
    Sound fxOgg = LoadSoundFromWave(ogg);        // Load OGG audio file

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        if (IsKeyPressed(KEY_SPACE)) PlaySound(fxWav);      // Play WAV sound
        if (IsKeyPressed(KEY_ENTER)) PlaySound(fxOgg);      // Play OGG sound
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            DrawText("Press SPACE to PLAY the WAV sound!", 200, 180, 20, LIGHTGRAY);
            DrawText("Press ENTER to PLAY the OGG sound!", 200, 220, 20, LIGHTGRAY);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadWave(wav);
    UnloadWave(ogg);
    UnloadSound(fxWav);     // Unload sound data
    UnloadSound(fxOgg);     // Unload sound data

    ClosePhysFS();          // Close PhysFS

    CloseAudioDevice();     // Close audio device

    CloseWindow();          // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
