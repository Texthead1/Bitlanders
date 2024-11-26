#include "LIB/neslib.h"
#include "LIB/nesdoug.h"
#include "game.h"
#include "metasprites.h"
#include "metatiles.h"
#include "echo.h"

#define XPOS 6
#define YPOS 8

void main(void) {

    ppu_off();      // screen off

    bank_spr(1);

    pal_spr(bean_palette);
    pal_bg(palette); //	load the BG palette

    set_vram_buffer();
    set_scroll_y(0xff); // shift the bg down 1 pixel
    ppu_on_all();   // let's turn back on the screen and continue with what we need

    // variables are defined in game.h

    emu_flags_prev = 1;
    player_setDirection(FACING_RIGHT);
    player_setGrounded(FALSE);
    player.height = 15;
    player.width = 15;

    while (TRUE) {
        ppu_wait_nmi();
        // Start of frame
        debug_draw_toggle = 0;

        pad1_poll = pad_poll(0);
        pad1_new = get_pad_new(0);


        if (pad1_new & PAD_START) {
            game_state = (game_state + 1) % 3;

            if (game_state == STATE_0) {
                set_vram_buffer();
                pal_bg(palette);
                pal_spr(bean_palette);
                emu_flags_prev = emu_flags + 1;
                ppu_off();
                //vram_adr(NAMETABLE_A);
                //vram_fill(0, 1024);
                ppu_on_all();
                
            } else if (game_state == STATE_1) {
                pal_bg(echo_palette);
                pal_spr(echo_palette);
                ppu_off();
                vram_adr(NAMETABLE_A);
                vram_unrle(echo);
                ppu_on_all();

            } else {
                pal_bg(palette);
                pal_spr(bean_palette);
                player_setGrounded(FALSE);
                loadRoom();
            }
            POKE(poke_fix + game_state, game_state);
        }

        switch (game_state) {
            // EMULATOR STATE DEBUGGER
            case STATE_0:
                temp_u8_0 = game_EmuState();
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
                        switch (temp_u8_0) {
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
                break;
            
            // ECHO DEMO SCENE
            case STATE_1:
                oam_clear();
                pad1_poll = demo_input[demo_counter];
                pad1_new = demo_counter != 0 ? (pad1_poll & ~demo_input[demo_counter - 1]) : pad1_poll;
                ++demo_counter;

                playerMovement();
                drawPlayer();
                break;
            
            // LEVEL LOADING SYSTEM
            case STATE_2:
                if (pad1_poll & PAD_SELECT && pad1_new & PAD_UP && level_pointer != 0xFF) {
                    ++level_pointer;
                    set_vram_buffer();
                    loadRoom();
                } else if (pad1_poll & PAD_SELECT && pad1_new & PAD_DOWN && level_pointer != 0x00) {
                    --level_pointer;
                    set_vram_buffer();
                    loadRoom();
                }

                oam_clear();

                playerMovement();
                drawPlayer();                
                break;
        }
        emu_flags_prev = emu_flags;
    }
}

void loadRoom(void) {
    ppu_off();

    set_data_pointer(levels[level_pointer]);
    set_mt_pointer(metatiles[level_pointer >> 1]);

    for (temp_u8_0 = 0; ; temp_u8_0 += 0x20) {
        for (temp_u8_1 = 0; ; temp_u8_1 += 0x20) {
            temp_s16_0 = get_ppu_addr(0x00, temp_u8_1, temp_u8_0);
            temp_u8_2 = (temp_u8_0 & 0xF0) + (temp_u8_1 >> 0x04);
            buffer_4_mt(temp_s16_0, temp_u8_2);
            flush_vram_update2();
            if (temp_u8_1 == 0xE0) break;
        }
        if (temp_u8_0 == 0xE0) break;
    }

    //set_vram_update(NULL);

    memcpy(collision_map, levels[level_pointer], 240);
    ppu_on_all();
}

void playerMovement(void) {
    if (pad1_poll & PAD_SELECT && pad1_new & PAD_B) {
        player_setDebugMode(!player_isDebug());

        if (!player_isDebug())
            player_setGrounded(TRUE);
    }

    if (player_isDebug()) {
        debugMovement();
        return;
    }

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
        player_collision_y = high_byte(player.y) - 1;
        if (pad1_new & PAD_A && !collisionUp()) {
            player_setGrounded(FALSE);
            player.vel_y -= JUMP_FORCE;
        }
    } else {
        if (player.vel_y < -0x280 && !(pad1_poll & PAD_A))
            player.vel_y = -0x280;
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

    if (game_state != STATE_2) {
        if (high_byte(player.y) >= 0xA0 && player.vel_y > 0) {
            high_byte(player.y) = 0xA0;
            player.vel_y = 0;
            player_setGrounded(TRUE);
        }
    } else {
        player_collision_x = high_byte(player.x);
        player_collision_y = high_byte(player.y);

        if (player_IsGrounded()) {
            playerCollisionGround();
        } else {
            playerCollisionAir();
        }
    }
}

void playerCollisionGround(void) {
    // horizontal collision
    if (player.vel_x > 0) { // handle going right
        if (collisionRight()) {
            high_byte(player.x) = high_byte(player.x) - eject_distance_right;
            player.x &= 0xFF00;
            player.vel_x = 0;
        }
    } else if (player.vel_x < 0) {  // handle going left
        if (collisionLeft()) {
            high_byte(player.x) = high_byte(player.x) - eject_distance_left;
            player.x &= 0xFF00;
            player.vel_x = 0;
        }
    }

    player_collision_x = high_byte(player.x);

    if (player.vel_y >= 0) {
        if (collisionDown()) {
            high_byte(player.y) = high_byte(player.y) - eject_distance_down;
            player.y &= 0xFF00;
            player.vel_y = 0;
        }
        ++player_collision_y;
        if (!collisionDown()) {
            player_setGrounded(FALSE);
        }
    } else if (player.vel_y < 0) {
        if (collisionUp()) {
            high_byte(player.y) = high_byte(player.y) - eject_distance_up;
            player.y &= 0xFF00;
            player.vel_y = 0;
        }
    }
}

void playerCollisionAir(void) {
    if (player.vel_y > 0) {
        if (collisionDown()) {
            high_byte(player.y) = high_byte(player.y) - eject_distance_down;
            player.y &= 0xFF00;
            player.vel_y = 0;
            player_setGrounded(TRUE);
        }
    } else if (player.vel_y < 0) {
        if (collisionUp()) {
            high_byte(player.y) = high_byte(player.y) - eject_distance_up;
            player.y &= 0xFF00;
            player.vel_y = 0;
        }
    }

    player_collision_y = high_byte(player.y);

    // horizontal collision
    if (player.vel_x > 0) { // handle going right
        if (collisionRight()) {
            high_byte(player.x) = high_byte(player.x) - eject_distance_right;
            player.x &= 0xFF00;
            player.vel_x = 0;
            //if (player_IsGrounded() && !collisionDown()) {
            //    player_setGrounded(FALSE);
            //}
        }
    } else if (player.vel_x < 0) {  // handle going left
        if (collisionLeft()) {
            high_byte(player.x) = high_byte(player.x) - eject_distance_left;
            player.x &= 0xFF00;
            player.vel_x = 0;
            //if (player_IsGrounded() && !collisionDown()) {
            //    player_setGrounded(FALSE);
            //}
        }
    }
}

char collisionLeft(void) {
    temp_u16_0 = player_collision_x;
    player_temp_x = (char)temp_u16_0;
    temp_u8_1 = temp_u16_0 >> 0x08;

    eject_distance_left = player_temp_x | 0xF0;
    player_temp_y = player_collision_y;

    if (checkCollision() & TILE_CMODE_HORIZ)
        return 1;

    player_temp_y = (player_collision_y + player.height);

    if (checkCollision() & TILE_CMODE_HORIZ)
        return 1;

    return 0;
}

char collisionRight(void) {
    temp_u16_0 = player_collision_x + player.width;
    player_temp_x = (char)temp_u16_0;
    temp_u8_1 = temp_u16_0 >> 0x08;

    eject_distance_right = (player_temp_x + 1) & 0x0F;
    player_temp_y = player_collision_y;

    if (checkCollision() & TILE_CMODE_HORIZ)
        return 1;

    player_temp_y = (player_collision_y + player.height);

    if (checkCollision() & TILE_CMODE_HORIZ)
        return 1;

    return 0;
}

char collisionUp(void) {
    temp_u16_0 = player_collision_x;
    player_temp_x = (char)temp_u16_0;
    temp_u8_1 = temp_u16_0 >> 0x08;

    player_temp_y = player_collision_y;
    eject_distance_up = player_temp_y | 0xF0;
    if (checkCollision() & TILE_CMODE_ALL)
        return 1;
    
    temp_u16_0 = player_collision_x + player.width;
    player_temp_x = (char)temp_u16_0;
    temp_u8_1 = temp_u16_0 >> 0x08;

    if (checkCollision() & TILE_CMODE_ALL)
        return 1;
    
    return 0;
}

char collisionDown(void) {
    temp_u16_0 = player_collision_x;
    player_temp_x = (char)temp_u16_0;
    temp_u8_1 = temp_u16_0 >> 0x08;

    player_temp_y = player_collision_y + player.height;

    eject_distance_down = (player_temp_y + 1) & 0x0F;
    if (checkCollision() & TILE_CMODE_UPONLY)
        return 1;
    
    temp_u16_0 = player_collision_x + player.width;
    player_temp_x = (char)temp_u16_0;
    temp_u8_1 = temp_u16_0 >> 0x08;

    if (checkCollision() & TILE_CMODE_UPONLY)
        return 1;
    
    return 0;
}

char collisionDown2(void) {
    temp_u16_0 = player_collision_x + 2;
    player_temp_x = (char)temp_u16_0;
    temp_u8_1 = temp_u16_0 >> 0x08;

    player_temp_y = player_collision_y + player.height + 2;

    if (checkCollision())
        return 1;
    
    temp_u16_0 = player_collision_x + player.width - 2;
    player_temp_x = (char)temp_u16_0;
    temp_u8_1 = temp_u16_0 >> 0x08;

    if (checkCollision())
        return 1;
    
    return 0;
}

void toString(int num, char* str) {
    i = 0;
    j = 0;
    digits[0] = 0;
    digits[1] = 0;
    digits[2] = 0;

    if (num == 0) {
        str[0] = '0';
        str[1] = '\0';
        return;
    }

    while (num != 0) {
        digits[i++] = num % 10;
        num = num / 10;
    }

    for (j = 0; j < i; j++) {
        str[j] = '0' + digits[i - j - 1];
    }

    str[j] = '\0';
}

void toHexString(int num, char* str) {
    i = 0;
    j = 0;
    digits[0] = 0;
    digits[1] = 0;
    digits[2] = 0;

    if (num == 0) {
        str[0] = '0';
        str[1] = '0';
        str[2] = '\0';
        return;
    }

    while (num != 0 && i < 2) {
        digits[i++] = num & 0xF;
        num = num / 0x10;
    }

    if (i < 2) {
        digits[i++] = 0;
    }

    for (j = 0; j < i; j++) {
        str[j] = hexDigits[digits[i - j - 1]];
    }

    str[j] = '\0';
}

char checkCollision(void) {
    if (player_temp_y >= 0xF0)
        return 0;
    
    temp_u8_0 = (player_temp_x >> 0x04) + (player_temp_y & 0xF0);

    temp_u8_1 = is_solid[collision_map[temp_u8_0]];
    temp_u8_2 = (temp_u8_1 >> TILE_CTYPE_SHIFT & TILE_CTYPE_MASK);

    if (temp_u8_1 & TILE_FLIPX) {
        eject_distance_down -= collisions[temp_u8_2][15 - (player_temp_x & 0x0F)];
    } else {
        eject_distance_down -= collisions[temp_u8_2][player_temp_x & 0x0F];
    }

    if (eject_distance_down > 0x80) {
        return 0;
    }
    return temp_u8_1;
}


void debugMovement(void) {
    if (pad1_poll & PAD_LEFT)
		--high_byte(player.x);
	else if (pad1_poll & PAD_RIGHT)
		++high_byte(player.x);
	
	if (pad1_poll & PAD_UP)
		--high_byte(player.y);
	else if (pad1_poll & PAD_DOWN)
		++high_byte(player.y);
}

void setPlayerAnim(unsigned char* anim) {
    if (current_anim != anim) {
        current_anim = anim;
        player.anim_timer = 0;
    }
}

#define RUN_THRESHOLD 0x300
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