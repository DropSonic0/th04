#ifndef PS3_COMPAT_HPP
#define PS3_COMPAT_HPP

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#ifndef __cplusplus
# ifndef inline
#  define inline static
# endif
# ifndef __bool_true_false_are_defined
typedef unsigned char bool;
#  define true 1
#  define false 0
#  define __bool_true_false_are_defined 1
# endif
#endif

#ifndef __cplusplus
# ifndef inline
#  define inline static
# endif
#endif

#ifndef __TURBOC__
#ifdef __cplusplus
struct BorlandCType {
	inline int operator[](int index) const {
		int c = index - 1;
		return (c >= 0 && c <= 255 && isdigit(c)) ? 2 : 0;
	}
};
static const BorlandCType _ctype;
#endif
#endif

#if !defined(__TURBOC__) && !defined(__MSDOS__)
#ifdef __cplusplus
extern "C" {
#endif

int pf_open_member(const char* path);
unsigned pf_read_member(void* out, unsigned count);
void pf_close_member(void);
extern int g_pf_active_handle;

#define MAX_DOS_HANDLES 64
extern FILE* g_dos_handles[MAX_DOS_HANDLES];

inline int dos_handle_alloc(FILE* f) {
	if (!f) return 0;
	for (int i = 1; i < MAX_DOS_HANDLES; i++) {
		if (g_dos_handles[i] == NULL) {
			g_dos_handles[i] = f;
			return i;
		}
	}
	printf("[TH04 PS3 DOS] ERROR: dos_handle_alloc out of handles!\n");
	return 0;
}

inline FILE* dos_handle_get(int handle) {
	if (handle > 0 && handle < MAX_DOS_HANDLES) {
		return g_dos_handles[handle];
	}
	return NULL;
}

inline void dos_handle_free(int handle) {
	if (handle > 0 && handle < MAX_DOS_HANDLES) {
		g_dos_handles[handle] = NULL;
	}
}

inline void outport(unsigned short port, unsigned short val) {}
inline void outportb(unsigned short port, unsigned char val) {}
inline unsigned char __outportb__(unsigned short port, unsigned char val) { return val; }
inline unsigned short __outportw__(unsigned short port, unsigned short val) { return val; }
inline unsigned short inport(unsigned short port) { return 0; }
inline unsigned char inportb(unsigned short port) { return 0; }
inline unsigned char __inportb__(unsigned short port) { return 0; }
inline void disable(void) {}
inline void enable(void) {}
inline void __emit__(unsigned char b, ...) {}
inline void __int__(short i) {}

inline unsigned _dos_open(const char* filename, unsigned flags, int* handle) {
	printf("[TH04 PS3 DOS] _dos_open: %s\n", filename ? filename : "NULL");
	FILE* f = fopen(filename, "rb");
	if (!f && filename) {
		char path[512];
		snprintf(path, sizeof(path), "/app_home/%s", filename);
		printf("[TH04 PS3 DOS] Relative _dos_open failed, trying fallback: %s\n", path);
		f = fopen(path, "rb");
	}
	if (f) {
		*handle = dos_handle_alloc(f);
		printf("[TH04 PS3 DOS] _dos_open SUCCESS on disk: %s (handle=%d)\n", filename ? filename : "NULL", *handle);
		return 0;
	}

	if (filename) {
		int pf_handle = pf_open_member(filename);
		if (pf_handle) {
			*handle = pf_handle;
			g_pf_active_handle = pf_handle;
			printf("[TH04 PS3 DOS] _dos_open SUCCESS in GENSOU.DAT: %s (handle=%d)\n", filename, pf_handle);
			return 0;
		}
	}

	printf("[TH04 PS3 DOS] ERROR: _dos_open failed for %s\n", filename ? filename : "NULL");
	return 1;
}

inline unsigned _dos_close(int handle) {
	if (handle) {
		if (g_pf_active_handle && handle == g_pf_active_handle) {
			int pf_h = g_pf_active_handle;
			g_pf_active_handle = 0; // Clear BEFORE calling pf_close_member() to prevent infinite recursion
			printf("[TH04 PS3 DOS] _dos_close pf archive member handle %d\n", pf_h);
			pf_close_member();
			return 0;
		}
		FILE* f = dos_handle_get(handle);
		if (f) {
			printf("[TH04 PS3 DOS] _dos_close handle %d\n", handle);
			fclose(f);
			dos_handle_free(handle);
		}
	}
	return 0;
}

inline unsigned _dos_read(int handle, void* buf, unsigned count, unsigned* bytes_read) {
	if (!handle) {
		printf("[TH04 PS3 DOS] _dos_read failed: invalid handle\n");
		*bytes_read = 0;
		return 1;
	}
	FILE* f = dos_handle_get(handle);
	if (!f) {
		printf("[TH04 PS3 DOS] _dos_read failed: handle %d not found in handle table\n", handle);
		*bytes_read = 0;
		return 1;
	}
	*bytes_read = (unsigned)fread(buf, 1, count, f);
	printf("[TH04 PS3 DOS] _dos_read requested %u bytes, read %u bytes\n", count, *bytes_read);
	return 0;
}

inline unsigned _dos_allocmem(unsigned size_in_paragraphs, unsigned* segp) {
	if (size_in_paragraphs == 0xFFFFu) {
		printf("[TH04 PS3 DOS] _dos_allocmem query max available memory\n");
		if (segp) {
			*segp = 0x8000u; // 512 KB available in paragraphs
		}
		return 8; // ENOMEM error code in DOS when requesting impossible size
	}
	size_t size_bytes = (size_t)size_in_paragraphs * 16;
	void* raw = malloc(size_bytes + 32);
	if (!raw) {
		printf("[TH04 PS3 DOS] _dos_allocmem failed to allocate %u paragraphs (%zu bytes)\n", size_in_paragraphs, size_bytes);
		return 8; // ENOMEM
	}
	uintptr_t aligned = ((uintptr_t)raw + sizeof(void*) + 15) & ~((uintptr_t)15);
	((void**)aligned)[-1] = raw;
	if (segp) {
		*segp = (unsigned)(aligned >> 4);
	}
	printf("[TH04 PS3 DOS] _dos_allocmem allocated %u paragraphs at seg 0x%04X (ptr %p)\n", size_in_paragraphs, segp ? *segp : 0, (void*)aligned);
	return 0;
}

inline unsigned _dos_freemem(unsigned seg) {
	if (!seg) return 0;
	uintptr_t aligned = (uintptr_t)seg << 4;
	void* raw = ((void**)aligned)[-1];
	printf("[TH04 PS3 DOS] _dos_freemem seg 0x%04X (raw ptr %p)\n", seg, raw);
	if (raw) free(raw);
	return 0;
}

#ifdef __cplusplus
}
#endif
#endif

