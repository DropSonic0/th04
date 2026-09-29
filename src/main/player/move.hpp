#ifndef TH04_MAIN_PLAYER_MOVE_HPP
#define TH04_MAIN_PLAYER_MOVE_HPP

#include "th04/hardware/input.h"
#include "src/main/math/subpixel.hpp"

// All values are signed. (Yes, allowing you to invert the controls with
// negative values!)
#if (GAME == 5)
extern subpixel_t playchar_speed_aligned;
extern subpixel_t playchar_speed_diagonal;
#else
static const subpixel_t playchar_speed_aligned = TO_SP(4);
static const subpixel_t playchar_speed_diagonal = TO_SP(3);
#endif

enum move_ret_t {
	MOVE_INVALID = 0,
	MOVE_VALID = 1,
	MOVE_NOINPUT = 2,
};

// Applies the given [input] to the player's velocity. Both games assume this
// is (0, 0) prior to calling the function.
move_ret_t pascal near player_move(input_t input);

#endif
