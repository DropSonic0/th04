#include "src/shared/platform/ps3_compat.hpp"

#ifndef __TURBOC__

#include "src/shared/runtime/api.hpp"
#include "src/shared/hardware/graphics.hpp"
#include "src/shared/hardware/vram_planes.hpp"
#include "src/shared/config/score.hpp"
#include "src/shared/formats/cdg.hpp"
#include "th04/sprites/main_pat.h"
#include "src/main/boss/boss.hpp"
#include "th04/main/boss/backdrop.hpp"
#include "th04/main/boss/bosses.hpp"
#include "th04/main/custom.hpp"
#include "src/main/bullet/laser_t.hpp"
#include "th04/main/bullet/bullet.hpp"
#include "th04/main/enemy/enemy.hpp"
#include "th04/main/player/player.hpp"
#include "src/main/player/shot.hpp"
#include "th04/main/gather.hpp"
#include "th04/main/circle.hpp"
#include "th04/main/spark.hpp"
#include "th04/main/item/item.hpp"
#include "th04/main/hud/hud.hpp"
#include "th04/main/hud/overlay.hpp"
#include "th04/main/score.hpp"
#include "th04/main/rank.hpp"
#include "th04/main/pointnum/pointnum.hpp"
#include "src/main/math/randring.hpp"

struct yuuka6_bg_shape_t {
    SPPoint pos;
    unsigned char angle;
    SubpixelLength8 speed;
};

// Registers (C linkage)
extern "C" {
    uint32_t _EAX = 0;
    uint32_t _EBX = 0;
    uint32_t _ECX = 0;
    uint32_t _EDX = 0;

    uint16_t _AX = 0;
    uint16_t _BX = 0;
    uint16_t _CX = 0;
    uint16_t _DX = 0;
    uint16_t _SI = 0;
    uint16_t _DI = 0;
    uint16_t _ES = 0;
    uint16_t _DS = 0;
    uint16_t _CS = 0;
    uint16_t _SS = 0;

    uint8_t  _AL = 0;
    uint8_t  _AH = 0;
    uint8_t  _BL = 0;
    uint8_t  _BH = 0;
    uint8_t  _CL = 0;
    uint8_t  _CH = 0;
    uint8_t  _DL = 0;
    uint8_t  _DH = 0;

    volatile unsigned int vsync_Count1 = 0;
    volatile unsigned int vsync_Count2 = 0;

    // C-linkage API functions
    int TH04_PASCAL file_ropen(const char TH04_PTR *filename) { return 0; }
    int TH04_PASCAL file_read(void far *buf, unsigned wsize) { return 0; }
    long TH04_PASCAL file_size(void) { return 0; }
    int TH04_PASCAL file_create(const char TH04_PTR *filename) { return 0; }
    int TH04_PASCAL file_append(const char TH04_PTR *filename) { return 0; }
    int TH04_PASCAL file_write(const void far *buf, unsigned wsize) { return 0; }
    void TH04_PASCAL file_seek(long pos, int dir) {}
    void TH04_PASCAL file_close(void) {}
    int TH04_PASCAL file_exist(const char TH04_PTR *filename) { return 0; }
    int TH04_PASCAL file_delete(const char TH04_PTR *filename) { return 0; }

    unsigned TH04_PASCAL ems_allocate(unsigned long len) { return 0; }
    int TH04_PASCAL ems_exist(void) { return 0; }
    int TH04_PASCAL ems_read(unsigned handle, long offset, void far *mem, long size) { return 0; }
    int TH04_PASCAL ems_setname(unsigned handle, const char TH04_PTR * name) { return 0; }
    unsigned long TH04_PASCAL ems_space(void) { return 0; }
    int TH04_PASCAL ems_write(unsigned handle, long offset, const void far *mem, long size) { return 0; }

    int TH04_PASCAL iatan2(int y, int x) { return 0; }
    int TH04_PASCAL isqrt(long x) { return 0; }
    int TH04_PASCAL ihypot(int x, int y) { return 0; }

    void TH04_PASCAL vsync_start(void) {}
    void TH04_PASCAL vsync_end(void) {}

    int TH04_PASCAL js_start() { return 0; }
    void TH04_PASCAL js_end(void) {}
    int TH04_PASCAL js_sense(void) { return 0; }

    void TH04_PASCAL cdg_load_single(int slot, const char *fn, int image) {}
    void TH04_PASCAL cdg_load_single_noalpha(int slot, const char *fn, int image) {}
    void TH04_PASCAL cdg_load_all(int slot_first, const char *fn) {}
    void TH04_PASCAL cdg_load_all_noalpha(int slot_first, const char *fn) {}
    void TH04_PASCAL cdg_free(int slot) {}
    void TH04_PASCAL cdg_free_all(void) {}
    void TH04_PASCAL cdg_put_8(screen_x_t left, vram_y_t top, int slot) {}
    void TH04_PASCAL cdg_put_noalpha_8(screen_x_t left, vram_y_t top, int slot) {}
    void TH04_PASCAL cdg_put_plane(screen_x_t left, vram_y_t top, int slot, int plane) {}

    void TH04_PASCAL grcg_circle(screen_x_t center_x, vram_y_t center_y, unsigned r) {}
    void TH04_PASCAL grcg_circlefill(int x, int y, unsigned r) {}
    void TH04_PASCAL grcg_setcolor(int mode, vc2 color) {}

    void TH04_PASCAL super_clean(int min_pat, int max_pat) {}
    void TH04_PASCAL super_roll_put_1plane(int x, int y, int num, int pattern_plane, unsigned put_plane) {}
    void TH04_PASCAL super_put_1plane(int x, int y, int num, int pattern_plane, unsigned put_plane) {}
    void TH04_PASCAL super_wave_put(int x, int y, int num, int len, char amp, int ph) {}
    void TH04_PASCAL super_zoom(int x, int y, int num, int zoom) {}

    void TH04_PASCAL text_putsa(unsigned x, unsigned y, const char TH04_PTR *str, unsigned atrb) {}
    void TH04_PASCAL text_putca(unsigned x, unsigned y, unsigned ch, unsigned atrb) {}
    void TH04_PASCAL text_fillca(unsigned ch, unsigned atrb) {}

    int TH04_PASCAL select_for_rank(int for_easy, int for_normal, int for_hard, int for_lunatic) { return for_normal; }

    unsigned super_patnum = 0;
    void __seg *super_buffer = 0;
    unsigned super_patdata[512] = {0};
    unsigned super_patsize[512] = {0};

    unsigned pferrno = 0;
    unsigned char pfkey = 0;
    int TH04_PASCAL pf_hook_install(void) { return 0; }
    void TH04_PASCAL pf_hook_remove(void) {}
}

