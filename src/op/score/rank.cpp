// OP SCORE_TEXT high-score background and rank renderer.
#include "src/op/score/scoredat.hpp"
#include "src/shared/hardware/graphics.hpp"
#include "src/shared/formats/pi.hpp"

extern unsigned char rank;
void pascal near place_put(int place);

enum { PAT_RANK_1 = 10, PAT_RANK_2 = 11 };
static const pixel_t RANK_W = 128;
static const screen_x_t RANK_LEFT = (RES_X - GLYPH_FULL_W - RANK_W);
static const screen_y_t RANK_TOP = (RES_Y - (GLYPH_H / 2) - GLYPH_H);

#ifdef TH04P
#pragma codeseg OP_NATIVE_TEXT OP_NATIVE_01
#else
#pragma codeseg SCORE_TEXT op_01
#endif
#include "src/op/score/rank_render.inl"
#pragma codeseg
