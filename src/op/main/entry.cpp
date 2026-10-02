#if defined(_WIN32) || defined(__MSDOS__) || defined(__TURBOC__)
#include <conio.h>
#endif

#include <stddef.h>

#include "src/shared/platform/types.hpp"
#include "src/shared/config/resident.hpp"
#include "src/shared/core/game_init.hpp"
#include "src/shared/hardware/frame_delay.hpp"
#include "src/shared/hardware/graphics.hpp"
#include "src/shared/hardware/input.hpp"
#include "src/shared/runtime/api.hpp"
#include "src/shared/sound/api.hpp"

// OP installs the resident pointer from MIKO.CFG before it enters the menu.
resident_t far *resident;
extern size_t mem_assign_paras;

typedef void (near pascal *near menu_unput_and_put_func_t)(int, vc2);
int8_t menu_sel = 0;
bool quit = false;
int8_t in_option = 0;
int8_t main_menu_unused_1 = 1;
menu_unput_and_put_func_t menu_unput_and_put;

void near cfg_load(void);
void near cfg_save_exit(void);
void near setup_menu(void);
void near zunsoft_animate(void);
void near op_animate(void);
void near cleardata_and_regist_view_sprites_load(void);
void near main_cdg_load(void);
void near main_cdg_free(void);
void near main_update_and_render(void);
void near start_demo(void);
void near option_update_and_render(void);
void game_exit_to_dos(void);

static const unsigned char OP_AND_END_PF_FN[] = "\x8c\xb6\x91z\x8b\xbd" "ed.dat";
static const char GAIJI_FN[] = "GAMEFT.bft";
static const char SE_FN[] = "miko";
static const char MEMORY_INSUFFICIENT[] = "\012\213\363\202\253\203\201\203\202\203\212\225s\221\253\202\305\202\267\201B\203\201\203\202\203\212\213\363\202\253\202\360\221\235\202\342\202\265\202\304\202\251\202\347\216\300\215s\202\265\202\304\202\313\012";

enum { RANK_NORMAL = 1, RANK_SHOW_SETUP_MENU = 0xff };

#define input_reset_sense_interface input_reset_sense
#define snd_redetermine_modes_and_reload_se() { \
    snd_determine_modes(resident->bgm_mode, resident->se_mode); \
    snd_load(SE_FN, SND_LOAD_SE); \
}

#ifdef TH04P
#pragma codeseg OP_NATIVE_TEXT OP_NATIVE_01
#else
#pragma codeseg OP_MAIN_TEXT cfg_load_01
#endif
#include "src/op/main/main.inl"
#pragma codeseg