// C++ linkage VRAM plane pointers
uint8_t far *VRAM_PLANE_B = 0;
uint8_t far *VRAM_PLANE_R = 0;
uint8_t far *VRAM_PLANE_G = 0;
uint8_t far *VRAM_PLANE_E = 0;

// C++ linkage helper stubs and data
void near grcg_setmode_tdw(void) {}
void near grcg_setmode_rmw(void) {}
void near grcg_setcolor_direct_raw(void) {}
void near grcg_fill_playfield_rows(void) {}
void near grcg_vline(int, int, int) {}
void near grcg_line(int, int, int, int) {}

void TH04_PASCAL egc_shift_left(screen_x_t x1, vram_y_t y1, screen_x_t x2, vram_y_t y2, pixel_t dots) {}
void TH04_PASCAL egc_shift_right(screen_x_t x1, vram_y_t y1, screen_x_t x2, vram_y_t y2, pixel_t dots) {}
void TH04_PASCAL egc_shift_up(screen_x_t x1, vram_y_t y1, screen_x_t x2, vram_y_t y2, pixel_t dots) {}
void TH04_PASCAL egc_shift_down(screen_x_t x1, vram_y_t y1, screen_x_t x2, vram_y_t y2, pixel_t dots) {}

void near reimu_marisa_backdrop_colorfill(void) {}
void near yuuka5_backdrop_colorfill(void) {}

