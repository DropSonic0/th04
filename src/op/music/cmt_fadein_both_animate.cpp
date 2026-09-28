extern "C" {
extern int graph_putsa_fx_func;
}

void near cmt_put(void);
void near music_update_render_and_flip(void);

#ifdef TH04P
#pragma codeseg OP_NATIVE_TEXT OP_NATIVE_01
#else
#pragma codeseg OP_MUSIC_TEXT op_music_01
#endif
#include "src/op/music/cmt_fadein_both_animate.inl"
#pragma codeseg
