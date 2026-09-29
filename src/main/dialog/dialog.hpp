#ifndef TH04_MAIN_DIALOG_HPP
#define TH04_MAIN_DIALOG_HPP

#include "src/shared/platform/types.hpp"

// Runs the stage-frame transition that enters a blocking dialog when the
// scrolling and page state reaches the pre-boss boundary.
bool near std_update_frames_then_animate_dialog_and_activate_boss_if_done(
    void
);

// Set to the above callback while running the pre-boss part of a stage.
extern bool (near *std_update)(void);

// Runs the next dialog scene in a blocking way.
void dialog_animate(void);

#if (GAME == 4)
// Dialog-related image functions with optional EMS support.
void near dialog_init(void);
void near dialog_exit(void);
#endif

#endif
