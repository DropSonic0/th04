void near egc_start_copy(void)
{
	egc_on();

	_AX = 0xFFF0; _DX = EGC_ACTIVEPLANEREG; outport(_DX, _AX);
	_AX = 0x00FF; _DX = EGC_READPLANEREG; outport(_DX, _AX);
	_AX = 0x3100; _DX = EGC_MODE_ROP_REG; outport(_DX, _AX);
	_AX = 0xFFFF; _DX = EGC_MASKREG; outport(_DX, _AX);

#ifdef __TURBOC__
	asm{ mov ax, 0; }
#else
	_AX = 0;
#endif
	_DX = EGC_ADDRRESSREG; outport(_DX, _AX);

	_AX = 0x000F; _DX = EGC_BITLENGTHREG; outport(_DX, _AX);
}