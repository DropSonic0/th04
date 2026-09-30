#ifndef TH04_MAIN_FORMATS_CDG_HPP
#define TH04_MAIN_FORMATS_CDG_HPP

#include "src/shared/platform/abi.hpp"
#include "src/shared/platform/pc98.hpp"
#include "src/shared/formats/cdg.hpp"
#include "src/main/hardware/planar.hpp"

// CDG loading and blitting ABI used by TH04 MAIN.EXE. The target's large
// memory model makes these far Pascal functions; TH04_PASCAL preserves the
// corresponding distance in every supported 16-bit memory model.
extern "C" {
void TH04_PASCAL cdg_load_all(int slot_first, const char *fn);
void TH04_PASCAL cdg_load_all_noalpha(int slot_first, const char *fn);
void TH04_PASCAL cdg_load_single(int slot, const char *fn, int image);
void TH04_PASCAL cdg_load_single_noalpha(int slot, const char *fn, int image);
void TH04_PASCAL cdg_free(int slot);
void TH04_PASCAL cdg_free_all(void);
void TH04_PASCAL cdg_put_8(screen_x_t left, vram_y_t top, int slot);
void TH04_PASCAL cdg_put_noalpha_8(screen_x_t left, vram_y_t top, int slot);

// Displays only the alpha plane of a CDG image. MAIN's EMS header includes
// the historical TH04 CDG surface even though this path is not used by the
// EMS preload functions themselves.
void TH04_PASCAL cdg_put_nocolors_8(
    screen_x_t left, vram_y_t top, int slot
);
}

// Stage 5 uses the wrapped single-plane path. Its historical declaration has
// C++ linkage, matching the decorated entry emitted by cdg_put_plane_roll.asm.
void TH04_PASCAL cdg_put_plane_roll_8(
    screen_x_t left,
    vram_y_t top,
    int slot,
    vram_plane_t plane_src_id,
    dots8_t __seg *plane_dst
);

#endif
