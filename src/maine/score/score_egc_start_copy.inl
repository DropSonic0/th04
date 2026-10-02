void near score_egc_start_copy(void)
{
	// Cross-game PC-98 GRCG/EGC enable primitive. Registered TH01-TH05
	// release targets preserve this exact immediate-port sequence.
#ifdef __TURBOC__
	asm{
	mov al, 0
	out 0x7C, al
	mov al, 7
	out 0x6A, al
	mov al, 5
	out 0x6A, al
	mov al, 0x80
	out 0x7C, al
	mov al, 6
	out 0x6A, al
}
#else
	outportb(0x7C, 0);
	outportb(0x6A, 7);
	outportb(0x6A, 5);
	outportb(0x7C, 0x80);
	outportb(0x6A, 6);
#endif

	// Same cross-game EGC word-register setup primitive accepted by v833.
	_AX = 0xFFF0; _DX = EGC_ACTIVEPLANEREG; outport(_DX, _AX);
	_AX = 0x00FF; _DX = EGC_READPLANEREG; outport(_DX, _AX);
	_AX = 0x3100; _DX = EGC_MODE_ROP_REG; outport(_DX, _AX);
	_AX = 0xFFFF; _DX = EGC_MASKREG; outport(_DX, _AX);

	// TC4.02 otherwise peepholes literal zero to XOR AX,AX. The complete
	// cross-game setup core consistently contains MOV AX,0 here.
#ifdef __TURBOC__
	asm{ mov ax, 0; }
#else
	_AX = 0;
#endif
	_DX = EGC_ADDRRESSREG; outport(_DX, _AX);

	_AX = 0x000F; _DX = EGC_BITLENGTHREG; outport(_DX, _AX);
}
