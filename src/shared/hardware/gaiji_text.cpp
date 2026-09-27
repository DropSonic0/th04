#pragma option -zCSHARED -3

#include <dos.h>

#include "src/shared/hardware/graphics.hpp"

// A PC-98 text cell uses one glyph word and one attribute word in separate
// planes. A gaiji occupies two adjacent cells, one per glyph half.
static unsigned gaiji_first_half(unsigned char c)
{
	return (((unsigned)(c & 0x7F)) << 8) | (0x56 + (c >> 7));
}

static unsigned far *gaiji_cell(unsigned x, unsigned y)
{
	const unsigned row_segment = 0xA000 + (y << 3) + (y << 1);
	return (unsigned far *)MK_FP(row_segment, x << 1);
}

extern "C" void TH04_PASCAL gaiji_putca(
	unsigned x, unsigned y, unsigned c, unsigned atrb
)
{
	unsigned far *cell = gaiji_cell(x, y);
	const unsigned first = gaiji_first_half((unsigned char)c);
	cell[0] = first;
	cell[1] = first | 0x8000;
	cell[0x1000] = atrb;
	cell[0x1001] = atrb;
}

extern "C" void TH04_PASCAL gaiji_putsa(
	unsigned x, unsigned y, const char TH04_PTR *str, unsigned atrb
)
{
	unsigned far *cell = gaiji_cell(x, y);
	while(*str) {
		const unsigned first = gaiji_first_half((unsigned char)*str++);
		cell[0] = first;
		cell[1] = first | 0x8000;
		cell[0x1000] = atrb;
		cell[0x1001] = atrb;
		cell += 2;
	}
}
