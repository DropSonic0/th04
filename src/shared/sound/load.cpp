#pragma option -zCSHARED -Z-

#include "src/shared/platform/x86.hpp"
#include "src/shared/runtime/api.hpp"
#include "src/shared/sound/api.hpp"
#include "src/shared/sound/impl.hpp"

#define PF_FN_LEN 13

extern char snd_load_fn[PF_FN_LEN];
extern const char *SND_LOAD_EXT[4];

#include "src/shared/sound/load_prefix.inl"
#include "src/shared/sound/load_push_ds.inl"
#include "src/shared/sound/load_open.inl"
#include "src/shared/sound/load_handle_mov.inl"
#include "src/shared/sound/load_func.inl"
#include "src/shared/sound/load_dispatch_read.inl"
#include "src/shared/sound/load_pop_ds.inl"
#include "src/shared/sound/load_tail.inl"
