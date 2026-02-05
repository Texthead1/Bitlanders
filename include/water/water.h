extern unsigned char irq_handler_ptr[2];
#pragma zpsym("irq_handler_ptr");

void setup_water_irq(unsigned char waterline);

extern const unsigned char water_palette[];

extern void water_irq(void);