#ifndef TH04_MAIN_TILE_HPP
#define TH04_MAIN_TILE_HPP

#include "src/shared/platform/pc98.hpp"
#include "src/shared/formats/tile.hpp"
#include "src/main/math/subpixel.hpp"

// MAIN stores tile-image and tile-ring addresses as signed 16-bit VRAM
// offsets. Keep this artifact-local instead of importing the legacy planar
// header through the compatibility tree.
typedef int16_t vram_offset_t;

#define TILE_VRAM_W (TILE_W / BYTE_DOTS)

#define TILE_FLAG_H (TILE_H / 2)
#define TILE_FLAGS_Y (TILES_Y * (TILE_H / TILE_FLAG_H))

static const int TILE_ROWS_PER_SECTION = 5;
static const int TILE_SECTION_COUNT_MAX = 32;

extern const uint16_t TILE_SECTION_OFFSETS[TILE_SECTION_COUNT_MAX];
extern vram_offset_t tile_ring[TILES_Y][TILES_MEMORY_X];
extern int8_t tile_row_in_section;

static const int TILE_IMAGE_COUNT = 100;
static const int TILE_AREA_ROWS = (RES_Y / TILE_H);
static const int TILE_AREA_COLUMNS = (TILE_IMAGE_COUNT / TILE_AREA_ROWS);
static const screen_x_t TILE_AREA_LEFT = (
	RES_X - (TILE_AREA_COLUMNS * TILE_W)
);
static const screen_y_t TILE_AREA_TOP = 0;
static const vram_x_t TILE_AREA_VRAM_LEFT = (TILE_AREA_LEFT / BYTE_DOTS);

inline vram_offset_t tile_image_vo(int id)
{
	return static_cast<vram_offset_t>(
		(TILE_AREA_VRAM_LEFT + ((id / TILE_AREA_ROWS) * TILE_VRAM_W)) +
		(TILE_AREA_TOP + ((id % TILE_AREA_ROWS) * (TILE_H * ROW_SIZE)))
	);
}

void pascal near tiles_fill_initial(void);
void pascal near tiles_render_all(void);
void pascal tile_ring_set_vo(
	subpixel_t x, subpixel_t y, vram_offset_t image_vo
);

#define tile_ring_set(x, y, id) ( \
	tile_ring_set_vo(x, y, tile_image_vo(id)) \
)

extern bool halftiles_dirty[TILE_FLAGS_Y][TILES_MEMORY_X];
extern point_t tile_invalidate_box;

void near tiles_invalidate_reset(void);
void near tiles_invalidate_all(void);
void pascal near tiles_redraw_invalidated(void);
void pascal near tiles_render(void);
void tiles_activate(void);
void pascal tiles_activate_and_render_all_for_next_N_frames(uint8_t n);

inline void tiles_render_after_custom(const int& frame)
{
	if(frame <= 2) {
		tiles_render_all();
	} else {
		tiles_render();
	}
}

#define tiles_invalidate_around_xy(center_x, center_y) \
	tiles_invalidate_around(center_y, center_x)

#define tiles_invalidate_around_vram_xy(center_x, center_y) \
	tiles_invalidate_around_xy(to_sp(center_x), to_sp(center_y))

#endif
