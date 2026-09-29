/* TH04 MAIN decompilation helpers recovered from the pinned ReC98 surface. */

#ifndef DECOMP_HPP
#define DECOMP_HPP

#include "x86real.h"

// Alternate version that sets the value first.
#define outport2(port, val) _asm { \
	mov ax, val; \
	mov dx, port; \
	out dx, ax; \
}

// Bytewise access used by a small number of historically compiler-shaped
// copies. The wrapper is intentionally transparent to the owning type.
template <class T> union StupidBytewiseWrapperAround {
	T t;
	int8_t byte[sizeof(T)];
	uint8_t ubyte[sizeof(T)];
};

// These helpers preserve the target's register setup around REP MOVSW. They
// are source-level mechanisms, not target byte storage or inline byte arrays.
#define prepare_di_si(dst, dst_offset, src, src_offset) { \
	_DI = dst; \
	_DI += dst_offset; \
	_SI = src; \
	_SI += src_offset; \
}

#define prepare_si_di(dst, dst_offset, src, src_offset) { \
	_SI = src; \
	_SI += src_offset; \
	_DI = dst; \
	_DI += dst_offset; \
}

#if (GAME == 5)
#define copy_near_struct_member( \
	dst, dst_offset, src, src_offset, size, prepare_func \
) { \
	_CX = (size / sizeof(uint16_t)); \
	asm { push ds; pop es; } \
	prepare_func(FP_OFF(&dst), dst_offset, FP_OFF(&src), src_offset); \
	asm { rep movsw; } \
}
#else
#define copy_near_struct_member( \
	dst, dst_offset, src, src_offset, size, prepare_func \
) { \
	asm { push ds; pop es; } \
	prepare_func(FP_OFF(&dst), dst_offset, FP_OFF(&src), src_offset); \
	_CX = (size / sizeof(uint16_t)); \
	asm { rep movsw; } \
}
#endif

// Compiler-layout barriers used by reconstructed source. They intentionally
// remain separate from semantic target DATA/BSS ownership.
#if defined(__TURBOC__) && defined(__MSDOS__)
template <class T> inline T keep_0(T x) {
	if(x == 0) {
		extern void *near address_0;
		return reinterpret_cast<T>(&address_0);
	}
	return x;
}

#define inhibit_Z3(x) \
	*reinterpret_cast<int16_t near *>(reinterpret_cast<uint16_t>(&x))

inline bool optimization_barrier(void) {
	return false;
}
#else
#define keep_0(x) x
#define inhibit_Z3(x) x
#define optimization_barrier()
#endif

// 32-bit instructions unavailable to Turbo C++'s built-in assembler.
#define MOVSD	__emit__(0x66, 0xA5);
#define STOSD	__emit__(0x66, 0xAB);
#define REP  	__emit__(0xF3);

#endif /* DECOMP_HPP */
