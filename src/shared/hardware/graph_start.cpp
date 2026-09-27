#include <dos.h>

#include "src/shared/platform/abi.hpp"

extern "C" {
extern unsigned int __cdecl PaletteTone;
extern unsigned char __cdecl Palettes[48];
extern unsigned char __cdecl PalettesInit[48];
void TH04_PASCAL palette_show(void);
void TH04_PASCAL graph_show(void);
void TH04_PASCAL graph_400line(void);
void TH04_PASCAL graph_clear(void);
}

// Keep the startup caller in its own code segment so TC4J emits actual far
// calls across independently owned PC-98 graphics segments.
#pragma codeseg GRAPHSTART_TEXT GROUP_01
extern "C" void TH04_PASCAL palette_init(void)
{
	for(unsigned i = 0; i < 48; i++) {
		Palettes[i] = PalettesInit[i];
	}
	outportb(0x6A, 0);
	outportb(0xAE, 0x04);
	outportb(0xAC, 0x26);
	outportb(0xAA, 0x15);
	outportb(0xA8, 0x37);
	outportb(0x6A, 1);
	PaletteTone = 100;
	palette_show();
}

extern "C" void TH04_PASCAL graph_start(void)
{
	outportb(0x6A, 0x41);
	PaletteTone = 0;
	palette_show();
	outportb(0xA4, 0);
	outportb(0xA6, 0);
	graph_show();
	graph_400line();
	graph_clear();
	palette_init();
}
#pragma codeseg
