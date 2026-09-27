// MAINE SCORE_TEXT score-entry row dispatcher.
#include "src/maine/score/scoredat.hpp"
#include "src/shared/platform/types.hpp"

void pascal near place_row_put(int place, unsigned char rendered_playchar);

#if defined(TH04P)
#pragma codeseg SCORE_TEXT GROUP_01
#else
#pragma codeseg SCORE_TEXT score_01
#endif
#include "src/maine/score/places_put.inl"
#pragma codeseg
