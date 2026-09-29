// Map tile section order + enemy script format.

#ifndef TH04_FORMATS_STD_HPP
#define TH04_FORMATS_STD_HPP

#include "src/main/math/subpixel.hpp"

extern uint8_t __seg *std_seg;

#if (GAME == 5)
extern int std_map_section_p;
#else
extern int std_map_section_id;
#endif

extern SubpixelLength8 near *std_scroll_speed;

#define STD_ENEMY_SCRIPT_COUNT 32

extern void near *std_enemy_scripts[STD_ENEMY_SCRIPT_COUNT];
extern void far *std_ip;

extern func_t_near stage_vm;

void near std_load(void);
void near std_free(void);
void pascal std_run();

#endif
