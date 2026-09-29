#ifndef TH04_MAIN_HISCORE_HPP
#define TH04_MAIN_HISCORE_HPP

// High-score entry points used by the MAIN game-over and session paths.
// Storage for the score tables remains owned by the score-data implementation.
void near hiscore_continue_enter(void);
void near hiscore_load(void);

#endif
