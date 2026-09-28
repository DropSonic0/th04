#ifndef TH04_MAIN_MATH_VECTOR_HPP
#define TH04_MAIN_MATH_VECTOR_HPP

#include "src/main/math/subpixel.hpp"

extern "C" {
void pascal vector2(
	int &ret_x,
	int &ret_y,
	unsigned char angle,
	int length
);

void pascal vector2_between_plus(
	int x1,
	int y1,
	int x2,
	int y2,
	unsigned char plus_angle,
	int &ret_x,
	int &ret_y,
	int length
);

void pascal near vector2_near(
	SPPoint near &ret,
	unsigned char angle,
	subpixel_t length
);

void pascal vector2_at(
	SPPoint near &ret,
	subpixel_t origin_x,
	subpixel_t origin_y,
	subpixel_t length,
	int angle
);
}

#endif
