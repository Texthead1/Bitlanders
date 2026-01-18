#include "hats/11_tophat.h"
#include "hats/17_bone.h"
#include "hats/22_cowboy.h"

//typedef unsigned char hat_t;

#define HAT_COUNT 3

unsigned char hat_pointer = 0;
const unsigned char* const* const hats[] = { cowboy_hat, cowboy_hat, cowboy_hat };
const unsigned char* const hat_palettes[] = { cowboy_hat_palette, bone_head_palette, top_hat_palette };