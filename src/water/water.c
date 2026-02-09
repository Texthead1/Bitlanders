#include "water/water.h"

void setup_water_irq(unsigned char waterline) {
    *(unsigned char*)0xC001 = waterline;
    *(unsigned char*)0xC000 = waterline;
    *(unsigned char*)0xE001 = 0;
}