extern "C" {
extern int graph_putsa_fx_func;
void far pascal bgimage_put_rect_16(int left, int top, int w, int h);
}

void near music_update_render_and_flip(void);

#ifdef TH04P
#pragma codeseg OP_NATIVE_TEXT OP_NATIVE_01
#else
#pragma codeseg OP_MUSIC_TEXT op_music_01
#endif
#include "src/op/music/cmt_unput_both_animate.inl"
#pragma codeseg
