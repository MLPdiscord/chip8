#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include <stdbool.h>

typedef struct Display
{
    uint8_t byte_rows[32][8];
    int height; // pixels
    int width;  // bytes
} Display;

bool Chip8DrawSprite(Display *display, uint8_t x, uint8_t y, uint8_t *ram, uint16_t start, uint8_t height);

#endif