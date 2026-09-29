#ifndef TH04_MAIN_DIALOG_STATE_HPP
#define TH04_MAIN_DIALOG_STATE_HPP

#include "src/shared/platform/pc98.hpp"

// GAME=4 keeps dialog coordinates in display pixels.  The historical
// shared dialog producer uses the same two-word layout for its cursor.
typedef screen_x_t dialog_x_t;
typedef screen_y_t dialog_y_t;

struct dialog_cursor_t {
	dialog_x_t x;
	dialog_y_t y;
};

extern dialog_cursor_t dialog_cursor;

enum dialog_side_t {
	SIDE_PLAYCHAR = 0,
	SIDE_BOSS = 1,

	_dialog_side_t_FORCE_INT16 = 0x7FFF
};

extern dialog_side_t dialog_side;

#endif
