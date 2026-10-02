#pragma option -zCSHARED -3

#if defined(__TURBOC__) || defined(__MSDOS__)
# include <dos.h>
#endif

#include "src/shared/hardware/graphics.hpp"

extern "C" unsigned __cdecl graph_VramSeg;

static void gaiji_graph_begin(vc2 color)
{
	// Preserve the caller's interrupt flag while selecting GRCG RMW mode.
#ifdef __TURBOC__
	asm {
		pushf
		cli
		mov al, 0C0h
		out 07Ch, al
		popf
	}
#else
	outportb(0x7C, 0xC0);
#endif
	for(unsigned plane = 0; plane < 4; plane++) {
		outportb(0x7E, (color & (1u << plane)) ? 0xFF : 0);
	}
	outportb(0x68, 0x0B); // CG dot access
}

static void gaiji_graph_end(void)
{
	outportb(0x68, 0x0A); // CG code access
	outportb(0x7C, 0);    // GRCG off
}

static void gaiji_graph_draw_one(unsigned x, unsigned y, unsigned c)
{
	// TH04's fixed master-library variant uses an ordinary ADD here.
	const unsigned jis = (c + 0x5680u) & 0xFF7Fu;
	outportb(0xA1, (unsigned char)jis);
	outportb(0xA3, (unsigned char)(jis >> 8));

	const unsigned shift = x & 7u;
	const unsigned offset = (y * 80u) + (x >> 3);
	unsigned char far *dest = (unsigned char far *)MK_FP(graph_VramSeg, offset);
	for(unsigned row = 0; row < 16; row++) {
		outportb(0xA5, (unsigned char)(row | 0x20));
		const unsigned first = inportb(0xA9);
		outportb(0xA5, (unsigned char)row);
		const unsigned second = inportb(0xA9);
		const unsigned aligned = ((first << 8) | second) >> shift;
		const unsigned spill = (second << 8) >> shift;
		if (dest) {
			dest[0] = (unsigned char)(aligned >> 8);
			dest[1] = (unsigned char)aligned;
			dest[2] = (unsigned char)spill;
			dest += 80;
		}
	}
}

extern "C" void TH04_PASCAL graph_gaiji_putc(
	screen_x_t left, vram_y_t top, int c, vc2 color
)
{
	gaiji_graph_begin(color);
	gaiji_graph_draw_one((unsigned)left, (unsigned)top, (unsigned)c);
	gaiji_graph_end();
}

extern "C" void TH04_PASCAL graph_gaiji_puts(
	screen_x_t left, vram_y_t top, pixel_t step,
	const char TH04_PTR *str, vc2 color
)
{
	gaiji_graph_begin(color);
	unsigned x = (unsigned)left;
	while(*str) {
		gaiji_graph_draw_one(x, (unsigned)top, (unsigned char)*str++);
		x += (unsigned)step;
	}
	gaiji_graph_end();
}