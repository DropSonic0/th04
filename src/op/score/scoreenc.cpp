// OP SCORE_TEXT score-file encoder, reconstructed from target bytes.
#include <stddef.h>
#include <stdlib.h>
#include "src/op/score/scoredat.hpp"
#include "src/shared/runtime/api.hpp"

#ifdef TH04P
#pragma codeseg OP_NATIVE_TEXT OP_NATIVE_01
#else
#pragma codeseg SCORE_TEXT op_01
#endif
void near scoredat_encode(void)
#include "src/op/score/scoreenc.inl"
#pragma codeseg
