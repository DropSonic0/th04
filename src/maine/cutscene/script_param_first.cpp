#include <ctype.h>

extern unsigned char near *script_p;
extern int script_param_number_default;

#if defined(TH04P)
#pragma codeseg CUTSCENE_TEXT GROUP_01
#else
#pragma codeseg CUTSCENE_TEXT cutscene_01
#endif
#include "src/maine/cutscene/script_param_first.inl"
#pragma codeseg
