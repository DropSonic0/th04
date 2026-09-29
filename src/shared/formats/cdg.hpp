#ifndef TH04_SHARED_FORMATS_CDG_HPP
#define TH04_SHARED_FORMATS_CDG_HPP

#include <stddef.h>

#include "src/shared/platform/pc98.hpp"

// The 16-byte TH04 CDG slot layout is also used by cdg_load.asm and
// cdg_put.asm. The two final words hold allocated plane segments. Keep the
// segment accessors as references so MAIN's historical CDG callers preserve
// the original pointer expression and ABI. If a pinned TH03/TH04 CDG header
// already supplied the equivalent [CDG] type, reuse that declaration rather
// than redeclaring the shared [cdg_slots] symbol.
#ifndef TH03_FORMATS_CDG_H
struct cdg_slot_t {
    uint16_t bitplane_size;
    int16_t pixel_w;
    int16_t pixel_h;
    int16_t offset_at_bottom_left;
    uint16_t vram_dword_w;
    uint8_t image_count;
    int8_t plane_layout;
    uint8_t __seg *seg[2];

    uint8_t __seg *&seg_alpha() {
        return seg[0];
    }

    uint8_t __seg *&seg_colors() {
        return seg[1];
    }
};

typedef char cdg_slot_layout_check[
    (sizeof(cdg_slot_t) == 16) &&
    (offsetof(cdg_slot_t, pixel_w) == 2) &&
    (offsetof(cdg_slot_t, seg) == 12) ? 1 : -1
];
#endif

extern "C" {
#ifndef TH03_FORMATS_CDG_H
extern cdg_slot_t cdg_slots[64];
#endif

void pascal cdg_load_single(int slot, const char *fn, int image);
void pascal cdg_load_single_noalpha(int slot, const char *fn, int image);
void pascal cdg_load_all(int slot_first, const char *fn);
void pascal cdg_load_all_noalpha(int slot_first, const char *fn);
void pascal cdg_free(int slot);
void pascal cdg_free_all(void);
void pascal cdg_put_8(screen_x_t left, vram_y_t top, int slot);
void pascal cdg_put_noalpha_8(screen_x_t left, vram_y_t top, int slot);
void pascal cdg_put_plane(screen_x_t left, vram_y_t top, int slot, int plane);
}

#endif
