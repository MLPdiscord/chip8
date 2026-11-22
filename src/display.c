#include "display.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

bool Chip8DrawSprite(Display *display, uint8_t x, uint8_t y, uint8_t *ram, uint16_t start, uint8_t height)
{
    int offset = x % 8;

    bool collision = false;
    int res_x = (x / 8) % display->width;
    int res_x2 = (x / 8 + 1) % display->width;

    if (offset == 0)
    {
        for (int i = 0; i < height; i++)
        {
            int res_y = (y + i) % display->height;

            uint8_t was = display->byte_rows[res_y][res_x];
            uint8_t new = ram[(start + i) & 0x0fff];
            display->byte_rows[res_y][res_x] ^= new;
            if (display->byte_rows[res_y][res_x] != (was | new))
            {
                collision = true;
            }
        }
        return collision;
    }

    for (int i = 0; i < height; i++)
    {
        int res_y = (y + i) % display->height;
        uint8_t was = display->byte_rows[res_y][res_x];
        uint16_t new_big = (((uint16_t)ram[(start + i) & 0x0fff]) << 8) >> offset;
        uint8_t new = new_big >> 8;

        display->byte_rows[res_y][res_x] ^= new;
        if (display->byte_rows[res_y][res_x] != (was | new))
        {
            collision = true;
        }

        was = display->byte_rows[res_y][res_x2];
        new = new_big & 0xff;
        display->byte_rows[res_y][res_x2] ^= new;
        if (display->byte_rows[res_y][res_x2] != (was | new))
        {
            collision = true;
        }
    }

    return collision;
}