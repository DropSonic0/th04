#include "src/shared/platform/types.hpp"
#include "src/shared/hardware/graphics.hpp"
#include "src/shared/hardware/vram_planes.hpp"

// Pixel-accurate GRCG RMW writes. The caller selects the GRCG color and mode;
// a write to the B-plane address applies the mask to all selected planes.
static void near grcg_hline_pixels(int x1, int x2, int y)
{
    if(y < 0 || y >= RES_Y) return;
    if(x1 > x2) { int t = x1; x1 = x2; x2 = t; }
    if(x2 < 0 || x1 >= RES_X) return;
    if(x1 < 0) x1 = 0;
    if(x2 >= RES_X) x2 = RES_X - 1;

    unsigned first_byte = x1 / BYTE_DOTS;
    unsigned last_byte = x2 / BYTE_DOTS;
    unsigned base = y * ROW_SIZE;
    unsigned char first_mask = 0xffu >> (x1 & 7);
    unsigned char last_mask = 0xffu << (7 - (x2 & 7));
    volatile unsigned char far *vram = VRAM_PLANE_B;
    if(first_byte == last_byte) {
        vram[base + first_byte] = first_mask & last_mask;
        return;
    }
    vram[base + first_byte] = first_mask;
    for(unsigned b = first_byte + 1; b < last_byte; b++) {
        vram[base + b] = 0xff;
    }
    vram[base + last_byte] = last_mask;
}

extern "C" void TH04_PASCAL grcg_hline(int x1, int x2, int y)
{
    grcg_hline_pixels(x1, x2, y);
}

extern "C" void TH04_PASCAL grcg_boxfill(int x1, int y1, int x2, int y2)
{
    if(y1 > y2) { int t = y1; y1 = y2; y2 = t; }
    if(y1 < 0) y1 = 0;
    if(y2 >= RES_Y) y2 = RES_Y - 1;
    for(int y = y1; y <= y2; y++) {
        grcg_hline_pixels(x1, x2, y);
    }
}