void near super_roll_put(int, int, int) {}
void near super_large_put(int, int, int) {}
void near z_super_roll_put_tiny_16x16_raw(int) {}
void near z_super_roll_put_tiny_32x32_raw(int) {}
void near z_super_put_16x16_mono_raw(int) {}
int TH04_PASCAL super_convert_tiny(int num) { return 0; }

void near pointnums_update(void) {}
void near pointnums_render(void) {}
void near pointnums_invalidate(void) {}
void near pointnums_init(void) {}

void near pellets_render_top(void) {}
void near pellets_render_bottom(void) {}
void near pellets_render(void) {}

void near playperf_raise(int) {}
void near playperf_lower(int) {}

void near enemy_bullet_template_push(void) {}
void near item_splash_dot_render(void) {}
void near vector2_near(void) {}
void near vector2(int&, int&, int, int) {}

void near playfield_fillm_0_40_384_274(void) {}
void near playfield_fill(void) {}
void near playfield_checkerboard_grcg_tdw_(void) {}

void pascal near tiles_invalidate_around(const SPPoint) {}
void near tiles_bb_invalidate_raw(int) {}
void near tiles_redraw_invalidated(void) {}
void near tiles_render_all(void) {}
void near tiles_bb_put_raw(int) {}
void near tiles_fill_initial(void) {}

void near SHOT_LASER_PUT_RAW(void) {}
void near bgm_timer_start(void) {}
void near bgm_timer_stop(void) {}

void near input_reset_sense(void) {}
void near input_sense(void) {}
void near scoredat_encode(void*) {}
void near scoredat_decode(void*) {}
void near egc_start_copy_noframe(void) {}

void near sub_3680(void) {}
void near sub_11DE6(void) {}
void near sub_BAEE(void) {}
void near sub_12024(void) {}

// C++ BSS / Data definitions
boss_stuff_t boss;
bool boss_phase_timed_out = false;
SPPoint boss_hitbox_radius;
func_t_near boss_update = 0;
nearfunc_t_near boss_fg_render = 0;
func_t_near boss_update_func = 0;
nearfunc_t_near boss_bg_render_func = 0;
nearfunc_t_near boss_fg_render_func = 0;
nearfunc_t_near boss_backdrop_colorfill = 0;
unsigned char boss_statebyte[16] = {0};

Palette8 Palettes;
unsigned int PaletteTone = 100;
bool palette_changed = false;

unsigned int stage_graze = 0;
unsigned char stage_point_items_collected = 0;

PlayfieldMotion player_pos;
bool player_is_hit = false;
unsigned char player_invincibility_time = 0;
uint8_t power = 0;
uint8_t shot_level = 0;

const short CosTable8[256] = {0};
const short SinTable8[256] = {0};

const int ITEM_PATNUM[IT_COUNT] = {0};
const Subpixel ITEM_MISS_VELOCITIES[MISS_FIELD_COUNT][2][ITEM_MISS_COUNT] = {0};
unsigned int items_spawned = 0;
unsigned int items_collected = 0;
unsigned int total_point_items_collected = 0;
unsigned int total_max_valued_point_items_collected = 0;
unsigned char item_playperf_lower = 0;
unsigned char item_playperf_raise = 0;
bool items_pull_to_player = false;
item_t items[ITEM_COUNT];

score_lebcd_t hiscore;
score_lebcd_t score;
unsigned int graze_score = 0;
unsigned char extends_gained = 0;
unsigned long score_delta = 0;

unsigned int bb_boss_seg = 0;
unsigned int tiles_bb_seg = 0;
unsigned char tiles_bb_col = 0;

nearfunc_t_near midboss_update_func = 0;
nearfunc_t_near midboss_render_func = 0;
int midboss_frames_until = 0;
unsigned char midboss_defeat_angle = 0;
unsigned char midboss1_angle = 0;
int midboss1_vram_y = 0;
unsigned char midboss2_pattern = 0;
unsigned char midboss2_direction = 0;
unsigned char midboss2_patterns_done = 0;
unsigned char midboss3_pattern = 0;
unsigned char midboss3_mirror = 0;
unsigned char midboss3_patterns_done = 0;
unsigned char MIDBOSS3_FLY_ANGLES[8] = {0};
unsigned char midboss4_pattern = 0;
unsigned char midboss4_aim_toggle = 0;
unsigned char midboss4_unknown_state = 0;
unsigned char midboss4_patterns_done = 0;
void (near pascal *midboss_invalidate)(void) = 0;
int midboss_active = 0;

