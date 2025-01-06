#include "LIB/neslib.h"
#include "LIB/nesdoug.h"
#include "game.h"
#include "metasprites.h"
#include "metatiles.h"
#include "echo.h"
#include "title.h"

#define XPOS 6
#define YPOS 8

void main(void) {

    ppu_off();      // screen off

    bank_spr(1);
    POKE(poke_fix, 0);

    vram_adr(NAMETABLE_A);
    vram_unrle(title);
    //set_vram_buffer();
    set_scroll_y(0xB0);
    ppu_on_all();

    // variables are defined in game.h

    unground_speed = 0;
    emu_flags_prev = 1;
    player_setDirection(FACING_RIGHT);
    player_setGrounded(FALSE);

    while (TRUE) {
        ppu_wait_nmi();
        // Start of frame

        pad1_poll = pad_poll(0);
        pad1_new = get_pad_new(0);

        player_old_x = player.x;
        player_old_y = player.y;

        if (game_state == STATE_TITLE)
        {
            temp_u8_0 = (get_frame_count() >> 3) & 3;
            pal_bg(title_palettes[temp_u8_0]);
        }

        if (pad1_new & PAD_START) {
            if (game_state == STATE_TITLE) {
                //pal_clear();
                //delay(1);
                game_state = STATE_0;
                set_vram_buffer();
            } else {
                game_state = (game_state + 1) % 3;
            }

            if (game_state == STATE_0) {
                pal_bg(palette);
                pal_spr(bean_palette);
                set_scroll_y(0xFF);
                set_scroll_x(0);
                ppu_off();
                vram_adr(NAMETABLE_A);
                vram_fill(0,1024);
                ppu_on_all();
                writeEmuText();
                POKE(poke_fix + game_state + 1, 1);
            } else if (game_state == STATE_1) {
                pal_bg(echo_palette);
                pal_spr(echo_palette);
                set_scroll_x(0);
                ppu_off();
                vram_adr(NAMETABLE_A);
                vram_unrle(echo);
                ppu_on_all();
                POKE(poke_fix + game_state + 1, 2);
            } else {
                pal_bg(palette);
                pal_spr(bean_palette);
                player_setGrounded(FALSE);
                //cam_x = 0;
                set_scroll_x(cam_x);
                loadRoom();
                POKE(poke_fix + game_state + 1, 1);
            }
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
                    writeEmuText();
                }
                break;
            
            // ECHO DEMO SCENE
            case STATE_1:
                oam_clear();
                pad1_poll = demo_input[demo_counter];
                pad1_new = pad1_poll & ~demo_input[(char)demo_counter - 1];
                ++demo_counter;

                playerMovement();
                drawPlayer();
                break;
            
            // LEVEL LOADING SYSTEM
            case STATE_2:
                if (pad1_poll & PAD_SELECT) {
                    if (pad1_new & PAD_UP) {
                        ++level_pointer;
                        changeRoom();
                    } else if (pad1_new & PAD_DOWN) {
                        --level_pointer;
                        changeRoom();
                    }
                }
                oam_clear();
                playerMovement();
                drawPlayer();                
                break;
        }
        emu_flags_prev = emu_flags;
    }
}

