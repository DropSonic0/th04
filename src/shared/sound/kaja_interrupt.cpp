#pragma option -zCSHARED
#include "src/shared/sound/api.hpp"
#include "src/shared/sound/impl.hpp"

extern "C" void ps3_sound_kaja(int func, int param);

int16_t pascal snd_kaja_interrupt(int16_t ax)
{
	if(!snd_bgm_active()) {
		return _AX;
	}

#if defined(__PS3__) || defined(CELL_SDK) || defined(__CELLOS_LV2__) || defined(SN_TARGET_PS3)
	ps3_sound_kaja((ax >> 8) & 0xFF, ax & 0xFF);
	return ax;
#else

	// TH04 should use snd_get_param() here, but doesn't....
#if (GAME == 5)
	_AX = snd_get_param(ax);
#else
	_AX = ax;
#endif

	if(snd_bgm_is_fm()) {
		geninterrupt(PMD);
	} else {
		geninterrupt(MMD);
	}
	return _AX;
#endif
}
