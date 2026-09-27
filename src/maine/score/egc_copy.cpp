#include "src/shared/hardware/graphics.hpp"
#include "src/shared/hardware/vram_planes.hpp"

typedef unsigned int vram_offset_t;
typedef unsigned short dots16_t;

#if defined(TH04P)
#pragma codeseg SCORE_TEXT GROUP_01
#else
#pragma codeseg SCORE_TEXT score_01
#endif
#include "src/maine/score/score_egc_start_copy.inl"
#include "src/maine/score/score_rect_copy.inl"
#pragma codeseg
