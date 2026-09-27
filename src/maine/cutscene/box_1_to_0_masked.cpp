#include "src/shared/hardware/graphics.hpp"
#include "src/maine/cutscene/box_mask.hpp"

typedef unsigned int vram_offset_t;
typedef unsigned short egc_temp_t;

static const screen_x_t BOX_LEFT = 80;
static const screen_y_t BOX_TOP = 320;
static const screen_y_t BOX_BOTTOM = 384;
static const pixel_t BOX_W = 480;

extern const unsigned short BOX_MASKS[][4];
extern unsigned char far *VRAM_PLANE_B;


#if defined(TH04P)
#pragma codeseg CUTSCENE_TEXT GROUP_01
#else
#pragma codeseg CUTSCENE_TEXT cutscene_01
#endif
#include "src/maine/cutscene/box_1_to_0_masked.inl"
#pragma codeseg
