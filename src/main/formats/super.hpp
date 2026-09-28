#ifndef TH04_MAIN_FORMATS_SUPER_HPP
#define TH04_MAIN_FORMATS_SUPER_HPP

// Micro-optimized, vertically-wrapped sprite display functions. The caller
// sets ES to a VRAM plane and enables GRCG RMW mode before entering these APIs.
extern "C" {

#define z_super_put_16x16_mono(left, top, patnum) \
	_AX = top; \
	_CX = left; \
	z_super_put_16x16_mono_raw(patnum);
void pascal near z_super_put_16x16_mono_raw(int patnum);

#define z_super_roll_put_tiny_16x16(left, top, patnum) \
	_DX = top; \
	_AX = left; \
	z_super_roll_put_tiny_16x16_raw(patnum);
void pascal near z_super_roll_put_tiny_16x16_raw(int patnum);

#define z_super_roll_put_tiny_32x32(left, top, patnum) \
	_DX = top; \
	_AX = left; \
	z_super_roll_put_tiny_32x32_raw(patnum);
void pascal near z_super_roll_put_tiny_32x32_raw(int patnum);

}

#endif
