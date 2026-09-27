#include <ctype.h>

#include "src/maine/cutscene/state.hpp"
#include "src/shared/hardware/graphics.hpp"
#include "src/shared/hardware/input.hpp"
#include "src/shared/hardware/putsa.hpp"
#include "src/shared/hardware/v_colors.hpp"

static const int BOX_LEFT = 80;
static const int BOX_TOP = 320;
static const int TEXT_INTERVAL_DEFAULT = 1;

enum script_ret_t { CONTINUE = 0, STOP = -1 };
#define str_sep_control_or_space(c) (iscntrl(c) || ((c) == ' '))
#define text_fx graph_putsa_fx_func

#if defined(TH04P)
#pragma codeseg CUTSCENE_TEXT GROUP_01
#else
#pragma codeseg CUTSCENE_TEXT cutscene_01
#endif
void near box_bg_allocate_and_snap(void);
void near box_bg_put(void);
void near box_bg_free(void);
void near cursor_advance_and_animate(void);
script_ret_t pascal near script_op(unsigned char c);

inline void cutscene_input_sense(void) { input_reset_sense(); }

#include "src/maine/cutscene/cutscene_animate.inl"
#pragma codeseg
