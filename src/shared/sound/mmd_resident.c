#pragma option -zCSHARED -k-

#include "src/shared/platform/x86.hpp"
#include "src/shared/sound/api.hpp"

bool16 snd_mmd_resident(void)
{
#if defined(__TURBOC__) || defined(__MSDOS__)
	_ES = 0;
	if(kaja_isr_magic_matches(*(void far * __es *)(MMD * 4), 'M', 'M', 'D')) {
		snd_interrupt_if_midi = MMD;
		snd_midi_possible = true;
		return true;
	}
#endif
	return false;
}
