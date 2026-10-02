#pragma option -zCMAIN_032_TEXT -zPmain_03 -k-

#ifdef __TURBOC__
#include <dos.h>
#endif
#include "src/main/math/randring.hpp"

extern uint16_t randring_p;

uint16_t pascal near randring2_next16_and(uint16_t mask)
{
#ifdef __TURBOC__
	_BX = randring_p;
	_AX = reinterpret_cast<uint16_t near &>(randring[_BX]);
	reinterpret_cast<uint8_t near &>(randring_p)++;
	_BX = _SP;
	_AX &= peek(_SS, (_BX + 2)); /* = */ (mask);
	return _AX;
#else
	uint16_t val = reinterpret_cast<const uint16_t &>(randring[randring_p]);
	randring_p = static_cast<uint8_t>(randring_p + 1);
	return (val & mask);
#endif
}

#pragma option -k.