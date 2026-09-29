#ifndef TH04_MAIN_BG_HPP
#define TH04_MAIN_BG_HPP

#include "src/shared/platform/types.hpp"

extern nearfunc_t_near bg_render_not_bombing;
extern nearfunc_t_near bg_render_bombing;

// Kept in sync with bg_render_not_bombing outside a bomb animation.
extern nearfunc_t_near bg_render_bombing_func;

#endif