const char aSt00_bmt[] = "st00.bmt";
const char aSt00bk_cdg[] = "st00bk.cdg";
const char aSt00_bb[] = "st00.bb";
const char aSt01_bmt[] = "st01.bmt";
const char aSt01bk_cdg[] = "st01bk.cdg";
const char aSt01_bb[] = "st01.bb";
const char aSt02_bmt[] = "st02.bmt";
const char aSt02bk_cdg[] = "st02bk.cdg";
const char aSt02_bb[] = "st02.bb";
const char aSt03_bmt[] = "st03.bmt";
const char aSt03bk_cdg[] = "st03bk.cdg";
const char aSt03bk2_cdg[] = "st03bk2.cdg";
const char aSt03_bb[] = "st03.bb";
const char aSt04bk_cdg[] = "st04bk.cdg";
const char aSt04_bb[] = "st04.bb";
const char aSt04_cdg[] = "st04.cdg";
const char aSt05_bb[] = "st05.bb";
const char st06_bft[] = "st06.bft";
const char bss6_cd2[] = "bss6.cd2";
const char st06_mpn[] = "st06.mpn";
const char st05_bft[] = "st05.bft";
const char bss5_cd2[] = "bss5.cd2";
const char st05_mpn[] = "st05.mpn";
const char bss4_cd2[] = "bss4.cd2";
const char st04_bft[] = "st04.bft";
const char st04_mpn[] = "st04.mpn";
const char bss2_cd2[] = "bss2.cd2";
const char st02_bft[] = "st02.bft";
const char st02_mpn[] = "st02.mpn";
const char bss1_cd2[] = "bss1.cd2";
const char st01_bft[] = "st01.bft";
const char st01_mpn[] = "st01.mpn";
const char bss0_cd2[] = "bss0.cd2";
const char st00_bft[] = "st00.bft";
const char st00_mpn[] = "st00.mpn";
const char st10_mpn[] = "st10.mpn";
const char kao3_cd2[] = "kao3.cd2";
const char kao2_cd2[] = "kao2.cd2";
const char st03_bft[] = "st03.bft";
const char st03_mpn[] = "st03.mpn";
const char eye_rgb[] = "eye.rgb";
const char miko_bft[] = "miko.bft";
const char mari_bft[] = "mari.bft";
const char mikod_bft[] = "mikod.bft";
const char miko32_bft[] = "miko32.bft";
const char miko16_bft[] = "miko16.bft";
const char stage_bgm_name[16] = {0};
int stage_faceset_count = 0;

nearfunc_t_near stage_render = 0;
nearfunc_t_near stage_invalidate = 0;
unsigned char stage_frame_mod2 = 0;
unsigned char stage_frame_mod4 = 0;
unsigned char stage_frame_mod8 = 0;
unsigned char stage_frame_mod16 = 0;
unsigned int stage_frame = 0;
int stage_id = 0;
func_t_near stage_vm = 0;
int stage5_star_center_y = 0;
const char STAGE_CLEAR_BONUS_DESC[] = "";
const char gpCLEAR_BONUS[] = "";
const char aBONUS_STAGE[] = "";
const char aPOWERX50[] = "";
const char aBONUS_DREAM[] = "";
const char aGRAZEX50[] = "";
const char aBONUS_POINT[] = "";
const char aBONUS_TOTAL[] = "";
const char aBOMB_EXTEND[] = "";
const char gpCONGRATULATION[] = "";
const char aALL_CLEAR[] = "";
const char aPOWERX50_2[] = "";
const char aBONUS_DREAM_2[] = "";
const char aGRAZEX50_2[] = "";
const char aPLAYER_REM_10000[] = "";
const char aPLAYER_REM_30000[] = "";
const char aBONUS_POINT_2[] = "";
const char aBONUS_TOTAL_2[] = "";
int stage_bgm_title_len = 0;
int stage_title_id = 0;
const char STAGE_TITLES[8][32] = {0};
int stage_title_len = 0;
const char BGM_TITLES[16][32] = {0};
const char gStage_1[] = "";
const char gFINAL_STAGE[] = "";
const char gEXTRA_STAGE[] = "";

