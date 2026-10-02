#pragma option -zCSHARED -3

#if defined(__TURBOC__) || defined(__MSDOS__)
# include <dos.h>
#endif
#include <stdio.h>

#include "src/shared/runtime/api.hpp"

static unsigned gaiji_backup_segment;

static unsigned header_word(const unsigned char *header, unsigned at)
{
	return (unsigned)header[at] | ((unsigned)header[at + 1u] << 8);
}

static void gaiji_transfer_all(unsigned char far *patterns, int write)
{
	outportb(0x68, 0x0B);
	for(unsigned glyph = 0; glyph < 256u; glyph++) {
		const unsigned jis = (glyph + 0x5680u) & 0xFF7Fu;
		outportb(0xA1, (unsigned char)jis);
		outportb(0xA3, (unsigned char)(jis >> 8));
		for(unsigned row = 0; row < 16u; row++) {
			outportb(0xA5, (unsigned char)(row | 0x20u));
			if(write) {
				outportb(0xA9, *patterns++);
			} else {
				*patterns++ = inportb(0xA9);
			}
			outportb(0xA5, (unsigned char)row);
			if(write) {
				outportb(0xA9, *patterns++);
			} else {
				*patterns++ = inportb(0xA9);
			}
		}
	}
	outportb(0x68, 0x0A);
}

extern "C" int TH04_PASCAL gaiji_backup(void)
{
	printf("[TH04 PS3 Gaiji] gaiji_backup called\n");
	if(gaiji_backup_segment) {
		return 0;
	}
	void __seg *block = hmem_alloc(512u);
	if(!block) {
		printf("[TH04 PS3 Gaiji] gaiji_backup alloc failed\n");
		return 0;
	}
	gaiji_backup_segment = (unsigned)(uintptr_t)block;
#if defined(__TURBOC__) || defined(__MSDOS__)
	gaiji_transfer_all((unsigned char far *)MK_FP(gaiji_backup_segment, 0), 0);
#endif
	printf("[TH04 PS3 Gaiji] gaiji_backup SUCCESS\n");
	return 1;
}

extern "C" int TH04_PASCAL gaiji_restore(void)
{
	printf("[TH04 PS3 Gaiji] gaiji_restore called\n");
	if(!gaiji_backup_segment) {
		return 0;
	}
	const unsigned segment = gaiji_backup_segment;
	gaiji_backup_segment = 0;
#if defined(__TURBOC__) || defined(__MSDOS__)
	gaiji_transfer_all((unsigned char far *)MK_FP(segment, 0), 1);
#endif
	hmem_free((void __seg *)(uintptr_t)segment);
	return 1;
}

extern "C" int TH04_PASCAL gaiji_entry_bfnt(const char TH04_PTR *filename)
{
	printf("[TH04 PS3 Gaiji] gaiji_entry_bfnt called (filename=%s)\n", filename ? (const char*)filename : "NULL");
	if(!file_ropen(filename)) {
		printf("[TH04 PS3 Gaiji] gaiji_entry_bfnt: file_ropen failed\n");
		return 0;
	}
	int success = 0;
	void __seg *block = hmem_allocbyte(8192u);
	if(block) {
		unsigned char header[32];
		if(file_read(header, 32u) == 32u &&
		   header[0] == 'B' && header[1] == 'F' &&
		   header[2] == 'N' && header[3] == 'T' &&
		   header[4] == 0x1A && header[5] == 0 &&
		   header_word(header, 8u) == 16u &&
		   header_word(header, 10u) == 16u &&
		   header_word(header, 12u) == 0u &&
		   header_word(header, 14u) == 255u) {
			file_seek((long)header_word(header, 28u), SEEK_CUR);
			unsigned char far *patterns = (unsigned char far *)block;
			if(file_read(patterns, 8192u) == 8192u) {
#if defined(__TURBOC__) || defined(__MSDOS__)
				gaiji_transfer_all(patterns, 1);
#endif
				success = 1;
			}
		}
		hmem_free(block);
	}
	file_close();
	printf("[TH04 PS3 Gaiji] gaiji_entry_bfnt result=%d\n", success);
	return success;
}
