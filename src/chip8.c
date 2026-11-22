#include "chip8.h"
#include "input.h"
#include "stdio.h"
#include <stdlib.h>

void Chip8LoadFont(Chip8 *chip8)
{
    // clang-format off
    static const uint8_t font[5 * 8 * 16] = {
        0xf0, 0x90, 0x90, 0x90, 0xf0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xf0, 0x80, 0xf0, 0x80, 0xf0, // 2
        0xf0, 0x10, 0xf0, 0x10, 0xf0, // 3
        0x90, 0x90, 0xf0, 0x10, 0x10, // 4
        0xf0, 0x80, 0xf0, 0x10, 0xf0, // 5
        0xf0, 0x80, 0xf0, 0x90, 0xf0, // 6
        0xf0, 0x10, 0x20, 0x40, 0x40, // 7
        0xf0, 0x90, 0xf0, 0x90, 0xf0, // 8
        0xf0, 0x90, 0xf0, 0x10, 0xf0, // 9
        0xf0, 0x90, 0xf0, 0x90, 0x90, // a
        0xe0, 0x90, 0xe0, 0x90, 0xe0, // b
        0xf0, 0x80, 0x80, 0x80, 0xf0, // c
        0xe0, 0x90, 0x90, 0x90, 0xe0, // d
        0xf0, 0x80, 0xf0, 0x80, 0xf0, // e
        0xf0, 0x80, 0xf0, 0x80, 0x80, // f
    };
    // clang-format on

    for (int i = 0; i < 5 * 8 * 16; i++)
    {
        chip8->ram[i] = font[i];
    }
}

