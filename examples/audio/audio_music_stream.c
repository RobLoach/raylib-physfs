/*******************************************************************************************
*
*   raylib [audio] example - Music playing (streaming)
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

    InitWindow(screenWidth, screenHeight, "raylib [audio] example - music playing (streaming)");

    InitAudioDevice();

    // Initialize the file system, and mount a directory.
    InitPhysFS();
    MountPhysFS("resources", "res");

    Music music = LoadMusicStreamFromPhysFS("res/country.mp3");

    PlayMusicStream(music);

    float timePlayed = 0.0f;
    bool pause = false;

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        UpdateMusicStream(music);   // Update music buffer with new stream data

        // Restart music playing (stop and play)
        if (IsKeyPressed(KEY_SPACE))
        {
            StopMusicStream(music);
            PlayMusicStream(music);
        }

        // Pause/Resume music playing
        if (IsKeyPressed(KEY_P))
        {
            pause = !pause;

            if (pause) PauseMusicStream(music);
            else ResumeMusicStream(music);
        }

        // Get timePlayed scaled to bar dimensions (400 pixels)
        timePlayed = GetMusicTimePlayed(music)/GetMusicTimeLength(music)*400;

        if (timePlayed > 400) StopMusicStream(music);
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            DrawText("MUSIC SHOULD BE PLAYING!", 255, 150, 20, LIGHTGRAY);

            DrawRectangle(200, 200, 400, 12, LIGHTGRAY);
            DrawRectangle(200, 200, (int)timePlayed, 12, MAROON);
            DrawRectangleLines(200, 200, 400, 12, GRAY);

            DrawText("PRESS SPACE TO RESTART MUSIC", 215, 250, 20, LIGHTGRAY);
            DrawText("PRESS P TO PAUSE/RESUME MUSIC", 208, 280, 20, LIGHTGRAY);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadMusicStream(music);   // Unload music stream buffers from RAM

    CloseAudioDevice();         // Close audio device (music streaming is automatically stopped)

    ClosePhysFS();
    CloseWindow();              // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
