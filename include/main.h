#include "lib/libnes/neslib.h"
#include "lib/nesdoug/nesdoug.h"
#include "lib/libnespipes/nespipes.h"
#if MMC3
#include "engine/mapper/mmc3/mmc3_code.h"
#include "engine/mapper/mmc3/mmc3_code.c"
#endif
#include "characters/23_wreckingball.h"
#include "portal/portal_init.h"
#if MMC3
#include "water/water.h"
#endif
#include "metatiles.h"
#include "title.h"
#include "echo.h"
#include "hats.h"

#pragma bss-name(push, "ZEROPAGE")

// GLOBAL VARIABLES //
signed char temp_s8_0;
signed int temp_s16_0;
unsigned char temp_u8_0;
unsigned char temp_u8_1;
unsigned char temp_u8_2;
unsigned char temp_u8_3;
unsigned int temp_u16_0;
unsigned int temp_u16_1;

unsigned char has_hat;

// GAME VARIABLES
typedef enum {
    STATE_0,
    STATE_1,
    STATE_2,
    STATE_TITLE
} GameState;

unsigned char game_state;
unsigned char i;
unsigned char j;
int digits[3];
unsigned char writeNum;
unsigned char write[8];
const char hexDigits[] = "0123456789ABCDEF";
unsigned char pad1_poll;
unsigned char pad1_new;

// CAMERA VARIABLES
#define CAM_BOUND_LEFT 110
#define CAM_BOUND_RIGHT 130
#define CAM_BOUND_UP 92
#define CAM_BOUND_DOWN 132

signed int cam_x;
signed int cam_y;
signed int cam_latch_x;
signed int cam_latch_y;

// PLAYER VARIABLES
#define ACCELERATION 0x20
#define DECELERATION 0x20
#define TOP_SPEED shared_s16_0
#define BRAKES 0x7A
#define GRAVITY 0x4A
#define JUMP_FORCE 0x540
#define AIR_ACCEL 0x40
#define AIR_DRAG 0x14
#define TERMINAL_VELOCITY 0x500

typedef struct {
    unsigned int x;
    unsigned int y;
    signed int vel_x;
    signed int vel_y;
    unsigned char flags;
    unsigned char state;
    unsigned char anim_timer;
    unsigned char width;
    unsigned char height;
} Player;
const unsigned char* current_anim;
unsigned char current_anim_frame;

unsigned char current_hat_frame;

Player player = { 0x1000, 0x0000 };

#define GROUNDED_MASK 0b00000001
#define FACING_MASK 0b00000010
#define DEBUG_MASK 0b10000000
#define FACING_LEFT 0
#define FACING_RIGHT 1

#define PLAYER_SET_GROUNDED(grounded) (player.flags = grounded ? player.flags | GROUNDED_MASK : player.flags & ~GROUNDED_MASK)
#define PLAYER_IS_GROUNDED (player.flags & GROUNDED_MASK)

#define PLAYER_SET_DIRECTION(direction) (player.flags = direction ? player.flags | FACING_MASK : player.flags & ~FACING_MASK)
#define PLAYER_FACING_RIGHT (player.flags & FACING_MASK)

#define PLAYER_SET_DEBUG(value) (player.flags = value ? player.flags | DEBUG_MASK : player.flags & ~DEBUG_MASK);
#define PLAYER_IS_DEBUG (player.flags & DEBUG_MASK)

// COLLISION VARIABLES
#define TILE_CMODE_MASK      0b11
#define TILE_CMODE_NONE      0b00
#define TILE_CMODE_UPONLY    0b01
#define TILE_CMODE_HORIZ     0b10
#define TILE_CMODE_ALL       0b11

