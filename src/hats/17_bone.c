#include "hats/17_bone.h"
#include "lib/libnes/neslib.h"

const unsigned char bone_head_stand[] = {
    0x00, 0x00, 0x1D, 0x00 | 1,
    0x08, 0x00, 0x1E, 0x00 | 1,
    0x80
};

const unsigned char bone_head_tiltleft[] = {
    0x00, 0x01, 0x1D, 0x00 | 1,
    0x08, 0x00, 0x1E, 0x00 | 1,
    0x80
};

const unsigned char bone_head_tiltright[] = {
    0x00, 0x00, 0x1D, 0x00 | 1,
    0x08, 0x01, 0x1E, 0x00 | 1,
    0x80
};

const unsigned char bone_head_stand_flipped[] = {
    0x00, 0x00, 0x1E, 0x00 | OAM_FLIP_H | 1,
    0x08, 0x00, 0x1D, 0x00 | OAM_FLIP_H | 1,
    0x80
};

const unsigned char bone_head_tiltleft_flipped[] = {
    0x00, 0x00, 0x1E, 0x00 | OAM_FLIP_H | 1,
    0x08, 0x01, 0x1D, 0x00 | OAM_FLIP_H | 1,
    0x80
};

const unsigned char bone_head_tiltright_flipped[] = {
    0x00, 0x01, 0x1E, 0x00 | OAM_FLIP_H | 1,
    0x08, 0x00, 0x1D, 0x00 | OAM_FLIP_H | 1,
    0x80
};

const unsigned char* const bone_head[2][3] = {
    { bone_head_stand, bone_head_tiltleft, bone_head_tiltright },
    { bone_head_stand_flipped, bone_head_tiltleft_flipped, bone_head_tiltright_flipped }
};

const unsigned char bone_head_palette[] = {
    0x0F, 0x0C, 0x1C, 0x2C
};