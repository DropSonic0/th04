#ifndef TH04_MAIN_FORMATS_MPN_HPP
#define TH04_MAIN_FORMATS_MPN_HPP

#include <stddef.h>

#include "src/main/hardware/planar.hpp"
#include "src/shared/hardware/graphics.hpp"
#include "src/shared/formats/tile.hpp"
#include "src/shared/platform/pc98.hpp"

typedef dot_rect_t(TILE_W, TILE_H) mpn_plane_t;
typedef Planar<mpn_plane_t> mpn_image_t;

struct mpn_header_t {
	char magic[4];
	uint8_t count;
	int8_t unused;
};

// Internal .MPN slot structure.
struct mpn_t {
	mpn_image_t far *images;
	size_t count;
	Palette8 palette;
	int8_t unused[10];
};

// TH04 reserves memory for 8 slots, but only actually uses the first one.
static const int MPN_COUNT = 8;

extern mpn_t mpn_slots[MPN_COUNT];

extern "C" {

void pascal mpn_free(int slot);
void pascal mpn_palette_show(int slot);
int pascal mpn_load_palette_show(int slot, const char *fn);

}

#endif
