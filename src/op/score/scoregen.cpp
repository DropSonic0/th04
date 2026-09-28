// OP SCORE_TEXT score-file regeneration, reconstructed from target bytes.
#include "src/op/score/scoredat.hpp"
#include "src/shared/runtime/api.hpp"

unsigned char pascal near scoredat_decode(void);
void near scoredat_encode(void);

#ifdef TH04P
#pragma codeseg OP_NATIVE_TEXT OP_NATIVE_01
#else
#pragma codeseg SCORE_TEXT op_01
#endif
void near scoredat_recreate(void)
#include "src/op/score/scoregen.inl"
#pragma codeseg
