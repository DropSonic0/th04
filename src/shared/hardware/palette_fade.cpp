#include "src/shared/hardware/graphics.hpp"
#include "src/shared/runtime/api.hpp"

// Keep the historical six-tone steps and initial vertical-blank alignment.
// Analog palette output is owned by palette_show.cpp.
static void fade(int start, int end, int step, unsigned speed)
{
	int tone = start;
	const int waits = (int)speed;
	PaletteTone = start;
	vsync_wait();
	for(;;) {
		palette_show();
		for(int tick = 0; tick < waits; tick++) {
			vsync_wait();
		}
		tone += step;
		if((step < 0 && tone <= end) || (step > 0 && tone >= end)) {
			break;
		}
		PaletteTone = tone;
	}
	PaletteTone = end;
	palette_show();
}

void TH04_PASCAL palette_black_out(unsigned speed)
{
	fade(100, 0, -6, speed);
}

void TH04_PASCAL palette_black_in(unsigned speed)
{
	fade(0, 100, 6, speed);
}

void TH04_PASCAL palette_white_out(unsigned speed)
{
	fade(100, 200, 6, speed);
}

void TH04_PASCAL palette_white_in(unsigned speed)
{
	fade(200, 100, -6, speed);
}
