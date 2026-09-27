#include "src/shared/hardware/frame_delay.hpp"
#include "src/shared/hardware/graphics.hpp"
#include "src/maine/cutscene/box_mask.hpp"

// The reviewed body tests this target global through an 8-bit memory operand.
extern unsigned char fast_forward;
extern int text_interval;

void near egc_start_copy(void);
void pascal near box_1_to_0_masked(box_mask_t mask);

#pragma codeseg CUTSCENE_TEXT cutscene_01
#include "src/maine/cutscene/box_animate.inl"
#pragma codeseg
