#ifndef TH04_MAIN_PLAYFLD_HPP
#define TH04_MAIN_PLAYFLD_HPP

#include "src/main/math/subpixel.hpp"
#include "src/main/math/motion.hpp"

#define PLAYFIELD_LEFT 32
#define PLAYFIELD_TOP 16
#define PLAYFIELD_W 384
#define PLAYFIELD_H 368
#define PLAYFIELD_RIGHT (PLAYFIELD_LEFT + PLAYFIELD_W)
#define PLAYFIELD_BOTTOM (PLAYFIELD_TOP + PLAYFIELD_H)

// MAIN stores gameplay coordinates relative to the playfield in Q12.4 form.
struct PlayfieldPoint : public SPPoint {
};

struct PlayfieldMotion : public MotionBase<PlayfieldPoint> {
	PlayfieldPoint pascal near update_seg1();
	PlayfieldPoint pascal near update_seg3();
};

extern pixel_t playfield_shake_x;
extern pixel_t playfield_shake_y;
extern int playfield_shake_anim_time;
void near playfield_shake_update_and_render(void);

#define playfield_encloses(center_x, center_y, w, h) ( \
	(static_cast<subpixel_t>(center_x) > to_sp(0 - (w / 2))) && \
	(static_cast<subpixel_t>(center_x) < to_sp(PLAYFIELD_W + (w / 2))) && \
	(static_cast<subpixel_t>(center_y) > to_sp(0 - (h / 2))) && \
	(static_cast<subpixel_t>(center_y) < to_sp(PLAYFIELD_H + (h / 2))) \
)

#define playfield_encloses_point(center, w, h) \
	playfield_encloses(center.x, center.y, w, h)

#endif
