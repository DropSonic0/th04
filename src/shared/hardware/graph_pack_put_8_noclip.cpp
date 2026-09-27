#pragma option -zCSHARED

#include "src/shared/hardware/graphics.hpp"
#include "src/shared/hardware/vram_planes.hpp"

// The cutscene uses the otherwise invisible row 400 as an EGC source row.
// graph_pack_put_8() clips that row, so unpack the 4-bit PI pixels directly
// into the four VRAM planes selected by graph_accesspage().
extern "C" void far pascal graph_pack_put_8_noclip(
    screen_x_t left, screen_y_t top, const void far *linepat, pixel_t len
)
{
    int byte_x = (left >> BYTE_BITS);
    int count = (len >> BYTE_BITS);
    if((count <= 0) || (byte_x >= ROW_SIZE)) {
        return;
    }

    const unsigned char far *src = reinterpret_cast<const unsigned char far *>(linepat);
    if(byte_x < 0) {
        int skip = -byte_x;
        if(skip >= count) {
            return;
        }
        src += (skip * 4);
        count -= skip;
        byte_x = 0;
    }
    if((byte_x + count) > ROW_SIZE) {
        count = (ROW_SIZE - byte_x);
    }

    unsigned int destination = ((top << 6) + (top << 4) + byte_x);
    for(int x = 0; x < count; x++) {
        unsigned char blue = 0;
        unsigned char red = 0;
        unsigned char green = 0;
        unsigned char intensity = 0;
        for(int pixel = 0; pixel < BYTE_DOTS; pixel++) {
            unsigned char pair = src[(x * 4) + (pixel >> 1)];
            unsigned char color = ((pixel & 1) ? (pair & 0x0F) : (pair >> 4));
            unsigned char bit = (0x80 >> pixel);
            if(color & 0x01) blue |= bit;
            if(color & 0x02) red |= bit;
            if(color & 0x04) green |= bit;
            if(color & 0x08) intensity |= bit;
        }
        VRAM_PLANE_B[destination + x] = blue;
        VRAM_PLANE_R[destination + x] = red;
        VRAM_PLANE_G[destination + x] = green;
        VRAM_PLANE_E[destination + x] = intensity;
    }
}
