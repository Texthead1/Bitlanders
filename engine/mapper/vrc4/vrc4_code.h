// Maximum level of recursion to allow with banked_call and similar functions.
#define MAX_BANK_DEPTH 10

unsigned char bankLevel;
unsigned char bankBuffer[MAX_BANK_DEPTH];

void banked_call(unsigned char bankId, void (*method)(void));

void bank_push(unsigned char bankId);

void bank_pop(void);

void __fastcall__ set_prg_8000(unsigned char bank_id);

unsigned char __fastcall__ get_prg_8000(void);

void __fastcall__ set_prg_a000(unsigned char bank_id);

void __fastcall__ set_chr_mode_0(unsigned char chr_id);
void __fastcall__ set_chr_mode_1(unsigned char chr_id);
void __fastcall__ set_chr_mode_2(unsigned char chr_id);
void __fastcall__ set_chr_mode_3(unsigned char chr_id);
void __fastcall__ set_chr_mode_4(unsigned char chr_id);
void __fastcall__ set_chr_mode_5(unsigned char chr_id);

#define MIRROR_VERTICAL 0
#define MIRROR_HORIZONTAL 1

void __fastcall__ set_mirroring(unsigned char mirroring);

void disable_irq(void);

void set_irq_ptr(char * address);