void writeEmuText(void) {
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

void changeRoom(void) {
    // wait, what is the point of this here?
    set_vram_buffer();
    loadRoom();
}

void loadRoom(void) {
    ppu_off();

    set_data_pointer(levels[level_pointer]);
    set_mt_pointer(metatiles[(level_pointer < 2) ? 0 : 1]);

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

    for (temp_u8_0 = 0; ; temp_u8_0 += 0x20) {
        for (temp_u8_1 = 0; ; temp_u8_1 += 0x20) {
            temp_s16_0 = get_ppu_addr(1, temp_u8_1, temp_u8_0);
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

        if (!player_isDebug()) {
            player_setGrounded(TRUE);
        }
    }

    if (player_isDebug()) {
        debugMovement();
        player_collision_x = high_byte(player.x);
        player_collision_y = high_byte(player.y);
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

        player_collision_y = high_byte(player.y) - 3;
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

        // collision
        if (player_IsGrounded()) {
            if (player.vel_x > 0) {
                if (collisionRightGround()) {
                    player_collision_x = high_byte(player.x);
                    player_collision_y = high_byte(player.y);
                }
            } else if (player.vel_x < 0) {
                if (collisionLeftGround()) {
                    player_collision_x = high_byte(player.x);
                    player_collision_y = high_byte(player.y);
                }
            }
            if (collisionDown()) {
                player_collision_x = high_byte(player.x);
                player_collision_y = high_byte(player.y);
            }

        } else {
            if (collisionUp()) {
                player_collision_x = high_byte(player.x);
                player_collision_y = high_byte(player.y);
            }
            if (collisionDown()) {
                player_collision_x = high_byte(player.x);
                player_collision_y = high_byte(player.y);
            }
            if (collisionRight()) {
                player_collision_x = high_byte(player.x);
                player_collision_y = high_byte(player.y);
            }
            collisionLeft();
        }

        playerScroll();
    }
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
    
    if (game_state == STATE_2) {
        playerScroll();
    }
}

void playerScroll(void) {
    if (high_byte(player.x) > CAM_BOUND_RIGHT) {
        temp_u8_0 = MIN(6, high_byte(player.x) - CAM_BOUND_RIGHT);
        cam_x += temp_u8_0;
        high_byte(player.x) -= temp_u8_0;
    } else if (high_byte(player.x) < CAM_BOUND_LEFT) {
        temp_u8_0 = MIN(6, CAM_BOUND_LEFT - high_byte(player.x));
        cam_x -= temp_u8_0;
        high_byte(player.x) += temp_u8_0;
    }
    set_scroll_x(cam_x);
}

#define SENSOR_HORZ_OFFSET_X 2
#define SENSOR_HORZ_OFFSET_Y 2
#define SENSOR_VERT_OFFSET_X 2
#define SENSOR_VERT_OFFSET_Y 2
#define SENSOR_VERT_OFFSET_GROUND 5
#define SENSOR_SHIFT_Y 2

char evaluateCollisionLeft(void) {
    if (eject_distance <= 0) {
        high_byte(player.x) -= eject_distance;
        player.x &= 0xFF00;
        player.x += 0x80;
        player.vel_x = MAX(player.vel_x, 0);
        return 1;
    }
    return 0;
}

char collisionLeft(void) {
    sensor_position_x = player_collision_x + SENSOR_HORZ_OFFSET_X + cam_x;
    sensor_position_y = player_collision_y + SENSOR_HORZ_OFFSET_Y;

    calculateEjectDistanceLeft(&eject_distance_reserve0);

    sensor_position_y = player_collision_y + 0x0F - SENSOR_HORZ_OFFSET_Y;
    calculateEjectDistanceLeft(&eject_distance_reserve1);
    eject_distance = (eject_distance_reserve1 < eject_distance_reserve0) ? eject_distance_reserve1 : eject_distance_reserve0;

    return evaluateCollisionLeft();
}

char collisionLeftGround(void) {
    sensor_position_x = player_collision_x + SENSOR_HORZ_OFFSET_X + cam_x;
    sensor_position_y = player_collision_y + 0x0F - SENSOR_VERT_OFFSET_GROUND;
    calculateEjectDistanceLeft(&eject_distance);

    return evaluateCollisionLeft();
}

char evaluateCollisionRight(void) {
    if (eject_distance <= 0) {
        high_byte(player.x) += eject_distance;
        player.x &= 0xFF00;
        player.x += 0x80;
        player.vel_x = MIN(player.vel_x, 0);
        return 1;
    }
    return 0;
}

char collisionRight(void) {
    sensor_position_x = player_collision_x + 0x0F - SENSOR_HORZ_OFFSET_X + cam_x;
    sensor_position_y = player_collision_y + SENSOR_HORZ_OFFSET_Y;

    calculateEjectDistanceRight(&eject_distance_reserve0);

    sensor_position_y = player_collision_y + 0x0F - SENSOR_HORZ_OFFSET_Y;
    calculateEjectDistanceRight(&eject_distance_reserve1);
    eject_distance = (eject_distance_reserve1 < eject_distance_reserve0) ? eject_distance_reserve1 : eject_distance_reserve0;

    return evaluateCollisionRight();
}

char collisionRightGround(void) {
    sensor_position_x = player_collision_x + 0x0F - SENSOR_HORZ_OFFSET_X + cam_x;
    sensor_position_y = player_collision_y + 0x0F - SENSOR_VERT_OFFSET_GROUND;
    calculateEjectDistanceRight(&eject_distance);

    return evaluateCollisionRight();
}

char collisionUp(void) {
    sensor_position_x = player_collision_x + SENSOR_VERT_OFFSET_X + cam_x;
    sensor_position_y = player_collision_y + SENSOR_VERT_OFFSET_Y;

    calculateEjectDistanceUp(&eject_distance_reserve0);

    sensor_position_x = player_collision_x + 0x0F - SENSOR_VERT_OFFSET_X + cam_x;
    calculateEjectDistanceUp(&eject_distance_reserve1);

    if (eject_distance_reserve1 < eject_distance_reserve0) {
        eject_distance = /*(eject_distance_reserve1 < -8 && eject_distance_reserve0 >= -8) ? eject_distance_reserve0 : eject_distance_reserve1*/ eject_distance_reserve1;
    } else {
        eject_distance = /*(eject_distance_reserve0 < -8 && eject_distance_reserve1 >= -8) ? eject_distance_reserve1 : eject_distance_reserve0*/ eject_distance_reserve0;
    }

    if (eject_distance < 0) {
        if (eject_distance < -8) {
            return 1;
        }
        high_byte(player.y) -= eject_distance;
        player.y &= 0xFF00;
        player.vel_y = MAX(player.vel_y, 0);
        return 1;
    }
    return 0;
}

char collisionDown(void) {
    sensor_position_x = player_collision_x + SENSOR_VERT_OFFSET_X + cam_x;
    sensor_position_y = player_collision_y + 0x0F - SENSOR_VERT_OFFSET_Y + SENSOR_SHIFT_Y;

    calculateEjectDistanceDown(&eject_distance_reserve0);

    sensor_position_x = player_collision_x + 0x0F - SENSOR_VERT_OFFSET_X + cam_x;
    calculateEjectDistanceDown(&eject_distance_reserve1);

    if (eject_distance_reserve1 < eject_distance_reserve0) {
        eject_distance = (eject_distance_reserve1 < -8 && eject_distance_reserve0 >= -8) ? eject_distance_reserve0 : eject_distance_reserve1;
    } else {
        eject_distance = (eject_distance_reserve0 < -8 && eject_distance_reserve1 >= -8) ? eject_distance_reserve1 : eject_distance_reserve0;
    }

    if (eject_distance <= 0) {
        if (eject_distance < -8 || temp_u8_2 == TILE_CTYPE_FLAT && player.vel_y < 1 && !player_IsGrounded())
            return 1;
  
        high_byte(player.y) += eject_distance;
        player.y &= 0xFF00;
        player.vel_y = 0;
        player_setGrounded(TRUE);
        unground_speed = player.y - player_old_y;
        prev_tile_below = (temp_u8_2 != TILE_CTYPE_FLAT) ? temp_u8_2 : prev_tile_below;
        return 1;
    } else if (player_IsGrounded() && eject_distance <= 4) {
        high_byte(player.y) += eject_distance;
        player.y &= 0xFF00;
        player.vel_y = 0;
        unground_speed = player.y - player_old_y;
        prev_tile_below = (temp_u8_2 != TILE_CTYPE_FLAT) ? temp_u8_2 : prev_tile_below;
        return 1;
    } else if (player_IsGrounded()) {
        player_setGrounded(FALSE);

        // soften player y vel if running off of a convex edge
        if (prev_tile_below == TILE_CTYPE_CONVEXEDGE) {
            unground_speed = MIN(unground_speed, 0x100);
        }

        if (unground_speed < -0x200) {
            unground_speed = -0x200;
        }

        player.vel_y += unground_speed;
        player.y += player.vel_y;
    }
    return 0;
}

void fetchTileInfo(void) {
    player_collision_i = tile_x | (tile_y << 0x04);
    // solidity flags
    temp_u8_1 = is_solid[collision_map[player_collision_i]];
    // CTYPE (use a define in future)
    temp_u8_2 = (temp_u8_1 >> TILE_CTYPE_SHIFT & TILE_CTYPE_MASK);
}

void findCollisionSideR(void) {
    if (!(temp_u8_1 & TILE_CMODE_HORIZ)) {
        surface_distance_into_tile = 0x10;
        return;
    }

    switch (temp_u8_2) {
        case TILE_CTYPE_CONVEXEDGE:
            if (temp_u8_1 & TILE_FLIPY) {
                temp_u8_0 = collision_convexedge_side[15 - (sensor_position_y & 0x0F)];
            } else {
                temp_u8_0 = collision_convexedge_side[sensor_position_y & 0x0F];
            }
            break;
        
        case TILE_CTYPE_SHARP1:
            if (temp_u8_1 & TILE_FLIPY) {
                temp_u8_0 = collision_sharp1_side[15 - (sensor_position_y & 0x0F)];
            } else {
                temp_u8_0 = collision_sharp1_side[sensor_position_y & 0x0F];
            }
            break;
        
        case TILE_CTYPE_SHARP2:
            if (temp_u8_1 & TILE_FLIPY) {
                temp_u8_0 = collision_sharp2_side[15 - (sensor_position_y & 0x0F)];
            } else {
                temp_u8_0 = collision_sharp2_side[sensor_position_y & 0x0F];
            }
            break;
        
        case TILE_CTYPE_EXTRUDEDWALL:
            temp_u8_0 = 8;
            break;
        
        case TILE_CTYPE_FLAT:
            surface_distance_into_tile = 0;
            return;
        
        default:
            surface_distance_into_tile = 0x10;
            return;
    }

    if (temp_u8_1 & TILE_FLIPX) {
        //surface_distance_into_tile = (temp_u8_0 == 0x10) ? 0x10 : 0x00;
        surface_distance_into_tile = 0x10;
    } else {
        surface_distance_into_tile = temp_u8_0;
    }
}

void findCollisionSideL(void) {
    if (!(temp_u8_1 & TILE_CMODE_HORIZ)) {
        surface_distance_into_tile = 0x10;
        return;
    }

    switch (temp_u8_2) {
        case TILE_CTYPE_CONVEXEDGE:
            if (temp_u8_1 & TILE_FLIPY) {
                temp_u8_0 = collision_convexedge_side[15 - (sensor_position_y & 0x0F)];
            } else {
                temp_u8_0 = collision_convexedge_side[sensor_position_y & 0x0F];
            }
            break;
        
        case TILE_CTYPE_SHARP1:
            if (temp_u8_1 & TILE_FLIPY) {
                temp_u8_0 = collision_sharp1_side[15 - (sensor_position_y & 0x0F)];
            } else {
                temp_u8_0 = collision_sharp1_side[sensor_position_y & 0x0F];
            }
            break;
        
        case TILE_CTYPE_SHARP2:
            if (temp_u8_1 & TILE_FLIPY) {
                temp_u8_0 = collision_sharp2_side[15 - (sensor_position_y & 0x0F)];
            } else {
                temp_u8_0 = collision_sharp2_side[sensor_position_y & 0x0F];
            }
            break;
        
        case TILE_CTYPE_EXTRUDEDWALL:
            temp_u8_0 = 8;
            break;
        
        case TILE_CTYPE_FLAT:
            surface_distance_into_tile = 0;
            return;
        
        default:
            surface_distance_into_tile = 0x10;
            return;
    }

    if (temp_u8_1 & TILE_FLIPX) {
        surface_distance_into_tile = temp_u8_0;
    } else {
        surface_distance_into_tile = (temp_u8_0 == 0x10) ? 0x10 : 0x00;
    }
}

void findCollisionBottom(void) {
    if ((temp_u8_1 & TILE_CMODE_MASK) != TILE_CMODE_ALL) {
        surface_distance_into_tile = 0x10;
        return;
    }

    if (temp_u8_1 & TILE_FLIPX) {
        temp_u8_0 = collisions[temp_u8_2][15 - (sensor_position_x & 0x0F)];
    } else {
        temp_u8_0 = collisions[temp_u8_2][sensor_position_x & 0x0F];
    }

    if (temp_u8_1 & TILE_FLIPY) {
        surface_distance_into_tile = temp_u8_0;
    } else {
        surface_distance_into_tile = (temp_u8_0 == 0x10) ? 0x10: 0x00;
    }
}

void findCollisionSurface(void) {
    if (!(temp_u8_1 & TILE_CMODE_UPONLY)) {
        surface_distance_into_tile = 0x10;
        return;
    }

    if (temp_u8_1 & TILE_FLIPX) {
        temp_u8_0 = collisions[temp_u8_2][15 - (sensor_position_x & 0x0F)];
    } else {
        temp_u8_0 = collisions[temp_u8_2][sensor_position_x & 0x0F];
    }

    if (temp_u8_1 & TILE_FLIPY) {
        surface_distance_into_tile = (temp_u8_0 == 0x10) ? 0x10: 0x00;
    } else {
        surface_distance_into_tile = temp_u8_0;
    }
}

void calculateEjectDistanceLeft(signed char* value) {
    tile_x = sensor_position_x >> 0x04;
    tile_y = sensor_position_y >> 0x04;
    fetchTileInfo();
    findCollisionSideR();
    surface_position = (sensor_position_x & 0xF0) + (0x10 - surface_distance_into_tile);
    
    if (surface_distance_into_tile == 0x00) {
        ++tile_x;
        fetchTileInfo();

        findCollisionSideR();
        surface_position = ((sensor_position_x + 0x10) & 0xF0) + (0x10 - surface_distance_into_tile);
    } else if (surface_distance_into_tile == 0x10) {
        --tile_x;
        fetchTileInfo();

        findCollisionSideR();
        surface_position = ((sensor_position_x - 0x10) & 0xF0) + (0x10 - surface_distance_into_tile);
    }
    *value = sensor_position_x - surface_position;
}

void calculateEjectDistanceRight(signed char* value) {
    tile_x = sensor_position_x >> 0x04;
    tile_y = sensor_position_y >> 0x04;
    fetchTileInfo();
    findCollisionSideL();
    surface_position = (sensor_position_x & 0xF0) + (surface_distance_into_tile);

    if (surface_distance_into_tile == 0x00) {
        --tile_x;
        fetchTileInfo();

        findCollisionSideL();
        surface_position = ((sensor_position_x - 0x10) & 0xF0) + (surface_distance_into_tile);
    } else if (surface_distance_into_tile == 0x10) {
        ++tile_x;
        fetchTileInfo();

        findCollisionSideL();
        surface_position = ((sensor_position_x + 0x10) & 0xF0) + (surface_distance_into_tile);
    }
    *value = surface_position - (sensor_position_x + 1);
}

void calculateEjectDistanceUp(signed char* value) {
    tile_x = sensor_position_x >> 0x04;
    tile_y = sensor_position_y >> 0x04;
    fetchTileInfo();
    findCollisionBottom();
    surface_position = (sensor_position_y & 0xF0) + (0x10 - surface_distance_into_tile);

    if (surface_distance_into_tile == 0x00) {
        ++tile_y;
        fetchTileInfo();

        findCollisionBottom();
        surface_position = ((sensor_position_y + 0x10) & 0xF0) + (0x10 - surface_distance_into_tile);
    } else if (surface_distance_into_tile == 0x10) {
        --tile_y;
        fetchTileInfo();

        findCollisionBottom();
        surface_position = ((sensor_position_y - 0x10) & 0xF0) + (0x10 - surface_distance_into_tile);
    }
    *value = sensor_position_y - surface_position;
}

void calculateEjectDistanceDown(signed char* value) {
    tile_x = sensor_position_x >> 0x04;
    tile_y = sensor_position_y >> 0x04;
    fetchTileInfo();
    findCollisionSurface();
    surface_position = (sensor_position_y & 0xF0) + (surface_distance_into_tile);

    if (surface_distance_into_tile == 0x00) {
        --tile_y;
        fetchTileInfo();

        findCollisionSurface();
        surface_position = ((sensor_position_y - 0x10) & 0xF0) + (surface_distance_into_tile);
    } else if (surface_distance_into_tile == 0x10) {
        ++tile_y;
        fetchTileInfo();

        findCollisionSurface();
        surface_position = ((sensor_position_y + 0x10) & 0xF0) + (surface_distance_into_tile);
    }
    *value = surface_position - (sensor_position_y + 1);
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