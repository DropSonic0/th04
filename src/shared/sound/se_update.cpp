#include "src/shared/sound/api.hpp"
#include "src/shared/sound/impl.hpp"

// The complete TH04 sound-effect producer is independently accepted in OP
// and MAINE. Keep the accepted OP body as the one maintained implementation;
// this shared product TU gives MAINE a local owner without changing its
// isolated accepted wrapper's source composition.
static const int PMD_INTERRUPT = PMD;

extern "C" int far pascal bgm_sound(int num);

#pragma option -k-
#pragma codeseg SHARED se_update_01
#include "src/op/sound/se_update.inl"
