// TH04 packed PI reader. Each image is one paragraph-allocated block; the
// returned pointer skips the two reference rows at the front of that block.
#pragma option -zCSHARED -3

#include <dos.h>

#include "src/shared/hardware/graphics.hpp"
#include "src/shared/runtime/api.hpp"

struct PiInput {
	int handle;
	unsigned fill, next;
	unsigned char bytes[512];
	unsigned char bits;
	unsigned remaining;
	int failed;
	unsigned char colors[16][16];
};
static PiInput input;

static int pi_byte(void)
{
	if (input.next == input.fill) {
		unsigned got = 0;
		if (_dos_read(input.handle, input.bytes, sizeof(input.bytes), &got) || !got) {
			input.failed = 1;
			return -1;
		}
		input.fill = got;
		input.next = 0;
	}
	return input.bytes[input.next++];
}

static int pi_bit(void)
{
	if (!input.remaining) {
		int value = pi_byte();
		if (value < 0) {
			return -1;
		}
		input.bits = (unsigned char)value;
		input.remaining = 8;
	}
	int bit = (input.bits >> 7) & 1;
	input.bits <<= 1;
	input.remaining--;
	return bit;
}

static unsigned long pi_bits(unsigned count)
{
	unsigned long value = 0;
	while (count--) {
		int bit = pi_bit();
		if (bit < 0) {
			return 0;
		}
		value = (value << 1) | (unsigned)bit;
	}
	return value;
}

static int pi_word_be(void)
{
	int high = pi_byte();
	int low = pi_byte();
	if ((high < 0) || (low < 0)) {
		return -1;
	}
	return (high << 8) | low;
}

static int pi_color(unsigned context)
{
	int first = pi_bit();
	unsigned base, width;
	if (first < 0) {
		return -1;
	}
	if (first) {
		base = 0;
		width = 1;
	} else {
		int second = pi_bit();
		if (second < 0) {
			return -1;
		}
		if (!second) {
			base = 2;
			width = 1;
		} else {
			int third = pi_bit();
			if (third < 0) {
				return -1;
			}
			base = third ? 8 : 4;
			width = third ? 3 : 2;
		}
	}
	unsigned index = (base + (unsigned)pi_bits(width)) ^ 15u;
	unsigned char value = input.colors[context][index];
	for (unsigned at = index; at < 15u; at++) {
		input.colors[context][at] = input.colors[context][at + 1u];
	}
	input.colors[context][15] = value;
	return input.failed ? -1 : value;
}

static unsigned char pi_image_byte(unsigned base, unsigned long at)
{
	unsigned segment = base + (unsigned)(at >> 4);
	return *(unsigned char far *)MK_FP(segment, (unsigned)(at & 15u));
}

static void pi_image_put(unsigned base, unsigned long at, unsigned char value)
{
	unsigned segment = base + (unsigned)(at >> 4);
	*(unsigned char far *)MK_FP(segment, (unsigned)(at & 15u)) = value;
}

static int pi_unpack(unsigned base, unsigned width, unsigned long total)
{
	unsigned ctx, at;
	for (ctx = 0; ctx < 16u; ctx++) {
		for (at = 0; at < 16u; at++) {
			input.colors[ctx][at] = (unsigned char)((ctx + at + 1u) & 15u);
		}
	}
	input.remaining = 0;
	int first = pi_color(0);
	int second = first < 0 ? -1 : pi_color((unsigned)first);
	if (second < 0) {
		return 0;
	}
	unsigned char initial = (unsigned char)((first << 4) | second);
	for (at = 0; at < width; at++) {
		pi_image_put(base, at, initial);
	}
	unsigned long cursor = width;
	int previous_position = -1;
	while (cursor < total) {
		unsigned position = (unsigned)pi_bits(2);
		if (input.failed) {
			return 0;
		}
		if (position == 3u) {
			int extension = pi_bit();
			if (extension < 0) {
				return 0;
			}
			position += (unsigned)extension;
		}
		if (position == (unsigned)previous_position) {
			int continued;
			do {
				int high = pi_color(pi_image_byte(base, cursor - 1u) & 15u);
				int low = high < 0 ? -1 : pi_color((unsigned)high);
				if ((low < 0) || (cursor == total)) {
					return 0;
				}
				pi_image_put(base, cursor++, (unsigned char)((high << 4) | low));
				continued = pi_bit();
				if (continued < 0) {
					return 0;
				}
			} while (continued);
			previous_position = -1;
			continue;
		}

		unsigned length_bits = 0;
		int unary;
		while ((unary = pi_bit()) == 1) {
			if (++length_bits > 19u) {
				return 0;
			}
		}
		if (unary < 0) {
			return 0;
		}
		unsigned long length = (1UL << length_bits) | pi_bits(length_bits);
		if (input.failed || (length > total - cursor)) {
			return 0;
		}
		if (position == 0u) {
			unsigned char last = pi_image_byte(base, cursor - 1u);
			if ((last >> 4) == (last & 15u)) {
				while (length--) {
					pi_image_put(base, cursor++, last);
				}
			} else {
				unsigned char before = pi_image_byte(base, cursor - 2u);
				unsigned phase = 0;
				while (length--) {
					pi_image_put(base, cursor++, phase ? last : before);
					phase ^= 1u;
				}
			}
		} else if ((position == 1u) || (position == 2u)) {
			unsigned distance = position == 1u ? width / 2u : width;
			if ((distance == 0) || (cursor < distance)) {
				return 0;
			}
			while (length--) {
				pi_image_put(base, cursor, pi_image_byte(base, cursor - distance));
				cursor++;
			}
		} else {
			unsigned long pixels = position == 3u ?
				(unsigned long)width - 1UL : (unsigned long)width + 1UL;
			unsigned distance = (unsigned)((pixels + 1UL) / 2UL);
			if ((width < 3u) || (cursor < distance)) {
				return 0;
			}
			while (length--) {
				unsigned char left = pi_image_byte(base, cursor - distance);
				unsigned char right = pi_image_byte(base, cursor - distance + 1u);
				pi_image_put(base, cursor++, (unsigned char)((left << 4) | (right >> 4)));
			}
		}
		previous_position = (int)position;
	}
	return !input.failed;
}

