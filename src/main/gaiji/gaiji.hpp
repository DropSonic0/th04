#ifndef TH04_MAIN_GAIJI_GAIJI_HPP
#define TH04_MAIN_GAIJI_GAIJI_HPP

// Shared TH02 gaiji ranges used by the TH04/TH05 character table. Keep these
// macro names compatible with the historical from_2.h surface because the
// table below is an enum expansion, not storage.
#define gaiji_bar(start) \
	g_BAR_01W = start, \
	g_BAR_02W, \
	g_BAR_03W, \
	g_BAR_04W, \
	g_BAR_05W, \
	g_BAR_06W, \
	g_BAR_07W, \
	g_BAR_08W, \
	g_BAR_09W, \
	g_BAR_10W, \
	g_BAR_11W, \
	g_BAR_12W, \
	g_BAR_13W, \
	g_BAR_14W, \
	g_BAR_15W, \
	g_BAR_16W

#define BAR_GAIJI_MAX 16

#if (GAME == 2)
#define gb_MN_BUG gb_N, gb_M
#else
#define gb_MN_BUG gb_M, gb_N
#endif

#define gaiji_boldfont(start) \
	gb_0 = start, \
	gb_1, \
	gb_2, \
	gb_3, \
	gb_4, \
	gb_5, \
	gb_6, \
	gb_7, \
	gb_8, \
	gb_9, \
	gb_A, \
	gb_B, \
	gb_C, \
	gb_D, \
	gb_E, \
	gb_F, \
	gb_G, \
	gb_H, \
	gb_I, \
	gb_J, \
	gb_K, \
	gb_L, \
	gb_MN_BUG, \
	gb_O, \
	gb_P, \
	gb_Q, \
	gb_R, \
	gb_S, \
	gb_T, \
	gb_U, \
	gb_V, \
	gb_W, \
	gb_X, \
	gb_Y, \
	gb_Z

#define gaiji_symbols_th02(start) \
	gs_HEART = start, \
	gs_SKULL, \
	gs_GHOST, \
	gs_SIDDHAM_HAM, \
	gs_SPACE, \
	gs_ARROW_LEFT, \
	gs_ARROW_RIGHT

#define OVERLAY_FADE_CELS 8
#define RETURN_KEY_CELS 4u

typedef enum {
	g_NULL = '\0',
	g_EMPTY = 0x02,
	gs_NOTES,

	gs_HEART_2 = 0x06,
	gs_EXCLAMATION,
	gs_QUESTION,
	gs_SWEAT,
	gs_DOUBLE_EXCLAMATION,
	gs_EXCLAMATION_QUESTION,

#if (GAME == 5)
	ga_RETURN_KEY = 0x1C,
	ga_RETURN_KEY_last = (ga_RETURN_KEY + RETURN_KEY_CELS - 1),
#endif

	gaiji_bar(0x20),

	g_BAR_MAX_0,
	g_BAR_MAX_1,
	g_BAR_MAX_2,
	g_BAR_MAX_3,
	g_BAR_MAX_4,
	g_BAR_MAX_5,
	g_BAR_MAX_6,
	g_BAR_MAX_7,

	g_OVERLAY_FADE,
	g_OVERLAY_FADE_last = (g_OVERLAY_FADE + OVERLAY_FADE_CELS - 1),
	gaiji_boldfont(0xA0),
	gs_DOT = 0xC4,
	gaiji_symbols_th02(0xC9),
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

#endif
