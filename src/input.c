#include "input.h"
#include <raylib.h>

const KeyboardKey keys[16] = {
    KEY_X, KEY_ONE, KEY_TWO, KEY_THREE,
    KEY_Q, KEY_W, KEY_E, KEY_A,
    KEY_S, KEY_D, KEY_Z, KEY_C,
    KEY_FOUR, KEY_R, KEY_F, KEY_V};

bool Chip8GetKey(uint8_t key)
{
    return IsKeyDown(keys[key % 16]);
}

bool Chip8GetPressed(uint8_t key)
{
    return IsKeyPressed(keys[key % 16]);
}

bool Chip8GetReleased(uint8_t key)
{
    return IsKeyReleased(keys[key % 16]);
}