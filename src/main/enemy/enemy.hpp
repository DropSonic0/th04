#ifndef TH04_MAIN_ENEMY_ENEMY_HPP
#define TH04_MAIN_ENEMY_ENEMY_HPP

#include "src/main/bullet/bullet.hpp"
#include "src/main/item/item.hpp"
#include "src/main/sprites/main_pat.hpp"

enum enemy_flag_t {
	EF_FREE = 0,
	EF_ALIVE = 1,
	EF_KILLED = 2,
	EF_ALIVE_FIRST_FRAME = 3,

	// The kill animation is keyed by the enemy flag rather than the normal
	// animation fields.
	EF_KILL_ANIM = 0x80,
	EF_KILL_ANIM_last = (EF_KILL_ANIM + PAT_ENEMY_KILL - 1)
};

#if GAME == 4
struct enemy_t {
	unsigned char flag;
	unsigned char age;
	PlayfieldMotion pos;
	unsigned char patnum_base;
	int8_t unused_1;
	int hp;
	int16_t unused_2;
	int score;
	unsigned char near *script;
	int script_ip;
	unsigned char cur_instr_frame;
	unsigned char loop_i;
	Subpixel speed;
	unsigned char angle;
	unsigned char angle_delta;
	bool clip_x;
	bool clip_y;
	int8_t unused_3;
	item_type_t item;
	bool damaged_this_frame;
	unsigned char anim_cels;
	unsigned char anim_frames_per_cel;
	unsigned char anim_cur_cel;
	bool can_be_damaged;
	bool autofire;
	bool kills_player_on_collision;
	bool spawned_in_left_half;
	BulletTemplate bullet_template;
	unsigned char autofire_cur_frame;
	unsigned char autofire_interval;
};
#endif

#define ENEMY_COUNT 32

extern enemy_t enemies[ENEMY_COUNT];
extern enemy_t near *enemy_cur;

#define ENEMY_POS_RANDOM 999.0f

void near enemies_invalidate(void);
void pascal near enemies_render(void);

#endif
