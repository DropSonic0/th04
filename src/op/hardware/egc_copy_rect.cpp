#include "src/shared/hardware/graphics.hpp"

typedef uint16_t dots16_t;
typedef int16_t vram_offset_t;

extern vram_word_amount_t egcrect_w;
vram_word_amount_t egcrect_w;

#define DEFCONV pascal
#define FLAGS_SIGN (_FLAGS & 0x80)

#ifdef TH04P
#pragma codeseg OP_NATIVE_TEXT OP_NATIVE_01
#else
#pragma codeseg SHARED egcrect_01
#endif
#include "src/op/hardware/egc_start_copy.inl"
#include "src/op/hardware/egc_copy_rect_1_to_0_16.inl"
#pragma codeseg
