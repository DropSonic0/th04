#include "src/shared/sound/api.hpp"
#include "src/shared/sound/impl.hpp"

static const int PMD_INTERRUPT = PMD;

extern "C" int far pascal bgm_sound(int num);

#pragma option -k-
#ifdef TH04P
#pragma codeseg OP_NATIVE_TEXT OP_NATIVE_01
#else
#pragma codeseg SHARED se_update_01
#endif
#include "src/op/sound/se_update.inl"
