// MAINE SCORE_TEXT alphabet cursor renderer.
#include "src/shared/platform/types.hpp"
#include "src/shared/hardware/graphics.hpp"

extern unsigned char gALPHABET[];

#if defined(TH04P)
#pragma codeseg SCORE_TEXT GROUP_01
#else
#pragma codeseg SCORE_TEXT score_01
#endif
#include "src/maine/score/alphabet_cursor.inl"
#pragma codeseg
