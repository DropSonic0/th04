#include "src/maine/score/scoredat.hpp"
#include "src/shared/platform/types.hpp"
#include "src/shared/config/resident.hpp"

extern unsigned char entered_place;

#if defined(TH04P)
#pragma codeseg SCORE_TEXT GROUP_01
#else
#pragma codeseg SCORE_TEXT score_01
#endif
#include "src/maine/score/score_insert.inl"
#pragma codeseg