#define TILE_CTYPE_SHIFT        0x02
#define TILE_CTYPE_MASK         0b1111
#define TILE_CTYPE_FLAT         0b0000
#define TILE_CTYPE_STEEP        0b0001
#define TILE_CTYPE_RELAXED1     0b0010
#define TILE_CTYPE_RELAXED2     0b0011
#define TILE_CTYPE_CONVEX1      0b0100
#define TILE_CTYPE_CONVEX2      0b0101
#define TILE_CTYPE_CONVEX3      0b0110
#define TILE_CTYPE_STEEP2       0b0111
#define TILE_CTYPE_STEEP3       0b1000
#define TILE_CTYPE_CONCAVE1     0b1001
#define TILE_CTYPE_CONCAVE2     0b1010
#define TILE_CTYPE_CONCAVE3     0b1011
#define TILE_CTYPE_CONVEXEDGE   0b1100
#define TILE_CTYPE_SHARP1       0b1101
#define TILE_CTYPE_SHARP2       0b1110
#define TILE_CTYPE_EXTRUDEDWALL 0b1111

#define TILE_FLIPX              0b01000000
#define TILE_FLIPY              0b10000000

const unsigned char is_solid[] = {
    0,
    TILE_CMODE_ALL       | (TILE_CTYPE_FLAT << TILE_CTYPE_SHIFT),
    TILE_CMODE_UPONLY    | (TILE_CTYPE_STEEP << TILE_CTYPE_SHIFT),
    TILE_CMODE_UPONLY    | (TILE_CTYPE_RELAXED1 << TILE_CTYPE_SHIFT),
    TILE_CMODE_UPONLY    | (TILE_CTYPE_RELAXED2 << TILE_CTYPE_SHIFT),
    TILE_CMODE_UPONLY    | (TILE_CTYPE_CONVEX1 << TILE_CTYPE_SHIFT),
    TILE_CMODE_UPONLY    | (TILE_CTYPE_CONVEX2 << TILE_CTYPE_SHIFT),
    TILE_CMODE_UPONLY    | (TILE_CTYPE_CONVEX3 << TILE_CTYPE_SHIFT),
    TILE_CMODE_UPONLY    | (TILE_CTYPE_STEEP2 << TILE_CTYPE_SHIFT),
    TILE_CMODE_UPONLY    | (TILE_CTYPE_STEEP3 << TILE_CTYPE_SHIFT),
    TILE_CMODE_UPONLY    | (TILE_CTYPE_CONCAVE1 << TILE_CTYPE_SHIFT),
    TILE_CMODE_UPONLY    | (TILE_CTYPE_CONCAVE2 << TILE_CTYPE_SHIFT),
    TILE_CMODE_UPONLY    | (TILE_CTYPE_CONCAVE3 << TILE_CTYPE_SHIFT),
    TILE_CMODE_ALL       | (TILE_CTYPE_CONVEXEDGE << TILE_CTYPE_SHIFT),
    TILE_CMODE_ALL       | (TILE_CTYPE_SHARP1 << TILE_CTYPE_SHIFT),
    TILE_CMODE_ALL       | (TILE_CTYPE_SHARP2 << TILE_CTYPE_SHIFT),
    TILE_CMODE_ALL       | (TILE_CTYPE_EXTRUDEDWALL << TILE_CTYPE_SHIFT),
    TILE_CMODE_HORIZ     | (TILE_CTYPE_FLAT << TILE_CTYPE_SHIFT)            | TILE_FLIPX,
    TILE_CMODE_UPONLY    | (TILE_CTYPE_STEEP << TILE_CTYPE_SHIFT)           | TILE_FLIPX,
    TILE_CMODE_UPONLY    | (TILE_CTYPE_RELAXED1 << TILE_CTYPE_SHIFT)        | TILE_FLIPX,
    TILE_CMODE_UPONLY    | (TILE_CTYPE_RELAXED2 << TILE_CTYPE_SHIFT)        | TILE_FLIPX,
    TILE_CMODE_UPONLY    | (TILE_CTYPE_CONVEX1 << TILE_CTYPE_SHIFT)         | TILE_FLIPX,
    TILE_CMODE_UPONLY    | (TILE_CTYPE_CONVEX2 << TILE_CTYPE_SHIFT)         | TILE_FLIPX,
    TILE_CMODE_UPONLY    | (TILE_CTYPE_CONVEX3 << TILE_CTYPE_SHIFT)         | TILE_FLIPX,
    TILE_CMODE_UPONLY    | (TILE_CTYPE_STEEP2 << TILE_CTYPE_SHIFT)          | TILE_FLIPX,
    TILE_CMODE_UPONLY    | (TILE_CTYPE_STEEP3 << TILE_CTYPE_SHIFT)          | TILE_FLIPX,
    TILE_CMODE_UPONLY    | (TILE_CTYPE_CONCAVE1 << TILE_CTYPE_SHIFT)        | TILE_FLIPX,
    TILE_CMODE_UPONLY    | (TILE_CTYPE_CONCAVE2 << TILE_CTYPE_SHIFT)        | TILE_FLIPX,
    TILE_CMODE_UPONLY    | (TILE_CTYPE_CONCAVE3 << TILE_CTYPE_SHIFT)        | TILE_FLIPX,
    TILE_CMODE_ALL       | (TILE_CTYPE_CONVEXEDGE << TILE_CTYPE_SHIFT)      | TILE_FLIPX,
    TILE_CMODE_ALL       | (TILE_CTYPE_SHARP1 << TILE_CTYPE_SHIFT)          | TILE_FLIPX,
    TILE_CMODE_ALL       | (TILE_CTYPE_SHARP2 << TILE_CTYPE_SHIFT)          | TILE_FLIPX,
    TILE_CMODE_ALL       | (TILE_CTYPE_EXTRUDEDWALL << TILE_CTYPE_SHIFT)    | TILE_FLIPX,
    TILE_CMODE_ALL       | (TILE_CTYPE_STEEP << TILE_CTYPE_SHIFT)           | TILE_FLIPY
};

