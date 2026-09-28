#include <stddef.h>

#include "src/shared/platform/types.hpp"
#include "src/shared/config/resident.hpp"
#include "src/shared/formats/cdg.hpp"
#include "src/shared/formats/pi.hpp"
#include "src/shared/hardware/frame_delay.hpp"
#include "src/shared/hardware/graphics.hpp"
#include "src/shared/hardware/input.hpp"
#include "src/shared/hardware/putsa.hpp"
#include "src/shared/hardware/vram_planes.hpp"
#include "src/shared/memory/hmem.hpp"
#include "src/shared/runtime/api.hpp"
#include "src/shared/sound/api.hpp"
#include "src/op/title/cdg_slots.hpp"

typedef unsigned char dots8_t;
typedef int16_t vram_offset_t;
typedef unsigned char playchar_t;
extern unsigned char playchar_menu_sel;
extern unsigned char shottype_menu_sel;
extern unsigned char far *raise_bg[2];
extern bool selectable_with[2][2];
extern unsigned char cleared_with[2][5];
extern const char far *SHOTTYPE_TITLE[2][2];
extern const char SHOTTYPE_CLEARED[];
extern const char SHOTTYPE_CHOOSE[];

void near playchar_menu_put_initial(void);
void near shottype_menu_put_initial(void);
void near raise_bg_free(void);
void pascal near pic_darken(playchar_t playchar);
void pascal near playchar_titles_put(int sel);

enum { PLAYCHAR_REIMU = 0, PLAYCHAR_MARISA = 1, PLAYCHAR_COUNT = 2 };
enum { SHOTTYPE_A = 0, SHOTTYPE_B = 1, SHOTTYPE_COUNT = 2 };
enum { RANK_NORMAL = 1, RANK_EXTRA = 4, STAGE_EXTRA = 6 };
enum { SCOREDAT_CLEARED_A = 1, SCOREDAT_CLEARED_B = 2 };
enum { FX_WEIGHT_NORMAL = 0 };

static const int PIC_W = 256;
static const int PIC_H = 244;
static const int RAISE_W = 8;
static const int RAISE_H = 8;
static const int RAISE_BG_SIZE = ((PIC_W * RAISE_H + RAISE_W * PIC_H) / BYTE_DOTS) * PLANE_COUNT;
static const int REIMU_LEFT = 48;
static const int MARISA_LEFT = 336;
static const int PLAYCHAR_TOP = 52;
static const int SHOTTYPE_BOX_TOP = 312;
static const int SHOTTYPE_BOX_PADDING_Y = 4;
static const int SHOTTYPE_CHOOSE_LEFT = 128;
static const int SHOTTYPE_CHOOSE_PADDING_LEFT = 3 * GLYPH_HALF_W;
static const int SHOTTYPE_TITLE_LEFT = 320;
static const int SHOTTYPE_CHOOSE_W = 21 * GLYPH_HALF_W + SHOTTYPE_CHOOSE_PADDING_LEFT;
static const int SHOTTYPE_TITLE_W = 24 * GLYPH_HALF_W;
static const int BOX_ROUND = 8;
static const int SHADOW_DISTANCE = 8;
static const vc2 COL_SELECTED = 15;
static const vc2 COL_NOT_SELECTED = 3;
static const vc2 COL_BOX = 2;
static const vc2 COL_SHADOW = 1;

inline vram_offset_t vram_offset_shift(int x, int y) {
    return (y * ROW_SIZE + x / BYTE_DOTS);
}
inline vram_offset_t raise(vram_offset_t vo) {
    return vo - vram_offset_shift(RAISE_W, RAISE_H);
}
inline int playchar_other(int playchar) { return 1 - playchar; }

#define raise_bg_snap_and_advance_planar(p, vr, vm) { \
    raise_bg[0][p] = VRAM_PLANE_B[vr]; raise_bg[1][p++] = VRAM_PLANE_B[vm]; \
    raise_bg[0][p] = VRAM_PLANE_R[vr]; raise_bg[1][p++] = VRAM_PLANE_R[vm]; \
    raise_bg[0][p] = VRAM_PLANE_G[vr]; raise_bg[1][p++] = VRAM_PLANE_G[vm]; \
    raise_bg[0][p] = VRAM_PLANE_E[vr]; raise_bg[1][p++] = VRAM_PLANE_E[vm]; \
}
#define raise_bg_put_and_advance_planar(vo, p) { \
    VRAM_PLANE_B[vo] = *(p++); VRAM_PLANE_R[vo] = *(p++); \
    VRAM_PLANE_G[vo] = *(p++); VRAM_PLANE_E[vo] = *(p++); \
}

inline void dropshadow_put(int left, int top) {
    grcg_setcolor(GC_RMW, COL_SHADOW);
    grcg_boxfill_8(left + PIC_W, top + RAISE_W,
                   left + PIC_W, top + PIC_H - 1);
    grcg_boxfill_8(left + RAISE_W, top + PIC_H,
                   left + PIC_W, top + PIC_H + RAISE_H - 1);
    grcg_off();
}

#define box_put(left, top, w, h, pad) \
    grcg_round_boxfill(left, top, (left) + (w), \
                       (top) + (pad) * 2 + (h), BOX_ROUND)
#define box_shadow_put(left, top, w, h, pad) \
    box_put((left) + SHADOW_DISTANCE, (top) + SHADOW_DISTANCE, w, h, pad)
#define shottype_title_top_and_clearflag_for(top, flag, shot) { \
    top = SHOTTYPE_BOX_TOP + (shot) * (GLYPH_H + SHOTTYPE_BOX_PADDING_Y * 2); \
    flag = ((shot) == SHOTTYPE_A) ? SCOREDAT_CLEARED_A : SCOREDAT_CLEARED_B; \
}

inline void sync_pages_and_delay() {
    vsync_Count1 = 0;
    frame_delay(1);
    graph_showpage(1);
    graph_copy_page(0);
    vsync_Count1 = 0;
    frame_delay(1);
    graph_showpage(0);
}

inline bool16 near playchar_menu_leave(bool16 cancel) {
    palette_black_out(1);
    raise_bg_free();
    pi_free(0);
    return cancel;
}

#pragma codeseg OP_NATIVE_TEXT OP_NATIVE_01
#include "src/op/menu/raise_bg_allocate_and_snap.inl"
#include "src/op/menu/raise_bg_put.inl"

inline void pic_put_for(int selected, int selected_left, int other_left) {
    cdg_put_noalpha_8(selected_left - RAISE_W, PLAYCHAR_TOP - RAISE_H,
                      CDG_PIC + selected);
    raise_bg_put(playchar_other(selected));
    cdg_put_noalpha_8(other_left, PLAYCHAR_TOP, CDG_PIC + playchar_other(selected));
    pic_darken(playchar_other(selected));
    dropshadow_put(selected_left - RAISE_W, PLAYCHAR_TOP - RAISE_H);
    playchar_titles_put(selected);
}

#include "src/op/menu/pic_put.inl"
#include "src/op/menu/shottype_title_box_put.inl"
#include "src/op/menu/shottype_titles_put.inl"
#include "src/op/menu/playchar_menu.inl"
#pragma codeseg
