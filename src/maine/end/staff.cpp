#include "src/shared/platform/pc98.hpp"
#include "src/shared/hardware/graphics.hpp"
#include "src/shared/hardware/bgimage.hpp"
#include "src/shared/hardware/v_colors.hpp"
#include "src/shared/runtime/api.hpp"
#include "src/shared/math/polar.hpp"
#include "src/shared/formats/cdg.hpp"
#include "src/shared/formats/pi.hpp"
#include "src/shared/sound/api.hpp"

// The TH04 BGIMAGER ASM owner is FAR Pascal with four 16-bit arguments.
extern "C" void pascal bgimage_put_rect_16(
    screen_x_t left, screen_y_t top, pixel_t width, pixel_t height
);

// One current image slot and the dissolve phase are shared by all eight
// staffroll bodies. The callable type is near Pascal inside MAINE_01_TEXT.
uint8_t cdg_slot;
unsigned char radial_angle;
void (near pascal *near dissolve_put_func)(
    screen_x_t base_left, screen_y_t base_top, pixel_t distance
);

extern "C" {
extern char aSff1_pi[], aSff2_pi[], aStaff[];
extern char aSff1_cdg[], aSff1b_cdg[];
extern char aSff2_cdg[], aSff2b_cdg[];
extern char aSff3_cdg[], aSff3b_cdg[];
extern char aSff4_cdg[], aSff4b_cdg[];
extern char aSff5_cdg[], aSff5b_cdg[];
extern char aSff6_cdg[], aSff6b_cdg[];
extern char aSff7_cdg[], aSff7b_cdg[];
extern char aSff8_cdg[], aSff8b_cdg[];
extern char aSff9_cdg[], aSff9b_cdg[];
}

#pragma codeseg MAINE_01_TEXT maine_01
#include "src/maine/end/staffroll_dissolve_radial_put.inl"
#include "src/maine/end/staffroll_dissolve_diagonal_put.inl"
#include "src/maine/end/staffroll_dissolve_axis_put.inl"
#include "src/maine/end/staff_bgimage_expand_put.inl"
#include "src/maine/end/staffroll_dissolve_out.inl"
#include "src/maine/end/staffroll_dissolve_in.inl"
#include "src/maine/end/staffroll_dissolve_two.inl"
#include "src/maine/end/staffroll_animate.inl"
#pragma codeseg
