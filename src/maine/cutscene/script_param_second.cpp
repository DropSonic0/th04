extern unsigned char near *script_p;
extern int script_param_number_default;

void pascal near script_param_read_number_first(int& ret);

#if defined(TH04P)
#pragma codeseg CUTSCENE_TEXT GROUP_01
#else
#pragma codeseg CUTSCENE_TEXT cutscene_01
#endif
#include "src/maine/cutscene/script_param_second.inl"
#pragma codeseg
