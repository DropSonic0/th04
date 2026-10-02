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

// VRAM and text-RAM extents used by MAIN's background and HUD code. These
// values were formerly inherited from the TH02 playfield header.
#define PLAYFIELD_VRAM_LEFT (PLAYFIELD_LEFT / BYTE_DOTS)
#define PLAYFIELD_VRAM_W (PLAYFIELD_W / BYTE_DOTS)
#define PLAYFIELD_VRAM_RIGHT (PLAYFIELD_RIGHT / BYTE_DOTS)

#define PLAYFIELD_TRAM_LEFT (PLAYFIELD_LEFT / 8)
#define PLAYFIELD_TRAM_TOP (PLAYFIELD_TOP / GLYPH_H)
#define PLAYFIELD_TRAM_W (PLAYFIELD_W / 8)
#define PLAYFIELD_TRAM_CENTER_X \
	((PLAYFIELD_LEFT + (PLAYFIELD_W / 2)) / GLYPH_HALF_W)
#define PLAYFIELD_TRAM_CENTER_Y \
	((PLAYFIELD_TOP + (PLAYFIELD_H / 2)) / GLYPH_H)
#define PLAYFIELD_TRAM_RIGHT (PLAYFIELD_RIGHT / 8)
#define PLAYFIELD_TRAM_BOTTOM (PLAYFIELD_BOTTOM / GLYPH_H)

// HUD_LEFT is 56 in TH04; keep the derived clipping edge local to MAIN so
// this product header does not import the TH02 HUD declaration.
#define PLAYFIELD_CLIP_RIGHT (56 * GLYPH_HALF_W)
#define PLAYFIELD_ROLL_MARGIN (PLAYFIELD_TOP + (RES_Y - PLAYFIELD_BOTTOM))

#define playfield_clip_center_left_small(center_x, w) ( \
	center_x <= to_sp((w / 2) - PLAYFIELD_LEFT) \
)
#define playfield_clip_center_left_large(center_x, w) ( \
	center_x < to_sp((w / 2) - PLAYFIELD_LEFT) \
)
#define playfield_clip_center_right_small(center_x, w) ( \
	center_x >= to_sp(PLAYFIELD_CLIP_RIGHT - PLAYFIELD_LEFT - (w / 2)) \
)
#define playfield_clip_center_right_large(center_x, w) ( \
	center_x > to_sp(PLAYFIELD_CLIP_RIGHT - PLAYFIELD_LEFT - (w / 2)) \
)
#define playfield_clip_center_top_small_roll(center_y, h) ( \
	center_y <= to_sp((h / 2) - PLAYFIELD_ROLL_MARGIN) \
)
#define playfield_clip_center_top_large_roll(center_y, h) ( \
	center_y < to_sp((h / 2) - PLAYFIELD_ROLL_MARGIN) \
)
#define playfield_clip_center_bottom_small_roll(center_y, h) ( \
	center_y >= to_sp(PLAYFIELD_H + PLAYFIELD_ROLL_MARGIN - (h / 2)) \
)
#define playfield_clip_center_bottom_large_roll(center_y, h) ( \
	center_y > to_sp(PLAYFIELD_H + PLAYFIELD_ROLL_MARGIN - (h / 2)) \
)

#define playfield_clip_left_small(left, w) (left <= (PLAYFIELD_LEFT - w))
#define playfield_clip_right_small(left, w) (left >= PLAYFIELD_RIGHT)
#define playfield_clip_left_large(left, w) (left < 0)
#define playfield_clip_right_large(left, w) (left > (PLAYFIELD_CLIP_RIGHT - w))
#define playfield_clip_top_small(top, h) (top <= (PLAYFIELD_TOP - h))
#define playfield_clip_bottom_small(top, h) (top >= PLAYFIELD_BOTTOM)
#define playfield_clip_top_large(top, h) (top < 0)
#define playfield_clip_bottom_large(top, h) (top > (RES_Y - h))

#define playfield_clip_topleft_small(left, top, w, h) ( \
	playfield_clip_left_small(left, w) || \
	playfield_clip_right_small(left, w) || \
	playfield_clip_top_small(top, h) || \
	playfield_clip_bottom_small(top, h) \
)
#define playfield_clip_topleft_large(left, top, w, h) ( \
	playfield_clip_left_large(left, w) || \
	playfield_clip_right_large(left, w) || \
	playfield_clip_top_large(top, h) || \
	playfield_clip_bottom_large(top, h) \
)

#define playfield_clip_center_yx_small_roll(center_x, center_y, w, h) ( \
	(playfield_clip_center_top_small_roll((subpixel_t)(center_y), h)) || \
	(playfield_clip_center_bottom_small_roll((subpixel_t)(center_y), h)) || \
	(playfield_clip_center_left_small((subpixel_t)(center_x), w)) || \
	(playfield_clip_center_right_small((subpixel_t)(center_x), w)) \
)
#define playfield_clip_center_yx_large_roll(center_x, center_y, w, h) ( \
	(playfield_clip_center_top_large_roll((subpixel_t)(center_y), h)) || \
	(playfield_clip_center_bottom_large_roll((subpixel_t)(center_y), h)) || \
	(playfield_clip_center_left_large((subpixel_t)(center_x), w)) || \
	(playfield_clip_center_right_large((subpixel_t)(center_x), w)) \
)
#define playfield_clip_point_yx_small_roll(center, w, h) \
	playfield_clip_center_yx_small_roll(center.x, center.y, w, h)
#define playfield_clip_point_yx_large_roll(center, w, h) \
	playfield_clip_center_yx_large_roll(center.x, center.y, w, h)

#define playfield_encloses_yx_lt_ge(center_x, center_y, w, h) ( \
	(static_cast<subpixel_t>(center_y) >= to_sp(0 - (h / 2))) && \
	(static_cast<subpixel_t>(center_y) < to_sp(PLAYFIELD_H + (h / 2))) && \
	(static_cast<subpixel_t>(center_x) >= to_sp(0 - (w / 2))) && \
	(static_cast<subpixel_t>(center_x) < to_sp(PLAYFIELD_W + (w / 2))) \
)

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
			y.v + to_sp(PLAYFIELD_TOP - (sprite_h_if_centered / 2))
			);
	}

	vram_y_t to_vram_top_scrolled_seg3(pixel_t sprite_h_if_centered) const {
		return scroll_subpixel_y_to_vram_seg3(
			y.v + to_sp(PLAYFIELD_TOP - (sprite_h_if_centered / 2))
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
