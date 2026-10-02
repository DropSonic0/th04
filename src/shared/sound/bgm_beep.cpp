// TH04 PC-98 beeper effects from EFS decimal streams. PMD owns FM music.
#pragma option -zCSHARED -3

#include <dos.h>
#include <stdio.h>

#include "src/shared/runtime/api.hpp"

extern "C" unsigned bgm_timer_divisor;
extern "C" void TH04_PASCAL bgm_timer_start(void);
extern "C" void TH04_PASCAL bgm_timer_stop(void);

static unsigned music_segments[3];
static unsigned sound_segments[16];
static unsigned sound_count;
static unsigned initialized;
static unsigned char clock_8mhz;
static volatile unsigned effect_active;
static volatile unsigned effect_index;
static volatile unsigned effect_length;
static unsigned effect_hz[256];
static unsigned effect_divisor[256];
static volatile unsigned timer_tick;

struct EfsInput {
	int handle;
	unsigned fill, next;
	unsigned char bytes[512];
	int failed;
};
static EfsInput efs;

static int efs_byte(void)
{
	if (efs.next == efs.fill) {
		unsigned got = 0;
		if (_dos_read(efs.handle, efs.bytes, sizeof(efs.bytes), &got)) {
			efs.failed = 1;
			return -1;
		}
		if (!got) {
			return -1;
		}
		efs.fill = got;
		efs.next = 0;
	}
	return efs.bytes[efs.next++];
}

static void beep_off(void)
{
	unsigned count = bgm_timer_divisor >> 1;
	outportb(0x3FDB, (unsigned char)count);
	outportb(0x3FDB, (unsigned char)(count >> 8));
	outportb(0x37, 7);
}

static void bgm_release(void)
{
	unsigned i;
	for (i = 0; i < 16u; i++) {
		if (sound_segments[i]) {
			hmem_free((void __seg *)sound_segments[i]);
			sound_segments[i] = 0;
		}
	}
	for (i = 0; i < 3u; i++) {
		if (music_segments[i]) {
			hmem_free((void __seg *)music_segments[i]);
			music_segments[i] = 0;
		}
	}
}

extern "C" int TH04_PASCAL bgm_init(int bufsiz)
{
	unsigned i;
	printf("[TH04 PS3 Sound] bgm_init called (bufsiz=%d)\n", bufsiz);
	if (initialized) {
		printf("[TH04 PS3 Sound] bgm_init: already initialized\n");
		return 0;
	}
	unsigned music_size = bufsiz > 0 ? (unsigned)bufsiz : 4096u;
	for (i = 0; i < 3u; i++) {
		music_segments[i] = (unsigned)(uintptr_t)hmem_allocbyte(music_size);
		if (!music_segments[i]) {
			printf("[TH04 PS3 Sound] ERROR: music_segments[%u] alloc failed\n", i);
			bgm_release();
			return -8;
		}
	}
	for (i = 0; i < 16u; i++) {
		// 256 decimal samples plus a terminating zero word.
		sound_segments[i] = (unsigned)(uintptr_t)hmem_allocbyte(514u);
		if (!sound_segments[i]) {
			printf("[TH04 PS3 Sound] ERROR: sound_segments[%u] alloc failed\n", i);
			bgm_release();
			return -8;
		}
	}
#if !defined(__TURBOC__) && !defined(__MSDOS__)
	clock_8mhz = 0;
#else
	clock_8mhz = (*(unsigned char far *)MK_FP(0, 0x501) & 0x80u) != 0;
#endif
	// The historical 8 MHz clock base is rounded down to an even PIT count.
	bgm_timer_divisor = clock_8mhz ? 1996u : 2458u;
	sound_count = 0;
	effect_active = 0;
	effect_index = effect_length = timer_tick = 0;
	outportb(0x3FDF, 0x76); // PC-98 beeper PIT, square wave, low/high byte.
	beep_off();
	bgm_timer_start();
	initialized = 1;
	printf("[TH04 PS3 Sound] bgm_init SUCCESS (timer_divisor=%u)\n", bgm_timer_divisor);
	return 0;
}

extern "C" void TH04_PASCAL bgm_finish(void)
{
	printf("[TH04 PS3 Sound] bgm_finish called\n");
	effect_active = 0;
	if (initialized) {
		bgm_timer_stop();
	}
	beep_off();
	bgm_release();
	sound_count = 0;
	initialized = 0;
}

