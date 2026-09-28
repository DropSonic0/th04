#include "src/shared/platform/types.hpp"
#include "src/shared/formats/cdg.hpp"
#include "src/shared/formats/pi.hpp"
#include "src/shared/hardware/bgimage.hpp"
#include "src/shared/hardware/graphics.hpp"
#include "src/shared/hardware/input.hpp"
#include "src/shared/math/polar.hpp"
#include "src/shared/runtime/api.hpp"
#include "src/shared/sound/api.hpp"

enum { TRACK_COUNT = 22, POLYGON_COUNT = 16, POLYGONS_RENDERED = 16 };
enum { COL_TRACKLIST_SELECTED = 3, COL_TRACKLIST = 5 };
static const char BGM_MENU_MAIN_FN[] = "op";

struct cmt_line_t { unsigned char c[40]; };
cmt_line_t cmt[20];
unsigned char track_playing;
unsigned char music_sel;
unsigned char music_page_accessed;
unsigned char cmt_shown_initial;
unsigned char __seg *nopoly_B;

struct music_subpixel_t {
    int v;
    operator int() const { return v; }
};
struct polygon_point_t { int x; music_subpixel_t y; };
struct subpixel_word_t { int v; };
union space_changing_pixel_t { subpixel_word_t sp; screen_y_t pixel; };

bool polygons_initialized;
static screen_point_t points[10];
static polygon_point_t center[POLYGON_COUNT];
static polygon_point_t velocity[POLYGON_COUNT];
static unsigned char angle[POLYGON_COUNT];
static unsigned char angle_speed[POLYGON_COUNT];

extern const char far *MUSIC_FILES[22];
void pascal near polygon_build(screen_point_t near *, screen_x_t,
    space_changing_pixel_t, pixel_t, int, unsigned char);
void near music_update_render_and_flip(void);
void pascal near track_put_both(unsigned char, unsigned char);
void pascal near tracklist_put_both(unsigned char);
void near nopoly_B_snap(void);
void near nopoly_B_free(void);
void pascal near cmt_load_unput_and_put_both_animate(int);

inline int to_sp(float pixel) { return static_cast<int>(pixel * 16); }
inline int polygon_vertex_count(int index) { return index / 4 + 3; }
inline void music_input_sense() { input_reset_sense(); }

#pragma codeseg OP_NATIVE_TEXT OP_NATIVE_01
static void near init_polygon(int i, int center_y, int speed_x) {
    center[i].x = irand() % RES_X;
    center[i].y.v = center_y;
    velocity[i].x = speed_x ? speed_x : 1;
    velocity[i].y.v = to_sp(2.0f) + ((irand() & 3) << 4);
    angle[i] = irand();
    angle_speed[i] = 4 - (irand() & 7);
    if(angle_speed[i] == 0) angle_speed[i] = 4;
}
#define polygon_init(i, y, speed_x) init_polygon(i, y, speed_x)

#include "src/op/music/polygons_update_and_render.inl"
#pragma codeseg
