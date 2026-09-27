#ifndef TH04_MAINE_CUTSCENE_STATE_HPP
#define TH04_MAINE_CUTSCENE_STATE_HPP

// TH04 MAINE's cutscene storage and the types used to address it.
typedef unsigned short dots16_t;

struct cursor_t {
    int x;
    int y;
};

template<class T> struct Planar {
    T B;
    T R;
    T G;
    T E;
};
typedef Planar<dots16_t> planar16_t;

// The script interpreter writes one Shift-JIS glyph into this two-byte view.
struct ShiftJISKanji {
    char byte[2];
};

extern cursor_t cursor;
extern int text_interval;
extern unsigned char text_col;
extern unsigned char fast_forward;
extern planar16_t far *box_bg;
extern unsigned char script[8192];
extern unsigned char near *script_p;
extern int script_param_number_default;
extern const unsigned short BOX_MASKS[5][4];
extern ShiftJISKanji CUTSCENE_KANJI[2];

#endif
