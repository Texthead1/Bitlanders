#include "hats/22_cowboy.h"
#include "../lib/libnes/neslib.h"

const unsigned char cowboy_hat_stand[] = {
    0x00, 0x00, 0x0D, 0x00 | 1,
    0x08, 0x00, 0x0E, 0x00 | 1,
    0x80
};

const unsigned char cowboy_hat_tiltleft[] = {
    0x00, 0x01, 0x0D, 0x00 | 1,
    0x08, 0x00, 0x0E, 0x00 | 1,
    0x80
};

const unsigned char cowboy_hat_tiltright[] = {
    0x00, 0x00, 0x0D, 0x00 | 1,
    0x08, 0x01, 0x0E, 0x00 | 1,
    0x80
};

const unsigned char cowboy_hat_stand_flipped[] = {
    0x00, 0x00, 0x0E, 0x00 | OAM_FLIP_H | 1,
    0x08, 0x00, 0x0D, 0x00 | OAM_FLIP_H | 1,
    0x80
};

const unsigned char cowboy_hat_tiltleft_flipped[] = {
    0x00, 0x00, 0x0E, 0x00 | OAM_FLIP_H | 1,
    0x08, 0x01, 0x0D, 0x00 | OAM_FLIP_H | 1,
    0x80
};

const unsigned char cowboy_hat_tiltright_flipped[] = {
    0x00, 0x01, 0x0E, 0x00 | OAM_FLIP_H | 1,
    0x08, 0x00, 0x0D, 0x00 | OAM_FLIP_H | 1,
    0x80
};

const unsigned char* const cowboy_hat[2][3] = {
    { cowboy_hat_stand, cowboy_hat_tiltleft, cowboy_hat_tiltright },
    { cowboy_hat_stand_flipped, cowboy_hat_tiltleft_flipped, cowboy_hat_tiltright_flipped }
};

const unsigned char cowboy_hat_palette[] = {
    0x0F, 0x06, 0x26, 0x38
};

/*const unsigned char cowboy_hat_palette[] = {
    0x0F, 0x07, 0x16, 0x27
};*/