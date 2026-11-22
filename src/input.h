#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>
#include <stdbool.h>

bool Chip8GetKey(uint8_t key);
bool Chip8GetPressed(uint8_t key);
bool Chip8GetReleased(uint8_t key);

#endif