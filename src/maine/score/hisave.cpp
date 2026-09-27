#include "src/maine/score/scoredat.hpp"
#include "src/shared/runtime/api.hpp"

extern unsigned char rank;
extern unsigned char playchar;


unsigned char pascal near scoredat_decode(void);
unsigned char pascal near scoredat_encode(void);

#if defined(TH04P)
#pragma codeseg SCORE_TEXT GROUP_01
#else
#pragma codeseg SCORE_TEXT score_01
#endif
#include "src/maine/score/hisave.inl"
#pragma codeseg
