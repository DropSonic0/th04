// MAINE SCORE_TEXT score-file encoder, reconstructed from target behavior.
#include <stddef.h>
#include "src/maine/score/scoredat.hpp"
#include "src/shared/runtime/api.hpp"

#if defined(TH04P)
#pragma codeseg SCORE_TEXT GROUP_01
#else
#pragma codeseg SCORE_TEXT score_01
#endif
void pascal near scoredat_encode(void)
#include "src/maine/score/scoreenc.inl"
#pragma codeseg
