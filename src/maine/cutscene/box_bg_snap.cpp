#include <stddef.h>

#include "src/maine/cutscene/state.hpp"
#include "src/shared/platform/pc98.hpp"
#include "src/shared/hardware/graphics.hpp"
#include "src/shared/runtime/api.hpp"

typedef int vram_offset_t;

static const screen_x_t BOX_LEFT = 80;
static const screen_y_t BOX_TOP = 320;
static const pixel_t BOX_H = 64;
static const vram_byte_amount_t BOX_VRAM_W = 60;

extern unsigned char far *VRAM_PLANE_B;
extern unsigned char far *VRAM_PLANE_R;
extern unsigned char far *VRAM_PLANE_G;
extern unsigned char far *VRAM_PLANE_E;

#pragma codeseg CUTSCENE_TEXT cutscene_01
void near box_bg_free(void);
#include "src/maine/cutscene/box_bg_snap.inl"
#pragma codeseg
