#ifndef TH04_MAIN_BULLET_CLEARZAP_HPP
#define TH04_MAIN_BULLET_CLEARZAP_HPP

#include "src/shared/platform/types.hpp"
#include "src/main/sprites/cels.hpp"

// Set to true to clear all on-screen bullets, giving out a semi-exponential
// bonus for all bullets that were alive on the first frame of activity.
// The anonymous union preserves the target's byte-sized active flag and
// animation timer symbols used by the existing MAIN assembly slices.
extern union {
	bool active;
	uint8_t frame;
} bullet_zap;

static const int BULLET_ZAP_FRAMES_PER_CEL = 4;
static const int BULLET_ZAP_FRAMES = (
	BULLET_ZAP_CELS * BULLET_ZAP_FRAMES_PER_CEL
);

// Number of frames left during which all on-screen bullets should decay.
extern unsigned char bullet_clear_time;

#define bullets_clear() { \
	if(bullet_clear_time < 20) { \
		bullet_clear_time = 20; \
	} \
}

#endif
