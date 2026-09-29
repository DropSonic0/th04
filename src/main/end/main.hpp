#ifndef TH04_MAIN_END_MAIN_HPP
#define TH04_MAIN_END_MAIN_HPP

// Launch into MAINE.EXE, showing the respective ending.
#if (GAME == 5)
void end_game(void);
#else
void end_game_good(void);
void end_game_bad(void);

inline void end_game(void) {
	end_game_good();
}
#endif
void end_extra(void);

#endif
