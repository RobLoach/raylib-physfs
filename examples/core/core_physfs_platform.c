/*******************************************************************************************
*
*   raylib [core] example - PhysFS raylib platform
*
*   Demonstrates physfs_platform_raylib.c, which implements the PhysFS platform
*   abstraction layer using raylib's own file API. When PHYSFS_PLATFORM_RAYLIB is
*   defined, PhysFS reads and writes files through raylib's LoadFileData /
*   SaveFileData rather than calling the OS directly.
*
*   This is useful on platforms where raylib provides a custom file I/O layer,
*   such as web or embedded targets.
*
*   Copyright (c) 2026 Rob Loach (@RobLoach)
*
********************************************************************************************/

#include "raylib.h"

// Use raylib as the PhysFS platform layer instead of the default OS platform.
// physfs_platform_raylib.c must be present alongside raylib-physfs.h.
#define PHYSFS_PLATFORM_RAYLIB
#define RAYLIB_PHYSFS_IMPLEMENTATION
#include "raylib-physfs.h"

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;
    SetTraceLogLevel(LOG_DEBUG);

    InitWindow(screenWidth, screenHeight, "raylib [core] example - PhysFS raylib platform");

    InitPhysFS();
    MountPhysFS("resources/resources.zip", "res");

    Texture2D texture = LoadTextureFromPhysFS("res/raylib_logo.png");
    //---------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())
    {
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            DrawTexture(texture, screenWidth/2 - texture.width/2, screenHeight/2 - texture.height/2, WHITE);

            DrawText("texture loaded via physfs_platform_raylib", 240, 380, 20, GRAY);

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
