#pragma option -zCSHARED

#include "src/shared/sound/api.hpp"
#include "src/shared/sound/impl.hpp"

// Driver probes initialize the mode flags; the initial state disables sound.
unsigned char snd_se_mode = SND_SE_OFF;
snd_bgm_mode_t snd_bgm_mode = SND_BGM_OFF;
extern "C" {
bool snd_midi_possible = false;
char snd_interrupt_if_midi;
}

// The first SE has not started until snd_se_play() or snd_se_reset() runs.
unsigned char snd_se_playing = SE_NONE;
unsigned char snd_se_frame = 0;

// Seventeen TH04 sound effects, including the final zero sentinel. Both
// semantic tables occur exactly once in the attested MAINE load image.
unsigned char snd_se_priorities[17] = {
    0, 0, 32, 16, 2, 18, 18, 64, 16, 17, 2, 18, 32, 32, 32, 32, 0
};
unsigned char snd_se_priority_frames[17] = {
    0, 0, 36, 16, 4, 16, 8, 48, 80, 17, 4, 11, 80, 80, 80, 32, 0
};

// snd_load() assembles an 8.3 filename here before handing it to the driver.
char snd_load_fn[13];

// Indexed by snd_bgm_mode; modes 0 and 1 both use the FM26 extension.
extern "C" const char *SND_LOAD_EXT[4] = {
    "m26", "m26", "m86", "mmd"
};
