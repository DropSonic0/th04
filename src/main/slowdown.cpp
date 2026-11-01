#include "src/shared/runtime/api.hpp"
#include "th04/main/slowdown.hpp"

void near slowdown_frame_delay(void)
{
#if defined(__PS3__)
	for (int f = 0; f < slowdown_factor; f++) {
		vsync_end();
	}
	vsync_Count1 = 0;
	slowdown_factor = 1;
#else
	while(vsync_Count1 < (unsigned int)slowdown_factor) {
#if !defined(__TURBOC__) && !defined(__MSDOS__)
		vsync_end();
#endif
	}
	vsync_Count1 = 0;
	slowdown_factor = 1;
#endif
}