void Chip8Step(Chip8 *chip8)
{
    if (chip8->delay_timer > 0)
    {
        chip8->delay_timer -= 1;
    }
    if (chip8->sound_timer > 0)
    {
        chip8->sound_timer -= 1;
    }

    if (chip8->held_key > -1)
    {
        if (Chip8GetReleased(chip8->held_key))
        {
            chip8->held_key = -1;
        }
        else
        {
            return;
        }
    }

    uint16_t instr = (chip8->ram[chip8->program_counter] << 8) | chip8->ram[chip8->program_counter + 1];
    uint16_t top = instr & 0xf000;
    uint16_t nnn = instr & 0x0fff;
    uint8_t x = (instr & 0x0f00) >> 8;
    uint8_t y = (instr & 0x00f0) >> 4;
    uint8_t kk = (instr & 0x00ff);

    uint8_t t;

    // printf("%04x: %04x | regs: ", chip8->program_counter, instr);
    // for (int i = 0; i < 16; i++)
    // {
    //     printf("%02x ", chip8->vregs[i]);
    // }
    // printf("| i: %03x stack:", chip8->ireg);

    // for (int i = 0; i < 16; i++)
    // {
    //     if (i == chip8->stack_pointer)
    //     {
    //         printf(">");
    //     }
    //     else
    //     {
    //         printf(" ");
    //     }
    //     printf("%02x", chip8->stack[i]);
    // }
    // printf("\n");

    chip8->program_counter += 2;
    chip8->program_counter &= 0x0fff;

    switch (top)
    {
    case 0x1000:
        chip8->program_counter = nnn;
        return;
    case 0x2000:
        chip8->stack_pointer++;
        chip8->stack_pointer %= 16;
        chip8->stack[chip8->stack_pointer] = chip8->program_counter;
        chip8->program_counter = nnn;
        return;
    case 0x3000:
        if (chip8->vregs[x] == kk)
        {
            chip8->program_counter += 2;
        }
        return;
    case 0x4000:
        if (chip8->vregs[x] != kk)
        {
            chip8->program_counter += 2;
        }
        return;
    case 0x5000:
        if (chip8->vregs[x] == chip8->vregs[y])
        {
            chip8->program_counter += 2;
        }
        return;
    case 0x6000:
        chip8->vregs[x] = kk;
        return;
    case 0x7000:
        chip8->vregs[x] += kk;
        return;
    case 0x9000:
        if (chip8->vregs[x] != chip8->vregs[y])
        {
            chip8->program_counter += 2;
        }
        return;
    case 0xa000:
        chip8->ireg = nnn;
        return;
    case 0xb000:
        chip8->program_counter = nnn + chip8->vregs[0];
        return;
    case 0xc000:
        chip8->vregs[x] = rand() & kk;
        return;
    }

    if (instr == 0x00e0)
    {
        // CLS
        for (int y = 0; y < chip8->display.height; y++)
        {
            for (int x = 0; x < chip8->display.width; x++)
            {
                chip8->display.byte_rows[y][x] = 0;
            }
        }
        return;
    }
    if (instr == 0x00ee)
    {
        // RET
        chip8->program_counter = chip8->stack[chip8->stack_pointer];
        chip8->stack_pointer--;
        chip8->stack_pointer %= 16;
        return;
    }

    if (top == 0x8000)
    {
        switch (instr & 0x000f)
        {
        case 0:
            chip8->vregs[x] = chip8->vregs[y];
            return;
        case 1:
            chip8->vregs[x] |= chip8->vregs[y];
            return;
        case 2:
            chip8->vregs[x] &= chip8->vregs[y];
            return;
        case 3:
            chip8->vregs[x] ^= chip8->vregs[y];
            return;
        case 4:
            t = chip8->vregs[x];
            chip8->vregs[x] += chip8->vregs[y];
            // carry
            chip8->vregs[0xf] = chip8->vregs[x] < t;
            return;
        case 5:
            t = chip8->vregs[x];
            chip8->vregs[x] -= chip8->vregs[y];
            // not borrow
            chip8->vregs[0xf] = chip8->vregs[x] < t;
            return;
        case 6:
            t = chip8->vregs[x] & 1;
            chip8->vregs[x] >>= 1;
            chip8->vregs[0xf] = t;
            return;
        case 7:
            // not borrow
            t = chip8->vregs[y];
            chip8->vregs[x] = chip8->vregs[y] - chip8->vregs[x];
            chip8->vregs[0xf] = chip8->vregs[x] < t;
            return;
        case 0xe:
            t = (chip8->vregs[x] & 0b10000000) != 0;
            chip8->vregs[x] <<= 1;
            chip8->vregs[0xf] = t;
            return;
        }
        return;
    }

    if (top == 0xd000)
    {
        chip8->vregs[0xf] = Chip8DrawSprite(&chip8->display, chip8->vregs[x], chip8->vregs[y], chip8->ram, chip8->ireg, instr & 0xf);
        return;
    }

    if (top == 0xe000)
    {
        if (kk == 0x9e)
        {
            if (Chip8GetKey(chip8->vregs[x]))
            {
                chip8->program_counter += 2;
            }
        }
        else if (kk == 0xa1)
        {
            if (!Chip8GetKey(chip8->vregs[x]))
            {
                chip8->program_counter += 2;
            }
        }
        return;
    }

    if (top == 0xf000)
    {
        switch (kk)
        {
        case 0x07:
            chip8->vregs[x] = chip8->delay_timer;
            return;
        case 0x0a:
            for (int i = 0; i < 16; i++)
            {
                if (Chip8GetPressed(i))
                {
                    chip8->vregs[x] = i;
                    chip8->held_key = i;
                    return;
                }
            }
            chip8->program_counter -= 2;
            return;
        case 0x15:
            chip8->delay_timer = chip8->vregs[x];
            return;
        case 0x18:
            chip8->sound_timer = chip8->vregs[x];
            return;
        case 0x1e:
            chip8->ireg += chip8->vregs[x];
            chip8->ireg &= 0x0fff;
            return;
        case 0x29:
            chip8->ireg = chip8->vregs[x] * 5;
            return;
        case 0x33:
            t = chip8->vregs[x];
            chip8->ram[chip8->ireg + 2] = t % 10;
            t /= 10;
            chip8->ram[chip8->ireg + 1] = t % 10;
            t /= 10;
            chip8->ram[chip8->ireg] = t;
            return;
        case 0x55:
            for (int i = 0; i <= x; i++)
            {
                chip8->ram[(i + chip8->ireg) & 0x0fff] = chip8->vregs[i];
            }
            return;
        case 0x65:
            for (int i = 0; i <= x; i++)
            {
                chip8->vregs[i] = chip8->ram[(i + chip8->ireg) & 0x0fff];
            }
            return;
        }
        return;
    }
}

void Chip8LoadFile(Chip8 *chip8, const char *filename)
{
    FILE *fp = fopen(filename, "rb");

    fseek(fp, 0L, SEEK_END);
    int size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    printf("Loading file %s\n", filename);

    for (int i = 0; i < size; i++)
    {
        char c = getc(fp);
        chip8->ram[i + 0x200] = c;
        if (i % 2 == 0)
        {
            printf(" ");
        }
        printf("%02x", (unsigned char)c);
    }

    fclose(fp);
}
