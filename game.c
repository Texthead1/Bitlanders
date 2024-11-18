#include "LIB/neslib.h"
#include "LIB/nesdoug.h"
#include "game.h"
#include "metasprites.h"
#include "echo.h"

#define XPOS 6
#define YPOS 8

void main(void) {

    ppu_off();      // screen off

    // bank_spr() allows us to offset into our CHR ROM by an entire page.
    // Currently Wrecking Ball sits at the first page of the bank, so we'll keep/set this to 0
    //bank_spr(0);

    pal_spr(bean_palette);
    pal_bg(palette); //	load the BG palette

    // Set up the video RAM buffer for us to use successfully
    set_vram_buffer();

    ppu_on_all();   // let's turn back on the screen and continue with what we need

    // variables are defined in game.h

    emu_flags_prev = 1;
    player_setDirection(FACING_RIGHT);
    //player_setGrounded(FALSE);
    player.x = XPOS << 0x08;
    player.y = YPOS << 0x08;

    while (TRUE) {
        ppu_wait_nmi();

        pad1_poll = pad_poll(0);
        pad1_new = get_pad_new(0);

        // Start of frame

        if (pad1_new & PAD_START) {
            if (game_state == STATE_0) {
                game_state = STATE_1;
                //oam_clear();
                bank_spr(1);
                pal_bg(echo_palette);
                pal_spr(echo_palette);
                ppu_off();
                vram_adr(NAMETABLE_A);
                vram_unrle(echo);
                ppu_on_all();
                POKE(poke_fix + game_state, game_state);
            } else {
                game_state = STATE_0;
                bank_spr(0);
                pal_bg(palette);
                pal_spr(bean_palette);
                emu_flags_prev = emu_flags + 1;
                ppu_off();
                vram_adr(NAMETABLE_A);
                vram_fill(0, 1024);
                ppu_on_all();
                POKE(poke_fix + game_state, game_state);
            }
        }

        if (game_state == STATE_0) {
            i = game_EmuState();
            oam_clear();

            playerMovement();
            drawPlayer();

            if (emu_flags != emu_flags_prev) {
                ppu_off();
                vram_adr(NAMETABLE_A);
                vram_fill(0,1024);
                ppu_on_all();

                if (game_isEmu())
                {
                    switch (i) {
                        case EMU_REGULAR:
                            multi_vram_buffer_horz(emu, sizeof(emu), NTADR_A(XPOS, YPOS));
                            break;
                        
                        case EMU_MAGICMOMENT:
                            multi_vram_buffer_horz(mag, sizeof(mag), NTADR_A(XPOS, YPOS));
                            break;
                        
                        case EMU_PORTAL:
                            multi_vram_buffer_horz(prt, sizeof(prt), NTADR_A(XPOS, YPOS));
                            break;
                        
                        case EMU_MISC:
                            multi_vram_buffer_horz(unk, sizeof(unk), NTADR_A(XPOS, YPOS));
                            break;
                    }
                } else {
                    multi_vram_buffer_horz(reg, sizeof(reg), NTADR_A(XPOS, YPOS));
                }
            }
        } else {
            oam_clear();
            pad1_poll = demo_input[demo_counter];
            pad1_new = demo_counter != 0 ? (pad1_poll & ~demo_input[demo_counter - 1]) : pad1_poll;
            ++demo_counter;

            playerMovement();
            drawPlayer();
        }
        emu_flags_prev = emu_flags;
    }
}

