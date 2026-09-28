#include "src/shared/hardware/graphics.hpp"

// Fill a rectangle with quarter-circle corners using the PC-98 GRCG
// primitives. The radius is limited by both half-extents.
extern "C" void TH04_PASCAL grcg_round_boxfill(
    int x1, int y1, int x2, int y2, unsigned r
)
{
    if(x1 > x2) { int t = x1; x1 = x2; x2 = t; }
    if(y1 > y2) { int t = y1; y1 = y2; y2 = t; }

    unsigned half_w = (x2 - x1) / 2;
    unsigned half_h = (y2 - y1) / 2;
    if(r > half_w) r = half_w;
    if(r > half_h) r = half_h;
    if(r == 0) {
        grcg_boxfill(x1, y1, x2, y2);
        return;
    }

    int top = y1 + r;
    int bottom = y2 - r;
    if(bottom > top + 1) {
        grcg_boxfill(x1, top + 1, x2, bottom - 1);
    }

    int left = x1 + r;
    int right = x2 - r;
    int wx = r;
    int wy = 0;
    int decision = r;
    do {
        grcg_hline(left - wx, right + wx, top - wy);
        grcg_hline(left - wx, right + wx, bottom + wy);
        decision -= (wy * 2 + 1);
        if(decision < 0) {
            grcg_hline(left - wy, right + wy, top - wx);
            grcg_hline(left - wy, right + wy, bottom + wx);
            --wx;
            decision += wx * 2;
        }
        ++wy;
    } while(wx >= wy);
}