unsigned char playchar = 0;
bool reimu_trail_visible = false;
unsigned char reimu_pattern_angle_delta = 0;
bool reimu_orbs_visible = false;
int orb_patnum_base = 0;
Shot shots[SHOT_COUNT];
int player_respawn_motion_time = 0;
SPPoint player_option_pos_prev[2];
Shot near *shot_ptr = 0;
char shot_last_id = 0;
unsigned int shot_laser_time = 0;
shot_laser_style_t shot_laser_style = SLS_2;
SPPoint player_option_pos_cur[2];
PlayfieldMotion shot_laser_bottomcenter;
uint8_t shot_laser_ring_cycle = 0;
unsigned char shot_time = 0;
unsigned char reimu_shot_cycle = 0;
unsigned char byte_259A7 = 0;
unsigned int shots_alive_count = 0;
shot_alive_t shots_alive[SHOT_COUNT];
unsigned char byte_25980 = 0;
int player_input_prev = 0;
nearfunc_t_near playchar_shot_func = 0;
int player_option_patnum = 0;
const nearfunc_t_near SHOT_FUNCS_REIMU_A[10] = {0};
nearfunc_t_near playchar_shot_funcs[10] = {0};
const nearfunc_t_near SHOT_FUNCS_REIMU_B[10] = {0};
nearfunc_t_near player_bomb_func = 0;
nearfunc_t_near playchar_bomb_func = 0;
const nearfunc_t_near SHOT_FUNCS_MARISA_A[10] = {0};
const nearfunc_t_near SHOT_FUNCS_MARISA_B[10] = {0};
unsigned char player_state_unknown_0 = 0;
unsigned char player_state_unknown_1 = 0;
unsigned char player_state_unknown_2 = 0;
unsigned char player_state_unknown_3 = 0;
int player_miss_animation_frame = 0;

uint16_t randring_p = 0;
unsigned char randring[256] = {0};

const unsigned char BOSS_ITEM_DROPS[8] = {0};
const unsigned char ENEMY_DROPS[8] = {0};
int item_drop_cycle = 0;
int power_overflow = 0;
int POWER_OVERFLOW_BONUS = 0;
bool pointnum_times_2 = false;
int DREAM_SCORE_PER_ITEMS = 0;
int miss_time = 0;

unsigned char bullet_clear_time = 0;
unsigned char bullet_zap = 0;
bool bullet_zap_active = false;
BulletTemplate bullet_template;
bullet_special_u bullet_special = {};
bullet_special_angle_t bullet_template_special_angle = {};
bool bombing = false;
pixel_t playfield_shake_x = 0;
pixel_t playfield_shake_y = 0;
int slowdown_factor = 0;
bool bombing_disabled = false;
int playfield_shake_anim_time = 0;
int egc_shift_left_val = 0;
int egc_shift_right_val = 0;
int egc_shift_up_val = 0;
int egc_shift_down_val = 0;
int playfield_shake_redraw_time = 0;

unsigned char resident[256] = {0};
unsigned char rank = 0;
int score_delta_frame = 0;
unsigned long dream_score = 0;
int dream_items_collected = 0;
int entered_place = 0;
const char SCOREDAT_FN[] = "";
const char SCOREDAT_FN_0[] = "";
const char SCOREDAT_FN_1[] = "";
const char SCOREDAT_FN_2[] = "";
const char gCONTINUE_[] = "";

