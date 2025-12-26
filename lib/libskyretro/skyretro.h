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
unsigned char emu_flags;
unsigned char emu_flags_prev;

signed int shared_s16_0;

void game_setEmu(unsigned char value) {
    emu_flags = value ? emu_flags | IS_EMU_MASK : emu_flags & ~IS_EMU_MASK;
}

unsigned char game_isEmu(void) {
    return emu_flags & IS_EMU_MASK;
}

void game_setEmuState(unsigned char state) {
    emu_flags &= ~EMU_STATE_MASK;
    emu_flags |= (state << 0x01) & EMU_STATE_MASK;
}

unsigned char game_EmuState(void) {
    return (emu_flags & EMU_STATE_MASK) >> 0x01;
}

void test_setPlayerSharedS16(void) {
    shared_s16_0 = 0x300;
}