void pascal near track_put_both(unsigned char i, unsigned char col);

#ifdef TH04P
#pragma codeseg OP_NATIVE_TEXT OP_NATIVE_01
#else
#pragma codeseg OP_MUSIC_TEXT op_music_01
#endif
#include "src/op/music/tracklist_put_both.inl"
#pragma codeseg
