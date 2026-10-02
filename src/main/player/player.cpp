#pragma option -zPmain_01

#include "th04/main/player/player.hpp"

static const pixel_t MOVE_MARGIN_LEFT = 8;
static const pixel_t MOVE_MARGIN_TOP = 8;
static const pixel_t MOVE_MARGIN_RIGHT = 8;
static const pixel_t MOVE_MARGIN_BOTTOM = 16;

#define clamp(v, min, max) \
	v; \
	if(static_cast<subpixel_t>(v) < to_sp(min)) { \
		v = to_sp(min); \
	} else if(static_cast<subpixel_t>(v) > to_sp(max)) { \
		v = to_sp(max); \
	}

void near player_pos_update_and_clamp(void)
{
#ifdef __TURBOC__
	/* _DX:_AX = */ player_pos.update_seg1();
	_AX = clamp(_AX, (0 + MOVE_MARGIN_LEFT), (PLAYFIELD_W - MOVE_MARGIN_RIGHT));
	_DX = clamp(_DX, (0 + MOVE_MARGIN_TOP), (PLAYFIELD_H - MOVE_MARGIN_BOTTOM));
	player_pos.cur.x.v = _AX;
	player_pos.cur.y.v = _DX;
#else
	player_pos.update_seg1();
	if(player_pos.cur.x.v < to_sp(MOVE_MARGIN_LEFT)) {
		player_pos.cur.x.v = to_sp(MOVE_MARGIN_LEFT);
	} else if(player_pos.cur.x.v > to_sp(PLAYFIELD_W - MOVE_MARGIN_RIGHT)) {
		player_pos.cur.x.v = to_sp(PLAYFIELD_W - MOVE_MARGIN_RIGHT);
	}
	if(player_pos.cur.y.v < to_sp(MOVE_MARGIN_TOP)) {
		player_pos.cur.y.v = to_sp(MOVE_MARGIN_TOP);
	} else if(player_pos.cur.y.v > to_sp(PLAYFIELD_H - MOVE_MARGIN_BOTTOM)) {
		player_pos.cur.y.v = to_sp(PLAYFIELD_H - MOVE_MARGIN_BOTTOM);
	}
#endif
}