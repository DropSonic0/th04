#include "src/shared/hardware/graphics.hpp"
#include "src/shared/hardware/frame_delay.hpp"
#include "src/shared/formats/pi.hpp"

void near setup_bgm_menu(void);
void near setup_se_menu(void);

#ifdef TH04P
#pragma codeseg OP_NATIVE_TEXT OP_NATIVE_01
#else
#pragma codeseg OP_SETUP_TEXT setup_menu_01
#endif
#include "src/op/setup/setup_menu.inl"
#pragma codeseg
