#ifndef TH04_MAIN_CIRCLE_HPP
#define TH04_MAIN_CIRCLE_HPP

#include "src/main/math/subpixel.hpp"

// Color used to render all currently active circles.
extern vc_t circles_color;

// [center_x] and [center_y] are passed as subpixels, but the implementation
// stores them at pixel precision because grcg_circle() has no subpixel form.
void pascal circles_add_growing(subpixel_t center_x, subpixel_t center_y);
void pascal circles_add_shrinking(subpixel_t center_x, subpixel_t center_y);

void near circles_update(void);
void near circles_render(void);

#endif
