#pragma option -zCSHARED -3

#include "src/shared/hardware/graphics.hpp"

extern "C" unsigned __cdecl ClipYT, ClipYH;

extern "C" void TH04_PASCAL graph_pack_put_8(
	int x, int y, const void far *linepat, int len
)
{
	if((unsigned)(y - (int)ClipYT) > ClipYH) {
		return;
	}
	graph_pack_put_8_noclip(x, y, linepat, len);
}
