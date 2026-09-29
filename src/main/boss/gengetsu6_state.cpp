#pragma option -zCMAIN_036_TEXT -zPmain_03

#include "th04/main/custom.hpp"

// TH04's historical boss_x2.cpp owns Gengetsu's wave state. The spawncolumn
// layout is kept here because the storage is the shared custom-entity view
// used by the maintained column/render producers.
static const pixel_t WAVE_TARGET_MARGIN = (PLAYFIELD_W / 12);

struct spawncolumn_t {
    int8_t unused[2];
    PlayfieldPoint pos;
    int8_t padding[20];
};

uint8_t gengetsu_wave_amp = 0x00;
Subpixel gengetsu_wave_target_x;
