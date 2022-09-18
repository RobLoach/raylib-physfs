/*******************************************************************************************
*
*   raylib [textures] example - Image loading and texture creation
*
*   NOTE: Images are loaded in CPU memory (RAM); textures are loaded in GPU memory (VRAM)
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

    InitWindow(screenWidth, screenHeight, "raylib [text] example - basic loading");

    // Initialize the file system, and mount a directory.
    InitPhysFSEx("resources", "res");

    // Load the image from PhysFS.
    char* text = LoadFileTextFromPhysFS("res/helloworld.txt");

    //---------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Update your variables here
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            DrawText(text, 100, 50, 30, GRAY);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadFileText(text);

    ClosePhysFS();
    CloseWindow();                // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
