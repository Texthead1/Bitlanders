#pragma bss-name(push, "ZEROPAGE")

// GLOBAL VARIABLES //
signed int temp_s16_0;
unsigned char temp_u8_0;
unsigned char temp_u8_1;

// GAME VARIABLES
enum game_state {
    STATE_0 = 0,
    STATE_1 = 1
};

unsigned char game_state;
unsigned char i;
unsigned char pad1_poll;
unsigned char pad1_new;

// EMULATION VARIABLES
#define IS_EMU_MASK 0b00000001
#define EMU_STATE_MASK 0b00000110

enum emu_state {
    EMU_REGULAR = 0x00,
    /*
     * @brief Alerts when the external magic moment is active
     */
    EMU_MAGICMOMENT = 0x01,
    /*
     * @brief Alerts when there's an external issue with the Portal
     */
    EMU_PORTAL = 0x02,
    EMU_MISC = 0x03
};

// Flags:
// bit 0    - game is informed of emulation
// bits 1-2 - state of emulation (for use with portal)
// bits 3-7 - unused
unsigned char emu_flags;
unsigned char emu_flags_prev;

void game_setEmu(unsigned char value) {
    emu_flags = value ? emu_flags | IS_EMU_MASK : emu_flags & ~IS_EMU_MASK;
}

unsigned char game_isEmu(void) {
    return emu_flags & IS_EMU_MASK;
}

void game_setEmuState(unsigned char state) {
    emu_flags &= ~EMU_MISC;
    emu_flags |= (state << 0x01);
}

unsigned char game_EmuState(void) {
    return (emu_flags & EMU_STATE_MASK) >> 0x01;
}

// CAMERA VARIABLES
unsigned char cam_x;
unsigned char cam_y;

// PLAYER VARIABLES
#define GROUNDED_MASK 0b00000001
#define FACING_MASK 0b00000010
#define FACING_LEFT 0
#define FACING_RIGHT 1

#define ACCELERATION 0x20
#define DECELERATION 0x20
#define BRAKES 0x7A
#define TOP_SPEED 0x480
#define GRAVITY 0x4A
#define JUMP_FORCE 0x580
#define AIR_ACCEL 0x40
#define AIR_DRAG 0x14
#define TERMINAL_VELOCITY 0x1000

struct Player {
    unsigned int x;
    unsigned int y;
    signed int vel_x;
    signed int vel_y;
    unsigned char flags;
    unsigned char state;
    unsigned char anim_timer;
    unsigned char width;
    unsigned char height;
};
const unsigned char* current_anim;

struct Player player;

void player_setGrounded(unsigned char grounded) {
    player.flags = grounded ? player.flags | GROUNDED_MASK : player.flags & ~GROUNDED_MASK;
}

unsigned char player_IsGrounded(void) {
    return player.flags & GROUNDED_MASK;
}

void player_setDirection(unsigned char direction) {
    player.flags = direction ? player.flags | FACING_MASK : player.flags & ~FACING_MASK;
}

unsigned char player_facingRight(void) {
    return player.flags & FACING_MASK;
}

#pragma bss-name(push, "BSS")
#include "demogame.h"

// ENTITY VARIABLES
struct Entity {
    unsigned int x;
    unsigned int y;
    unsigned char width;
    unsigned char height;
};

#define ENTITY_QUEUE_MAX 0x10
unsigned char collision_map[240];

const unsigned char palette[] = {
    0x0F, 0x00, 0x10, 0x30,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00
};

const unsigned char bean_palette[] = {
    0x0F, 0x01, 0x1C, 0x2A,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00
};

const unsigned char poke_fix[] = { 0, 1 };

const unsigned char reg[] = "Regular playback";
const unsigned char emu[] = "Regular emulation";
const unsigned char mag[] = "Emulation Magic Moment";
const unsigned char prt[] = "Emulation Portal issue";
const unsigned char unk[] = "Miscellaneous emu state";

// PROTOTYPES
void playerMovement(void);
void drawPlayer(void);