#if MMC3
#include "water/water.h"

#pragma bss-name(push, "ZEROPAGE")

unsigned char water_scanline_rows_remaining;
unsigned char water_scanline_index;

#pragma bss-name(pop)

const unsigned char water_sprite[] = {
    0x00, 0x00, 0x0F, 3,
    0x80
};

const unsigned char water_palette[] = {
    0x0F, 0x11, 0x2C, 0x30
};

const unsigned char water_sprite_positions[][0x02] = {
    { 0x00, 0x40 },
    { 0x08, 0x48 },
    { 0x10, 0x50 },
    { 0x18, 0x58 },
    { 0x80, 0xC0 },
    { 0x88, 0xC8 },
    { 0x90, 0xD0 },
    { 0x98, 0xD8 }
};

// TODO: replace with precomputed sinewave
const unsigned char water_sprite_offsets[] = {
    0x00, 0x00, 0x02, 0x02, 0x04, 0x06, 0x08, 0x0C,
    0x10, 0x14, 0x16, 0x18, 0x1A, 0x1C, 0x1C, 0x1E,
    0x1E, 0x1E, 0x1C, 0x1C, 0x1A, 0x18, 0x16, 0x14,
    0x10, 0x0C, 0x08, 0x06, 0x04, 0x02, 0x02, 0x00,
};

const signed char water_scanline_offsets[] = {
    -3, -3, -2, -2, -2, -1, -1,  0,
     1,  1,  2,  2,  2,  3,  3,  3,
     3,  3,  2,  2,  2,  1,  1,  0,
    -1, -1, -2, -2, -2, -3, -3, -3
};

void setup_water_irq(unsigned char waterline) {
    water_scanline_rows_remaining = (240 - (waterline + 3) >> 1);
    *(unsigned char*)0xC000 = waterline - 5;
    *(unsigned char*)0xC001 = 0;
    *(unsigned char*)0xE001 = 0;
}
#endif