// TH04 MAIN's historical stages.cpp object combines the Stage 4 carpet
// lighting and Stage 5 star callbacks.  The checkerboard producer is a
// separate maintained physical owner.
#pragma option -3 -Z

#include "x86real.h"
#include "th04/formats/cdg.h"
#include "th04/main/frames.h"
#include "th04/main/null.hpp"
#include "th04/main/scroll.hpp"
#include "src/main/boss/boss.hpp"
#include "th04/main/stage/stage.hpp"
#include "th04/main/stage/stages.hpp"
#include "th04/main/tile/tile.hpp"
#include "th04/sprites/main_cdg.h"

// See tile.hpp for the reason why this declaration is necessary.
extern "C" void pascal near tiles_invalidate_around(
	subpixel_t center_y, subpixel_t center_x
);

static const uint8_t CARPET_LIGHT_LEVELS = 3;
static const uint8_t CARPET_LIGHTING_CELS = 8;

extern vram_offset_t CARPET_TILE_IMAGE_VOS[CARPET_LIGHT_LEVELS][TILES_X];
extern uint8_t CARPET_LIGHTING_ANIM[CARPET_LIGHTING_CELS][TILES_X];

#include "src/main/stage/carpet_lighting.inl"

#define carpet_lighting_update_and_render(cel, level_cur, level_target) { \
	carpet_lighting_put_new(cel, level_target); \
	if(stage_frame_mod4 == 0) { \
		cel++; \
		if(cel > (CARPET_LIGHTING_CELS - 1)) { \
			level_cur++; \
			cel = 0; \
		} \
	} \
}

#include "src/main/stage/render.inl"
