// libskyretro
// declares some fields in the zeropage for fast access, managed by SkyRetro
// SkyRetro also provides systems for interfacing with nes rom fields via an output labels file from the linker
// we reserve some bytes in the zeropage for SkyRetro use, these are related to the portal state
// SkyRetro uses its own address space for its other variables that aren't as performance critical
// this range is between $4020 and 4FFF (range $4020-$5FFF is reserved for mappers)
// using $5000 conflicts with MMC5; $4020-$4FFF is safer for SkyRetro

#define IS_SKYRETRO_EMU (*(volatile unsigned char*)0x4020)

// EMULATION VARIABLES
#define IS_EMU_MASK 0b00000001
#define EMU_STATE_MASK 0b00000110

enum emu_state {
    EMU_STATE_REGULAR = 0x00,
    /*
     * @brief Alerts when the external magic moment is active
     */
    EMU_STATE_MAGICMOMENT = 0x01,
    /*
     * @brief Alerts when there's an external issue with the Portal
     */
    EMU_STATE_PORTAL = 0x02,
    EMU_STATE_MISC = 0x03
};

// Flags:
// bit 0    - game is informed of emulation
// bits 1-2 - state of emulation (for use with portal)
// bits 3-7 - unused
extern volatile unsigned char emu_flags;
#pragma zpsym ("emu_flags");
extern volatile unsigned char emu_flags_prev;
#pragma zpsym ("emu_flags_prev");

signed int shared_s16_0;

void skyretro_update(void) {
    emu_flags_prev = emu_flags;
}

unsigned char skyretro_flags_changed(void) {
    return emu_flags != emu_flags_prev;
}

void skyretro_set_emu(unsigned char value) {
    emu_flags = value ? emu_flags | IS_EMU_MASK : emu_flags & ~IS_EMU_MASK;
}

unsigned char skyretro_is_emu(void) {
    return emu_flags & IS_EMU_MASK;
}

void skyretro_set_emu_state(unsigned char state) {
    emu_flags &= ~EMU_STATE_MASK;
    emu_flags |= (state << 0x01) & EMU_STATE_MASK;
}

unsigned char skyretro_emu_state(void) {
    return (emu_flags & EMU_STATE_MASK) >> 0x01;
}

void skyretro_set_test_shared_s16(void) {
    shared_s16_0 = 0x300;
}