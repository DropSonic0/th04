void main(void)
{
	int idle_frame = 0;
	printf("[TH04 PS3 Main] main() entered\n");

	text_clear();
	respal_create(); // ZUN bloat: These games don't use resident palettes.
	mem_assign_paras = (336000 >> 4);
	printf("[TH04 PS3 Main] calling game_init_op(%s)...\n", OP_AND_END_PF_FN ? (const char*)OP_AND_END_PF_FN : "NULL");
	if(game_init_op(OP_AND_END_PF_FN)) {
		printf("[TH04 PS3 Main] ERROR: game_init_op failed\n");
		dos_puts2(MEMORY_INSUFFICIENT);
		getch();
	}
	printf("[TH04 PS3 Main] game_init_op completed\n");

#if (GAME == 4)
	printf("[TH04 PS3 Main] gaiji_backup & gaiji_entry_bfnt(%s)...\n", GAIJI_FN);
	gaiji_backup();
	gaiji_entry_bfnt(GAIJI_FN);
	printf("[TH04 PS3 Main] gaiji loaded\n");
#endif

	printf("[TH04 PS3 Main] cfg_load()...\n");
	cfg_load();
	printf("[TH04 PS3 Main] cfg_load completed\n");

	if(resident->rank == RANK_SHOW_SETUP_MENU) {
		printf("[TH04 PS3 Main] setup_menu()...\n");
		setup_menu();
		resident->rank = RANK_NORMAL;
	}
	printf("[TH04 PS3 Main] snd_redetermine_modes_and_reload_se()...\n");
	snd_redetermine_modes_and_reload_se();
	printf("[TH04 PS3 Main] snd_redetermine_modes completed\n");

	if(!resident->zunsoft_shown) {
		printf("[TH04 PS3 Main] zunsoft_animate()...\n");
		zunsoft_animate();
		resident->zunsoft_shown = true;
		printf("[TH04 PS3 Main] zunsoft_animate completed\n");
	}

	if((GAME == 5) && resident->demo_num == 5) {
		resident->demo_num = 0;
	}
	if(resident->demo_num == 0) {
		printf("[TH04 PS3 Main] stopping song...\n");
		snd_kaja_func(KAJA_SONG_STOP, 0);
	}
	printf("[TH04 PS3 Main] op_animate()...\n");
	op_animate();
	printf("[TH04 PS3 Main] op_animate completed\n");

#if (GAME == 5)
	main_cdg_load();

	// ZUN bug: cleardata_and_regist_view_sprites_load() ends with a call to
	// super_entry_bfnt(), which overwrites the master.lib [Palettes] with the
	// palette from hi_m.bft's palette. In TH05, this file has a different
	// palette than the one we loaded from OP1.PI earlier during op_animate(),
	// thus resulting in [Palettes] going out of sync with the hardware
	// palette.
	// This is merely a landmine in this menu, but then turns into a bug in
	// regist_view_menu(). As this function calls palette_black_out(), which
	// operates on [Palettes], it thus fades out a much brighter palette than
	// the one currently shown if the High Score screen is the first subscreen
	// entered within a OP.EXE process.
	// The two most obvious ways of fixing this bug:
	// 1) Separate clear data loading from sprite loading (as any sane coder
	//    would do), and load the sprites inside regist_view_menu()
	// 2) Call this function before op_animate() and either bump or remove the
	//    memory limit of OP.EXE ([mem_assign_paras]) accordingly to reserve
	//    enough room in conventional RAM for both these sprites and all title
	//    animation cels
	cleardata_and_regist_view_sprites_load();
#else
	cleardata_and_regist_view_sprites_load();
	main_cdg_load();
#endif
	in_option = false;
	quit = false;
	menu_sel = 0;
	while(!quit) {
		input_reset_sense_interface();
		switch(in_option) {
		case false:
			main_update_and_render();
			if(idle_frame >= 640) {
				start_demo();
#if (GAME == 5)
				// ZUN bloat: Execution never gets here.
				idle_frame = 0;
#endif
			}
			break;

		case true:
			option_update_and_render();
			break;
		}

		// Holding Left+Right triggers the hidden Extra Stage replay in
		// start_demo(). Don't reset [idle_frame] for that specific input, as
		// that function would otherwise never be called.
		if(
			!key_det || ((GAME == 5) && (key_det == (INPUT_LEFT | INPUT_RIGHT)))
		) {
			idle_frame++;
		} else {
			idle_frame = 0;
		}

		resident->rand++;
		frame_delay(1);
	}
	main_cdg_free();
	cfg_save_exit();
#if (GAME == 4)
	gaiji_restore();
#endif
	text_clear();
	game_exit_to_dos();
	respal_free(); // ZUN bloat: These games don't use resident palettes.
}
