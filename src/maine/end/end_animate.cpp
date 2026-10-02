#include "src/shared/platform/types.hpp"
#include "src/shared/config/resident.hpp"

#ifdef __TURBOC__
typedef char end_playchar_offset_check[(offsetof(resident_t, playchar_ascii) == 0x12) ? 1 : -1];
typedef char end_shottype_offset_check[(offsetof(resident_t, shottype) == 0x19) ? 1 : -1];
typedef char end_type_offset_check[(offsetof(resident_t, end_type_ascii) == 0x25) ? 1 : -1];
#endif

int pascal near cutscene_script_load(const char far *fn);
void near cutscene_animate(void);
void near cutscene_script_free(void);

#if defined(TH04P)
#pragma codeseg MAINE_E_TEXT GROUP_01
#else
#pragma codeseg MAINE_E_TEXT maine_e_01
#endif
#include "src/maine/end/end_animate.inl"
#pragma codeseg
