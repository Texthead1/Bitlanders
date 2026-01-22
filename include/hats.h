#include "hats/11_tophat.h"
#include "hats/17_bone.h"
#include "hats/22_cowboy.h"

//typedef unsigned char hat_t;

#define HAT_COUNT 3

unsigned char current_hat = 0;
const unsigned char* const (* const hats[])[3] = { cowboy_hat, bone_head, top_hat };
const unsigned char* const hat_palettes[] = { cowboy_hat_palette, bone_head_palette, top_hat_palette };