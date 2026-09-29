#pragma option -zCMOTION_3_TEXT -zPmain_03

#include "x86real.h"
#include "src/main/math/subpixel.hpp"
#include "src/main/scroll/scroll.hpp"

static inline vram_y_t scroll_y_roll(vram_y_t y)
{
    if(y < 0) {
        y += RES_Y;
    } else if(y >= RES_Y) {
        y -= RES_Y;
    }
    return y;
}

// The original MAIN keeps these two seg3 helpers in a separate physical
// producer from the seg1 helper. Preserve that ownership and the Pascal-near
// ABI while expressing the same signed single-wrap arithmetic in C++.
extern "C" vram_y_t pascal near scroll_subpixel_y_to_vram_seg3(subpixel_t y)
{
    y = TO_PIXEL(y);
    if(scroll_active) {
        y += scroll_line;
    }
    return scroll_y_roll(y);
}

extern "C" vram_y_t pascal near scroll_subpixel_y_to_vram_always(subpixel_t y)
{
    y = TO_PIXEL(y) + scroll_line;
    return scroll_y_roll(y);
}