extern "C" int TH04_PASCAL graph_pi_load_pack(
	const char far *filename, PiHeader far *header, void far *far *bufptr
)
{
	int handle;
	unsigned base = 0;
	unsigned char __seg *extension = 0;
	unsigned comment_length = 0;
	unsigned width, height;
	unsigned long total;
	unsigned i;
	int value, aspect_n, aspect_m, plane;
	unsigned char far *palette;
	if (!header || !bufptr) {
		return -13;
	}
	header->comment = 0;
	header->commentlen = 0;
	header->maex = 0;
	header->maexlen = 0;
	*bufptr = 0;
	if (_dos_open(filename, 0, &handle)) {
		return -2;
	}
	input.handle = handle;
	input.fill = input.next = input.remaining = 0;
	input.failed = 0;
	if ((pi_byte() != 'P') || (pi_byte() != 'i')) {
		goto invalid;
	}
	while ((value = pi_byte()) >= 0 && value != 26) {
		if (comment_length == 65535u) {
			goto invalid;
		}
		comment_length++;
	}
	if (value < 0) {
		goto invalid;
	}
	header->commentlen = comment_length;
	while ((value = pi_byte()) > 0) { }
	if (value < 0) {
		goto invalid;
	}
	value = pi_byte();
	if (value < 0) {
		goto invalid;
	}
	header->mode = (unsigned char)value;
	aspect_n = pi_byte();
	aspect_m = pi_byte();
	plane = pi_byte();
	if ((aspect_n != 0) || (aspect_m != 0) || (plane != 4)) {
		goto invalid;
	}
	header->n = (unsigned char)aspect_n;
	header->m = (unsigned char)aspect_m;
	header->plane = (unsigned char)plane;
	for (i = 0; i < 4u; i++) {
		value = pi_byte();
		if (value < 0) {
			goto invalid;
		}
		header->machine[i] = (char)value;
	}
	value = pi_word_be();
	if (value < 0) {
		goto invalid;
	}
	header->maexlen = (unsigned)value;
	if (header->maexlen) {
		extension = (unsigned char __seg *)hmem_allocbyte(header->maexlen);
		if (!extension) {
			goto no_memory;
		}
		header->maex = (void far *)extension;
		for (i = 0; i < header->maexlen; i++) {
			value = pi_byte();
			if (value < 0) {
				goto invalid;
			}
			extension[i] = (unsigned char)value;
		}
	}
	value = pi_word_be();
	if (value < 0) {
		goto invalid;
	}
	width = (unsigned)value;
	value = pi_word_be();
	if (value < 0) {
		goto invalid;
	}
	height = (unsigned)value;
	if ((width < 3u) || (height == 0) ||
		((unsigned long)height + 2UL > 2097088UL / (unsigned long)width)) {
		goto invalid;
	}
	header->xsize = width;
	header->ysize = height;
	if (!(header->mode & 0x80u)) {
		palette = (unsigned char far *)&header->palette;
		for (i = 0; i < 48u; i++) {
			value = pi_byte();
			if (value < 0) {
				goto invalid;
			}
			palette[i] = (unsigned char)value;
		}
	}
	total = (unsigned long)width * ((unsigned long)height + 2UL) / 2UL;
	base = (unsigned)hmem_alloc((unsigned)((total + 15UL) >> 4));
	if (!base) {
		goto no_memory;
	}
	if (!pi_unpack(base, width, total)) {
		goto invalid;
	}
	*bufptr = MK_FP(base, width);
	_dos_close(handle);
	return 0;

invalid:
	if (base) {
		hmem_free((void __seg *)base);
	}
	if (extension) {
		hmem_free(extension);
		header->maex = 0;
		header->maexlen = 0;
	}
	_dos_close(handle);
	return -13;
no_memory:
	if (base) {
		hmem_free((void __seg *)base);
	}
	if (extension) {
		hmem_free(extension);
		header->maex = 0;
		header->maexlen = 0;
	}
	_dos_close(handle);
	return -8;
}
