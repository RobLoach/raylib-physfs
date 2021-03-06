/*******************************************************************************************
*
*   raylib [text] example - Font loading
*
*   raylib can load fonts from multiple file formats:
*
*     - TTF/OTF > Sprite font atlas is generated on loading, user can configure
*                 some of the generation parameters (size, characters to include)
*     - BMFonts > Angel code font fileformat, sprite font image must be provided
*                 together with the .fnt file, font generation cna not be configured
*     - XNA Spritefont > Sprite font image, following XNA Spritefont conventions,
*                 Characters in image must follow some spacing and order rules
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

    InitWindow(screenWidth, screenHeight, "raylib [text] example - font loading");

    InitPhysFS();           // Initialize PhysFS
    MountPhysFS("resources", "res");

    // Define characters to draw
    // NOTE: raylib supports UTF-8 encoding, following list is actually codified as UTF8 internally
    const char msg[256] = "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHI\nJKLMNOPQRSTUVWXYZ[]^_`abcdefghijklmn\nopqrstuvwxyz{|}~¿ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓ\nÔÕÖ×ØÙÚÛÜÝÞßàáâãäåæçèéêëìíîïðñòóôõö÷\nøùúûüýþÿ";

    // NOTE: Textures/Fonts MUST be loaded after Window initialization (OpenGL context is required)

    // TTF font : Font data and atlas are generated directly from TTF
    // NOTE: We define a font base size of 32 pixels tall and up-to 250 characters
    Font fontTtf = LoadFontFromPhysFS("res/pixantiqua.ttf", 32, 0, 250);

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------

        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            DrawTextEx(fontTtf, msg, (Vector2){ 20.0f, 100.0f }, (float)fontTtf.baseSize, 2, LIME);
            DrawText("Using TTF font generated", 20, GetScreenHeight() - 30, 20, GRAY);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadFont(fontTtf);    // TTF Font unloading

    CloseWindow();          // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
