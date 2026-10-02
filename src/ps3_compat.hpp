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

inline void outport(unsigned short port, unsigned short val) {}
inline void outportb(unsigned short port, unsigned char val) {}
inline unsigned short inport(unsigned short port) { return 0; }
inline unsigned char inportb(unsigned short port) { return 0; }
inline void disable(void) {}
inline void enable(void) {}

inline unsigned _dos_open(const char* filename, unsigned flags, int* handle) {
	FILE* f = fopen(filename, "rb");
	if (!f) return 1;
	*handle = (int)(intptr_t)f;
	return 0;
}

inline unsigned _dos_close(int handle) {
	if (handle) fclose((FILE*)(intptr_t)handle);
	return 0;
}

inline unsigned _dos_read(int handle, void* buf, unsigned count, unsigned* bytes_read) {
	if (!handle) { *bytes_read = 0; return 1; }
	*bytes_read = (unsigned)fread(buf, 1, count, (FILE*)(intptr_t)handle);
	return 0;
}

inline unsigned _dos_allocmem(unsigned size_in_paragraphs, unsigned* segp) {
	void* ptr = malloc((size_t)size_in_paragraphs * 16);
	if (!ptr) return 8; // ENOMEM
	if (segp) {
		*segp = (unsigned)(((uintptr_t)ptr) >> 4);
	}
	return 0;
}

inline unsigned _dos_freemem(unsigned seg) {
	void* ptr = (void*)(uintptr_t)((uint32_t)seg << 4);
	if (ptr) free(ptr);
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