#pragma option -zCSHARED

#include "src/shared/runtime/api.hpp"

void pascal frame_delay(int frames)
{
	vsync_Count1 = 0;
	while(vsync_Count1 < (unsigned int)frames) {
#if !defined(__TURBOC__) && !defined(__MSDOS__)
		vsync_end();
#endif
	}
}
