#include <dos.h>

#include "src/shared/hardware/graphics.hpp"

// PC-98 EGC initialization used by TH04 startup and cutscene page copies.
// These routines configure hardware state; their target bytes are not claimed.

void TH04_PASCAL egc_on(void)
{
	outportb(0x7C, 0x00); // GRCG off while changing EGC mode
	outportb(0x6A, 0x07); // Enable EGC register writes
	outportb(0x6A, 0x05); // Extended mode
	outportb(0x7C, 0x80); // GRCG active
	outportb(0x6A, 0x06); // Disable EGC register writes
}

void TH04_PASCAL egc_off(void)
{
	outport(0x4A0, 0xFFF0); // All four active planes
	outport(0x4A8, 0xFFFF); // Full bit mask
	outportb(0x6A, 0x07);
	outportb(0x6A, 0x04); // GRCG-compatible mode
	outportb(0x7C, 0x00);
	outportb(0x6A, 0x06);
}

void TH04_PASCAL egc_start(void)
{
	egc_on();
	outport(0x4A0, 0xFFF0); // Active planes
	outport(0x4A2, 0x00FF); // Read planes
	outport(0x4A8, 0xFFFF); // Full bit mask
	outport(0x4AC, 0x0000); // Address
	outport(0x4AE, 0x000F); // 16-bit transfer
	egc_off();
}