nearfunc_t_near overlay1 = 0;
nearfunc_t_near overlay2 = 0;
unsigned long overlay_popup_bonus = 0;
popup_id_t overlay_popup_id_new = POPUP_ID_HISCORE_ENTRY;
int overlay_fade = 0;
int titles_frame = 0;
int dissolve_sprite = 0;
int popup_id_cur = 0;
int popup_frame = 0;
int popup_shiftbuf = 0;
const char POPUP_STRINGS[8][32] = {0};
int popup_gaiji_len = 0;
int popup_cur_tram_left = 0;
int popup_dest_tram_left = 0;
bool popup_dest_reached = false;
const char PLAYFIELD_BLANK_ROW[] = "";
const int HUD_POWER_COLORS[4] = {0};
const char gHUD_HP_BLANK[] = "";
const int HUD_HP_COLORS[4] = {0};
const char gsENEMY[] = "";
int hud_bar_max = 0;
const char gsHISCORE[] = "";
const char gsSCORE[] = "";
const char gsREIGEKI[] = "";
const char gsBOMB[] = "";
const char gsREIMU[] = "";
const char gsPLAYER[] = "";
const char gsREIRYOKU[] = "";
const char gsPOWER[] = "";
const char glEASY[] = "";
int hud_hp_bar_value_prev = 0;

bool quit = false;
SPPoint homing_target;
unsigned char elly_scythe_flag = 0;
PlayfieldMotion elly_scythe_motion;
Subpixel bg_shape_flyout_speed;
unsigned char yuuka6_bg_fade = 0;
yuuka6_bg_shape_t bg_shapes[64];
unsigned char yuuka6_bg_state = 0;
void (near pascal *near bg_shape_clip)(yuuka6_bg_shape_t near&) = 0;
unsigned char yuuka6_bg_palette_latch = 0;
main_patnum_t bg_shape_patnum = PAT_STAGE;
unsigned char elly_scythe_mode = 0;
int elly_scythe_frame = 0;
unsigned char elly_scythe_angle = 0;
SubpixelLength8 elly_scythe_speed;
char elly_scythe_turn = 0;
int elly_orbit_frame = 0;
vc_t circles_color = 0;
gather_template_t gather_template;
unsigned char elly_pattern_group = 0;
Explosion explosions_small[16];
Explosion explosions_big;
int explosion_big_frame = 0;
custom_t custom_entities[32];
thicklaser_t thicklaser_template;
int gengetsu_damage_flash_cycle = 0;
bool kurumi_special_turn_toggle = false;
unsigned char kurumi_unknown_state = 0;
int MARISA_BIT_HP = 0;
int marisa_bit_angle_speed = 0;
int bits_alive = 0;
int bit_center_x = 0;
int bit_center_y = 0;
bool bit_fire = false;
int marisa_pattern_variant = 0;
int marisa_palette_direction = 0;
int marisa_bitless_cycle = 0;
int marisa_prev_mode = 0;
int marisa_prev_bits_alive = 0;
int mugetsu_damage_flash_cycle = 0;
int mugetsu_gather_frame_offset = 0;
SPPoint mugetsu_anchor;
nearfunc_t_near mugetsu_transition_func = 0;
int mugetsu_phase2_mode = 0;
int yuuka5_move_state = 0;
int yuuka5_sweep_x = 0;
int yuuka5_cloud_step = 0;
int yuuka5_cloud_accum = 0;
int yuuka5_palette_tone = 0;
unsigned char yuuka6_aux_flag = 0;
int yuuka6_damage_flash_cycle = 0;
unsigned char yuuka6_mirror_state = 0;
SPPoint yuuka6_mirror_pos;
unsigned char yuuka6_mirror_damage = 0;
int yuuka6_mirror_damage_flash_cycle = 0;
const unsigned char YUUKA6_PHASE2_FLY_ANGLES[8] = {0};
int yuuka6_phase2_fly_path = 0;
unsigned char yuuka6_sprite_flag = 0;
unsigned char yuuka6_aux_state = 0;
SPPoint yuuka6_aux_pos;
int yuuka6_anim_frame = 0;
unsigned char yuuka6_pattern_prev = 0;

bullet_t bullets[440];
thicklaser_t thicklasers[2];
unsigned long circles[40];
gather_t gather_circles[16];
SPPoint drawpoint;
spark_t sparks[96];
enemy_t enemies[32];
pointnum_t pointnums[400];
int group_fixedspeed = 0;
int group_i_spread_angle = 0;
int group_i_absolute_angle = 0;

