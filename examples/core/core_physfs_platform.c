/*******************************************************************************************
*
*   raylib [core] example - PhysFS as a raylib I/O platform
*
*   After calling SetPhysFSCallbacks(), all standard raylib file loading functions
*   (LoadTexture, LoadSound, LoadFont, etc.) transparently route through PhysFS.
*   This means you can use .zip archives without changing any existing raylib asset calls.
*
*   This example has been created using raylib-physfs 6.0.0 (https://github.com/RobLoach/raylib-physfs)
*   raylib-physfs is licensed under an unmodified zlib/libpng license (View raylib-physfs.h for details)
*
*   Copyright (c) 2026 Rob Loach (@RobLoach)
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

    InitWindow(screenWidth, screenHeight, "raylib [core] example - PhysFS platform");

    // Initialize PhysFS and mount a zip archive at a virtual path.
    InitPhysFS();
    MountPhysFS("resources/resources.zip", "res");

    // Route all raylib file I/O through PhysFS.
    // From this point on, standard raylib load functions use PhysFS transparently.
    SetPhysFSCallbacks();

    // Load using the standard raylib function — no "FromPhysFS" variant needed.
    Texture2D texture = LoadTexture("res/raylib_logo.png");
    //---------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())
    {
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            DrawTexture(texture, screenWidth/2 - texture.width/2, screenHeight/2 - texture.height/2, WHITE);

            DrawText("Loaded via SetPhysFSCallbacks() - no LoadTextureFromPhysFS() needed!", 60, 390, 15, GRAY);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadTexture(texture);

    ClosePhysFS();
    CloseWindow();
    //--------------------------------------------------------------------------------------

    return 0;
}
