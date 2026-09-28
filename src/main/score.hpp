#ifndef TH04_MAIN_SCORE_HPP
#define TH04_MAIN_SCORE_HPP

#include "src/shared/config/score.hpp"

extern score_lebcd_t hiscore;

static const unsigned int STAGE_GRAZE_CAP = 999;

extern unsigned int graze_score;
extern unsigned int stage_graze;
extern unsigned char extends_gained;
extern unsigned long score_delta;

void near score_update_and_render(void);
void pascal score_delta_commit(void);

#endif
