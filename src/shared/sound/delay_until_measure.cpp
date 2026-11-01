#pragma option -zCSHARED

#include "src/shared/hardware/frame_delay.hpp"
#include "src/shared/sound/measure.hpp"

void pascal snd_delay_until_measure(int measure, unsigned int frames_if_no_bgm)
{
#if defined(__PS3__)
	frame_delay(frames_if_no_bgm > 0 ? frames_if_no_bgm : 10);
#else
	if(!snd_bgm_active()) {
		frame_delay(frames_if_no_bgm);
		return;
	}
	while(snd_get_song_measure() < measure) {
	}
#endif
}
