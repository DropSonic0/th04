#ifndef TH04_SHARED_FORMATS_CDG_HPP
#define TH04_SHARED_FORMATS_CDG_HPP

#include <stddef.h>

#include "src/shared/platform/pc98.hpp"

// The 16-byte TH04 CDG slot layout is also used by cdg_load.asm and
// cdg_put.asm. The two final words hold allocated plane segments.
struct cdg_slot_t {
    uint16_t bitplane_size;
    int16_t pixel_w;
    int16_t pixel_h;
    int16_t offset_at_bottom_left;
    uint16_t vram_dword_w;
    uint8_t image_count;
    int8_t plane_layout;
    uint16_t seg_alpha;
    uint16_t seg_colors;
};

typedef char cdg_slot_layout_check[
    (sizeof(cdg_slot_t) == 16) &&
    (offsetof(cdg_slot_t, pixel_w) == 2) &&
    (offsetof(cdg_slot_t, seg_alpha) == 12) ? 1 : -1
];

extern "C" {
extern cdg_slot_t cdg_slots[64];

void pascal cdg_load_single(int slot, const char *fn, int image);
void pascal cdg_load_single_noalpha(int slot, const char *fn, int image);
void pascal cdg_free(int slot);
void pascal cdg_free_all(void);
void pascal cdg_put_8(screen_x_t left, vram_y_t top, int slot);
void pascal cdg_put_plane(screen_x_t left, vram_y_t top, int slot, int plane);
}

#endif
