#include <string.h>

void near item_splashes_init(void)
{
#ifdef __TURBOC__
	_CX = (sizeof(item_splashes) / sizeof(uint16_t));
	_ES = _DS;
	asm{ xor ax, ax; }
	reinterpret_cast<item_splash_t near *>(_DI) = item_splashes;
	asm{ rep stosw; }
#else
	memset(item_splashes, 0, sizeof(item_splashes));
#endif

	item_splash_last_id = 0;
}