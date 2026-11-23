#include <raylib.h>
#include "chip8.h"
#include "display.h"
#include <stdio.h>

#define BUFFER_SIZE 4096
#define SAMPLE_RATE 44100

int main()
{
    int pixel_scale = 4;

    Chip8 chip8;
    chip8.display.width = 8;
    chip8.display.height = 4 * 8;
    chip8.program_counter = 0x200;
    chip8.held_key = -1;
    for (int y = 0; y < chip8.display.height; y++)
    {
        for (int x = 0; x < chip8.display.width; x++)
        {
            chip8.display.byte_rows[y][x] = 0;
        }
    }

    Chip8LoadFont(&chip8);
    Chip8LoadFile(&chip8, "tests/Breakout [Carmelo Cortez, 1979].ch8");

    const int screenWidth = chip8.display.width * 8 * pixel_scale;
    const int screenHeight = chip8.display.height * pixel_scale;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

    InitAudioDevice();
    SetAudioStreamBufferSizeDefault(BUFFER_SIZE);
    float buffer[BUFFER_SIZE] = {};

    AudioStream stream = LoadAudioStream(SAMPLE_RATE, 32, 1);
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        if (i % 100 > 50)
        {
            buffer[i] = 0.03;
        }
        else
        {
            buffer[i] = -0.03;
        }
    }
    PlayAudioStream(stream);

    SetTargetFPS(240);

    double last_time = GetTime();
    double time_acc = 0;

    while (!WindowShouldClose())
    {
        if (chip8.sound_timer > 0 && chip8.sound_just_started)
        {
            PlayAudioStream(stream);
        }
        if (chip8.sound_timer == 0)
        {
            StopAudioStream(stream);
        }

        if (IsAudioStreamProcessed(stream))
        {
            UpdateAudioStream(stream, buffer, BUFFER_SIZE);
        }

        double this_time = GetTime();
        time_acc += (this_time - last_time) * 60;
        last_time = this_time;
        int elapsed = time_acc;
        time_acc -= elapsed;
        Chip8Step(&chip8, elapsed);

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