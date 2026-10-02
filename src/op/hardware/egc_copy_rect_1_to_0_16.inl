// Maintained hybrid reconstruction of TH04 OP's EGC page-1-to-page-0
// rectangle copier.
//
// TH05 OP and MAINE independently preserve the same parameter/register
// allocation and rectangle arithmetic, adding only their out-of-range guard.
// Keep those ordinary arithmetic/control-flow parts in C++. Retain only
// irreducible x86/PC-98 primitives symbolically; do not emit opcode bytes.
void DEFCONV egc_copy_rect_1_to_0_16(
	screen_x_t left, vram_y_t top, pixel_t w, pixel_t h
)
{
#ifdef __TURBOC__
	#define vo_tmp	_BX // vram_offset_t
	#define first_bit	_CX
	#define stride	static_cast<vram_byte_amount_t>(_BP)
	#define w_tmp	static_cast<vram_word_amount_t>(_AX)
	#define rows_remaining	static_cast<pixel_t>(_BX)
	#define dots	static_cast<dots16_t>(_DX)

	asm { cld; }
	egc_start_copy();

	outport(EGC_MODE_ROP_REG, 0x29F0);

	_AX = left;
	_DX = top;

	vo_tmp = _AX;
	static_cast<vram_offset_t>(vo_tmp) >>= EGC_REGISTER_BITS;

	asm { shl bx, 1; }

	_DX <<= 6;
	vo_tmp += _DX;
	_DX >>= 2;
	vo_tmp += _DX;

	_DI = vo_tmp;
	_AX &= EGC_REGISTER_MASK;
	first_bit = _AX;

	w_tmp = ((_AX + w) >> EGC_REGISTER_BITS);
	if(first_bit) {
		w_tmp++;
	}
	egcrect_w = w_tmp;

	_CX = (ROW_SIZE / EGC_REGISTER_SIZE);
	_CX -= w_tmp;
	asm { shl cx, 1; }

	rows_remaining = h;
	stride = _CX;
	_ES = SEG_PLANE_B;

	do {
		_CX = egcrect_w;
		put_loop: {
			_AL = 1;
			asm { out 0xA6, al; }
			dots = peek(_ES, _DI);

			_AX ^= _AX;
			asm { out 0xA6, al; }

			_AX = dots;
			asm { stosw; loop put_loop; }
		}
		_DI += stride;
		rows_remaining--;
	} while(!FLAGS_SIGN);

	egc_off();

	#undef dots
	#undef rows_remaining
	#undef w_tmp
	#undef stride
	#undef first_bit
	#undef vo_tmp
#else
	egc_off();
#endif
}