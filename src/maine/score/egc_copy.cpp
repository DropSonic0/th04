#include "src/shared/hardware/graphics.hpp"
#include "src/shared/hardware/vram_planes.hpp"

typedef unsigned int vram_offset_t;
typedef unsigned short dots16_t;

#pragma codeseg SCORE_TEXT score_01
#include "src/maine/score/score_egc_start_copy.inl"
#include "src/maine/score/score_rect_copy.inl"
#pragma codeseg
