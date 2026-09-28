#include "src/shared/memory/hmem.hpp"

extern unsigned char __seg *nopoly_B;

#ifdef TH04P
#pragma codeseg OP_NATIVE_TEXT OP_NATIVE_01
#else
#pragma codeseg OP_MUSIC_TEXT op_music_01
#endif
#include "src/op/music/nopoly_free.inl"
#pragma codeseg
