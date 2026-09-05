#include "lib/libnes/neslib.h"

extern unsigned char water_scanline_rows_remaining;
#pragma zpsym("water_scanline_rows_remaining");

extern unsigned char water_scanline_index;
#pragma zpsym("water_scanline_index");

extern unsigned char irq_handler_ptr[2];
#pragma zpsym("irq_handler_ptr");

void setup_water_irq(unsigned char waterline);

extern const unsigned char water_sprite[];
extern const unsigned char water_palette[];

extern const unsigned char water_sprite_positions[][0x02];
extern const unsigned char water_sprite_offsets[];
extern const signed char water_scanline_offsets[];

extern void water_irq(void);