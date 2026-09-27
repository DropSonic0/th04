#include "src/shared/hardware/graphics.hpp"
#include "src/shared/formats/pi.hpp"

typedef unsigned int vram_offset_t;
typedef unsigned short egc_temp_t;

static const screen_x_t CUTSCENE_PIC_W = 320;
static const screen_y_t CUTSCENE_PIC_H = 200;
static const int CUTSCENE_PIC_SLOT = 0;
static const vram_byte_amount_t CUTSCENE_PIC_VRAM_W = (CUTSCENE_PIC_W / BYTE_DOTS);
static const int PI_MASK_COUNT = 4;

extern const unsigned short PI_MASKS[PI_MASK_COUNT][4];
extern unsigned char far *VRAM_PLANE_B;

inline vram_offset_t vram_offset_shift(screen_x_t x, screen_y_t y) {
    return (x >> BYTE_BITS) + (y << 6) + (y << 4);
}

#define egc_chunk(vo) (*reinterpret_cast<egc_temp_t far *>(VRAM_PLANE_B + (vo)))
inline void egc_setup_copy_masked(unsigned short mask) {
    outport(EGC_READPLANEREG, 0x00FF);
    outport(EGC_MODE_ROP_REG, EGC_COMPAREREAD | EGC_WS_PATREG | EGC_RL_MEMREAD);
    outport(EGC_BITLENGTHREG, EGC_REGISTER_DOTS - 1);
    outport(EGC_MASKREG, mask);
}

#pragma codeseg CUTSCENE_TEXT cutscene_01
void near egc_start_copy(void);
void pascal near pic_copy_to_other(screen_x_t left, vram_y_t top);
#include "src/maine/cutscene/pic_put_both_masked.inl"
#pragma codeseg
