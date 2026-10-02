#pragma option -zCSHARED -Z-

#include "src/shared/platform/x86.hpp"
#include "src/shared/runtime/api.hpp"
#include "src/shared/sound/api.hpp"
#include "src/shared/sound/impl.hpp"

#define PF_FN_LEN 13

extern char snd_load_fn[PF_FN_LEN];
extern const char *SND_LOAD_EXT[4];

#ifdef __TURBOC__

#include "src/shared/sound/load_prefix.inl"
#include "src/shared/sound/load_push_ds.inl"
#include "src/shared/sound/load_open.inl"
#include "src/shared/sound/load_handle_mov.inl"
#include "src/shared/sound/load_func.inl"
#include "src/shared/sound/load_dispatch_read.inl"
#include "src/shared/sound/load_pop_ds.inl"
#include "src/shared/sound/load_tail.inl"

#else

// Implementación Stub para PS3 / Compiladores modernos
extern "C" void TH04_PASCAL snd_load(const char fn[PF_FN_LEN], snd_load_func_t func)
{
	// En PS3 la carga de archivos BGM/SE se gestionará por el motor de audio nativo
}

#endif