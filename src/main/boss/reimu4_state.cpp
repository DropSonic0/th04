#pragma option -zCMAIN_036_TEXT -zPmain_03

#include "th04/main/custom.hpp"

// TH04's historical boss_4r.cpp owns the Reimu orb template and the small
// state cells shared by the MAIN_036 orb/phase producers. Keep the layout in
// this physical TU; executable-facing code remains in the reviewed orb and
// phase owners.
static const pixel_t ORB_W = 32;
static const pixel_t ORB_H = 32;

enum orb_flag_t {
    OF_FREE = 0,
    OF_MOVEOUT_SPIN = 1,
    OF_MOVE = 2,
};

struct orb_t {
    orb_flag_t flag;
    unsigned char angle;
    PlayfieldPoint center;
    PlayfieldPoint origin;
    PlayfieldPoint velocity;
    unsigned int spin_time;
    Subpixel distance;
    int16_t unknown;
    int8_t unused[4];
    SubpixelLength8 move_speed;
    char angle_speed;
};

extern uint8_t orb_patnum_base;
orb_t orb_template;

unsigned char reimu_pattern8_angle = 0x00;
int8_t reimu_bg_pulse_direction = false;

void pascal near orbs_add_moving(void);
void pascal near orbs_add_spinning(unsigned char angle_offset, int count);
