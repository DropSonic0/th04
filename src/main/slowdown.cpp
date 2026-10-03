#include "src/shared/runtime/api.hpp"
#include "th04/main/slowdown.hpp"

void near slowdown_frame_delay(void)
{
	while(vsync_Count1 < (unsigned int)slowdown_factor) {
#if !defined(__TURBOC__) && !defined(__MSDOS__)
		vsync_end();
#endif
	}
	vsync_Count1 = 0;
	slowdown_factor = 1;
}
