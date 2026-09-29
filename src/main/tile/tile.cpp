// TH04 MAIN's historical tile.cpp object is one physical TILE_TEXT producer.
// Its maintained surface is split across the invalidation and render
// fragments below.  The low-level full renderer remains the separate
// src/main/tile/render_all.asm owner.
#pragma option -zPmain_01

#include "src/main/tile/tile.hpp"
#include "src/main/bg.hpp"

// Keep the umbrella declaration-only. Pulling the larger gameplay headers
// here would reintroduce legacy compatibility definitions into this one TU.
extern void near overlay_titles_invalidate(void);
extern void near player_invalidate(void);
extern void near shots_invalidate(void);
extern void near enemies_invalidate(void);
extern void near bullets_and_gather_invalidate(void);
extern void near items_invalidate(void);
extern void near sparks_invalidate(void);
extern void near pointnums_invalidate(void);
extern nearfunc_t_near midboss_invalidate;
extern nearfunc_t_near stage_invalidate;
extern void pascal near tiles_redraw_invalidated(void);

#define render_all_time tile_render_all_time
extern uint8_t render_all_time;

#include "src/main/tile/invalidate_set_all.inl"
#include "src/main/tile/invalidate_reset.inl"
#include "src/main/tile/render.inl"
