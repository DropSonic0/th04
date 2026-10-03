#include <dos.h>

#include "src/shared/runtime/api.hpp"

// Wait for the next PC-98 graphics vertical blank rising edge. This uses the
// GDC status port path; interrupt-backed waiting is a separate runtime owner.
void TH04_PASCAL vsync_wait(void)
{
#if defined(__TURBOC__) || defined(__MSDOS__)
	while(inportb(0xA0) & 0x20) {
	}
	while(!(inportb(0xA0) & 0x20)) {
	}
#else
	vsync_end();
#endif
}
