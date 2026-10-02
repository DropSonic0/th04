#ifndef TH04_MAIN_DEMO_HPP
#define TH04_MAIN_DEMO_HPP

#include <stddef.h>

#include "src/shared/platform/types.hpp"
#include "src/shared/runtime/api.hpp"
#include "src/shared/hardware/graphics.hpp"
#include "src/main/core/gameexecl.hpp"
#include "src/main/hardware/input.hpp"

#if (GAME == 5)
#define DEMO_N 5000
#define DEMO_N_EXTRA (DEMO_N * 4)
#else
#define DEMO_N 4000
#endif

// DEMO?.REC stores one input byte and one shift flag for every frame.
template <size_t Frames> struct REC {
	input_replay_t input[Frames];
	bool shift[Frames];
};

extern uint8_t far *DemoBuf;

// Playback ends by freeing the replay, fading the palette, and chaining to OP.
#ifdef __TURBOC__
#define demo_end() { \
	HMem<uint8_t>::free(DemoBuf); \
	palette_black_out((GAME == 5) ? 8 : 10); \
	/* Cross-segment GameExecl needs the target's far argument plus a near call. */ \
	_asm { \
	push ds; \
	push offset BINARY_OP; \
	nop; \
	push cs; \
	call near ptr GameExecl; \
} \
}
#else
#define demo_end() { \
	HMem<uint8_t>::free(DemoBuf); \
	palette_black_out((GAME == 5) ? 8 : 10); \
	GameExecl(BINARY_OP); \
}
#endif

void near DemoPlay(void);

#endif
