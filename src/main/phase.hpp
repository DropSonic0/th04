#ifndef TH04_MAIN_PHASE_HPP
#define TH04_MAIN_PHASE_HPP

static const unsigned char PHASE_HP_FILL = 0;
static const unsigned char PHASE_BOSS_ENTRANCE_BB = 1;
#if (GAME == 5)
static const unsigned char PHASE_BOSS_EXPLODE_SMALL = 253;
static const unsigned char PHASE_BOSS_EXPLODE_BIG = 254;
#endif
static const unsigned char PHASE_EXPLODE_BIG = 254;
static const unsigned char PHASE_NONE = 255;

#endif
