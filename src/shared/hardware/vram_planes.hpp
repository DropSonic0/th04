#ifndef TH04_SHARED_HARDWARE_VRAM_PLANES_HPP
#define TH04_SHARED_HARDWARE_VRAM_PLANES_HPP

#include "src/shared/platform/pc98.hpp"

// The four far pointers are data symbols owned by the graphics runtime.
// Their byte element type preserves the offset arithmetic used by TH04.
extern uint8_t far *VRAM_PLANE_B;
extern uint8_t far *VRAM_PLANE_R;
extern uint8_t far *VRAM_PLANE_G;
extern uint8_t far *VRAM_PLANE_E;

// Initializes the planar VRAM pointers used by the PC-98 graphics routines.
void vram_planes_set(void);

#endif
