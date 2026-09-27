#include "src/shared/runtime/api.hpp"

extern unsigned char script[8192];
extern unsigned char near *script_p;
void near cutscene_script_free(void);

#if defined(TH04P)
#pragma codeseg CUTSCENE_TEXT GROUP_01
#else
#pragma codeseg CUTSCENE_TEXT cutscene_01
#endif
#include "src/maine/cutscene/script_load.inl"
#pragma codeseg