unsigned char DemoBuf[512] = {0};
const char demo_fn[] = "";
int key_det = 0;
int shiftkey = 0;
int DEMOPLAY_BINARY_OP = 0;
bool gDEMO_PLAY = false;

const char eyename[] = "";
void* Ems = 0;
const char EMS_NAME[] = "";
const char bbname[] = "";
const char FACESET_REIMU_FN_0[] = "";
const char FACESET_MARISA_FN_0[] = "";

int frames_unused = 0;
int total_slow_frames = 0;
int total_frames = 0;
int page_front = 0;
int page_back = 0;
int playperf_min = 0;
int playperf_max = 0;
int total_std_frames = 0;
int enemies_gone = 0;
int enemies_killed = 0;
int gameover_fade_frame = 0;
bool gameover_erase_in = false;
bool gameover_erase_out = false;
const char gGAMEOVER[] = "";
const char maine_binary[] = "";
int continues_used = 0;
const char gCONTINUE_QUESTION[] = "";
const char gYES[] = "";
const char gNO[] = "";
const char gCREDIT[] = "";
void* dialog_p = 0;
const char dialog_fn[] = "";
const char dialog_fn_yuuka5_defeat_bad[] = "";
int script_param_number_default = 0;
int dialog_side = 0;
unsigned char dialog_kanji_buf[64] = {0};
int dialog_cursor = 0;
const char FACESET_REIMU_FN_1[] = "";
const char FACESET_MARISA_FN_1[] = "";
int number_of_calls_to_this_function_during_extra = 0;
const char FACESET_MUGETSU_DEFEAT_FN[] = "";
const char FACESET_GENGETSU_DEFEAT_FN[] = "";
const char BOMB_BG_REIMU_FN[] = "";
const char BOMB_BG_MARISA_FN[] = "";
void* std_enemy_scripts = 0;
enemy_t* enemy_cur = 0;
unsigned int std_seg = 0;
unsigned int bb_txt_seg = 0;
const char bb_txt_fn[] = "";
const char bb_txt2_fn[] = "";
const char map_fn[] = "";
unsigned int map_seg = 0;
int mpn_slots = 0;
bool mpn_show_palette_on_load = false;
int drawpoint_y = 0;
int drawpoint_x = 0;
vram_y_t scroll_line_on_page[2] = {0};
unsigned char byte_250FE = 0;
unsigned char byte_25104 = 0;
unsigned int word_25100 = 0;
int tile_row_in_section = 0;
int std_map_section_id = 0;
int std_scroll_speed = 0;
int tile_ring = 0;
uint16_t spark_ring_offset = 0;
int checkerboard = 0;
int halftiles_dirty = 0;
int carpet_lighting_cel = 0;
int carpet_light_level = 0;
int CARPET_LIGHTING_ANIM = 0;
int CARPET_TILE_IMAGE_VOS = 0;
void* std_fn = 0;
int std_ip = 0;
int tile_render_all_time = 0;
const char CFG_FN[] = "";
const char main_pf_fn[] = "";
const char gaiji_fn[] = "";
const char se_fn[] = "";
const char op_fn[] = "";
int fp_23D90 = 0;
nearfunc_t_near std_update = 0;
nearfunc_t_near bg_render_bombing = 0;
nearfunc_t_near bg_render_not_bombing = 0;
int bgm_timer_divisor = 0;

PlayfieldPoint PlayfieldMotion::update_seg1() { PlayfieldPoint p; p.x.v = 0; p.y.v = 0; return p; }
PlayfieldPoint PlayfieldMotion::update_seg3() { PlayfieldPoint p; p.x.v = 0; p.y.v = 0; return p; }

bool shots_hittest_against_boss = false;
int (shots_hittest)(void) { return 0; }
void shot_velocity_set(SPPoint*, unsigned char) {}
void pointnums_add_white(subpixel_t, subpixel_t, uint16_t) {}
void pointnums_add_yellow(subpixel_t, subpixel_t, uint16_t) {}
nearfunc_t_near bullets_add_regular = 0;
nearfunc_t_near bullets_add_special = 0;
void thicklaser_add(void) {}

#endif