const unsigned char collision_flat[0x10] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

const unsigned char collision_steep1[0x10] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15
};

const unsigned char collision_relaxed1[0x10] = {
    0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7
};

const unsigned char collision_relaxed2[0x10] = {
    8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15
};

const unsigned char collision_convex1[0x10] = {
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2
};

const unsigned char collision_convex2[0x10] = {
    3, 3, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 7, 8, 9, 9
};

const unsigned char collision_convex3[0x10] = {
    10, 11, 12, 13, 13, 14, 15, 15, 16, 16, 16, 16, 16, 16, 16, 16
};

const unsigned char collision_steep2[0x10] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7
};

const unsigned char collision_steep3[0x10] = {
    8, 9, 10, 11, 12, 13, 14, 15, 16, 16, 16, 16, 16, 16, 16, 16
};

const unsigned char collision_concave1[0x10] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 3, 3, 4, 5
};

const unsigned char collision_concave2[0x10] = {
    6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 12, 12
};

const unsigned char collision_concave3[0x10] = {
    13, 13, 13, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 15
};

const unsigned char collision_convexedge[0x10] = {
    0, 0, 0, 0, 1, 1, 1, 2, 2, 3, 4, 5, 6, 7, 16, 16
};

const unsigned char collision_sharp1[0x10] = {
    16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16
};


const unsigned char collision_sharp2[0x10] = {
    16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16
};

/*
const unsigned char collision_convexedge_true[0x10] = {
    0, 0, 0, 0, 1, 1, 1, 2, 2, 3, 4, 5, 6, 7, 9, 12
};

const unsigned char collision_sharp1_true[0x10] = {
    0, 2, 4, 6, 8, 10, 12, 14, 16, 16, 16, 16, 16, 16, 16, 16
};


const unsigned char collision_sharp2_true[0x10] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 4, 6, 8, 10, 12, 14
};
*/

const unsigned char collision_extrudedwall[0x10] = {
    0, 0, 0, 0, 0, 0, 0, 0, 16, 16, 16, 16, 16, 16, 16, 16
};

const unsigned char* const collisions[] = {
    collision_flat, collision_steep1, collision_relaxed1, collision_relaxed2, collision_convex1, collision_convex2, collision_convex3, collision_steep2,
    collision_steep3, collision_concave1, collision_concave2, collision_concave3, collision_convexedge, collision_sharp1, collision_sharp2, collision_extrudedwall
};

const unsigned char collision_convexedge_side[0x10] = {
    16, 16, 16, 16, 16, 16, 16, 16, 16, 1, 1, 1, 0, 0, 0, 0
};

