#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>
#include <stdbool.h>
#include "display.h"

typedef struct Chip8
{
    uint8_t ram[4096];
    Display display;
    uint8_t vregs[16];
    uint16_t ireg;
    uint8_t delay_timer;
    uint8_t sound_timer;

    uint16_t program_counter;
    uint8_t stack_pointer;
    uint16_t stack[16];

    int8_t held_key;
    bool sound_just_started;
} Chip8;

void Chip8LoadFont(Chip8 *chip8);

void Chip8Step(Chip8 *chip8);

void Chip8LoadFile(Chip8 *chip8, const char *filename);

#endif