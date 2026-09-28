#include "src/shared/platform/types.hpp"
#include "src/shared/formats/cdg.hpp"
#include "src/shared/formats/pi.hpp"
#include "src/shared/hardware/bgimage.hpp"
#include "src/shared/hardware/graphics.hpp"
#include "src/shared/hardware/input.hpp"
#include "src/shared/runtime/api.hpp"
#include "src/shared/sound/api.hpp"

enum { TRACK_COUNT = 22 };
enum { COL_TRACKLIST_SELECTED = 3, COL_TRACKLIST = 5 };
static const char BGM_MENU_MAIN_FN[] = "op";

extern unsigned char track_playing;
extern unsigned char music_sel;
extern unsigned char music_page_accessed;
extern unsigned char cmt_shown_initial;
extern const char far *MUSIC_FILES[22];

void near music_update_render_and_flip(void);
void pascal near track_put_both(unsigned char, unsigned char);
void pascal near tracklist_put_both(unsigned char);
void near nopoly_B_snap(void);
void near nopoly_B_free(void);
void pascal near cmt_load_unput_and_put_both_animate(int);

inline void music_input_sense(void) { input_reset_sense(); }

#define MUSICROOM_DISTANCE near
#pragma codeseg OP_NATIVE_TEXT OP_NATIVE_01
#include "src/op/music/musicroom_menu.inl"
#pragma codeseg
