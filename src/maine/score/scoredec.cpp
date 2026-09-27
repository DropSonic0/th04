// Natural-C++ reconstruction candidate for MAINE's SCORE_TEXT decoder.
#include <stddef.h>
#include "src/maine/score/scoredat.hpp"

#if defined(TH04P)
#pragma codeseg SCORE_TEXT GROUP_01
#else
#pragma codeseg SCORE_TEXT score_01
#endif
unsigned char pascal near scoredat_decode(void)
#include "src/maine/score/scoredec.inl"
#pragma codeseg
