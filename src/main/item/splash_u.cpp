// TH04 MAIN's historical it_spl_u.cpp object is one physical producer.
// Keep the separately reviewed init and add/update fragments together; the
// splash BSS declarations remain an independent native-link ownership task.
#pragma option -zCIT_SPL_U_TEXT -zPmain_03
#include "th04/main/item/splash.hpp"

#define ITEM_SPLASH_RADIUS_START 2.0f
#define ITEM_SPLASH_RADIUS_DELTA 2.0f
#define ITEM_SPLASH_RADIUS_END 32.0f

extern unsigned char item_splash_last_id;

#include "src/main/item/splashes_init.inl"
#include "src/main/item/splashes.inl"
