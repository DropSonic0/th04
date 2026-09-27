#ifndef TH04_MAINE_CUTSCENE_BOX_MASK_HPP
#define TH04_MAINE_CUTSCENE_BOX_MASK_HPP

// The text-box EGC copy uses a 16-bit enum parameter in MAINE's cutscene ABI.
typedef enum {
    BOX_MASK_0 = 0,
    BOX_MASK_1,
    BOX_MASK_2,
    BOX_MASK_3,
    BOX_MASK_COPY,
    BOX_MASK_COUNT,
    box_mask_t_force_uint16 = 0xFFFF
} box_mask_t;

#endif
