#include "src/shared/hardware/graphics.hpp"

// Convex polygon scan conversion for OP's music-room GRCG animation. The
// polygon builder includes a copy of point zero after the active vertices.
extern "C" void TH04_PASCAL grcg_polygon_c(
    const screen_point_t far *points, int count
)
{
    if(!points || count < 3 || count > 10) return;
    int top = points[0].y;
    int bottom = top;
    for(int i = 1; i < count; i++) {
        if(points[i].y < top) top = points[i].y;
        if(points[i].y > bottom) bottom = points[i].y;
    }
    if(top < 0) top = 0;
    if(bottom >= RES_Y) bottom = RES_Y - 1;

    for(int y = top; y <= bottom; y++) {
        int left = 32767;
        int right = -32768;
        for(int i = 0; i < count; i++) {
            int y0 = points[i].y;
            int y1 = points[i + 1].y;
            if(y0 == y1) continue;
            if(y0 > y1) {
                int t = y0; y0 = y1; y1 = t;
                int x = points[i + 1].x;
                long at = (long)x +
                    (long)(points[i].x - x) * (2L * y + 1 - 2L * y0) /
                    (2L * (y1 - y0));
                if(y >= y0 && y < y1) {
                    if(at < left) left = at;
                    if(at > right) right = at;
                }
            } else if(y >= y0 && y < y1) {
                long at = (long)points[i].x +
                    (long)(points[i + 1].x - points[i].x) *
                    (2L * y + 1 - 2L * y0) / (2L * (y1 - y0));
                if(at < left) left = at;
                if(at > right) right = at;
            }
        }
        if(left <= right) grcg_hline(left, right, y);
    }
}