const unsigned char collision_sharp1_side[0x10] = {
    15, 15, 14, 14, 13, 13, 12, 12, 11, 11, 10, 10, 9, 9, 8, 8
};

const unsigned char collision_sharp2_side[0x10] = {
    7, 7, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 0, 0
};

unsigned int player_old_x;
unsigned int player_old_y;
unsigned char sensor_position_x;
unsigned char sensor_position_y;
unsigned char player_collision_x;
unsigned char player_collision_y;
unsigned char tile_x;
unsigned char tile_y;
unsigned char surface_distance_into_tile;
unsigned char surface_position;
unsigned char sensor_distance_into_tile;
unsigned char player_collision_i;
signed char eject_distance;
signed char eject_distance_reserve0;
signed char eject_distance_reserve1;
signed int unground_speed;
unsigned char prev_tile_below;
unsigned char lock_controls;
unsigned char game_begun;

#pragma bss-name(push, "BSS")
unsigned char use_player_palette_for_hat;
unsigned char collision_map[240];
unsigned char collision_map2[240];

extern const unsigned char demo_input[];
unsigned char demo_counter;

unsigned char profile;

#if MMC3
#pragma bss-name(push, "XRAM")
// extra RAM at $6000-$7fff
unsigned char wram_array[0x2000];
#endif
#pragma bss-name(pop)

// ENTITY VARIABLES
typedef struct {
    unsigned int x;
    unsigned int y;
    unsigned char width;
    unsigned char height;
} Entity;

#define ENTITY_QUEUE_MAX 0x10

const unsigned char palette[] = {
    0x0F, 0x00, 0x10, 0x30,
    0x0F, 0x14, 0x21, 0x2B,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00
};

const unsigned char reg[] = "Regular playback";
const unsigned char emu[] = "Regular emulation";
const unsigned char mag[] = "Emulation Magic Moment";
const unsigned char prt[] = "Emulation Portal issue";
const unsigned char unk[] = "Miscellaneous emu state";

// print utils
void toHexString(unsigned char num, char* str) {
    i = 0;
    j = 0;

    if (num == 0) {
        str[0] = '0';
        str[1] = '0';
        str[2] = '\0';
        return;
    }

    while (num != 0 && i < 2) {
        digits[i++] = num & 0xF;
        num >>= 4;
    }

    while (i < 2) {
        digits[i++] = 0;
    }

    for (j = 0; j < i; j++) {
        str[j] = hexDigits[digits[i - j - 1]];
    }

    str[j] = '\0';
}

void toHexStringSigned(signed char num, char* str) {
    i = 0;
    j = 0;

    if (num == 0) {
        str[0] = '0';
        str[1] = '0';
        str[2] = '\0';
        return;
    }

    if (num < 0) {
        str[0] = '-';
        num = -num;
        i++;
    }

    writeNum = (unsigned char)num;

    while (writeNum != 0 && i < 3) {
        digits[i++] = writeNum & 0xF;
        writeNum >>= 4;
    }

    while (i < 3) {
        digits[i++] = 0;
    }

    for (j = 0; j < i; j++) {
        str[j + (str[0] == '-' ? 1 : 0)] = hexDigits[digits[i - j - 1]];
    }

    str[j + (str[0] == '-' ? 1 : 0)] = '\0';
}

unsigned char text[0x10];

#include "levels.h"

// PROTOTYPES
#if MMC3
void set_irq_handler(void (*handler)(void));
void irq_stub(void);
#endif

void change_scene(void);
void write_emu_text(void);
void change_room(void);
void load_room(void);
void player_movement(void);
void debug_movement(void);
void player_scroll(void);
char evaluate_collision_left(void);
char collision_left(void);
char collision_left_ground(void);
char evaluate_collision_right(void);
char collision_right(void);
char collision_right_ground(void);
char collision_up(void);
char collision_down(void);
void calculate_eject_distance_left(signed char*);
void calculate_eject_distance_right(signed char*);
void calculate_eject_distance_up(signed char*);
void calculate_eject_distance_down(signed char*);
void set_hat_palette(const char);
void set_hat_palette_to_player(void);
void draw_player(void);
#if MMC3
void draw_water(void);
#endif