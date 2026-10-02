/// MAIN dialog translation unit
/// ---------------------------

#include <ctype.h>
#include <stddef.h>

#include "decomp.hpp"
#include "shiftjis.hpp"
#include "src/shared/runtime/api.hpp"
#include "src/shared/hardware/graphics.hpp"
#include "src/shared/hardware/frame_delay.hpp"
#include "src/shared/formats/cdg.hpp"
#include "src/shared/formats/tile.hpp"
#include "src/shared/sound/api.hpp"

#include "src/main/hardware/planar.hpp"
#include "src/main/hardware/grcg.hpp"
#include "src/main/hardware/input.hpp"
#include "src/main/math/subpixel.hpp"
#include "th04/common.h"
#include "src/main/playfld.hpp"
#include "src/main/formats/cdg.hpp"
#include "src/main/formats/dialog.hpp"
#include "src/main/formats/map.hpp"
#include "src/main/formats/std.hpp"
#include "src/main/ems.hpp"
#include "src/main/bg.hpp"
#include "src/main/boss/boss.hpp"
#include "src/main/dialog/dialog.hpp"
#include "src/main/dialog/state.hpp"
#include "src/main/scroll/scroll.hpp"
#include "src/main/stage/stage.hpp"
#include "src/main/playchar.hpp"
#include "th04/main/frames.h"
#include "th04/main/tile/tile.hpp"
#include "src/main/sprites/main_cdg.hpp"
#include "src/main/sprites/main_pat.hpp"
#include "src/main/shiftjis/fns.hpp"

// The low-level copy helper is an existing shared ASM entry. Keeping its
// declaration local avoids pretending that it belongs to dialog storage.
extern void near egc_start_copy_noframe(void);
extern unsigned char page_front;
extern unsigned char page_back;
extern nearfunc_t_near overlay1;
extern unsigned char bgm_title_id;
extern void near overlay_wipe(void);
extern void pascal near overlay_boss_bgm_update_and_render(void);

#define FLAGS_ZERO (_FLAGS & 0x40)

// Metrics and coordinate conversion
// ----------------------------------

inline tram_x_t to_dialog_x(screen_x_t screen_x) {
	return screen_x;
}

inline tram_y_t to_dialog_y(screen_y_t screen_y) {
	return screen_y;
}

inline tram_x_t to_tram_x(dialog_x_t dialog_x) {
	return (dialog_x / GLYPH_HALF_W);
}

inline tram_y_t to_tram_y(dialog_y_t dialog_y) {
	return (dialog_y / GLYPH_H);
}

static const pixel_t BOX_W = 320;
static const pixel_t BOX_H = 48;
static const tram_cell_amount_t BOX_TRAM_H = (BOX_H / GLYPH_H);

#define BOX_TILE_W 16
#define BOX_TILE_H 4
#define BOX_TILE_COUNT 3

#define BOX_TILE_VRAM_W (BOX_TILE_W / BYTE_DOTS)
#define BOX_VRAM_W (BOX_W / BYTE_DOTS)
static const size_t BOX_TILE_SIZE = (BOX_TILE_VRAM_W * BOX_TILE_H);

static const pixel_t MARGIN = 16;
static const pixel_t TEXT_W = (PLAYFIELD_W - MARGIN - FACE_W);
static const tram_cell_amount_t TEXT_TRAM_W = (TEXT_W / GLYPH_HALF_W);

static const screen_x_t BOX_PLAYCHAR_LEFT = (PLAYFIELD_RIGHT - MARGIN - BOX_W);
static const screen_y_t BOX_PLAYCHAR_TOP = (PLAYFIELD_BOTTOM - MARGIN - BOX_H);
static const screen_x_t TEXT_PLAYCHAR_LEFT = (
	PLAYFIELD_RIGHT - MARGIN - TEXT_W
);
static const screen_y_t TEXT_PLAYCHAR_TOP = BOX_PLAYCHAR_TOP;
static const screen_x_t FACE_PLAYCHAR_LEFT = (TEXT_PLAYCHAR_LEFT - FACE_W);
static const screen_y_t FACE_PLAYCHAR_TOP = (
	PLAYFIELD_BOTTOM - MARGIN - FACE_H
);

static const screen_x_t BOX_BOSS_LEFT = (PLAYFIELD_LEFT + MARGIN);
static const screen_y_t BOX_BOSS_TOP = (BOX_PLAYCHAR_TOP - FACE_H);
static const screen_x_t TEXT_BOSS_LEFT = BOX_BOSS_LEFT;
static const screen_y_t TEXT_BOSS_TOP = BOX_BOSS_TOP;
static const screen_x_t FACE_BOSS_LEFT = (TEXT_BOSS_LEFT + TEXT_W);
static const screen_y_t FACE_BOSS_TOP = (FACE_PLAYCHAR_TOP - FACE_H);

// The data owner is separate so this TU only provides the reference to it.
extern const dot_rect_t(BOX_TILE_W, BOX_TILE_H) near BOX_TILES[BOX_TILE_COUNT];

// Shared dialog helpers that were historically emitted before dialog_op.
// These declarations and inline bodies are kept in the physical TU so the
// product source has one truthful owner instead of a copied scaffold file.
void near playfield_copy_front_to_back(void);
void pascal near dialog_face_unput_8(uscreen_x_t left, uvram_y_t top);