static int efs_store(unsigned long value, unsigned *sequence, unsigned *length)
{
	if ((value > 65535UL) || (*sequence >= 16u)) {
		return 0;
	}
	unsigned far *dest = (unsigned far *)MK_FP(sound_segments[*sequence], 0);
	if (value == 0) {
		dest[*length] = 0;
		(*sequence)++;
		*length = 0;
		return 1;
	}
	if (*length >= 256u) {
		return 0;
	}
	dest[(*length)++] = (unsigned)value;
	return 1;
}

static int efs_parse(void)
{
	unsigned sequence = 0, length = 0;
	unsigned long value = 0;
	int in_number = 0, in_comment = 0, c;
	while ((c = efs_byte()) >= 0) {
		if (in_comment) {
			if (c == '\n') {
				in_comment = 0;
			}
			continue;
		}
		if ((c >= '0') && (c <= '9')) {
			value = value * 10UL + (unsigned)(c - '0');
			if (value > 65535UL) {
				return -11;
			}
			in_number = 1;
			continue;
		}
		if (in_number) {
			if (!efs_store(value, &sequence, &length)) {
				return -13;
			}
			value = 0;
			in_number = 0;
		}
		if (c == ';') {
			in_comment = 1;
		}
	}
	if (efs.failed) {
		return -13;
	}
	if (in_number && !efs_store(value, &sequence, &length)) {
		return -13;
	}
	if (!sequence || length) {
		return -11;
	}
	sound_count = sequence;
	return 0;
}

extern "C" int TH04_PASCAL bgm_read_sdata(const char far *filename)
{
	int handle;
	printf("[TH04 PS3 Sound] bgm_read_sdata called (filename=%s)\n", filename ? (const char*)filename : "NULL");
	if (!initialized) {
		printf("[TH04 PS3 Sound] bgm_read_sdata ERROR: not initialized\n");
		return -8;
	}
	if (!filename || _dos_open(filename, 0, &handle)) {
		printf("[TH04 PS3 Sound] bgm_read_sdata ERROR: _dos_open failed for %s\n", filename ? (const char*)filename : "NULL");
		return -2;
	}
	effect_active = 0;
	beep_off();
	sound_count = 0;
	efs.handle = handle;
	efs.fill = efs.next = 0;
	efs.failed = 0;
	int result = efs_parse();
	_dos_close(handle);
	printf("[TH04 PS3 Sound] bgm_read_sdata result=%d (sound_count=%u)\n", result, sound_count);
	return result;
}

extern "C" int TH04_PASCAL bgm_sound(int num)
{
	printf("[TH04 PS3 Sound] bgm_sound called (num=%d)\n", num);
	if (!initialized || (num < 1) || ((unsigned)num > sound_count)) {
		return -13;
	}
	effect_active = 0;
	beep_off();
	unsigned far *source =
		(unsigned far *)MK_FP(sound_segments[(unsigned)num - 1u], 0);
	unsigned long clock = clock_8mhz ? 1996800UL : 2457600UL;
	unsigned count;
	for (count = 0; count < 256u && source[count]; count++) {
		unsigned hz = source[count];
		unsigned long divisor = clock / (unsigned long)hz;
		effect_hz[count] = hz;
		effect_divisor[count] =
			divisor > 65535UL ? 65535u : (unsigned)divisor;
	}
	effect_length = count;
	effect_index = 0;
	effect_active = count != 0;
	return 0;
}

// Called by the code-resident PC-98 IRQ 8 wrapper. Return the emitted Hertz
// value for deterministic DOS diagnostics; the interrupt ignores AX.
extern "C" unsigned TH04_PASCAL bgm_tick(void)
{
	unsigned tick = timer_tick + 1u;
	if (tick >= 20u) {
		timer_tick = 0;
		return 0;
	}
	timer_tick = tick;
	if ((tick & 3u) || !effect_active) {
		return 0;
	}
	unsigned index = effect_index;
	if (index >= effect_length) {
		effect_active = 0;
		beep_off();
		return 0;
	}
	effect_index = index + 1u;
	unsigned count = effect_divisor[index];
	outportb(0x37, 6);
	outportb(0x3FDB, (unsigned char)count);
	outportb(0x3FDB, (unsigned char)(count >> 8));
	return effect_hz[index];
}
