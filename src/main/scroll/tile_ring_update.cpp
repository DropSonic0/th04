#pragma option -zCEND_TEXT -zPmain_01

#ifdef __TURBOC__
#include <dos.h>
#include <mem.h>
#else
#include <string.h>
#endif

#define FLAGS_SIGN (_FLAGS & 0x80)

extern "C" unsigned char byte_250FE;
extern "C" unsigned char byte_25104;
extern "C" unsigned int word_25100;

extern unsigned char scroll_speed;
extern unsigned int scroll_line;
extern unsigned char scroll_active;
extern signed char tile_row_in_section;
extern unsigned int std_map_section_id;
extern unsigned int std_scroll_speed;
extern unsigned char __seg *std_seg;
extern unsigned char __seg *map_seg;
extern const unsigned int TILE_SECTION_OFFSETS[32];
extern unsigned int tile_ring[][32];

extern void near egc_start_copy_noframe();
extern "C" void near sub_BAEE();
extern "C" void pascal far egc_off(void);

void near scroll_tile_ring_update()
{
    unsigned char previous_copy_request;
    if((byte_250FE == 0) && (byte_25104 == 0)) {
        return;
    }
    if(scroll_speed == 0) {
        return;
    }

#ifdef __TURBOC__
    _AX = scroll_line;
    _AX >>= 4;
    if(_AX != word_25100) {
        word_25100 = _AX;

        _BX = (unsigned int)std_seg;
        _ES = _BX;
        tile_row_in_section--;
        if(FLAGS_SIGN) {
            tile_row_in_section = 4;
            std_map_section_id++;
            std_scroll_speed++;

            _BX = std_scroll_speed;
            _DL = *reinterpret_cast<unsigned char __es *>(_BX);
            scroll_speed = _DL;
            if(_DL == 0) {
                scroll_line = 0;
                byte_250FE = 0;
                byte_25104 = 0;
                return;
            }
        }

        _AX <<= 6;
        _AX += (unsigned int)&tile_ring[0][0];
        asm { mov di, ax; }

        asm { xor ax, ax; }
        _AL = tile_row_in_section;
        _AX <<= 6;

        _BX = std_map_section_id;
        _BL = *reinterpret_cast<unsigned char __es *>(_BX);
        asm {
            xor bh, bh
            add bl, bl
        }
        _BX = *reinterpret_cast<const unsigned int *>(
            reinterpret_cast<const unsigned char *>(TILE_SECTION_OFFSETS) + _BX
        );
        asm {
            mov si, ax
            add si, bx
            push ds
            pop es
            push ds
            mov ax, map_seg
            mov ds, ax
            mov cx, 24
            rep movsw
            pop ds
        }
    }
#else
    unsigned short ax = (scroll_line >> 4);
    if(ax != word_25100) {
        word_25100 = ax;

        tile_row_in_section--;
        if(tile_row_in_section < 0) {
            tile_row_in_section = 4;
            std_map_section_id++;
            std_scroll_speed++;

            unsigned char speed = std_seg[std_scroll_speed];
            scroll_speed = speed;
            if(speed == 0) {
                scroll_line = 0;
                byte_250FE = 0;
                byte_25104 = 0;
                return;
            }
        }

        unsigned short ring_dst_offset = (ax << 6);
        unsigned short map_src_offset = (static_cast<unsigned short>(tile_row_in_section) << 6);

        unsigned char section_idx = std_seg[std_map_section_id];
        unsigned short section_offset = TILE_SECTION_OFFSETS[section_idx];

        map_src_offset += section_offset;

        if(tile_ring && map_seg) {
            memcpy(
                reinterpret_cast<unsigned char *>(tile_ring) + ring_dst_offset,
                map_seg + map_src_offset,
                24 * sizeof(unsigned short)
            );
        }
    }
#endif

    previous_copy_request = byte_250FE;
    byte_250FE = byte_25104;
    byte_25104 += previous_copy_request;
    if(scroll_active == 0) {
        byte_25104 = 0;
        return;
    }
    egc_start_copy_noframe();
    sub_BAEE();
    byte_25104 = 0;
    egc_off();
}