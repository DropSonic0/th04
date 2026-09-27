#ifndef TH04_MAINE_CUTSCENE_STATE_HPP
#define TH04_MAINE_CUTSCENE_STATE_HPP

// TH04 MAINE's cutscene storage and the types used to address it.
typedef unsigned short dots16_t;

struct cursor_t {
    int x;
    int y;
};

struct planar16_t {
    dots16_t B;
    dots16_t R;
    dots16_t G;
    dots16_t E;
};

extern cursor_t cursor;
extern int text_interval;
extern unsigned char fast_forward;
extern planar16_t far *box_bg;
extern unsigned char script[8192];
extern unsigned char near *script_p;
extern int script_param_number_default;
extern const unsigned short BOX_MASKS[5][4];

#endif
