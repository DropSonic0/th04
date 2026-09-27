#pragma option -zCSHARED

#include "src/shared/hardware/vram_planes.hpp"

void vram_planes_set(void)
{
	VRAM_PLANE_B = reinterpret_cast<uint8_t __seg *>(SEG_PLANE_B);
	VRAM_PLANE_R = reinterpret_cast<uint8_t __seg *>(SEG_PLANE_R);
	VRAM_PLANE_G = reinterpret_cast<uint8_t __seg *>(SEG_PLANE_G);
	VRAM_PLANE_E = reinterpret_cast<uint8_t __seg *>(SEG_PLANE_E);
}
