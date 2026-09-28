#ifndef TH04_MAIN_PLAYER_BOMB_HPP
#define TH04_MAIN_PLAYER_BOMB_HPP

#include "src/shared/platform/types.hpp"

static const unsigned char BOMB_CIRCLE_FRAMES = 32;

extern bool bombing;
void pascal near player_bomb(void);
void near bomb_update_and_render(void);

extern bool bombing_disabled;
extern unsigned char bomb_frame;
#if (GAME == 4)
// Pointless indirection to player_bomb().
extern nearfunc_t_near player_bomb_func;
#endif

// Character-specific bomb update and render functions.
extern nearfunc_t_near playchar_bomb_func;

void pascal near bomb_reimu(void);
void pascal near bomb_marisa(void);

#if (GAME == 4)
void near bomb_update_and_render(void);
#endif

#endif
