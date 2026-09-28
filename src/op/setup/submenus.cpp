#include "src/shared/platform/types.hpp"
#include "src/shared/config/resident.hpp"
#include "src/shared/hardware/frame_delay.hpp"
#include "src/shared/hardware/input.hpp"
#include "src/shared/hardware/putsa.hpp"
#include "src/shared/hardware/v_colors.hpp"
#include "src/shared/sound/api.hpp"

struct setup_window_t { int w; int h; };
extern setup_window_t window;
extern shiftjis_t SETUP_BGM_CAPTION[];
extern shiftjis_t SETUP_SE_CAPTION[];

void pascal near singleline(screen_x_t left, screen_y_t top);
void pascal near dropdown(screen_x_t left, screen_y_t top);
void pascal near rollup(screen_x_t left, screen_y_t top);
void pascal near bgm_choice_put(int mode, vc2 col);
void pascal near se_choice_put(int mode, vc2 col);
void near bgm_help_put(void);
void near se_help_put(void);

static const int CAPTION_W = 52 * GLYPH_HALF_W;
static const int CAPTION_LEFT = (RES_X - CAPTION_W) / 2;
static const int CAPTION_TOP = 88;
static const int CHOICE_LEFT = 48;
static const int CHOICE_TOP = 136;
static const int CHOICE_W = 16 * GLYPH_HALF_W;
static const int HELP_LEFT = CHOICE_LEFT + 16 + CHOICE_W + 16;
static const int HELP_TOP = CHOICE_TOP;
static const int HELP_W = 46 * GLYPH_HALF_W;
static const int HELP_LINES = 9;

typedef void (near pascal *near setup_anim_t)(screen_x_t, screen_y_t);
typedef void (near pascal *near setup_choice_t)(int, vc2);
typedef void (near *near setup_help_t)(void);

#pragma codeseg OP_NATIVE_TEXT OP_NATIVE_01

static void near animate_window(
    int left, int top, int content_w, int lines, setup_anim_t animate
)
{
    window.w = (16 + content_w + 16) / 16;
    if(lines != 1) {
        window.h = (16 + lines * GLYPH_H) / 16;
    }
    animate(left - 16, top - 8);
}

static void near setup_submenu_impl(
    int& selection, const shiftjis_t *caption, int choice_count,
    int choice_default, setup_choice_t put_choice,
    setup_help_t put_help, input_t increment_input
)
{
    animate_window(CAPTION_LEFT, CAPTION_TOP, CAPTION_W, 1, singleline);
    graph_putsa_fx(CAPTION_LEFT, CAPTION_TOP, V_WHITE, caption);

    animate_window(CHOICE_LEFT, CHOICE_TOP, CHOICE_W, choice_count, dropdown);
    for(selection = 0; selection < choice_count; selection++) {
        put_choice(selection, selection == choice_default ? V_WHITE : 0);
    }

    animate_window(HELP_LEFT, HELP_TOP, HELP_W, HELP_LINES, dropdown);
    put_help();

    selection = choice_default;
    for(;;) {
        input_wait_for_change(0);
        frame_delay(1);
        if(key_det & (INPUT_OK | INPUT_SHOT)) break;
        if(key_det & increment_input) {
            put_choice(selection, 0);
            selection = (selection + 1 == choice_count) ? 0 : selection + 1;
            put_choice(selection, V_WHITE);
        }
        if(key_det & ((INPUT_UP | INPUT_DOWN) & ~increment_input)) {
            put_choice(selection, 0);
            selection = (selection == 0) ? choice_count - 1 : selection - 1;
            put_choice(selection, V_WHITE);
        }
    }

    animate_window(HELP_LEFT, HELP_TOP, HELP_W, HELP_LINES, rollup);
    animate_window(CHOICE_LEFT, CHOICE_TOP, CHOICE_W, choice_count, rollup);
}

#define setup_submenu(sel, caption, count, initial, choice, help, inc) \
    setup_submenu_impl(sel, caption, count, initial, choice, help, inc)

#include "src/op/setup/setup_bgm_menu.inl"
#include "src/op/setup/setup_se_menu.inl"
#pragma codeseg
