#ifndef TH04_MAIN_FORMATS_SCOREDAT_HPP
#define TH04_MAIN_FORMATS_SCOREDAT_HPP

#include "src/shared/platform/types.hpp"
#include "src/shared/config/score.hpp"

#define SCOREDAT_FN "GENSOU.SCR"

#if GAME == 5
#define SCOREDAT_PLACES 5
#define SCOREDAT_NOT_CLEARED 18
#define SCOREDAT_CLEARED 0x80
#else
#define SCOREDAT_PLACES 10
#define SCOREDAT_NOT_CLEARED 25
#define SCOREDAT_CLEARED_A 1
#define SCOREDAT_CLEARED_B 2
#define SCOREDAT_CLEARED_BOTH (SCOREDAT_CLEARED_A | SCOREDAT_CLEARED_B)
#endif

#define SCOREDAT_NAME_LEN 8

struct scoredat_t {
	unsigned char g_name[SCOREDAT_PLACES][SCOREDAT_NAME_LEN + 1];
	score_lebcd_t g_score[SCOREDAT_PLACES];

#if GAME == 5
	unsigned char g_stage[SCOREDAT_PLACES];
	unsigned char cleared;
	unsigned char unused_1;
#else
	unsigned char cleared;
	unsigned char unused_1;
	unsigned char g_stage[SCOREDAT_PLACES];
	unsigned char unused_2[SCOREDAT_PLACES];
#endif
};

struct scoredat_section_t {
	int8_t key1;
	int8_t key2;
	int16_t score_sum;
	scoredat_t score;
};

extern scoredat_section_t hi;
extern scoredat_section_t hi2;

#if (BINARY == 'M') && (GAME == 4)
uint8_t pascal near scoredat_decode(scoredat_section_t near *hi);
void pascal near scoredat_encode(scoredat_section_t near *hi);

#define scoredat_decode_func() scoredat_decode(&hi)
#define scoredat_encode_func() scoredat_encode(&hi)
#else
uint8_t pascal near scoredat_decode(void);
void pascal near scoredat_encode(void);

#define scoredat_decode_func scoredat_decode
#define scoredat_encode_func scoredat_encode
#endif

void near scoredat_recreate(void);

#endif
