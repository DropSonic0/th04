#include "src/shared/memory/hmem.hpp"

extern unsigned char far *raise_bg[2];

#ifdef TH04P
#pragma codeseg OP_NATIVE_TEXT OP_NATIVE_01
#else
#pragma codeseg OP_01_TEXT op_01
#endif
#include "src/op/menu/raise_bg_free.inl"
#pragma codeseg
