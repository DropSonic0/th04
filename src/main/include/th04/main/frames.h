#ifndef TH04_MAIN_FRAMES_H
#define TH04_MAIN_FRAMES_H

// MAIN-owned frame counters. The widths are part of the 16-bit DOS ABI.
extern unsigned long frames_unused;

// Reset when a new stage begins.
extern unsigned int stage_frame;
extern unsigned char stage_frame_mod2;
extern unsigned char stage_frame_mod4;
extern unsigned char stage_frame_mod8;
extern unsigned char stage_frame_mod16;

// Accumulated across stages, excluding blocking animations.
extern unsigned long total_slow_frames;
extern unsigned long total_frames;
extern unsigned int total_std_frames;

#endif
