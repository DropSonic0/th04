#ifndef TH04_MAIN_HUD_OVERLAY_HPP
#define TH04_MAIN_HUD_OVERLAY_HPP

#include "src/shared/platform/pc98.hpp"
#include "src/shared/platform/types.hpp"

#include "src/main/gaiji/gaiji.hpp"

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
