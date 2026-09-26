void pascal near box_1_to_0_masked(box_mask_t mask)
{
	for(screen_y_t y = BOX_TOP; y < BOX_BOTTOM; y++) {
		// Same cross-game EGC word-register primitive as egc_start_copy().
		_AX = 0x00FF; _DX = EGC_READPLANEREG; outport(_DX, _AX);
		_AX = (EGC_COMPAREREAD | EGC_WS_PATREG | EGC_RL_MEMREAD);
		_DX = EGC_MODE_ROP_REG; outport(_DX, _AX);
		_AX = (EGC_REGISTER_DOTS - 1);
		_DX = EGC_BITLENGTHREG; outport(_DX, _AX);

		// Dynamic mask loading already gives the target AX-before-DX order.
		outport(EGC_MASKREG, BOX_MASKS[mask][y & 3]);

		vram_offset_t vo = ((y << 6) + (y << 4) + (BOX_LEFT / BYTE_DOTS));
		pixel_t x = 0;
		while(x < BOX_W) {
			graph_accesspage(1);
			egc_temp_t tmp = *reinterpret_cast<egc_temp_t far *>(
				VRAM_PLANE_B + vo
			);
			graph_accesspage(0);
			*reinterpret_cast<egc_temp_t far *>(VRAM_PLANE_B + vo) = tmp;
			x += EGC_REGISTER_DOTS;
			vo += EGC_REGISTER_SIZE;
		}
	}
}