#ifndef BINARY
#define BINARY 'M'
#endif

#ifdef CONTINUE
#undef CONTINUE
#endif
#ifdef STOP
#undef STOP
#endif

#ifndef GAME
#define GAME 4
#endif

#ifndef MK_FP
# define MK_FP(seg, off) ((void*)(uintptr_t)(((uint32_t)(seg) << 4) + (uint32_t)(off)))
#endif

#ifndef __TURBOC__

#ifndef getch
#define getch() (void)0
#endif

#define __far
#define far
#define near
#define __near
#define pascal
#define interrupt
#define __cdecl
#define cdecl
#define __seg
#define __es
#define __ds
#define __cs
#define __ss

#ifndef TH04_FAR
#define TH04_FAR
#endif

#include <stdint.h>

#ifdef _DI
#undef _DI
#endif
#ifdef _SI
#undef _SI
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern uint32_t _EAX;
extern uint32_t _EBX;
extern uint32_t _ECX;
extern uint32_t _EDX;

extern uint16_t _AX;
extern uint16_t _BX;
extern uint16_t _CX;
extern uint16_t _DX;
extern uint16_t _SI;
extern uint16_t _DI;
extern uint16_t _ES;
extern uint16_t _DS;
extern uint16_t _CS;
extern uint16_t _SS;

extern uint8_t  _AL;
extern uint8_t  _AH;
extern uint8_t  _BL;
extern uint8_t  _BH;
extern uint8_t  _CL;
extern uint8_t  _CH;
extern uint8_t  _DL;
extern uint8_t  _DH;

#ifdef __cplusplus
}
#endif

#endif

#endif // PS3_COMPAT_HPP