#pragma codeseg OP_NATIVE_TEXT OP_NATIVE_01

#include "src/shared/platform/types.hpp"
#include "src/shared/formats/pi.hpp"
#include "src/shared/hardware/bgimage.hpp"
#include "src/shared/hardware/graphics.hpp"
#include "src/shared/hardware/input.hpp"
#include "src/shared/math/polar.hpp"
#include "src/shared/runtime/api.hpp"
#include "src/shared/sound/api.hpp"

enum { SUBPIXEL_FACTOR = 16 };

struct Subpixel { int v; };
struct SPPoint { Subpixel x, y; };
struct pyro_t {
    bool alive;
    uint8_t age;
    SPPoint origin;
    Subpixel distance_prev;
    Subpixel distance;
    Subpixel speed;
    unsigned char angle;
    unsigned char patnum_base;
};
typedef char pyro_layout_check[(sizeof(pyro_t) == 14) ? 1 : -1];

Palette8 zunsoft_palette;
pyro_t pyros[256];
char zun00_pi[] = "zun00.pi";
char logo[] = "logo";
char zun02_bft[] = "zun02.bft";
char zun04_bft[] = "zun04.bft";
char zun01_bft[] = "zun01.bft";
char zun03_bft[] = "zun03.bft";

extern "C" void pascal near zunsoft_pyro_new(int, int, int, char);
extern "C" void pascal near zunsoft_update_and_render(void);
extern "C" void pascal near zunsoft_palette_update_and_show(int);

#include "src/op/music/zunsoft_pyro_new.inl"
#include "src/op/music/zunsoft_update_and_render.inl"
#include "src/op/music/zunsoft_palette_update_and_show.inl"
#include "src/op/music/zunsoft_animate.inl"
#pragma codeseg
