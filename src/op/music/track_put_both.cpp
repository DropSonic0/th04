#include "src/shared/hardware/graphics.hpp"

typedef unsigned char shiftjis_t;

static const screen_x_t TRACKLIST_LEFT = 16;
extern unsigned char music_page_accessed;
extern const shiftjis_t far *MUSIC_CHOICES[];

extern "C" void far pascal graph_putsa_fx(
	screen_x_t left, screen_y_t top, int col, const shiftjis_t far *str
);

#ifdef TH04P
#pragma codeseg OP_NATIVE_TEXT OP_NATIVE_01
#else
#pragma codeseg OP_MUSIC_TEXT op_music_01
#endif
#include "src/op/music/track_put_both.inl"
#pragma codeseg
