#ifndef TH04_MAIN_PLAYFLD_HPP
#define TH04_MAIN_PLAYFLD_HPP

#include "src/main/math/subpixel.hpp"
#include "src/main/math/motion.hpp"
#include "src/main/scroll/scroll.hpp"

#define PLAYFIELD_LEFT 32
#define PLAYFIELD_TOP 16
#define PLAYFIELD_W 384
#define PLAYFIELD_H 368
#define PLAYFIELD_RIGHT (PLAYFIELD_LEFT + PLAYFIELD_W)
#define PLAYFIELD_BOTTOM (PLAYFIELD_TOP + PLAYFIELD_H)

static inline pixel_t playfield_fraction_x(float fraction = 1.0f) {
	return static_cast<pixel_t>(PLAYFIELD_W * fraction + 0.0001f);
}

static inline pixel_t playfield_fraction_y(float fraction = 1.0f) {
	return static_cast<pixel_t>(PLAYFIELD_H * fraction + 0.0001f);
}

#define playfield_to_screen_left(subpixel_center_x, sprite_w) ( \
	PLAYFIELD_LEFT + TO_PIXEL(subpixel_center_x) - (sprite_w / 2) \
)

#define playfield_to_screen_top(subpixel_center_y, sprite_h) ( \
	PLAYFIELD_TOP + TO_PIXEL(subpixel_center_y) - (sprite_h / 2) \
)

// MAIN stores gameplay coordinates relative to the playfield in Q12.4 form.
struct PlayfieldPoint : public SPPoint {
	screen_x_t to_screen_left(pixel_t sprite_w_if_centered = 0) const {
		return playfield_to_screen_left(x, sprite_w_if_centered);
	}

	screen_y_t to_screen_top(pixel_t sprite_h_if_centered = 0) const {
		return playfield_to_screen_top(y, sprite_h_if_centered);
	}

	vram_y_t to_vram_top_scrolled_seg1(pixel_t sprite_h_if_centered) const {
		return scroll_subpixel_y_to_vram_seg1(
			y + (PLAYFIELD_TOP - (sprite_h_if_centered / 2))
		);
	}

	vram_y_t to_vram_top_scrolled_seg3(pixel_t sprite_h_if_centered) const {
		return scroll_subpixel_y_to_vram_seg3(
			y + (PLAYFIELD_TOP - (sprite_h_if_centered / 2))
		);
	}
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
