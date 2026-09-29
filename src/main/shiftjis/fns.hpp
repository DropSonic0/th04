#ifndef TH04_MAIN_SHIFTJIS_FNS_HPP
#define TH04_MAIN_SHIFTJIS_FNS_HPP

#define BINARY_OP "op"

// Packfile loaded during OP.EXE and MAINE.EXE.  The first six bytes are the
// original Shift-JIS filename bytes; spelling them as escapes keeps this
// product header UTF-8-safe without changing the emitted string literal.
#define OP_AND_END_PF_FN "\x8c\xb6\x91\x7a\x8b\xbd" "ed.dat"

#define EYECATCH_FN_FORMAT "eye0.cdg"

#if (GAME == 5)
#define FACESET_REIMU_FN  "KAO0.cd2"
#define FACESET_MARISA_FN "KAO1.cd2"
#define FACESET_MIMA_FN   "KAO2.cd2"
#define FACESET_YUUKA_FN  "KAO3.cd2"

#define main_cdg_load_faceset_playchar() { \
	switch(playchar) { \
	case PLAYCHAR_REIMU: \
		cdg_load_all(CDG_FACESET_PLAYCHAR, FACESET_REIMU_FN); \
		break; \
	case PLAYCHAR_MARISA: \
		cdg_load_all(CDG_FACESET_PLAYCHAR, FACESET_MARISA_FN); \
		break; \
	case PLAYCHAR_MIMA: \
		cdg_load_all(CDG_FACESET_PLAYCHAR, FACESET_MIMA_FN); \
		break; \
	case PLAYCHAR_YUUKA: \
		cdg_load_all(CDG_FACESET_PLAYCHAR, FACESET_YUUKA_FN); \
		break; \
	} \
}
#else
#define FACESET_REIMU_FN  "KAO0.cd2"
#define FACESET_MARISA_FN "KAO1.cd2"

#define main_cdg_load_faceset_playchar() { \
	if(playchar == PLAYCHAR_REIMU) { \
		cdg_load_all(CDG_FACESET_PLAYCHAR, FACESET_REIMU_FN); \
	} else { \
		cdg_load_all(CDG_FACESET_PLAYCHAR, FACESET_MARISA_FN); \
	} \
}
#endif

#define BOSS_BB_MUGETSU_FN  "st06.bb"
#define BOSS_BB_GENGETSU_FN "st06b.bb"

#define BOMB_BG_FORMAT "BB0.CDG"

#define BOSS_BG_MUGETSU_FN  "st06bk.cdg"
#define BOSS_BG_GENGETSU_FN "st06bk2.cdg"

#endif