#define dialog_box_wipe(left, top) { \
	tram_y_t y = to_tram_y(top); \
	while(y < (to_tram_y(top) + BOX_TRAM_H)) { \
		tram_x_t x = to_tram_x(left); \
		while(x < (to_tram_x(left) + TEXT_TRAM_W)) { \
			text_putca(x, y, ' ', TX_WHITE); \
			x++; \
		} \
		y++; \
	} \
}

void near dialog_box_fade_in_animate(void);

inline void dialog_text_put(shiftjis_t* const& text) {
	text_putsa(
		to_tram_x(dialog_cursor.x), to_tram_y(dialog_cursor.y), (const char *)text, TX_WHITE
		);
	dialog_cursor.x += to_dialog_x(GLYPH_FULL_W);
}

#define dialog_delay(speedup_cycle) { \
	if(key_det == INPUT_NONE) { \
		frame_delay(2); \
	} else if(speedup_cycle & 1) { \
		frame_delay(1); \
	} \
}

inline void dialog_pre(void) {
	overlay_wipe();
	palette_settone(100);
	graph_accesspage(page_front);
	dialog_box_fade_in_animate();
	playfield_copy_front_to_back();
}

inline void dialog_post(void) {
	graph_accesspage(page_back);
	frame_delay(1);
}

template <class T> inline void dialog_op_gaiji(const T& c) {
	gaiji_putca(
		to_tram_x(dialog_cursor.x), to_tram_y(dialog_cursor.y), c, TX_WHITE
	);
	dialog_cursor.x += to_dialog_x(GAIJI_W);
}

inline void dialog_op_linebreak(void) {
	dialog_cursor.y += to_dialog_y(GLYPH_H);
	dialog_cursor.x = ((dialog_side == SIDE_PLAYCHAR)
		? to_dialog_x(TEXT_PLAYCHAR_LEFT)
		: to_dialog_x(TEXT_BOSS_LEFT)
	);
}

#define dialog_op_super_clean_stage() { \
	super_clean(PAT_STAGE, (PAT_STAGE_last + 1)); \
}

// Existing maintained low-level dialog bodies retain their historical order
// inside the shared portion of the physical translation unit.
#include "src/main/dialog/render_primitives.inl"
#include "src/main/dialog/fade_in.inl"
#include "src/main/dialog/stage_transition.inl"

// The original source included the common plaintext-script header here. Its
// small MAIN surface is recovered locally to keep product compilation free of
// a direct TH03/ReC98 include.
enum script_ret_t {
	CONTINUE = 0,
	STOP = -1,
};

extern int script_param_number_default;

#define script_p dialog_p
#define str_sep_control_or_space(c) \
	(iscntrl(c) || ((c) == ' '))

#define script_param_read_fn(ret, temp_len, temp_c) { \
	for(temp_len = 0; temp_len < (PF_FN_LEN - 1); temp_len++) { \
		temp_c = *script_p; \
		script_p++; \
		if(str_sep_control_or_space(temp_c)) { \
			break; \
		} \
		ret[temp_len] = temp_c; \
	} \
	ret[temp_len] = '\0'; \
}

#define script_op_bgm(stop_before_load, temp_c, temp_fn, temp_len) { \
	temp_c = *script_p; \
	if(temp_c == '$') { \
		script_p++; \
		snd_kaja_func(KAJA_SONG_STOP, 0); \
		if(stop_before_load) { \
			return CONTINUE; \
		} \
	} else if(temp_c == '*') { \
		script_p++; \
		snd_kaja_func(KAJA_SONG_PLAY, 0); \
		if(stop_before_load) { \
			return CONTINUE; \
		} \
	} else if(temp_c == ',') { \
		script_p++; \
		script_param_read_fn(temp_fn, temp_len, temp_c); \
		if(stop_before_load) { \
			snd_kaja_func(KAJA_SONG_STOP, 0); \
		} \
		snd_load(temp_fn, SND_LOAD_SONG); \
		snd_kaja_func(KAJA_SONG_PLAY, 0); \
	} \
}

#define script_op_fade(c, func_in, func_out, temp_p1) { \
	script_p++; \
	script_param_read_number_first(temp_p1, 1); \
	if((c) == 'i') { \
		func_in(p1); \
	} else { \
		func_out(p1); \
	} \
}

#define script_op_shake(fast_forward, temp_i, temp_p1) { \
	script_param_read_number_first(temp_p1, 8); \
	for(temp_i = 0; temp_i <= temp_p1; temp_i++) { \
		if(temp_i & 1) { \
			graph_scrollup(4); \
		} else { \
			graph_scrollup(RES_Y - 4); \
		} \
		if(!(fast_forward)) { \
			frame_delay(1); \
		} \
	} \
	graph_scrollup(0); \
}

#include "src/main/dialog/script_params.inl"
#include "src/main/dialog/op.inl"
#include "src/main/dialog/run.inl"
#include "src/main/dialog/animate.inl"

#undef script_op_shake
#undef script_op_fade
#undef script_op_bgm
#undef script_param_read_fn
#undef str_sep_control_or_space
#undef script_p
#undef dialog_op_super_clean_stage
#undef dialog_box_wipe
#undef BOX_VRAM_W
#undef BOX_TILE_VRAM_W
#undef BOX_TILE_COUNT
#undef BOX_TILE_H
#undef BOX_TILE_W
#undef FLAGS_ZERO
