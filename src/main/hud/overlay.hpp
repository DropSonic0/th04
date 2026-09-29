#ifndef TH04_MAIN_HUD_OVERLAY_HPP
#define TH04_MAIN_HUD_OVERLAY_HPP

#include "src/shared/platform/pc98.hpp"
#include "src/shared/platform/types.hpp"

// This is the TH04 subset of the gaiji table used by the overlay producer.
// Keeping the enum here makes the overlay translation unit independent from
// the still-unlocalized th04/gaiji/gaiji.h include used by other MAIN owners.
#ifndef TH04_MAIN_OVERLAY_GAIJI_TYPES_HPP
#define TH04_MAIN_OVERLAY_GAIJI_TYPES_HPP

#define TH04_MAIN_OVERLAY_GAIJI_BARS \
	g_BAR_01W = 0x20, g_BAR_02W, g_BAR_03W, g_BAR_04W, \
	g_BAR_05W, g_BAR_06W, g_BAR_07W, g_BAR_08W, \
	g_BAR_09W, g_BAR_10W, g_BAR_11W, g_BAR_12W, \
	g_BAR_13W, g_BAR_14W, g_BAR_15W, g_BAR_16W

#define TH04_MAIN_OVERLAY_GAIJI_BOLDFONT \
	gb_0 = 0xA0, gb_1, gb_2, gb_3, gb_4, gb_5, gb_6, gb_7, \
	gb_8, gb_9, gb_A, gb_B, gb_C, gb_D, gb_E, gb_F, \
	gb_G, gb_H, gb_I, gb_J, gb_K, gb_L, gb_M, gb_N, \
	gb_O, gb_P, gb_Q, gb_R, gb_S, gb_T, gb_U, gb_V, \
	gb_W, gb_X, gb_Y, gb_Z

#define TH04_MAIN_OVERLAY_GAIJI_SYMBOLS \
	gs_HEART = 0xC9, gs_SKULL, gs_GHOST, gs_SIDDHAM_HAM, \
	gs_SPACE, gs_ARROW_LEFT, gs_ARROW_RIGHT

typedef enum {
	g_NULL = 0x00,
	g_EMPTY = 0x02,
	gs_NOTES,

	gs_HEART_2 = 0x06,
	gs_EXCLAMATION,
	gs_QUESTION,
	gs_SWEAT,
	gs_DOUBLE_EXCLAMATION,
	gs_EXCLAMATION_QUESTION,

	TH04_MAIN_OVERLAY_GAIJI_BARS,

	g_BAR_MAX_0,
	g_BAR_MAX_1,
	g_BAR_MAX_2,
	g_BAR_MAX_3,
	g_BAR_MAX_4,
	g_BAR_MAX_5,
	g_BAR_MAX_6,
	g_BAR_MAX_7,

	g_OVERLAY_FADE,
	g_OVERLAY_FADE_last = (g_OVERLAY_FADE + 8 - 1),
	TH04_MAIN_OVERLAY_GAIJI_BOLDFONT,
	gs_DOT = 0xC4,
	TH04_MAIN_OVERLAY_GAIJI_SYMBOLS,
	gs_BOMB = 0xD3,
	gs_YINYANG,
	gs_END,
	gs_TEN = 0xE6,
	gs_YUME,
	gs_TAMA,
	gs_ALL,
	g_HISCORE_STAGE_EMPTY = 0xEF,
	g_NONE = 0xFF,
} gaiji_th04_t;

#undef TH04_MAIN_OVERLAY_GAIJI_BARS
#undef TH04_MAIN_OVERLAY_GAIJI_BOLDFONT
#undef TH04_MAIN_OVERLAY_GAIJI_SYMBOLS

#define OVERLAY_FADE_CELS 8
#define RETURN_KEY_CELS 4u

#endif

// The pinned TH04 overlay header normally gets these from shiftjis.hpp. Keep
// the narrow scalar aliases local until that common header is localized.
typedef uint8_t shiftjis_t;
typedef int shiftjis_kanji_amount_t;
typedef unsigned int ushiftjis_kanji_amount_t;

#pragma codeseg HUD_OVRL_TEXT main_01

// Shared overlay callbacks. They are near Pascal function pointers; the
// pointed-to function and the pointer variable both remain near in MAIN.
extern nearfunc_t_near overlay1;
extern nearfunc_t_near overlay2;

// Fills the playfield area on text RAM with transparent or opaque cells.
void near overlay_wipe(void);
void near overlay_black(void);

// The TH02 overlay header supplied these macros to all later games. Keep the
// expansion local so overlay.cpp does not depend on an upstream header.
#define overlay_line_fill(y, atrb) { \
	extern const shiftjis_t* PLAYFIELD_BLANK_ROW; \
	text_putsa(PLAYFIELD_TRAM_LEFT, y, PLAYFIELD_BLANK_ROW, atrb); \
}

#if (GAME == 5)
#define overlay_line_fill_slow overlay_line_fill
#else
#define overlay_line_fill_slow(y, atrb) { \
	for(tram_x_t x = PLAYFIELD_TRAM_LEFT; x < PLAYFIELD_TRAM_RIGHT; x++) { \
		text_putca(x, y, ' ', atrb); \
	} \
}
#endif

#define overlay_fill(atrb) { \
	for(tram_y_t y = PLAYFIELD_TRAM_TOP; y < PLAYFIELD_TRAM_BOTTOM; y++) { \
		overlay_line_fill_slow(y, atrb); \
	} \
}

static const int OVERLAY_FADE_INTERVAL = 8;
static const int OVERLAY_FADE_FRAMES = (
	(OVERLAY_FADE_CELS + 1) * OVERLAY_FADE_INTERVAL
);

void pascal near overlay_stage_enter_update_and_render(void);
void pascal near overlay_stage_leave_update_and_render(void);

inline void overlay_stage_enter(void) {
	overlay1 = overlay_stage_enter_update_and_render;
}

inline void overlay_stage_leave(void) {
	overlay1 = overlay_stage_leave_update_and_render;
}

enum popup_id_t {
	POPUP_ID_HISCORE_ENTRY = 0,
	POPUP_ID_EXTEND = 1,
	POPUP_ID_BONUS = 2,
	POPUP_ID_FULL_POWERUP = 3,
#if (GAME == 5)
	POPUP_ID_DREAMBONUS_MAX = 4,
#endif

	_popup_id_t_FORCE_UINT8 = 0xFF
};

extern popup_id_t overlay_popup_id_new;
extern unsigned long overlay_popup_bonus;

void pascal near overlay_popup_update_and_render(void);

inline void overlay_popup_show(popup_id_t popup_new) {
	overlay_popup_id_new = popup_new;
	overlay2 = overlay_popup_update_and_render;
}

extern unsigned char bgm_title_id;
#if (GAME == 5)
extern shiftjis_t far *stage_title;
extern shiftjis_t far *stage_bgm_title;
extern shiftjis_t far *boss_bgm_title;
#endif

void near overlay_titles_invalidate(void);
void pascal near overlay_titles_update_and_render(void);
void pascal near overlay_boss_bgm_update_and_render(void);

#pragma codeseg

#endif
