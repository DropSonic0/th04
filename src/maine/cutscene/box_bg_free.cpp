#include "src/shared/memory/hmem.hpp"

extern unsigned char far *box_bg;

#if defined(TH04P)
#pragma codeseg CUTSCENE_TEXT GROUP_01
#else
#pragma codeseg CUTSCENE_TEXT cutscene_01
#endif
#include "src/maine/cutscene/box_bg_free.inl"
#pragma codeseg
