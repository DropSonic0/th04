#include "src/maine/cutscene/state.hpp"

typedef char cursor_size_check[(sizeof(cursor_t) == 4) ? 1 : -1];
typedef char planar16_size_check[(sizeof(planar16_t) == 8) ? 1 : -1];

// The five EGC masks occupy the unique 40-byte table at MAINE's decoded load
// offset 0xEB5C (candidate MAP 0E53:062C). Words describe copy behavior;
// they are data, not generated instruction bytes or alignment filler.
extern const unsigned short BOX_MASKS[5][4] = {
    { 0x8888, 0x0000, 0x2222, 0x0000 },
    { 0x8888, 0x4444, 0x2222, 0x1111 },
    { 0xAAAA, 0x4444, 0xAAAA, 0x1111 },
    { 0xAAAA, 0x4444, 0xAAAA, 0x5555 },
    { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF },
};

cursor_t cursor;
int text_interval;
unsigned char text_col;
unsigned char fast_forward;
planar16_t far *box_bg;
unsigned char script[8192];
unsigned char near *script_p;
int script_param_number_default;
// The target's cutscene glyph buffer starts with two spaces and a terminator.
ShiftJISKanji CUTSCENE_KANJI[2] = { { { ' ', ' ' } }, { { 0, 0 } } };
