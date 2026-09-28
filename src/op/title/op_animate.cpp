#include "src/shared/platform/types.hpp"
#include "src/shared/config/resident.hpp"
#include "src/shared/formats/pi.hpp"
#include "src/shared/hardware/frame_delay.hpp"
#include "src/shared/hardware/v_colors.hpp"
#include "src/shared/sound/api.hpp"

static const char BGM_MENU_MAIN_FN[] = "op";
static const char MENU_MAIN_BG_FN[] = "op1.pi";

#pragma codeseg OP_NATIVE_TEXT OP_NATIVE_01
#include "src/op/title/op_animate.inl"
#pragma codeseg