void playerMovement(void) {
    // grounded control code
    if (player_IsGrounded()) {
        player.vel_y = 0;

        // if we press right, accelerate to top speed if moving right, brake if moving left
        if (pad1_poll & PAD_RIGHT) {
            if (player.vel_x >= 0) {
                player_setDirection(FACING_RIGHT);
                player.vel_x = MIN(player.vel_x + ACCELERATION, TOP_SPEED);
            } else {
                if ((player.vel_x += BRAKES) >= 0) {
                    player_setDirection(FACING_RIGHT);
                }
            }

        // if we press left, accelerate to negative top speed if moving left, brake if moving right
        } else if (pad1_poll & PAD_LEFT) {
            if (player.vel_x <= 0) {
                player_setDirection(FACING_LEFT);
                player.vel_x = MAX(player.vel_x - ACCELERATION, -TOP_SPEED);
            } else {
                if ((player.vel_x -= BRAKES) <= 0) {
                    player_setDirection(FACING_LEFT);
                }
            }

        // no input, natural deceleration kicks in
        } else {
            if (player.vel_x != 0) {
                temp_s16_0 = player.vel_x;
                if (player.vel_x > 0)
                    player.vel_x = MAX(player.vel_x - DECELERATION, 0);
                else if (player.vel_x < 0)
                    player.vel_x = MIN(player.vel_x + DECELERATION, 0);
            }
        }

        // jump!
        if (pad1_new & PAD_A) {
            player_setGrounded(FALSE);
            player.vel_y -= JUMP_FORCE;
        }
    } else {
        if (player.vel_y < -0x2B0 && !(pad1_poll & PAD_A))
            player.vel_y = -0x2B0;
        else
            player.vel_y = MIN(player.vel_y + GRAVITY, TERMINAL_VELOCITY);

        if (pad1_poll & PAD_RIGHT) {
            player_setDirection(FACING_RIGHT);
            player.vel_x = MIN(player.vel_x + AIR_ACCEL, TOP_SPEED);
        } else if (pad1_poll & PAD_LEFT) {
            player_setDirection(FACING_LEFT);
            player.vel_x = MAX(player.vel_x - AIR_ACCEL, -TOP_SPEED);
        } else {
            if (player.vel_x > 0) {
                player.vel_x = MAX(player.vel_x - AIR_DRAG, 0);
            } else if (player.vel_x < 0) {
                player.vel_x = MIN(player.vel_x + AIR_DRAG, 0);
            }
        }
    }

    player.x += player.vel_x;
    player.y += player.vel_y;

    if (high_byte(player.y) >= 0xA0 && player.vel_y > 0) {
        *((unsigned char*)&player.y+1) = 0xA0;
        player.vel_y = 0;
        player_setGrounded(TRUE);
    }
}

void setPlayerAnim(unsigned char* anim) {
    if (current_anim != anim) {
        current_anim = anim;
        player.anim_timer = 0;
    }
}

#define RUN_THRESHOLD 0x400
#define JOG_THRESHOLD 0x100
void drawPlayer(void) {
    ++player.anim_timer;

    if (!player_IsGrounded()) {
        if (player.vel_y < 0)
            setPlayerAnim(player_facingRight() ? bean_jump : bean_jump_flipped);
        else
            setPlayerAnim(player_facingRight() ? bean_fall : bean_fall_flipped);
        
        goto draw;
    }

    if (player.vel_x == 0) {
        if (current_anim != bean_idle && current_anim != bean_idle_flipped)
            if (player.anim_timer > 0x8A && (current_anim == bean_stand || current_anim == bean_stand_flipped))
                setPlayerAnim(player_facingRight() ? bean_idle : bean_idle_flipped);
            else
                setPlayerAnim(player_facingRight() ? bean_stand : bean_stand_flipped);
        goto draw;
    }

    if (current_anim == bean_walk_anim[0] || current_anim == bean_walk_anim[1] || current_anim == bean_walk_anim[2] ||
        current_anim == bean_walk_anim_flipped[0] || current_anim == bean_walk_anim_flipped[1] || current_anim == bean_walk_anim_flipped[2])
        temp_u8_0 = player.anim_timer;
    else
        temp_u8_0 = 0;

    if (player.vel_x > RUN_THRESHOLD || player.vel_x < -RUN_THRESHOLD)
        temp_u8_1 = (temp_u8_0 >> 1) & 0xF;
    else if (player.vel_x > JOG_THRESHOLD || player.vel_x < -JOG_THRESHOLD)
        temp_u8_1 = (temp_u8_0 >> 2) & 0xF;
    else
        temp_u8_1 = (temp_u8_0 >> 3) & 0xF;
    
    setPlayerAnim(player_facingRight() ? bean_walk_anim[temp_u8_1 & 3] : bean_walk_anim_flipped[temp_u8_1 & 3]);
    player.anim_timer = temp_u8_0;

    draw:
    oam_meta_spr(high_byte(player.x), high_byte(player.y), current_anim);
}