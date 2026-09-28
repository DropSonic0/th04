// OP SCORE_TEXT two-column score digit renderer.
#include "src/op/score/scoredat.hpp"
#include "src/shared/hardware/graphics.hpp"

#ifdef TH04P
#pragma codeseg OP_NATIVE_TEXT OP_NATIVE_01
#else
#pragma codeseg SCORE_TEXT op_01
#endif
void pascal near scores_put(screen_y_t top, int place)
#include "src/op/score/scores_put.inl"
#pragma codeseg
