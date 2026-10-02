#ifndef TH04_MAIN_POINTNUM_POINTNUM_HPP
#define TH04_MAIN_POINTNUM_POINTNUM_HPP

#include "src/main/playfld.hpp"

#define POINTNUM_POPUP_DISTANCE (12.0f)
#define POINTNUM_POPUP_FRAMES 24
#define POINTNUM_FRAMES 36

#if (GAME == 5)
#define POINTNUM_DIGITS 5
#define POINTNUM_YELLOW_COUNT 80
#else
#define POINTNUM_DIGITS 4
#define POINTNUM_YELLOW_COUNT 200
#define POINTNUM_TIMES_2_W (POINTNUM_W * 2)
#endif

#define POINTNUM_WHITE_COUNT 200
#define POINTNUM_COUNT (POINTNUM_WHITE_COUNT + POINTNUM_YELLOW_COUNT)

// [digits_lebcd] excludes the final zero digit. [width] includes that digit
// and records the number of active digits counted from the end of the array.
struct pointnum_t {
	char flag;
	unsigned char age;
	SPPoint center_cur;
	Subpixel center_prev_y;
#if (GAME == 5)
	upixel_t width;
	unsigned char digits_lebcd[POINTNUM_DIGITS];
#else
	unsigned char digits_lebcd[POINTNUM_DIGITS];
	upixel_t width;
	bool times_2;
#endif
	int8_t padding;
};

// White point numbers occupy the first ring-buffer range, followed by yellow
// numbers. These are owned by the existing pointnum assembly producers.
extern pointnum_t pointnums[POINTNUM_COUNT];
extern unsigned char pointnum_yellow_p;
extern unsigned char pointnum_white_p;
extern bool pointnum_times_2;

#if (GAME == 5)
upixel_t pascal near pointnum_digits_set(
	unsigned char near *last_digit, uint16_t points
);
#else
void pascal near pointnum_digits_set(
	unsigned char near *last_digit, uint16_t points
);
#endif

#ifdef __cplusplus
extern "C" {
#endif
void pascal near pointnums_add_white(
	subpixel_t center_x, subpixel_t center_y, uint16_t points
);
void pascal near pointnums_add_yellow(
	subpixel_t center_x, subpixel_t center_y, uint16_t points
);
void pascal near pointnums_init(void);
void pascal near pointnums_update(void);
void pascal near pointnums_render(void);
#ifdef __cplusplus
}
#endif
void near pointnums_invalidate(void);

// Pointers collected for the current frame. The first yellow pointer marks
// the boundary between the white and yellow portions of the ring.
extern pointnum_t near *pointnums_alive[POINTNUM_COUNT + 1];
extern pointnum_t near *pointnum_first_yellow_alive;

#define pointnum_put(left, top, numeral) \
	_CX = numeral; \
	pointnum_put_raw(top, left);
void __fastcall near pointnum_put_raw(vram_y_t top, screen_x_t left);

#endif
