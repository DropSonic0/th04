void near egc_start_copy(void)
{
	egc_on();

	// Cross-game EGC copy setup primitive. TH01/TH02/TH03/TH04/TH05 release
	// targets independently preserve this AX=value -> DX=port -> OUT DX,AX
	// ordering as one complete six-register setup sequence.
	_AX = 0xFFF0; _DX = EGC_ACTIVEPLANEREG; outport(_DX, _AX);
	_AX = 0x00FF; _DX = EGC_READPLANEREG; outport(_DX, _AX);
	_AX = 0x3100; _DX = EGC_MODE_ROP_REG; outport(_DX, _AX);
	_AX = 0xFFFF; _DX = EGC_MASKREG; outport(_DX, _AX);

	// TC4J otherwise peepholes literal zero to XOR AX,AX. The release-target
	// setup core consistently contains MOV AX,0 at this position.
	asm { mov ax, 0; }
	_DX = EGC_ADDRRESSREG; outport(_DX, _AX);

	_AX = 0x000F; _DX = EGC_BITLENGTHREG; outport(_DX, _AX);
}
