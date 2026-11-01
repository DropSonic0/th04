#pragma codeseg OP_HARDWARE_TEXT

#include "src/shared/runtime/api.hpp"

void pascal frame_delay_2(int frames)
{
#if defined(__PS3__)
	if (frames <= 0) return;
	for (int f = 0; f < frames; f++) {
		vsync_end();
	}
#else
	vsync_Count1 = 0;
	while(vsync_Count1 < (unsigned int)frames) {
#if !defined(__TURBOC__) && !defined(__MSDOS__)
		vsync_end();
#endif
	}
#endif
}
