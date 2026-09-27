#ifndef TH04_SHARED_HARDWARE_PUTSA_HPP
#define TH04_SHARED_HARDWARE_PUTSA_HPP

#include "src/shared/platform/pc98.hpp"

typedef char shiftjis_t;

enum { FX_WEIGHT_BOLD = 2 };
extern unsigned int graph_putsa_fx_func;

// graph_putsa_fx.asm owns a FAR Pascal renderer and ends with RETF 10.
extern "C" void pascal graph_putsa_fx(
    screen_x_t left, screen_y_t top, int color, const shiftjis_t *text
);

#endif
