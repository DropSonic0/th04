#include "src/shared/hardware/graphics.hpp"

void near nopoly_B_put(void);
void near polygons_update_and_render(void);
void far pascal frame_delay_2(int frames);
extern unsigned char music_page_accessed;

#ifdef TH04P
#pragma codeseg OP_NATIVE_TEXT OP_NATIVE_01
#else
#pragma codeseg OP_MUSIC_TEXT op_music_01
#endif
#include "src/op/music/music_update_render_and_flip.inl"
#pragma codeseg
