#include <raylib.h>
#include "chip8.h"
#include "display.h"
#include <stdio.h>

int main()
{
    int pixel_scale = 4;

    Chip8 chip8;
    chip8.display.width = 8;
    chip8.display.height = 4 * 8;
    chip8.program_counter = 0x200;
    chip8.held_key = -1;

    Chip8LoadFont(&chip8);
    Chip8LoadFile(&chip8, "tests/6-keypad.ch8");

    const int screenWidth = chip8.display.width * 8 * pixel_scale;
    const int screenHeight = chip8.display.height * pixel_scale;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

    // SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        Chip8Step(&chip8);

        BeginDrawing();

        ClearBackground(BLACK);

        for (int y = 0; y < chip8.display.height; y++)
        {
            for (int x = 0; x < chip8.display.width; x++)
            {
                uint8_t byte = chip8.display.byte_rows[y][x];
                for (int xx = 0; xx < 8; xx++)
                {
                    if (byte & (0b10000000 >> xx))
                    {
                        DrawRectangle((x * 8 + xx) * pixel_scale, y * pixel_scale, pixel_scale, pixel_scale, WHITE);
                    }
                }
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}