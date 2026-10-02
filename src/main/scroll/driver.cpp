#pragma option -zCMAI_TEXT -zPmain_01

#include "src/shared/platform/types.hpp"

#define FLAGS_SIGN (_FLAGS & 0x80)

// MAI_TEXT scroll driver at MAIN.EXE load 0xCCD6. The two unresolved BSS
// flags keep their target-address names until their wider ownership is known.
extern unsigned char page_back;
extern unsigned int scroll_line_on_page[2];
extern int scroll_line;
extern "C" unsigned char byte_250FE;
extern unsigned char scroll_active;
extern unsigned int scroll_last_delta;
extern unsigned char scroll_subpixel_line;
extern unsigned char scroll_speed;
extern "C" unsigned char byte_25104;

extern "C" void pascal far graph_scrollup(unsigned line);
extern void near scroll_tile_ring_update(void);

void near scroll_driver()
{
    scroll_line_on_page[page_back] = scroll_line;
    if(byte_250FE && scroll_active) {
        graph_scrollup(static_cast<unsigned>(scroll_line));
    }

    scroll_last_delta = 0;
    if((scroll_subpixel_line = (scroll_subpixel_line + scroll_speed)) >= 16) {
#ifdef __TURBOC__
        // Keep the quotient in AX. The signed cast tells TC4J that AX is the
        // complete 16-bit RHS, avoiding both a byte-local spill and an
        // unnecessary second zero-extension of AL.
        _AH = 0;
        _AX >>= 4;
        scroll_line -= static_cast<int>(_AX);
        if(FLAGS_SIGN) {
            scroll_line += 400;
        }
        byte_25104 = _AL;
        scroll_subpixel_line &= 15;
        _AX <<= 4;
        scroll_last_delta = _AX;
#else
        unsigned short lines_delta = (scroll_subpixel_line >> 4);
        scroll_line -= static_cast<int>(lines_delta);
        if(scroll_line < 0) {
            scroll_line += 400;
        }
        byte_25104 = static_cast<unsigned char>(lines_delta);
        scroll_subpixel_line &= 15;
        scroll_last_delta = (lines_delta << 4);
#endif
    }
    scroll_tile_ring_update();
}