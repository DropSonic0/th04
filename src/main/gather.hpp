#ifndef TH04_MAIN_GATHER_HPP
#define TH04_MAIN_GATHER_HPP

#include "src/main/bullet/bullet.hpp"
#include "src/main/core/entity.hpp"

struct gather_t {
	entity_flag_t flag;
	vc_t col;
	PlayfieldMotion center;
	Subpixel radius_cur;
	int ring_points;
	unsigned char angle_cur;
	unsigned char angle_delta;
#if (GAME == 4)
	BulletTemplate bullet_template;
#endif
	Subpixel radius_prev;
	Subpixel radius_delta;
#if (GAME == 5)
	BulletTemplate bullet_template;
#endif
};

struct gather_template_t {
	PlayfieldPoint center;
	PlayfieldPoint velocity;
	Subpixel radius;
	int ring_points;
	vc_t col;
	unsigned char angle_delta;
};

#define GATHER_POINT_W 8
#define GATHER_POINT_H 8
#define GATHER_FRAMES 32
#define GATHER_RADIUS_START 64.0f
#define GATHER_RADIUS_END to_sp(2.0f)

#define GATHER_COUNT 16
static const int GATHER_CAP = ((GAME == 5) ? 8 : GATHER_COUNT);

extern gather_t gather_circles[GATHER_COUNT];
extern gather_template_t gather_template;

inline void gather_template_init(void) {
	gather_template.radius.set(GATHER_RADIUS_START);
	gather_template.angle_delta = 0x02;
	gather_template.ring_points = 8;
}

void near gather_add_bullets(void);
void near gather_add_only(void);
void pascal near gather_add_only_3stack(
	int frame, vc2 col_for_0, vc2 col_for_2_and_4
);
void __fastcall near gather_point_render(screen_x_t left, vram_y_t top);
void gather_update(void);
void gather_render(void);

#endif
