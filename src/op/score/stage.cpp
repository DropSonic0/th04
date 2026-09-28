// OP SCORE_TEXT high-score stage renderer.
#include "src/shared/platform/types.hpp"
#include "src/shared/hardware/graphics.hpp"

enum {
    g_NONE = 0xFF,
    g_HISCORE_STAGE_EMPTY = 0xEF,
    COL_SHADOW = 14,
    COL_STAGE = 7,
};

#ifdef TH04P
#pragma codeseg OP_NATIVE_TEXT OP_NATIVE_01
#else
#pragma codeseg SCORE_TEXT op_01
#endif
#include "src/op/score/stage_put.inl"
#pragma codeseg
