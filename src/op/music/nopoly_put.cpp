#include "src/shared/platform/x86.hpp"
#include "src/shared/platform/pc98.hpp"

extern unsigned char __seg *nopoly_B;

#ifdef TH04P
#pragma codeseg OP_NATIVE_TEXT OP_NATIVE_01
#else
#pragma codeseg OP_MUSIC_TEXT nopoly_put_01
#endif
#include "src/op/music/nopoly_put.inl"
#pragma codeseg
