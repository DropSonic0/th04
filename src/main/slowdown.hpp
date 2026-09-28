#ifndef TH04_MAIN_SLOWDOWN_HPP
#define TH04_MAIN_SLOWDOWN_HPP

#include "src/shared/platform/types.hpp"

// MAIN-local frame pacing state. The byte/word widths are part of the DOS ABI.
extern bool turbo_mode;
extern unsigned int slowdown_factor;

#if (GAME == 5)
extern bool slowdown_caused_by_bullets;
#endif

void near slowdown_frame_delay(void);

#endif
