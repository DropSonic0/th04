#pragma option -zCSHARED -3

#if defined(__TURBOC__) || defined(__MSDOS__)
#include <dos.h>
#else
#include <stdint.h>
#include <string.h>
#endif

#include "src/shared/hardware/graphics.hpp"

extern uint8_t g_gaiji_patterns[256][32];
extern bool g_gaiji_loaded;

#if !defined(__TURBOC__) && !defined(__MSDOS__)
extern uint16_t s_tram_jis[25][80];
extern uint16_t s_tram_atrb[25][80];
#endif

// A PC-98 text cell uses one glyph word and one attribute word in separate
// planes. A gaiji occupies two adjacent cells, one per glyph half.
static unsigned gaiji_first_half(unsigned char c)
{
	return (((unsigned)(c & 0x7F)) << 8) | (0x56 + (c >> 7));
}

#if defined(__TURBOC__) || defined(__MSDOS__)
static unsigned far *gaiji_cell(unsigned x, unsigned y)
{
	const unsigned row_segment = 0xA000 + (y << 3) + (y << 1);
	return (unsigned far *)MK_FP(row_segment, x << 1);
}
#endif

extern "C" void TH04_PASCAL gaiji_putca(
	unsigned x, unsigned y, unsigned c, unsigned atrb
)
{
#if defined(__TURBOC__) || defined(__MSDOS__)
	unsigned far *cell = gaiji_cell(x, y);
	const unsigned first = gaiji_first_half((unsigned char)c);
	cell[0] = first;
	cell[1] = first | 0x8000;
	cell[0x1000] = atrb;
	cell[0x1001] = atrb;
#else
	if (x < 79 && y < 25) {
		unsigned first = gaiji_first_half((unsigned char)c);
		s_tram_jis[y][x] = (uint16_t)first;
		s_tram_jis[y][x + 1] = (uint16_t)(first | 0x8000);
		s_tram_atrb[y][x] = (uint16_t)atrb;
		s_tram_atrb[y][x + 1] = (uint16_t)atrb;
	}
#endif
}

extern "C" void TH04_PASCAL gaiji_putsa(
	unsigned x, unsigned y, const char TH04_PTR *str, unsigned atrb
)
{
#if defined(__TURBOC__) || defined(__MSDOS__)
	unsigned far *cell = gaiji_cell(x, y);
	while(*str) {
		const unsigned first = gaiji_first_half((unsigned char)*str++);
		cell[0] = first;
		cell[1] = first | 0x8000;
		cell[0x1000] = atrb;
		cell[0x1001] = atrb;
		cell += 2;
	}
#else
	if (!str) return;
	while(*str && x < 79 && y < 25) {
		unsigned first = gaiji_first_half((unsigned char)*str++);
		s_tram_jis[y][x] = (uint16_t)first;
		s_tram_jis[y][x + 1] = (uint16_t)(first | 0x8000);
		s_tram_atrb[y][x] = (uint16_t)atrb;
		s_tram_atrb[y][x + 1] = (uint16_t)atrb;
		x += 2;
	}
#endif
}
