#ifndef TH04_MAIN_TILE_BB_HPP
#define TH04_MAIN_TILE_BB_HPP

#include "src/main/formats/bb.hpp"
#include "src/shared/formats/tile.hpp"

// The monochrome animations loaded from the ST*.BB and BB*.BB files, shown as
// one colored 16x16 tile per bit. Used for transitions from regular stage
// backgrounds to bomb and boss backgrounds.

// Why store ((TILES_X / 8) * TILES_Y) = 69 bytes, when you can store 128?
static const pixel_t TILES_BB_CEL_W = 512;
static const pixel_t TILES_BB_CEL_H = 512;
static const unsigned int TILES_BB_CEL_SIZE = (
	((TILES_BB_CEL_W / TILE_W) / 8) * (TILES_BB_CEL_H / TILE_H)
);
static const int TILES_BB_CELS = (BB_SIZE / TILES_BB_CEL_SIZE);

extern unsigned char tiles_bb_col;

extern bb_tiles8_t __seg *tiles_bb_seg;

// Fills the playfield tile starting at (floor(left/8)*8, top) with the
// current GRCG tile. Assumes that GRCG is set to TDW mode.
void __fastcall near grcg_tile_bb_put_8(screen_x_t left, vram_y_t top);

// Renders animation [cel] in [seg] to the playfield area in VRAM. All tiles
// with a corresponding 1 bit are filled with [tiles_bb_col].
#define tiles_bb_put(seg, cel) { \
	void pascal near tiles_bb_put_raw(int); \
	\
	tiles_bb_seg = seg; \
	tiles_bb_put_raw(cel); \
}

// Interprets animation [cel] in [seg] as a mask, and marks all stage
// background tiles with a corresponding 1 bit for redrawing.
#define tiles_bb_invalidate(seg, cel) { \
	void pascal near tiles_bb_invalidate_raw(int); \
	\
	tiles_bb_seg = seg; \
	tiles_bb_invalidate_raw(cel); \
}

#endif
