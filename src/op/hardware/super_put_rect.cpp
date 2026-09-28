#include "src/shared/hardware/graphics.hpp"

// The standalone super_put renderer clips each pixel against the full
// 640-by-400 VRAM area, so it also satisfies OP's logo sprite call here.
extern "C" void TH04_PASCAL super_put_rect(int x, int y, int num)
{
    super_put(x, y, num);
}
