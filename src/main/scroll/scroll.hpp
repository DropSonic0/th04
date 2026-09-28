#ifndef TH04_MAIN_SCROLL_SCROLL_HPP
#define TH04_MAIN_SCROLL_SCROLL_HPP

#include "src/main/math/subpixel.hpp"

// Current line at the top of VRAM.
extern vram_y_t scroll_line;

// Advanced by one pixel for every 16 subpixel units.
extern SubpixelLength8 scroll_subpixel_line;
extern SubpixelLength8 scroll_speed;

extern vram_y_t scroll_line_on_page[PAGE_COUNT];
extern Subpixel scroll_last_delta;
extern bool scroll_active;

#pragma codeseg mai_TEXT main_01

extern "C" vram_y_t pascal near scroll_subpixel_y_to_vram_seg1(subpixel_t y);

#pragma codeseg

extern "C" {
vram_y_t pascal near scroll_subpixel_y_to_vram_seg3(subpixel_t y);
vram_y_t pascal near scroll_subpixel_y_to_vram_always(subpixel_t y);
}

#endif
