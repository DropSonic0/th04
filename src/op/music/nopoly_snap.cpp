#include "src/shared/memory/hmem.hpp"

extern unsigned char __seg *nopoly_B;
extern unsigned char far *VRAM_PLANE_B;

#ifdef TH04P
#pragma codeseg OP_NATIVE_TEXT OP_NATIVE_01
#else
#pragma codeseg OP_MUSIC_TEXT op_music_01
#endif
#include "src/op/music/nopoly_snap.inl"
#pragma codeseg
