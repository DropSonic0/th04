#include <dos.h>

#include "src/shared/hardware/graphics.hpp"

// PC-98 analog 16-color palette output. Each stored 8-bit component contributes
// its high nibble; tones 0..100 fade toward black, 100..200 toward white.
// LCD-specific palette behavior requires a separate runtime observation.
static unsigned char component_at_tone(unsigned char component, int tone)
{
	const int base = (component >> 4);
	if(tone <= 100) {
		return (unsigned char)((base * tone) / 100);
	}
	return (unsigned char)(15 - (((15 - base) * (200 - tone)) / 100));
}

void TH04_PASCAL palette_show(void)
{
	int tone = (int)PaletteTone;
	if(tone < 0) {
		tone = 0;
	} else if(tone > 200) {
		tone = 200;
	}

	for(int color = 0; color < 16; color++) {
		outportb(0xA8, color);
		outportb(0xAC, component_at_tone(Palettes[color].v[0], tone));
		outportb(0xAA, component_at_tone(Palettes[color].v[1], tone));
		outportb(0xAE, component_at_tone(Palettes[color].v[2], tone));
	}
}
