#ifndef GAME
#define GAME 4
#endif

#include "src/shared/platform/ps3_compat.hpp"

#ifndef __TURBOC__

#include <stdio.h>
#include <string.h>
#include <math.h>

#if defined(__PS3__) || defined(CELL_SDK) || defined(__CELLOS_LV2__) || defined(SN_TARGET_PS3)
#include <PSGL/psgl.h>
#include <PSGL/psglu.h>
#include <sys/sys_time.h>
#include <sys/event.h>
#include <cell/audio.h>
#include <cell/sysmodule.h>
#include <cell/pad.h>
#include <sys/ppu_thread.h>
#include <sys/timer.h>
#include "src/shared/sound/pmdmini/pmdmini.h"
#endif

#include "src/shared/runtime/api.hpp"
#include "src/shared/hardware/graphics.hpp"
#include "src/shared/hardware/input.hpp"
#include "src/shared/hardware/bgimage.hpp"
#include "src/main/hardware/palette.hpp"
#include "src/shared/hardware/vram_planes.hpp"
#include "src/shared/config/score.hpp"
#include "src/shared/config/resident.hpp"
#include "src/shared/config/cfg.hpp"
#include "src/shared/formats/cdg.hpp"
#include "th04/sprites/main_pat.h"
#include "src/main/hardware/planar.hpp"
#include "src/main/boss/boss.hpp"
#include "th04/main/boss/backdrop.hpp"
#include "th04/main/boss/bosses.hpp"
#include "src/main/midboss/midboss.hpp"
#include "src/main/spark/spark.hpp"
#include "src/main/scroll/scroll.hpp"
#include "th04/main/custom.hpp"
#include "src/main/bullet/laser_t.hpp"
#include "th04/main/bullet/bullet.hpp"
#include "th04/main/enemy/enemy.hpp"
#include "th04/main/player/player.hpp"
#include "src/main/player/shot.hpp"
#include "th04/main/gather.hpp"
#include "th04/main/circle.hpp"
#include "th04/main/spark.hpp"
#include "th04/main/item/item.hpp"
#include "th04/main/hud/hud.hpp"
#include "th04/main/hud/overlay.hpp"
#include "th04/main/score.hpp"
#include "src/main/score/scoredat.hpp"
#include "th04/main/rank.hpp"
#include "th04/main/pointnum/pointnum.hpp"
#include "src/main/math/randring.hpp"
#include "th04/formats/std.hpp"
#include "src/main/tile/tile.hpp"
#include "src/main/formats/map.hpp"
#include "src/main/formats/mpn.hpp"

#define PS3_STUB_LOG(name) do { static int s_n = 0; if (s_n < 3) { printf("[TH04 STUB] %s llamada #%d\n", name, ++s_n); } } while (0)

struct yuuka6_bg_shape_t {
	SPPoint pos;
	unsigned char angle;
	SubpixelLength8 speed;
};

// Registers (C linkage)
extern "C" {
	uint32_t _EAX = 0;
	uint32_t _EBX = 0;
	uint32_t _ECX = 0;
	uint32_t _EDX = 0;

	uint16_t _AX = 0;
	uint16_t _BX = 0;
	uint16_t _CX = 0;
	uint16_t _DX = 0;
	uint16_t _SI = 0;
	uint16_t _DI = 0;
	uint16_t _ES = 0;
	uint16_t _DS = 0;
	uint16_t _CS = 0;
	uint16_t _SS = 0;

	uint8_t  _AL = 0;
	uint8_t  _AH = 0;
	uint8_t  _BL = 0;
	uint8_t  _BH = 0;
	uint8_t  _CL = 0;
	uint8_t  _CH = 0;
	uint8_t  _DL = 0;
	uint8_t  _DH = 0;

	volatile unsigned int vsync_Count1 = 0;
	volatile unsigned int vsync_Count2 = 0;

	// CellAudio PS3 State
	static int g_ps3_audio_initialized = 0;
	static uint32_t g_ps3_audio_port = 0xFFFFFFFF;
	static sys_ppu_thread_t s_audio_thread_id = 0;
	static volatile bool s_audio_running = false;
	static volatile bool s_music_playing = false;
	static sys_event_queue_t s_audio_event_queue = 0;
	static sys_ipc_key_t s_audio_queue_key = 0;

	static void audio_thread_entry(uint64_t arg) {
		printf("[TH04 PS3 Audio] Background audio streaming thread started\n");
		static short pcm_s16[256 * 2];
		static float pcm_f32[256 * 2];
		int block_cnt = 0;

		if (s_audio_event_queue) {
			sys_event_queue_drain(s_audio_event_queue);
		}

		while (s_audio_running) {
			if (s_audio_event_queue) {
				sys_event_t event;
				int q_ret = sys_event_queue_receive(s_audio_event_queue, &event, 40000);
				if (!s_audio_running) break;
				if (q_ret != CELL_OK && q_ret != ETIMEDOUT) {
					sys_timer_usleep(5333);
				}
			}
			else {
				sys_timer_usleep(5333);
			}

			if (s_music_playing && g_ps3_audio_port != 0xFFFFFFFF) {
				pmd_renderer(pcm_s16, 256);
				short s_min = 0, s_max = 0;
				for (int i = 0; i < 256 * 2; i++) {
					short v = pcm_s16[i];
					if (v < s_min) s_min = v;
					if (v > s_max) s_max = v;
					pcm_f32[i] = (float)v * (0.40f / 32768.0f);
				}
				int ret = cellAudioAddData(g_ps3_audio_port, pcm_f32, 256, 1.0f);
				block_cnt++;
				if ((block_cnt % 500) == 0) {
					printf("[TH04 PS3 Audio] Block #%d, ret=0x%08X, samples min=%d, max=%d\n",
						block_cnt, (unsigned)ret, (int)s_min, (int)s_max);
				}
			}
		}
		printf("[TH04 PS3 Audio] Background audio streaming thread stopped\n");
		sys_ppu_thread_exit(0);
	}

	void ps3_start_audio_thread(void) {
		if (!s_audio_running) {
			s_audio_running = true;
			sys_ppu_thread_create(&s_audio_thread_id, audio_thread_entry, 0, 1500, 0x8000, 0, "th04_audio");
			printf("[TH04 PS3 Audio] Created audio thread (pri=1500)\n");
		}
	}

	extern "C" void ps3_sound_load_bgm(const char *fn) {
		if (!fn || !*fn) return;
		printf("[TH04 PS3 Sound] ps3_sound_load_bgm: %s\n", fn);

		char upper_fn[128];
		char lower_fn[128];
		strncpy(upper_fn, fn, sizeof(upper_fn)-1); upper_fn[sizeof(upper_fn)-1] = 0;
		strncpy(lower_fn, fn, sizeof(lower_fn)-1); lower_fn[sizeof(lower_fn)-1] = 0;
		for (int i = 0; upper_fn[i]; i++) upper_fn[i] = (char)toupper((unsigned char)upper_fn[i]);
		for (int i = 0; lower_fn[i]; i++) lower_fn[i] = (char)tolower((unsigned char)lower_fn[i]);

		const char* names[] = { upper_fn, lower_fn, fn };
		const char* exts[] = { "M86", "m86", "M26", "m26" };
		const char* prefixes[] = {
			"/app_home/BGM",
			"/app_home",
			"BGM",
			"."
		};

		char matched_path[512] = { 0 };
		FILE* f = NULL;

		for (int n = 0; n < 3 && !f; n++) {
			for (int p = 0; p < 4 && !f; p++) {
				for (int e = 0; e < 4 && !f; e++) {
					char test_path[512];
					snprintf(test_path, sizeof(test_path), "%s/%s.%s", prefixes[p], names[n], exts[e]);
					f = fopen(test_path, "rb");
					if (f) {
						strncpy(matched_path, test_path, sizeof(matched_path)-1);
						break;
					}
				}
			}
		}

		if (f) {
			fclose(f);
			printf("[TH04 PS3 Sound] Found BGM track file: %s\n", matched_path);
			s_music_playing = false;
			pmd_stop();
			int err = pmd_play(matched_path, NULL);
			if (err == 0) {
				s_music_playing = true;
				printf("[TH04 PS3 Sound] BGM playback active: %s (duration=%ds)\n", matched_path, pmd_length_sec());
			}
			else {
				printf("[TH04 PS3 Sound] ERROR: pmd_play failed (%d) for %s\n", err, matched_path);
			}
		}
		else {
			printf("[TH04 PS3 Sound] WARNING: Could not find BGM file for %s\n", fn);
		}
	}

	extern "C" void ps3_sound_kaja(int func, int param) {
		printf("[TH04 PS3 Sound] ps3_sound_kaja(func=0x%02X, param=%d)\n", func, param);
		if (func == 0x00) { // KAJA_SONG_PLAY
			s_music_playing = true;
		}
		else if (func == 0x01 || func == 0x02) { // KAJA_SONG_STOP or FADE
			s_music_playing = false;
			pmd_stop();
		}
	}

	void ps3_audio_init(void) {
		if (g_ps3_audio_initialized) return;
		printf("[TH04 PS3 Audio] Initializing CellAudio...\n");
#if defined(__PS3__) || defined(CELL_SDK) || defined(__CELLOS_LV2__) || defined(SN_TARGET_PS3)
		int res = cellAudioInit();
		if (res == CELL_OK || res == CELL_AUDIO_ERROR_ALREADY_INIT) {
			CellAudioPortParam portParam;
			memset(&portParam, 0, sizeof(portParam));
			portParam.nChannel = CELL_AUDIO_PORT_2CH;
			portParam.nBlock = 32;
			portParam.attr = CELL_AUDIO_PORTATTR_INITLEVEL;
			portParam.level = 1.0f;
			if (cellAudioPortOpen(&portParam, &g_ps3_audio_port) == CELL_OK) {
				cellAudioPortStart(g_ps3_audio_port);
				cellAudioSetPortLevel(g_ps3_audio_port, 1.0f);
				printf("[TH04 PS3 Audio] CellAudio port %u opened & started\n", g_ps3_audio_port);
			}

			int q_err = cellAudioCreateNotifyEventQueue(&s_audio_event_queue, &s_audio_queue_key);
			if (q_err == CELL_OK) {
				cellAudioSetNotifyEventQueue(s_audio_queue_key);
				printf("[TH04 PS3 Audio] Created and registered 48kHz audio sync queue (key=0x%llX)\n",
					(unsigned long long)s_audio_queue_key);
			}
			else {
				printf("[TH04 PS3 Audio] cellAudioCreateNotifyEventQueue returned 0x%08X\n", (unsigned)q_err);
			}
		}
		pmd_init();
		pmd_setrate(48000);
		ps3_start_audio_thread();
#endif
		g_ps3_audio_initialized = 1;
	}

	void ps3_audio_finish(void) {
#if defined(__PS3__) || defined(CELL_SDK) || defined(__CELLOS_LV2__) || defined(SN_TARGET_PS3)
		if (g_ps3_audio_initialized) {
			if (s_audio_queue_key) {
				cellAudioRemoveNotifyEventQueue(s_audio_queue_key);
				s_audio_queue_key = 0;
			}
			if (s_audio_event_queue) {
				sys_event_queue_destroy(s_audio_event_queue, 0);
				s_audio_event_queue = 0;
			}
			if (g_ps3_audio_port != 0xFFFFFFFF) {
				cellAudioPortStop(g_ps3_audio_port);
				cellAudioPortClose(g_ps3_audio_port);
				g_ps3_audio_port = 0xFFFFFFFF;
			}
			cellAudioQuit();
			g_ps3_audio_initialized = 0;
		}
#endif
	}

	// PSGL State
	static int g_psgl_initialized = 0;
#if defined(__PS3__) || defined(CELL_SDK) || defined(__CELLOS_LV2__) || defined(SN_TARGET_PS3)
#define NUM_FRAME_BUFFERS 2
	static GLuint g_psgl_pbos[NUM_FRAME_BUFFERS] = { 0, 0 };
	static GLuint g_psgl_textures[NUM_FRAME_BUFFERS] = { 0, 0 };
	static uint32_t* g_psgl_mapped_ptrs[NUM_FRAME_BUFFERS] = { NULL, NULL };
	static int g_psgl_current_buf = 0;
	static uint32_t g_psgl_rgba_buffer_fallback[640 * 400];
	static PSGLdevice* g_psgl_device = NULL;
	static PSGLcontext* g_psgl_context = NULL;
#endif


	static uint16_t s_nibble_table[16][16][16][16];
	static int s_nibble_table_inited = 0;

	static void init_nibble_table(void) {
		if (s_nibble_table_inited) return;
		for (int e = 0; e < 16; e++) {
			for (int g = 0; g < 16; g++) {
				for (int r = 0; r < 16; r++) {
					for (int b = 0; b < 16; b++) {
						uint16_t v = 0;
						for (int bit = 3; bit >= 0; bit--) {
							uint8_t col = ((b >> bit) & 1) |
								(((r >> bit) & 1) << 1) |
								(((g >> bit) & 1) << 2) |
								(((e >> bit) & 1) << 3);
							v = (v << 4) | col;
						}
						s_nibble_table[e][g][r][b] = v;
					}
				}
			}
		}
		s_nibble_table_inited = 1;
	}

	void ps3_psgl_init(void) {
		init_nibble_table();
		if (g_psgl_initialized) return;
		printf("[TH04 PS3 PSGL] Initializing PSGL...\n");
#if defined(__PS3__) || defined(CELL_SDK) || defined(__CELLOS_LV2__) || defined(SN_TARGET_PS3)
		cellSysmoduleLoadModule(CELL_SYSMODULE_GCM_SYS);
		cellSysmoduleLoadModule(CELL_SYSMODULE_IO);

		PSGLinitOptions options;
		memset(&options, 0, sizeof(options));
		options.enable = PSGL_INIT_MAX_SPUS | PSGL_INIT_INITIALIZE_SPUS | PSGL_INIT_HOST_MEMORY_SIZE;
		options.maxSPUs = 1;
		options.initializeSPUs = 1;
		options.hostMemorySize = 32 * 1024 * 1024;
		psglInit(&options);

		PSGLdeviceParameters params;
		memset(&params, 0, sizeof(params));
		params.enable = PSGL_DEVICE_PARAMETERS_COLOR_FORMAT |
			PSGL_DEVICE_PARAMETERS_DEPTH_FORMAT |
			PSGL_DEVICE_PARAMETERS_MULTISAMPLING_MODE |
			PSGL_DEVICE_PARAMETERS_BUFFERING_MODE |
			PSGL_DEVICE_PARAMETERS_RESC_ADJUST_ASPECT_RATIO |
			PSGL_DEVICE_PARAMETERS_RESC_RATIO_MODE;
		params.bufferingMode = PSGL_BUFFERING_MODE_TRIPLE;
		params.colorFormat = GL_ARGB_SCE;
		params.depthFormat = GL_DEPTH_COMPONENT24;
		params.multisamplingMode = GL_MULTISAMPLING_NONE_SCE;
		params.rescRatioMode = RESC_RATIO_MODE_FULLSCREEN;

		g_psgl_device = psglCreateDeviceExtended(&params);
		if (!g_psgl_device) {
			params.depthFormat = GL_NONE;
			g_psgl_device = psglCreateDeviceExtended(&params);
		}
		g_psgl_context = psglCreateContext();
		if (g_psgl_context && g_psgl_device) {
			psglMakeCurrent(g_psgl_context, g_psgl_device);
			psglResetCurrentContext();
		}

		if (psglGetCurrentContext()) {
			glGenBuffers(NUM_FRAME_BUFFERS, g_psgl_pbos);
			glGenTextures(NUM_FRAME_BUFFERS, g_psgl_textures);

			for (int i = 0; i < NUM_FRAME_BUFFERS; i++) {
				glBindBuffer(GL_TEXTURE_REFERENCE_BUFFER_SCE, g_psgl_pbos[i]);
				glBufferData(GL_TEXTURE_REFERENCE_BUFFER_SCE, 640 * 400 * 4, NULL, GL_SYSTEM_DRAW_SCE);
				g_psgl_mapped_ptrs[i] = (uint32_t*)glMapBuffer(GL_TEXTURE_REFERENCE_BUFFER_SCE, GL_READ_WRITE);

				glBindTexture(GL_TEXTURE_2D, g_psgl_textures[i]);
				glTextureReferenceSCE(GL_TEXTURE_2D, 1, 640, 400, 1, GL_RGBA, 640 * 4, 0);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				printf("[TH04 PS3 PSGL] PBO/Texture %d ready (ptr=%p)\n", i, g_psgl_mapped_ptrs[i]);
			}
			glBindBuffer(GL_TEXTURE_REFERENCE_BUFFER_SCE, 0);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
#endif
		g_psgl_initialized = 1;
		printf("[TH04 PS3 PSGL] PSGL Initialized Successfully\n");
	}

	static inline uint8_t ps3_component_at_tone(uint8_t component, int tone) {
		int base = (component > 15) ? (component >> 4) : component;
		if (tone <= 100) {
			return (uint8_t)((base * tone * 255) / (100 * 15));
		}
		return (uint8_t)((15 - (((15 - base) * (200 - tone)) / 100)) * 255 / 15);
	}

	extern "C" void ps3_start_vsync_thread(void) {
		// VSync is driven synchronously by psglSwap() on each frame
	}

	extern uint8_t s_vram_pages[2][4][32768];
	extern int s_vram_shown_page;
	extern int s_vram_access_page;

	uint16_t s_tram_jis[25][80] = { { 0 } };
	uint16_t s_tram_atrb[25][80] = { { 0 } };
	extern uint8_t g_gaiji_patterns[256][32];
	extern bool g_gaiji_loaded;

	static void ps3_dbg_swap_state(int tone) {
		static uint32_t s_last_hash[2] = { 0, 0 };
		static int s_last_tone = -1;
		static int s_frame = 0;
		static int s_prints = 0;
		s_frame++;

		uint32_t hash[2];
		for (int p = 0; p < 2; p++) {
			uint32_t h = 2166136261u;
			for (int pl = 0; pl < 4; pl++) {
				const uint8_t* q = s_vram_pages[p][pl];
				for (int i = 0; i < 32000; i++) {
					h = (h ^ q[i]) * 16777619u;
				}
			}
			hash[p] = h;
		}

		bool changed = (hash[0] != s_last_hash[0]) || (hash[1] != s_last_hash[1]) || (tone != s_last_tone);
		if (changed && s_prints < 200) {
			printf("[TH04 State] frame=%d tone=%d shown=%d acc=%d hash0=%08X hash1=%08X\n",
				s_frame, tone, s_vram_shown_page, s_vram_access_page, hash[0], hash[1]);
			s_prints++;
		}
		s_last_hash[0] = hash[0];
		s_last_hash[1] = hash[1];
		s_last_tone = tone;
	}

	volatile int g_ps3_hide_tile_strip = 0;

	static void ps3_mask_tile_strip(uint32_t* frame, uint32_t color) {
		for (int y = 0; y < 400; y++) {
			uint32_t* row = frame + y * 640 + 576;
			for (int x = 0; x < 64; x++) {
				row[x] = color;
			}
		}
	}

	void ps3_psgl_swap(void) {
#if defined(__PS3__) || defined(CELL_SDK) || defined(__CELLOS_LV2__) || defined(SN_TARGET_PS3)
		if (!g_psgl_initialized) return;

		uint64_t t_start = sys_time_get_system_time();

		if (psglGetCurrentContext() != g_psgl_context) {
			if (g_psgl_context && g_psgl_device) {
				psglMakeCurrent(g_psgl_context, g_psgl_device);
			}
		}
		if (!psglGetCurrentContext()) return;

		PSGLdevice* dev = psglGetCurrentDevice();
		if (dev) {
			GLuint w = 0, h = 0;
			psglGetDeviceDimensions(dev, &w, &h);
			if (w > 0 && h > 0) {
				glViewport(0, 0, w, h);
			}
		}

		uint32_t lut[16];
		int tone = (int)PaletteTone;
		if (tone < 0) tone = 0;
		else if (tone > 200) tone = 200;
		ps3_dbg_swap_state(tone);

		static int s_swap_dbg = 0;
		for (int c = 0; c < 16; c++) {
			uint8_t r = ps3_component_at_tone(Palettes[c].v[0], tone);
			uint8_t g = ps3_component_at_tone(Palettes[c].v[1], tone);
			uint8_t b = ps3_component_at_tone(Palettes[c].v[2], tone);
			lut[c] = ((uint32_t)r << 24) | ((uint32_t)g << 16) | ((uint32_t)b << 8) | 0xFFu;
		}

		if (++s_swap_dbg % 60 == 0) {
			int nz0 = 0, nz1 = 0;
			for (int i = 0; i < 32768; i++) {
				if (s_vram_pages[0][0][i]) nz0++;
				if (s_vram_pages[1][0][i]) nz1++;
			}
			printf("[TH04 Debug Swap] Tone=%u, shown=%d, acc=%d, nz0=%d, nz1=%d, pal2=(%d,%d,%d), pal4=(%d,%d,%d), pal5=(%d,%d,%d), lut4=0x%08X\n",
				PaletteTone, s_vram_shown_page, s_vram_access_page, nz0, nz1,
				Palettes[2].v[0], Palettes[2].v[1], Palettes[2].v[2],
				Palettes[4].v[0], Palettes[4].v[1], Palettes[4].v[2],
				Palettes[5].v[0], Palettes[5].v[1], Palettes[5].v[2],
				lut[4]);
		}

		const uint8_t* p_b = s_vram_pages[s_vram_shown_page][0];
		const uint8_t* p_r = s_vram_pages[s_vram_shown_page][1];
		const uint8_t* p_g = s_vram_pages[s_vram_shown_page][2];
		const uint8_t* p_e = s_vram_pages[s_vram_shown_page][3];

		int buf_idx = g_psgl_current_buf;
		uint32_t* dst = g_psgl_mapped_ptrs[buf_idx] ? g_psgl_mapped_ptrs[buf_idx] : g_psgl_rgba_buffer_fallback;
		uint32_t* p_frame = dst;
		init_nibble_table();
		for (int y = 0; y < 400; y++) {
			int vram_y = (y + (int)s_graph_scroll_line) % 400;
			int row_off = vram_y * 80;
			for (int byte_x = 0; byte_x < 80; byte_x++) {
				int offset = row_off + byte_x;
				uint8_t b_byte = p_b[offset];
				uint8_t r_byte = p_r[offset];
				uint8_t g_byte = p_g[offset];
				uint8_t e_byte = p_e[offset];

				uint16_t hi = s_nibble_table[e_byte >> 4][g_byte >> 4][r_byte >> 4][b_byte >> 4];
				dst[0] = lut[(hi >> 12) & 0xF];
				dst[1] = lut[(hi >> 8) & 0xF];
				dst[2] = lut[(hi >> 4) & 0xF];
				dst[3] = lut[hi & 0xF];

				uint16_t lo = s_nibble_table[e_byte & 0xF][g_byte & 0xF][r_byte & 0xF][b_byte & 0xF];
				dst[4] = lut[(lo >> 12) & 0xF];
				dst[5] = lut[(lo >> 8) & 0xF];
				dst[6] = lut[(lo >> 4) & 0xF];
				dst[7] = lut[lo & 0xF];
				dst += 8;
			}
		}
		if (g_ps3_hide_tile_strip) ps3_mask_tile_strip(p_frame, lut[0]);
		// Overlay PC-98 Text RAM (HUD, text, and gaiji glyphs)
		for (int ty = 0; ty < 25; ty++) {
			for (int tx = 0; tx < 80; tx++) {
				uint16_t jis = s_tram_jis[ty][tx];
				uint16_t atrb = s_tram_atrb[ty][tx];
				if (jis == 0 || jis == ' ') continue;

				int px = tx * 8;
				int py = ty * 16;

				// Check for 16x16 Gaiji (cell tx and tx+1)
				if (((jis & 0x8000) == 0) && (jis >= 0x0056) && tx < 79 && (s_tram_jis[ty][tx + 1] == (jis | 0x8000))) {
					uint8_t low = (uint8_t)jis;
					uint8_t high = (uint8_t)(jis >> 8);
					uint8_t c = (high & 0x7F) | ((low - 0x56) << 7);

					if (c != 0x02 && g_gaiji_loaded) {
						const uint8_t* glyph = g_gaiji_patterns[c];
						bool b_on = (atrb & 0x20) != 0;
						bool r_on = (atrb & 0x40) != 0;
						bool g_on = (atrb & 0x80) != 0;
						uint8_t cr = r_on ? 0xFF : 0;
						uint8_t cg = g_on ? 0xFF : 0;
						uint8_t cb = b_on ? 0xFF : 0;
						if (!r_on && !g_on && !b_on) { cr = cg = cb = 0; }
						uint32_t fg = ((uint32_t)cr << 24) | ((uint32_t)cg << 16) | ((uint32_t)cb << 8) | 0xFFu;

						for (int r = 0; r < 16; r++) {
							uint8_t b0 = glyph[r * 2 + 0];
							uint8_t b1 = glyph[r * 2 + 1];
							uint32_t* line = &p_frame[(py + r) * 640 + px];
							for (int bit = 0; bit < 8; bit++) {
								if (b0 & (0x80 >> bit)) line[bit] = fg;
							}
							for (int bit = 0; bit < 8; bit++) {
								if (b1 & (0x80 >> bit)) line[8 + bit] = fg;
							}
						}
					}
					tx++;
					continue;
				}

				// ASCII Digits ('0'..'9') mapped to Gaiji 0xA0..0xA9
				if (jis >= '0' && jis <= '9' && g_gaiji_loaded) {
					uint8_t c = 0xA0 + (jis - '0');
					const uint8_t* glyph = g_gaiji_patterns[c];
					bool b_on = (atrb & 0x20) != 0;
					bool r_on = (atrb & 0x40) != 0;
					bool g_on = (atrb & 0x80) != 0;
					uint8_t cr = r_on ? 0xFF : 0;
					uint8_t cg = g_on ? 0xFF : 0;
					uint8_t cb = b_on ? 0xFF : 0;
					if (!r_on && !g_on && !b_on) { cr = cg = cb = 0; }
					uint32_t fg = ((uint32_t)cr << 24) | ((uint32_t)cg << 16) | ((uint32_t)cb << 8) | 0xFFu;

					for (int r = 0; r < 16; r++) {
						uint8_t b0 = glyph[r * 2 + 0];
						uint32_t* line = &p_frame[(py + r) * 640 + px];
						for (int bit = 0; bit < 8; bit++) {
							if (b0 & (0x80 >> bit)) line[bit] = fg;
						}
					}
				}
			}
		}
		uint64_t t_conv = sys_time_get_system_time();

		if (g_psgl_mapped_ptrs[buf_idx]) {
			glBindBuffer(GL_TEXTURE_REFERENCE_BUFFER_SCE, g_psgl_pbos[buf_idx]);
			glBindTexture(GL_TEXTURE_2D, g_psgl_textures[buf_idx]);
		}
		else {
			glBindTexture(GL_TEXTURE_2D, g_psgl_textures[0]);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 640, 400, GL_RGBA, GL_UNSIGNED_BYTE, dst);
		}
		uint64_t t_tex = sys_time_get_system_time();

		glDisable(GL_DEPTH_TEST);
		glEnable(GL_TEXTURE_2D);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrthof(0.0f, 640.0f, 400.0f, 0.0f, -1.0f, 1.0f);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		static const GLfloat vtx[] = {
			0.0f, 0.0f,
			640.0f, 0.0f,
			0.0f, 400.0f,
			640.0f, 400.0f
		};
		static const GLfloat tex[] = {
			0.0f, 0.0f,
			1.0f, 0.0f,
			0.0f, 1.0f,
			1.0f, 1.0f
		};

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glVertexPointer(2, GL_FLOAT, 0, vtx);
		glTexCoordPointer(2, GL_FLOAT, 0, tex);

		glDisable(GL_CULL_FACE);
		glDisable(GL_BLEND);
		glDisable(GL_SCISSOR_TEST);
		glDisable(GL_ALPHA_TEST);
		glDisable(GL_LIGHTING);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glDepthMask(GL_FALSE);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);

		glBindTexture(GL_TEXTURE_2D, 0);
		if (g_psgl_mapped_ptrs[buf_idx]) {
			glBindBuffer(GL_TEXTURE_REFERENCE_BUFFER_SCE, 0);
		}
		uint64_t t_draw = sys_time_get_system_time();

		psglSwap();
		uint64_t t_swap = sys_time_get_system_time();

		g_psgl_current_buf = (g_psgl_current_buf + 1) % NUM_FRAME_BUFFERS;
		vsync_Count1++;
		vsync_Count2++;

		static int s_swap_cnt = 0;
		s_swap_cnt++;
		if ((s_swap_cnt % 10) == 0 || (t_swap - t_start) > 20000) {
			printf("[TH04 Perf Swap #%d] Total: %.2f ms (Conv: %.2f ms, Tex: %.2f ms, Draw: %.2f ms, Swap: %.2f ms)\n",
				s_swap_cnt,
				(float)(t_swap - t_start) / 1000.0f,
				(float)(t_conv - t_start) / 1000.0f,
				(float)(t_tex - t_conv) / 1000.0f,
				(float)(t_draw - t_tex) / 1000.0f,
				(float)(t_swap - t_draw) / 1000.0f);
		}
#endif
	}

	// C-linkage API functions
	int TH04_PASCAL pf_open_member(const char far *path);
	long TH04_PASCAL pf_size_member(void);
	unsigned TH04_PASCAL pf_read_member(void far *out, unsigned count);
	void TH04_PASCAL pf_seek_member(long offset, int origin);
	void TH04_PASCAL pf_close_member(void);

	FILE* g_dos_handles[MAX_DOS_HANDLES] = { 0 };
	static FILE* g_current_file = NULL;
	int g_pf_active_handle = 0;
	int g_in_pf_read = 0;

	static resident_t s_resident = {
		"HUMAConfig", // id
		3,            // rem_lives
		3,            // credit_lives
		3,            // rem_bombs
		3,            // credit_bombs
		1,            // rank
		1,            // bgm_mode (1 = FM)
		0,            // stage
		'0',          // playchar_ascii
		'0',          // stage_ascii
		12345,        // rand
		1,            // se_mode (1 = FM)
		0,            // shottype
	};

	int TH04_PASCAL file_ropen(const char TH04_PTR *filename) {
		if (g_current_file) {
			fclose(g_current_file);
			g_current_file = NULL;
		}
		if (g_pf_active_handle) {
			pf_close_member();
			g_pf_active_handle = 0;
		}
		if (!filename || !*filename) {
			printf("[TH04 PS3 I/O] file_ropen failed: empty filename\n");
			return 0;
		}
		printf("[TH04 PS3 I/O] Opening file: %s\n", filename);
		g_current_file = fopen(filename, "rb");
		if (!g_current_file) {
			char path[512];
			snprintf(path, sizeof(path), "/app_home/%s", filename);
			printf("[TH04 PS3 I/O] Relative open failed, trying fallback: %s\n", path);
			g_current_file = fopen(path, "rb");
		}
		if (strstr(filename, "MIKO.CFG")) {
			long sz = 0;
			if (g_current_file) {
				fseek(g_current_file, 0, SEEK_END);
				sz = ftell(g_current_file);
				if (sz > 0) {
					fseek(g_current_file, 0, SEEK_SET);
				}
				else {
					fclose(g_current_file);
					g_current_file = NULL;
				}
			}
			if (!g_current_file) {
				printf("[TH04 PS3 I/O] MIKO.CFG missing or empty, writing default configuration...\n");
				FILE *fcreate = fopen(filename, "wb");
				if (!fcreate) {
					char path[512];
					snprintf(path, sizeof(path), "/app_home/%s", filename);
					fcreate = fopen(path, "wb");
				}
				if (fcreate) {
					cfg_t default_cfg = {};
					default_cfg.opts.rank = 1;
					default_cfg.opts.lives = 3;
					default_cfg.opts.bombs = 2;
					default_cfg.opts.bgm_mode = 0;
					default_cfg.opts.se_mode = 0;
					default_cfg.opts.turbo_mode = 0;
					default_cfg.resident = (resident_t __seg*)&s_resident;
					default_cfg.debug = 0;
					default_cfg.opts_sum = (signed char)(1 + 3 + 2 + 0 + 0 + 0);
					fwrite(&default_cfg, 1, sizeof(default_cfg), fcreate);
					fclose(fcreate);

					g_current_file = fopen(filename, "rb");
					if (!g_current_file) {
						char path[512];
						snprintf(path, sizeof(path), "/app_home/%s", filename);
						g_current_file = fopen(path, "rb");
					}
				}
			}
		}
		else if (!g_current_file && strstr(filename, "GENSOU.SCR")) {
			printf("[TH04 PS3 I/O] Save file missing (%s), creating default zeroed file...\n", filename);
			FILE *fcreate = fopen(filename, "wb");
			if (!fcreate) {
				char path[512];
				snprintf(path, sizeof(path), "/app_home/%s", filename);
				fcreate = fopen(path, "wb");
			}
			if (fcreate) {
				char dummy[512] = { 0 };
				fwrite(dummy, 1, sizeof(dummy), fcreate);
				fclose(fcreate);
				g_current_file = fopen(filename, "rb");
				if (!g_current_file) {
					char path[512];
					snprintf(path, sizeof(path), "/app_home/%s", filename);
					g_current_file = fopen(path, "rb");
				}
			}
		}
		if (g_current_file) {
			printf("[TH04 PS3 I/O] Successfully opened file on disk: %s\n", filename);
			return 1;
		}

		// Disk open failed, check pf_open_member (archive GENSOU.DAT member)
		int pf_handle = pf_open_member(filename);
		if (pf_handle) {
			g_pf_active_handle = PF_VIRTUAL_HANDLE;
			printf("[TH04 PS3 I/O] Successfully loaded archive member from GENSOU.DAT: %s (pf handle=0x%X)\n", filename, g_pf_active_handle);
			return 1;
		}

		printf("[TH04 PS3 I/O] ERROR: Could not open file or archive member: %s\n", filename);
		return 0;
	}

	int TH04_PASCAL file_read(void far *buf, unsigned wsize) {
		if (g_pf_active_handle) {
			unsigned read_bytes = pf_read_member(buf, wsize);
			return (int)read_bytes;
		}
		if (!g_current_file || !buf) {
			return 0;
		}
		int read_bytes = (int)fread(buf, 1, wsize, g_current_file);
		return read_bytes;
	}

	long TH04_PASCAL file_size(void) {
		if (g_pf_active_handle) {
			long sz = pf_size_member();
			printf("[TH04 PS3 I/O] pf file_size: %ld bytes\n", sz);
			return sz;
		}
		if (!g_current_file) return 0;
		long cur = ftell(g_current_file);
		fseek(g_current_file, 0, SEEK_END);
		long sz = ftell(g_current_file);
		fseek(g_current_file, cur, SEEK_SET);
		printf("[TH04 PS3 I/O] file_size: %ld bytes\n", sz);
		return sz;
	}

	int TH04_PASCAL file_create(const char TH04_PTR *filename) {
		if (g_current_file) {
			fclose(g_current_file);
			g_current_file = NULL;
		}
		if (!filename || !*filename) return 0;
		printf("[TH04 PS3 I/O] Creating file: %s\n", filename);
		g_current_file = fopen(filename, "wb+");
		if (!g_current_file) {
			char path[512];
			snprintf(path, sizeof(path), "/app_home/%s", filename);
			printf("[TH04 PS3 I/O] Fallback creating file: %s\n", path);
			g_current_file = fopen(path, "wb+");
		}
		return (g_current_file != NULL) ? 1 : 0;
	}

	int TH04_PASCAL file_append(const char TH04_PTR *filename) {
		if (g_current_file) {
			fclose(g_current_file);
			g_current_file = NULL;
		}
		if (!filename || !*filename) return 0;
		printf("[TH04 PS3 I/O] Appending file: %s\n", filename);
		g_current_file = fopen(filename, "ab+");
		if (!g_current_file) {
			char path[512];
			snprintf(path, sizeof(path), "/app_home/%s", filename);
			g_current_file = fopen(path, "ab+");
		}
		return (g_current_file != NULL) ? 1 : 0;
	}

	int TH04_PASCAL file_write(const void far *buf, unsigned wsize) {
		if (!g_current_file || !buf) return 0;
		int written = (int)fwrite(buf, 1, wsize, g_current_file);
		printf("[TH04 PS3 I/O] file_write requested %u bytes, wrote %d bytes\n", wsize, written);
		return written;
	}

	void TH04_PASCAL file_seek(long pos, int dir) {
		if (g_pf_active_handle) {
			pf_seek_member(pos, dir);
			return;
		}
		if (!g_current_file) return;
		int origin = SEEK_SET;
		if (dir == 1) origin = SEEK_CUR;
		else if (dir == 2) origin = SEEK_END;
		fseek(g_current_file, pos, origin);
	}

	void TH04_PASCAL file_close(void) {
		if (g_pf_active_handle) {
			printf("[TH04 PS3 I/O] Closing pf archive member (handle=%d)\n", g_pf_active_handle);
			pf_close_member();
			g_pf_active_handle = 0;
		}
		if (g_current_file) {
			printf("[TH04 PS3 I/O] Closing file\n");
			fclose(g_current_file);
			g_current_file = NULL;
		}
	}

	int TH04_PASCAL file_exist(const char TH04_PTR *filename) {
		if (!filename || !*filename) return 0;
		printf("[TH04 PS3 I/O] Checking file_exist: %s\n", filename);
		FILE *f = fopen(filename, "rb");
		if (!f) {
			char path[512];
			snprintf(path, sizeof(path), "/app_home/%s", filename);
			f = fopen(path, "rb");
		}
		if (f) {
			printf("[TH04 PS3 I/O] file_exist: TRUE on disk (%s)\n", filename);
			fclose(f);
			return 1;
		}

		int pf_handle = pf_open_member(filename);
		if (pf_handle) {
			printf("[TH04 PS3 I/O] file_exist: TRUE in GENSOU.DAT (%s)\n", filename);
			pf_close_member();
			return 1;
		}

		printf("[TH04 PS3 I/O] file_exist: FALSE (%s)\n", filename);
		return 0;
	}

	int TH04_PASCAL file_delete(const char TH04_PTR *filename) {
		if (!filename || !*filename) return 0;
		printf("[TH04 PS3 I/O] Deleting file: %s\n", filename);
		if (remove(filename) == 0) return 1;
		char path[512];
		snprintf(path, sizeof(path), "/app_home/%s", filename);
		return (remove(path) == 0) ? 1 : 0;
	}

	unsigned TH04_PASCAL ems_allocate(unsigned long len) { return 0; }
	int TH04_PASCAL ems_exist(void) { return 0; }
	int TH04_PASCAL ems_read(unsigned handle, long offset, void far *mem, long size) { return 0; }
	int TH04_PASCAL ems_setname(unsigned handle, const char TH04_PTR * name) { return 0; }
	unsigned long TH04_PASCAL ems_space(void) { return 0; }
	int TH04_PASCAL ems_write(unsigned handle, long offset, const void far *mem, long size) { return 0; }

	int TH04_PASCAL iatan2(int y, int x) {
		if (x == 0 && y == 0) return 0;
		double rad = atan2((double)y, (double)x);
		double angle = rad * (128.0 / 3.14159265358979323846);
		int a = (int)floor(angle + 0.5);
		return a & 0xFF;
	}
	int TH04_PASCAL isqrt(long x) {
		if (x <= 0) return 0;
		return (int)sqrt((double)x);
	}
	int TH04_PASCAL ihypot(int x, int y) {
		return (int)sqrt((double)x * (double)x + (double)y * (double)y);
	}

	extern "C" void ps3_start_vsync_thread(void);
	void TH04_PASCAL vsync_start(void) {
		ps3_psgl_init();
		ps3_audio_init();
		ps3_start_vsync_thread();
	}
	void TH04_PASCAL vsync_end(void) {
		ps3_psgl_swap();
	}
	void TH04_PASCAL graph_start(void) {
		printf("[TH04 PS3 Graphics] graph_start() called\n");
		ps3_psgl_init();
		ps3_audio_init();
		ps3_start_vsync_thread();
	}

	int TH04_PASCAL js_start() {
		printf("[TH04 PS3 JS] js_start() initialized\n");
		return 0;
	}
	void TH04_PASCAL js_end(void) {}
	int TH04_PASCAL js_sense(void) { return 0; }

	void TH04_PASCAL cdg_free(int slot) {
		if (slot < 0 || slot >= 64) return;
		if (cdg_slots[slot].seg[0]) {
			hmem_free(cdg_slots[slot].seg[0]);
			cdg_slots[slot].seg[0] = 0;
		}
		if (cdg_slots[slot].seg[1]) {
			hmem_free(cdg_slots[slot].seg[1]);
			cdg_slots[slot].seg[1] = 0;
		}
		memset(&cdg_slots[slot], 0, sizeof(cdg_slot_t));
	}
	void TH04_PASCAL cdg_free_all(void) {
		for (int i = 0; i < 64; i++) cdg_free(i);
	}
	void TH04_PASCAL cdg_load_single(int slot, const char *fn, int image) {
		if (slot < 0 || slot >= 64) return;
		cdg_free(slot);
		if (!file_ropen(fn)) return;
		uint8_t hdr[16];
		if (file_read(hdr, 16) != 16) { file_close(); return; }
		cdg_slot_t &s = cdg_slots[slot];
		s.bitplane_size = (uint16_t)(hdr[0] | (hdr[1] << 8));
		s.pixel_w = (int16_t)(hdr[2] | (hdr[3] << 8));
		s.pixel_h = (int16_t)(hdr[4] | (hdr[5] << 8));
		s.offset_at_bottom_left = (int16_t)(hdr[6] | (hdr[7] << 8));
		s.vram_dword_w = (uint16_t)(hdr[8] | (hdr[9] << 8));
		s.image_count = hdr[10];
		s.plane_layout = hdr[11];
		s.seg[0] = 0; s.seg[1] = 0;
		int plane_size = s.bitplane_size;
		int img_bytes = (s.plane_layout == 0) ? (4 * plane_size) : ((s.plane_layout == 2) ? plane_size : (5 * plane_size));
		if (image > 0) file_seek(image * img_bytes, 1);
		if (s.plane_layout != 0) {
			void __seg *seg_a = hmem_allocbyte(plane_size);
			s.seg[0] = (uint8_t __seg *)seg_a;
			file_read((void*)seg_a, plane_size);
		}
		if (s.plane_layout != 2) {
			void __seg *seg_c = hmem_allocbyte(4 * plane_size);
			s.seg[1] = (uint8_t __seg *)seg_c;
			file_read((void*)seg_c, 4 * plane_size);
		}
		file_close();
	}
	void TH04_PASCAL cdg_load_single_noalpha(int slot, const char *fn, int image) {
		if (slot < 0 || slot >= 64) return;
		cdg_free(slot);
		if (!file_ropen(fn)) return;
		uint8_t hdr[16];
		if (file_read(hdr, 16) != 16) { file_close(); return; }
		cdg_slot_t &s = cdg_slots[slot];
		s.bitplane_size = (uint16_t)(hdr[0] | (hdr[1] << 8));
		s.pixel_w = (int16_t)(hdr[2] | (hdr[3] << 8));
		s.pixel_h = (int16_t)(hdr[4] | (hdr[5] << 8));
		s.offset_at_bottom_left = (int16_t)(hdr[6] | (hdr[7] << 8));
		s.vram_dword_w = (uint16_t)(hdr[8] | (hdr[9] << 8));
		s.image_count = hdr[10];
		s.plane_layout = hdr[11];
		s.seg[0] = 0; s.seg[1] = 0;
		int plane_size = s.bitplane_size;
		int img_bytes = (s.plane_layout == 0) ? (4 * plane_size) : ((s.plane_layout == 2) ? plane_size : (5 * plane_size));
		if (image > 0) file_seek(image * img_bytes, 1);
		if (s.plane_layout != 0) file_seek(plane_size, 1);
		if (s.plane_layout != 2) {
			void __seg *seg_c = hmem_allocbyte(4 * plane_size);
			s.seg[1] = (uint8_t __seg *)seg_c;
			file_read((void*)seg_c, 4 * plane_size);
		}
		file_close();
	}
	void TH04_PASCAL cdg_load_all(int slot_first, const char *fn) {
		if (!file_ropen(fn)) return;
		uint8_t hdr[16];
		if (file_read(hdr, 16) != 16) { file_close(); return; }
		uint16_t bitplane_size = (uint16_t)(hdr[0] | (hdr[1] << 8));
		int16_t pixel_w = (int16_t)(hdr[2] | (hdr[3] << 8));
		int16_t pixel_h = (int16_t)(hdr[4] | (hdr[5] << 8));
		int16_t offset_at_bottom_left = (int16_t)(hdr[6] | (hdr[7] << 8));
		uint16_t vram_dword_w = (uint16_t)(hdr[8] | (hdr[9] << 8));
		uint8_t image_count = hdr[10];
		int8_t plane_layout = hdr[11];
		for (int i = 0; i < (int)image_count; i++) {
			int slot = slot_first + i;
			if (slot >= 64) break;
			cdg_free(slot);
			cdg_slot_t &s = cdg_slots[slot];
			s.bitplane_size = bitplane_size;
			s.pixel_w = pixel_w;
			s.pixel_h = pixel_h;
			s.offset_at_bottom_left = offset_at_bottom_left;
			s.vram_dword_w = vram_dword_w;
			s.image_count = image_count;
			s.plane_layout = plane_layout;
			s.seg[0] = 0; s.seg[1] = 0;
			if (plane_layout != 0) {
				void __seg *seg_a = hmem_allocbyte(bitplane_size);
				s.seg[0] = (uint8_t __seg *)seg_a;
				file_read((void*)seg_a, bitplane_size);
			}
			if (plane_layout != 2) {
				void __seg *seg_c = hmem_allocbyte(4 * bitplane_size);
				s.seg[1] = (uint8_t __seg *)seg_c;
				file_read((void*)seg_c, 4 * bitplane_size);
			}
		}
		file_close();
	}
	void TH04_PASCAL cdg_load_all_noalpha(int slot_first, const char *fn) {
		if (!file_ropen(fn)) return;
		uint8_t hdr[16];
		if (file_read(hdr, 16) != 16) { file_close(); return; }
		uint16_t bitplane_size = (uint16_t)(hdr[0] | (hdr[1] << 8));
		int16_t pixel_w = (int16_t)(hdr[2] | (hdr[3] << 8));
		int16_t pixel_h = (int16_t)(hdr[4] | (hdr[5] << 8));
		int16_t offset_at_bottom_left = (int16_t)(hdr[6] | (hdr[7] << 8));
		uint16_t vram_dword_w = (uint16_t)(hdr[8] | (hdr[9] << 8));
		uint8_t image_count = hdr[10];
		int8_t plane_layout = hdr[11];
		for (int i = 0; i < (int)image_count; i++) {
			int slot = slot_first + i;
			if (slot >= 64) break;
			cdg_free(slot);
			cdg_slot_t &s = cdg_slots[slot];
			s.bitplane_size = bitplane_size;
			s.pixel_w = pixel_w;
			s.pixel_h = pixel_h;
			s.offset_at_bottom_left = offset_at_bottom_left;
			s.vram_dword_w = vram_dword_w;
			s.image_count = image_count;
			s.plane_layout = plane_layout;
			s.seg[0] = 0; s.seg[1] = 0;
			if (plane_layout != 0) file_seek(bitplane_size, 1);
			if (plane_layout != 2) {
				void __seg *seg_c = hmem_allocbyte(4 * bitplane_size);
				s.seg[1] = (uint8_t __seg *)seg_c;
				file_read((void*)seg_c, 4 * bitplane_size);
			}
		}
		file_close();
	}
	static int s_grcg_mode = 0;
	static vc2 s_grcg_color = 0;
	static bool s_grcg_enabled = false;

	void TH04_PASCAL cdg_put_8(screen_x_t left, vram_y_t top, int slot) {
		if (slot < 0 || slot >= 64) return;
		cdg_slot_t &s = cdg_slots[slot];
		if (!s.seg[1] && !s.seg[0]) return;
		int byte_w = s.vram_dword_w * 4;
		int h = s.pixel_h;
		int plane_size = s.bitplane_size;
		int left_b = left / 8;
		const uint8_t *alpha = s.seg[0] ? (const uint8_t *)s.seg[0] : NULL;
		const uint8_t *colors = s.seg[1] ? (const uint8_t *)s.seg[1] : NULL;
		const uint8_t *col_b = colors ? colors : NULL;
		const uint8_t *col_r = colors ? colors + plane_size : NULL;
		const uint8_t *col_g = colors ? colors + 2 * plane_size : NULL;
		const uint8_t *col_e = colors ? colors + 3 * plane_size : NULL;

		bool use_grcg = s_grcg_enabled || (!colors);
		uint8_t grcg_b = (s_grcg_color & 1) ? 0xFF : 0x00;
		uint8_t grcg_r = (s_grcg_color & 2) ? 0xFF : 0x00;
		uint8_t grcg_g = (s_grcg_color & 4) ? 0xFF : 0x00;
		uint8_t grcg_e = (s_grcg_color & 8) ? 0xFF : 0x00;

		for (int r = 0; r < h; r++) {
			int y = top + (h - 1 - r);
			if (y < 0 || y >= 400) continue;
			int dst_row = y * 80 + left_b;
			int src_row = r * byte_w;
			for (int x = 0; x < byte_w; x++) {
				int d = dst_row + x;
				int s_idx = src_row + x;
				if (d < 0 || d >= 32000 || s_idx >= plane_size) continue;
				uint8_t mask = alpha ? alpha[s_idx] : 0xFF;
				if (!mask) continue;
				uint8_t inv_mask = (uint8_t)~mask;

				uint8_t src_b = use_grcg ? grcg_b : (col_b ? col_b[s_idx] : 0);
				uint8_t src_r = use_grcg ? grcg_r : (col_r ? col_r[s_idx] : 0);
				uint8_t src_g = use_grcg ? grcg_g : (col_g ? col_g[s_idx] : 0);
				uint8_t src_e = use_grcg ? grcg_e : (col_e ? col_e[s_idx] : 0);

				if (VRAM_PLANE_B) VRAM_PLANE_B[d] = (VRAM_PLANE_B[d] & inv_mask) | (src_b & mask);
				if (VRAM_PLANE_R) VRAM_PLANE_R[d] = (VRAM_PLANE_R[d] & inv_mask) | (src_r & mask);
				if (VRAM_PLANE_G) VRAM_PLANE_G[d] = (VRAM_PLANE_G[d] & inv_mask) | (src_g & mask);
				if (VRAM_PLANE_E) VRAM_PLANE_E[d] = (VRAM_PLANE_E[d] & inv_mask) | (src_e & mask);
			}
		}
	}
	void TH04_PASCAL cdg_put_noalpha_8(screen_x_t left, vram_y_t top, int slot) {
		if (slot < 0 || slot >= 64) return;
		cdg_slot_t &s = cdg_slots[slot];
		if (!s.seg[1]) return;
		int byte_w = s.vram_dword_w * 4;
		int h = s.pixel_h;
		int plane_size = s.bitplane_size;
		int left_b = left / 8;
		const uint8_t *colors = (const uint8_t *)s.seg[1];
		const uint8_t *col_b = colors;
		const uint8_t *col_r = colors + plane_size;
		const uint8_t *col_g = colors + 2 * plane_size;
		const uint8_t *col_e = colors + 3 * plane_size;
		for (int r = 0; r < h; r++) {
			int y = top + (h - 1 - r);
			if (y < 0 || y >= 400) continue;
			int dst_row = y * 80 + left_b;
			int src_row = r * byte_w;
			for (int x = 0; x < byte_w; x++) {
				int d = dst_row + x;
				int s_idx = src_row + x;
				if (d < 0 || d >= 32000 || s_idx >= plane_size) continue;
				if (VRAM_PLANE_B) VRAM_PLANE_B[d] = col_b[s_idx];
				if (VRAM_PLANE_R) VRAM_PLANE_R[d] = col_r[s_idx];
				if (VRAM_PLANE_G) VRAM_PLANE_G[d] = col_g[s_idx];
				if (VRAM_PLANE_E) VRAM_PLANE_E[d] = col_e[s_idx];
			}
		}
	}
	void TH04_PASCAL cdg_put_plane(screen_x_t left, vram_y_t top, int slot, int plane) {
		if (slot < 0 || slot >= 64) return;
		cdg_slot_t &s = cdg_slots[slot];
		if (!s.seg[1]) return;
		int byte_w = s.vram_dword_w * 4;
		int h = s.pixel_h;
		int plane_size = s.bitplane_size;
		int left_b = left / 8;
		const uint8_t *colors = (const uint8_t *)s.seg[1];
		const uint8_t *src_p = colors + plane * plane_size;
		uint8_t *dst_p = (plane == 0) ? VRAM_PLANE_B : (plane == 1) ? VRAM_PLANE_R : (plane == 2) ? VRAM_PLANE_G : VRAM_PLANE_E;
		if (!dst_p) return;
		for (int r = 0; r < h; r++) {
			int y = top + (h - 1 - r);
			if (y < 0 || y >= 400) continue;
			int dst_row = y * 80 + left_b;
			int src_row = r * byte_w;
			for (int x = 0; x < byte_w; x++) {
				int d = dst_row + x;
				int s_idx = src_row + x;
				if (d < 0 || d >= 32000 || s_idx >= plane_size) continue;
				dst_p[d] = src_p[s_idx];
			}
		}
	}

	static void ps3_grcg_pset(int x, int y) {
		if (x < 0 || x >= 640 || y < 0 || y >= 400) return;
		int off = y * 80 + (x >> 3);
		uint8_t mask = (uint8_t)(0x80 >> (x & 7));
		if (s_grcg_color & 1) VRAM_PLANE_B[off] |= mask; else VRAM_PLANE_B[off] &= ~mask;
		if (s_grcg_color & 2) VRAM_PLANE_R[off] |= mask; else VRAM_PLANE_R[off] &= ~mask;
		if (s_grcg_color & 4) VRAM_PLANE_G[off] |= mask; else VRAM_PLANE_G[off] &= ~mask;
		if (s_grcg_color & 8) VRAM_PLANE_E[off] |= mask; else VRAM_PLANE_E[off] &= ~mask;
	}

	void TH04_PASCAL grcg_circle(screen_x_t center_x, vram_y_t center_y, unsigned r) {
		if (!VRAM_PLANE_B || !VRAM_PLANE_R || !VRAM_PLANE_G || !VRAM_PLANE_E) return;
		int cx = center_x;
		int cy = center_y;
		if (r == 0) { ps3_grcg_pset(cx, cy); return; }
		int x = 0;
		int y = (int)r;
		int d = 1 - (int)r;
		while (x <= y) {
			ps3_grcg_pset(cx + x, cy + y);
			ps3_grcg_pset(cx - x, cy + y);
			ps3_grcg_pset(cx + x, cy - y);
			ps3_grcg_pset(cx - x, cy - y);
			ps3_grcg_pset(cx + y, cy + x);
			ps3_grcg_pset(cx - y, cy + x);
			ps3_grcg_pset(cx + y, cy - x);
			ps3_grcg_pset(cx - y, cy - x);
			if (d < 0) {
				d += 2 * x + 3;
			}
			else {
				d += 2 * (x - y) + 5;
				y--;
			}
			x++;
		}
	}

	void TH04_PASCAL grcg_circlefill(int cx, int cy, unsigned r) {
		if (!VRAM_PLANE_B || !VRAM_PLANE_R || !VRAM_PLANE_G || !VRAM_PLANE_E) return;
		int rad = (int)r;
		for (int dy = -rad; dy <= rad; dy++) {
			int y = cy + dy;
			if (y < 0 || y >= 400) continue;
			int dx = (int)sqrt((double)(rad * rad - dy * dy));
			int x1 = cx - dx;
			int x2 = cx + dx;
			if (x1 < 0) x1 = 0;
			if (x2 >= 640) x2 = 639;
			for (int x = x1; x <= x2; x++) {
				ps3_grcg_pset(x, y);
			}
		}
	}

	void TH04_PASCAL super_clean(int min_pat, int max_pat) {
		printf("[TH04 PS3 Super] super_clean(min=%d, max=%d)\n", min_pat, max_pat);
	}
	void TH04_PASCAL super_roll_put_1plane(int x, int y, int num, int pattern_plane, unsigned put_plane) {
		if (num < 0 || (unsigned)num >= super_patnum ||
			!super_patdata[num] || !super_patsize[num] ||
			pattern_plane < 0 || pattern_plane > 4 ||
			!VRAM_PLANE_B || !VRAM_PLANE_R || !VRAM_PLANE_G || !VRAM_PLANE_E) {
			return;
		}

		unsigned packed = super_patsize[num];
		unsigned bytes_per_row = packed >> 8;
		unsigned height = packed & 255u;
		unsigned plane_bytes = bytes_per_row * height;
		const unsigned char *data = (const unsigned char *)(uintptr_t)super_patdata[num];
		const unsigned char *shape = data + (unsigned)pattern_plane * plane_bytes;

		unsigned char tile_on = ((put_plane >> 8) & 0xFFu) ? 1 : 0;
		unsigned char disabled = (unsigned char)(put_plane & 0x0Fu);
		unsigned char *planes[4] = { VRAM_PLANE_B, VRAM_PLANE_R, VRAM_PLANE_G, VRAM_PLANE_E };

		for (unsigned py = 0; py < height; py++) {
			int sy = (y + (int)py) % 400;
			if (sy < 0) sy += 400;

			for (unsigned px = 0; px < (bytes_per_row << 3); px++) {
				int sx = x + (int)px;
				if (sx < 0 || sx >= 640) continue;

				unsigned src_at = py * bytes_per_row + (px >> 3);
				unsigned char src_bit = (unsigned char)(128u >> (px & 7u));
				if (!(shape[src_at] & src_bit)) continue;

				unsigned dst_at = (unsigned)sy * 80u + ((unsigned)sx >> 3);
				unsigned char dst_bit = (unsigned char)(128u >> (sx & 7));
				for (unsigned plane = 0; plane < 4u; plane++) {
					if (disabled & (1u << plane)) continue;
					if (tile_on) {
						planes[plane][dst_at] |= dst_bit;
					}
					else {
						planes[plane][dst_at] &= (unsigned char)~dst_bit;
					}
				}
			}
		}
	}
	void TH04_PASCAL super_put_1plane(int x, int y, int num, int pattern_plane, unsigned put_plane) { PS3_STUB_LOG("super_put_1plane"); }
	void TH04_PASCAL super_wave_put(int x, int y, int num, int len, char amp, int ph) {}
	void TH04_PASCAL super_zoom(int x, int y, int num, int zoom) {}

	void TH04_PASCAL text_putsa(unsigned x, unsigned y, const char TH04_PTR *str, unsigned atrb) {
		if (!str) return;
		while (*str && x < 80 && y < 25) {
			s_tram_jis[y][x] = (uint16_t)((unsigned char)*str++);
			s_tram_atrb[y][x] = (uint16_t)atrb;
			x++;
		}
	}
	void TH04_PASCAL text_puts(unsigned x, unsigned y, const char TH04_PTR *str) {
		text_putsa(x, y, str, 0xE1);
	}
	void TH04_PASCAL text_putca(unsigned x, unsigned y, unsigned ch, unsigned atrb) {
		if (x < 80 && y < 25) {
			s_tram_jis[y][x] = (uint16_t)(ch & 0xFF);
			s_tram_atrb[y][x] = (uint16_t)atrb;
		}
	}
	void TH04_PASCAL text_fillca(unsigned ch, unsigned atrb) {
		for (int y = 0; y < 25; y++) {
			for (int x = 0; x < 80; x++) {
				s_tram_jis[y][x] = (uint16_t)(ch & 0xFF);
				s_tram_atrb[y][x] = (uint16_t)atrb;
			}
		}
	}
	void TH04_PASCAL text_boxfilla(unsigned x1, unsigned y1, unsigned x2, unsigned y2, unsigned atrb) {
		for (unsigned y = y1; y <= y2 && y < 25; y++) {
			for (unsigned x = x1; x <= x2 && x < 80; x++) {
				s_tram_jis[y][x] = ' ';
				s_tram_atrb[y][x] = (uint16_t)atrb;
			}
		}
	}

	void TH04_PASCAL key_beep_off(void) {
		printf("[TH04 PS3 Input] key_beep_off() called\n");
	}
	void TH04_PASCAL key_beep_on(void) {}
	void TH04_PASCAL text_systemline_hide(void) {
		printf("[TH04 PS3 Text] text_systemline_hide() called\n");
	}
	void TH04_PASCAL text_systemline_show(void) {}
	void TH04_PASCAL text_cursor_hide(void) {
		printf("[TH04 PS3 Text] text_cursor_hide() called\n");
	}
	void TH04_PASCAL text_cursor_show(void) {}
	void TH04_PASCAL dos_puts2(const char TH04_PTR *str) {}

	void TH04_PASCAL grcg_setcolor(int mode, vc2 color) {
		s_grcg_mode = mode;
		s_grcg_color = color;
		s_grcg_enabled = true;
	}
	void TH04_PASCAL grcg_off(void) {
		s_grcg_enabled = false;
	}
	void TH04_PASCAL grcg_byteboxfill_x(int left_b, int top, int right_b, int bottom) {
		if (top < 0) top = 0;
		if (bottom >= 400) bottom = 399;
		if (left_b < 0) left_b = 0;
		if (right_b >= 80) right_b = 79;
		if (top > bottom || left_b > right_b) return;

		uint8_t b_val = (s_grcg_color & 1) ? 0xFF : 0x00;
		uint8_t r_val = (s_grcg_color & 2) ? 0xFF : 0x00;
		uint8_t g_val = (s_grcg_color & 4) ? 0xFF : 0x00;
		uint8_t e_val = (s_grcg_color & 8) ? 0xFF : 0x00;

		int count = right_b - left_b + 1;
		for (int y = top; y <= bottom; y++) {
			int off = y * 80 + left_b;
			if (VRAM_PLANE_B) memset(VRAM_PLANE_B + off, b_val, count);
			if (VRAM_PLANE_R) memset(VRAM_PLANE_R + off, r_val, count);
			if (VRAM_PLANE_G) memset(VRAM_PLANE_G + off, g_val, count);
			if (VRAM_PLANE_E) memset(VRAM_PLANE_E + off, e_val, count);
		}
	}

	static uint8_t s_bgimage_buf[4][32000] = { { 0 } };

	extern "C" void ps3_bgimage_snap(void) {
		if (bgimage.B == 0) {
			bgimage.B = (unsigned char __seg *)s_bgimage_buf[0];
			bgimage.R = (unsigned char __seg *)s_bgimage_buf[1];
			bgimage.G = (unsigned char __seg *)s_bgimage_buf[2];
			bgimage.E = (unsigned char __seg *)s_bgimage_buf[3];
		}
		if (VRAM_PLANE_B) memcpy(s_bgimage_buf[0], VRAM_PLANE_B, 32000);
		if (VRAM_PLANE_R) memcpy(s_bgimage_buf[1], VRAM_PLANE_R, 32000);
		if (VRAM_PLANE_G) memcpy(s_bgimage_buf[2], VRAM_PLANE_G, 32000);
		if (VRAM_PLANE_E) memcpy(s_bgimage_buf[3], VRAM_PLANE_E, 32000);
	}

	extern "C" void ps3_bgimage_put(void) {
		if (VRAM_PLANE_B) memcpy(VRAM_PLANE_B, s_bgimage_buf[0], 32000);
		if (VRAM_PLANE_R) memcpy(VRAM_PLANE_R, s_bgimage_buf[1], 32000);
		if (VRAM_PLANE_G) memcpy(VRAM_PLANE_G, s_bgimage_buf[2], 32000);
		if (VRAM_PLANE_E) memcpy(VRAM_PLANE_E, s_bgimage_buf[3], 32000);
	}

	void TH04_PASCAL bgimage_put_rect_16(int left, int top, int w, int h) {
		int left_b = left / 8;
		int wb = (w + 7) / 8;
		for (int y = top; y < top + h && y < 400; y++) {
			int off = y * 80 + left_b;
			if (off + wb <= 32000) {
				if (VRAM_PLANE_B) memcpy(VRAM_PLANE_B + off, s_bgimage_buf[0] + off, wb);
				if (VRAM_PLANE_R) memcpy(VRAM_PLANE_R + off, s_bgimage_buf[1] + off, wb);
				if (VRAM_PLANE_G) memcpy(VRAM_PLANE_G + off, s_bgimage_buf[2] + off, wb);
				if (VRAM_PLANE_E) memcpy(VRAM_PLANE_E + off, s_bgimage_buf[3] + off, wb);
			}
		}
	}

	void TH04_PASCAL graph_putsa_fx(int left, int top, int col, const unsigned char far *str) {
		// Text string renderer for captions and options
		if (!str) return;
		// Basic 8x16 font rendering:
		// If col has planes, we can draw a simple representation or use gaiji
	}
	int graph_putsa_fx_func = 0;
	unsigned short ClipXL = 0;
	unsigned short ClipXW = 639;
	unsigned short ClipXR = 639;
	unsigned short ClipYT = 0;
	unsigned short ClipYH = 399;
	unsigned short ClipYB = 399;
	unsigned short ClipYT_seg = 0xA800;
	unsigned short ClipYB_adr = 31920;
	unsigned int graph_VramWords = 640 * 400 / 16;
	unsigned int graph_VramSeg = 0xA800;
	unsigned short graph_VramWidth = 80;
	unsigned short graph_VramLines = 400;

	int TH04_PASCAL select_for_rank(int for_easy, int for_normal, int for_hard, int for_lunatic) { return for_normal; }

	unsigned super_patnum = 0;
	void __seg *super_buffer = 0;
	unsigned super_patdata[512] = { 0 };
	unsigned super_patsize[512] = { 0 };

	unsigned pferrno = 0;
	unsigned char pfkey = 0;
	int TH04_PASCAL pf_hook_install(void) { return 1; }
	void TH04_PASCAL pf_hook_remove(void) {}

	void pascal near tiles_invalidate_around(const SPPoint) {}
	void pascal vector2(int &ret_x, int &ret_y, unsigned char angle, int length) {
		ret_x = (int)((((long)length) * CosTable8[angle]) >> 8);
		ret_y = (int)((((long)length) * SinTable8[angle]) >> 8);
	}
	void pascal near vector2_near(SPPoint near &ret, unsigned char angle, subpixel_t length) {
		ret.x.v = (subpixel_t)((((long)length) * CosTable8[angle]) >> 8);
		ret.y.v = (subpixel_t)((((long)length) * SinTable8[angle]) >> 8);
	}
	void near playfield_fill(void) {}
	void near playfield_fillm_0_40_384_274(void) {}
	void near grcg_fill_playfield_rows(void) {}
	void near z_super_roll_put_tiny_16x16_raw(int) {}
	void near z_super_roll_put_tiny_32x32_raw(int) {}
	void near pellets_render_top(void) {}
	void near pellets_render_bottom(void) {}
	void near pellets_render(void) { PS3_STUB_LOG("pellets_render"); }
	void TH04_PASCAL grcg_boxfill(int x1, int y1, int x2, int y2) {
#if !defined(__TURBOC__) && !defined(__MSDOS__)
		if (!VRAM_PLANE_B) return;
		if (y1 > y2) { int t = y1; y1 = y2; y2 = t; }
		if (y1 < 0) y1 = 0;
		if (y2 >= 400) y2 = 399;
		if (x1 > x2) { int t = x1; x1 = x2; x2 = t; }
		if (x2 < 0 || x1 >= 640) return;
		if (x1 < 0) x1 = 0;
		if (x2 >= 640) x2 = 639;

		unsigned first_byte = x1 / 8;
		unsigned last_byte = x2 / 8;
		unsigned char first_mask = 0xffu >> (x1 & 7);
		unsigned char last_mask = 0xffu << (7 - (x2 & 7));

		for (int y = y1; y <= y2; y++) {
			unsigned base = y * 80;
			if (first_byte == last_byte) {
				VRAM_PLANE_B[base + first_byte] |= (first_mask & last_mask);
			}
			else {
				VRAM_PLANE_B[base + first_byte] |= first_mask;
				for (unsigned b = first_byte + 1; b < last_byte; b++) {
					VRAM_PLANE_B[base + b] = 0xff;
				}
				VRAM_PLANE_B[base + last_byte] |= last_mask;
			}
		}
#endif
	}
	int TH04_PASCAL grc_setclip(int xl, int yt, int xr, int yb) {
		if (xl < 0 && xr < 0) return 0;
		if (yt < 0 && yb < 0) return 0;
		if (xl > xr) { int t = xl; xl = xr; xr = t; }
		if (yt > yb) { int t = yt; yt = yb; yb = t; }
		if (xl < 0) xl = 0;
		if (yt < 0) yt = 0;
		if (xr >= 640) xr = 639;
		if (yb >= 400) yb = 399;
		if (xl > xr || yt > yb) return 0;

		ClipXL = (unsigned short)xl;
		ClipXR = (unsigned short)xr;
		ClipXW = (unsigned short)(xr - xl);
		ClipYT = (unsigned short)yt;
		ClipYB = (unsigned short)yb;
		ClipYH = (unsigned short)(yb - yt);
		ClipYB_adr = (unsigned short)(yb * 80);
		ClipYT_seg = (unsigned short)(graph_VramSeg + (yt * 5));
		return 1;
	}
	void near clear_dwords(void) {}
	void TH04_PASCAL text_clear(void) {
		memset(s_tram_jis, 0, sizeof(s_tram_jis));
		for (int y = 0; y < 25; y++) {
			for (int x = 0; x < 80; x++) {
				s_tram_atrb[y][x] = 0xE1;
			}
		}
	}
	void pascal near pointnums_update(void) {}
	void pascal near pointnums_render(void) {}
	void pascal near pointnums_init(void) {}
	unsigned char playperf = 0;
	unsigned char playperf_max = 32;
	char playperf_min = 0;
	void near playperf_raise(int delta) {
		int val = playperf + delta;
		if (val > playperf_max) val = playperf_max;
		playperf = (unsigned char)val;
	}
	void near playperf_lower(int delta) {
		int val = playperf - delta;
		if (val < playperf_min) val = playperf_min;
		playperf = (unsigned char)val;
	}
	void TH04_PASCAL graph_clear(void) {
#if !defined(__TURBOC__) && !defined(__MSDOS__)
		if (VRAM_PLANE_B) memset(VRAM_PLANE_B, 0, 32768);
		if (VRAM_PLANE_R) memset(VRAM_PLANE_R, 0, 32768);
		if (VRAM_PLANE_G) memset(VRAM_PLANE_G, 0, 32768);
		if (VRAM_PLANE_E) memset(VRAM_PLANE_E, 0, 32768);
#endif
	}
	void TH04_PASCAL ems_free(unsigned) {}
	void TH04_PASCAL graph_hide(void) {}
	void pascal near hud_score_put(void) {}
	void TH04_PASCAL graph_400line(void) {}
	static unsigned s_graph_scroll_line = 0;
	void TH04_PASCAL graph_scrollup(unsigned line) {
		if (line >= 400) line = 399;
		s_graph_scroll_line = line;
	}
	void near enemy_bullet_template_push(void) {}
	void TH04_PASCAL sub_3680(int left, int top, int slot, int image) {
#if !defined(__TURBOC__) && !defined(__MSDOS__)
		static int s_calls = 0;
		static int s_early_logs = 0;
		s_calls++;

		if (!VRAM_PLANE_B || !VRAM_PLANE_R || !VRAM_PLANE_G || !VRAM_PLANE_E) {
			if (s_early_logs < 5) {
				printf("[TH04 sub_3680] VRAM nula, salgo (call #%d)\n", s_calls);
				s_early_logs++;
			}
			return;
		}
		if (slot < 0 || slot >= MPN_COUNT) slot = 0;
		mpn_t &mpn = mpn_slots[slot];
		if (!mpn.images) {
			if (s_early_logs < 5) {
				printf("[TH04 sub_3680] mpn_slots[%d].images es NULL, no dibujo (call #%d, left=%d top=%d image=%d)\n",
					slot, s_calls, left, top, image);
				s_early_logs++;
			}
			return;
		}

		const mpn_image_t *img_ptr = reinterpret_cast<const mpn_image_t *>(mpn.images) + image;

		if (s_calls <= 8 || (s_calls % 500) == 0) {
			printf("[TH04 sub_3680] call #%d left=%d top=%d slot=%d image=%d acc=%d row0 B=%04X R=%04X G=%04X E=%04X\n",
				s_calls, left, top, slot, image, s_vram_access_page,
				(unsigned)img_ptr->B.row[0], (unsigned)img_ptr->R.row[0],
				(unsigned)img_ptr->G.row[0], (unsigned)img_ptr->E.row[0]);
		}

		for (int line = 0; line < 16; line++) {
			int vram_off = (top + line) * 80 + (left / 8);
			if (vram_off < 0 || vram_off >= 32768 - 2) continue;

			uint16_t b_word = img_ptr->B.row[line];
			uint16_t r_word = img_ptr->R.row[line];
			uint16_t g_word = img_ptr->G.row[line];
			uint16_t e_word = img_ptr->E.row[line];

			*reinterpret_cast<uint16_t *>(VRAM_PLANE_B + vram_off) = b_word;
			*reinterpret_cast<uint16_t *>(VRAM_PLANE_R + vram_off) = r_word;
			*reinterpret_cast<uint16_t *>(VRAM_PLANE_G + vram_off) = g_word;
			*reinterpret_cast<uint16_t *>(VRAM_PLANE_E + vram_off) = e_word;
		}
#endif
	}
	int near IRand(void) {
		return irand() & 0xFF;
	}
	void near sub_11DE6(void) {}
	void near item_splash_dot_render(void) {}
	void near SHOT_LASER_PUT_RAW(void) {}
	void near sub_BAEE(void) {
		tiles_render_all();
	}
	void near bgm_timer_start(void) {}
	void near bgm_timer_stop(void) {}
}

void near tiles_fill_initial(void) {
#if !defined(__TURBOC__) && !defined(__MSDOS__)
	if (!std_seg || !map_seg) {
		printf("[TH04 PS3 Tiles] tiles_fill_initial FAILED: std_seg=%p, map_seg=%p\n", std_seg, map_seg);
		return;
	}
	const uint8_t *std_ptr = reinterpret_cast<const uint8_t *>(std_seg);
	const uint16_t *map_ptr = reinterpret_cast<const uint16_t *>(map_seg);

	printf("[TH04 PS3 Tiles] tiles_fill_initial: std_seg=%p, map_seg=%p, sec_ids=[%u,%u,%u,%u,%u]\n",
		std_seg, map_seg, std_ptr[0], std_ptr[1], std_ptr[2], std_ptr[3], std_ptr[4]);

	for (int s = 0; s < 5; s++) {
		uint8_t sec_id = std_ptr[s];
		uint16_t sec_offset_words = (TILE_SECTION_OFFSETS[sec_id]) / 2;

		for (int r = 0; r < 5; r++) {
			int ring_row = (4 - s) * 5 + (4 - r);
			int sec_row = 4 - r;
			for (int x = 0; x < 24; x++) {
				tile_ring[ring_row][x] = map_ptr[sec_offset_words + sec_row * 32 + x];
			}
		}
	}
	printf("[TH04 PS3 Tiles] tiles_fill_initial DONE, tile_ring[0][0]=%u, tile_ring[24][0]=%u\n",
		(unsigned)tile_ring[0][0], (unsigned)tile_ring[24][0]);

	g_ps3_hide_tile_strip = 1;
#endif
}

bgimage_planes_t bgimage = { 0, 0, 0, 0 };

// C++ linkage VRAM plane pointers and allocated buffers (double-buffered pages 0 & 1)
__attribute__((aligned(16))) uint8_t s_vram_pages[2][4][32768] = { { { 0 } } };
int s_vram_access_page = 0;
int s_vram_shown_page = 0;

extern "C" void ps3_clear_playfield(void) {
	for (int p = 0; p < 2; p++) {
		for (int pl = 0; pl < 4; pl++) {
			for (int y = 0; y < 400; y++) {
				memset(&s_vram_pages[p][pl][y * 80], 0, 72);
			}
		}
	}
}

uint8_t far *VRAM_PLANE_B = s_vram_pages[0][0];
uint8_t far *VRAM_PLANE_R = s_vram_pages[0][1];
uint8_t far *VRAM_PLANE_G = s_vram_pages[0][2];
uint8_t far *VRAM_PLANE_E = s_vram_pages[0][3];

extern "C" void ps3_set_access_page(int page) {
	s_vram_access_page = page & 1;
	VRAM_PLANE_B = s_vram_pages[s_vram_access_page][0];
	VRAM_PLANE_R = s_vram_pages[s_vram_access_page][1];
	VRAM_PLANE_G = s_vram_pages[s_vram_access_page][2];
	VRAM_PLANE_E = s_vram_pages[s_vram_access_page][3];
}

extern "C" void ps3_set_show_page(int page) {
	s_vram_shown_page = page & 1;
}

extern "C" void ps3_outportb(unsigned short port, unsigned char val) {
	if (port == 0xA6) {
		ps3_set_access_page(val);
	}
	else if (port == 0xA4) {
		ps3_set_show_page(val);
	}
}

extern "C" int ps3_graph_copy_page(int to_page) {
	int to_p = to_page & 1;
	int from_p = to_p ^ 1;
	for (int p = 0; p < 4; p++) {
		memcpy(s_vram_pages[to_p][p], s_vram_pages[from_p][p], 32768);
	}
	return 1;
}

extern "C" void ps3_egc_copy_rect_1_to_0_16(int left, int top, int w, int h) {
	int left_b = left / 8;
	int wb = (w + 7) / 8;
	for (int y = top; y < top + h && y < 400; y++) {
		int off = y * 80 + left_b;
		if (off + wb <= 32000) {
			for (int p = 0; p < 4; p++) {
				memcpy(s_vram_pages[0][p] + off, s_vram_pages[1][p] + off, wb);
			}
		}
	}
}

// C++ linkage helper stubs and data
void near grcg_setmode_tdw(void) {}
void near grcg_setmode_rmw(void) {}
void near grcg_setcolor_direct_raw(void) {}
void near grcg_vline(int x, int y1, int y2) {
	if (x < 0 || x >= 640) return;
	if (y1 > y2) { int t = y1; y1 = y2; y2 = t; }
	if (y1 < 0) y1 = 0;
	if (y2 >= 400) y2 = 399;
	for (int y = y1; y <= y2; y++) {
		ps3_grcg_pset(x, y);
	}
}

void near grcg_line(int x1, int y1, int x2, int y2) {
	int dx = abs(x2 - x1);
	int dy = abs(y2 - y1);
	int sx = (x1 < x2) ? 1 : -1;
	int sy = (y1 < y2) ? 1 : -1;
	int err = dx - dy;
	while (true) {
		ps3_grcg_pset(x1, y1);
		if (x1 == x2 && y1 == y2) break;
		int e2 = 2 * err;
		if (e2 > -dy) {
			err -= dy;
			x1 += sx;
		}
		if (e2 < dx) {
			err += dx;
			y1 += sy;
		}
	}
}

void TH04_PASCAL egc_shift_left(screen_x_t x1, vram_y_t y1, screen_x_t x2, vram_y_t y2, pixel_t dots) {}
void TH04_PASCAL egc_shift_right(screen_x_t x1, vram_y_t y1, screen_x_t x2, vram_y_t y2, pixel_t dots) {}
void TH04_PASCAL egc_shift_up(screen_x_t x1, vram_y_t y1, screen_x_t x2, vram_y_t y2, pixel_t dots) {}
void TH04_PASCAL egc_shift_down(screen_x_t x1, vram_y_t y1, screen_x_t x2, vram_y_t y2, pixel_t dots) {}

void near reimu_marisa_backdrop_colorfill(void) {}
void near yuuka5_backdrop_colorfill(void) {}

void near super_roll_put(int x, int y, int num) {
	if (num < 0 || (unsigned)num >= super_patnum ||
		!super_patdata[num] || !super_patsize[num] ||
		!VRAM_PLANE_B || !VRAM_PLANE_R || !VRAM_PLANE_G || !VRAM_PLANE_E) {
		return;
	}

	unsigned packed = super_patsize[num];
	unsigned bytes_per_row = packed >> 8;
	unsigned height = packed & 255u;
	unsigned plane_bytes = bytes_per_row * height;
	const unsigned char *data = (const unsigned char *)(uintptr_t)super_patdata[num];
	unsigned char *planes[4] = { VRAM_PLANE_B, VRAM_PLANE_R, VRAM_PLANE_G, VRAM_PLANE_E };

	for (unsigned py = 0; py < height; py++) {
		int sy = (y + (int)py) % 400;
		if (sy < 0) sy += 400;

		for (unsigned px = 0; px < (bytes_per_row << 3); px++) {
			int sx = x + (int)px;
			if (sx < 0 || sx >= 640) continue;

			unsigned src_at = py * bytes_per_row + (px >> 3);
			unsigned char src_bit = (unsigned char)(128u >> (px & 7u));
			if (!(data[src_at] & src_bit)) continue;

			unsigned dst_at = (unsigned)sy * 80u + ((unsigned)sx >> 3);
			unsigned char dst_bit = (unsigned char)(128u >> (sx & 7));
			for (unsigned plane = 0; plane < 4u; plane++) {
				if (data[(plane + 1u) * plane_bytes + src_at] & src_bit) {
					planes[plane][dst_at] |= dst_bit;
				}
				else {
					planes[plane][dst_at] &= (unsigned char)~dst_bit;
				}
			}
		}
	}
}
void near super_large_put(int, int, int) { PS3_STUB_LOG("super_large_put"); }
void near z_super_put_16x16_mono_raw(int) { PS3_STUB_LOG("z_super_put_16x16_mono_raw"); }
int TH04_PASCAL super_convert_tiny(int num) { return 0; }

void near playfield_checkerboard_grcg_tdw_(void) {}
void near pointnums_invalidate(void) {}

void pascal near sparks_add_circle(Subpixel center_x, Subpixel center_y, subpixel_t distance, int count) {
	if ((unsigned int)center_x.v > (unsigned int)(PLAYFIELD_W << 4) ||
		(unsigned int)center_y.v > (unsigned int)(PLAYFIELD_H << 4) ||
		count <= 0) return;

	for (int i = 0; i < count; i++) {
		int idx = (spark_ring_offset / sizeof(spark_t)) % SPARK_COUNT;
		spark_t *p = &sparks[idx];
		if (p->flag == F_FREE) {
			p->flag = F_ALIVE;
			p->age = 0;
			p->center.cur.x = center_x;
			p->center.cur.y = center_y;
			unsigned char angle = (unsigned char)((0x100 * i) / count);
			vector2_near(p->center.velocity, angle, distance);
		}
		spark_ring_offset = (uint16_t)((spark_ring_offset + sizeof(spark_t)) % (SPARK_COUNT * sizeof(spark_t)));
	}
}

void pascal sparks_add_random(Subpixel center_x, Subpixel center_y, subpixel_t radius_min, int count) {
	if ((unsigned int)center_x.v > (unsigned int)(PLAYFIELD_W << 4) ||
		(unsigned int)center_y.v > (unsigned int)(PLAYFIELD_H << 4) ||
		count <= 0) return;

	for (int i = 0; i < count; i++) {
		int idx = (spark_ring_offset / sizeof(spark_t)) % SPARK_COUNT;
		spark_t *p = &sparks[idx];
		if (p->flag == F_FREE) {
			p->flag = F_ALIVE;
			p->age = 0;
			p->center.cur.x = center_x;
			p->center.cur.y = center_y;
			uint16_t rand_len = (uint16_t)(radius_min + (irand() & 0x1F));
			unsigned char angle = (unsigned char)(irand() & 0xFF);
			vector2_near(p->center.velocity, angle, (subpixel_t)rand_len);
		}
		spark_ring_offset = (uint16_t)((spark_ring_offset + sizeof(spark_t)) % (SPARK_COUNT * sizeof(spark_t)));
	}
}

void near sparks_update(void) {
	for (int i = 0; i < SPARK_COUNT; i++) {
		spark_t *p = &sparks[i];
		if (p->flag == F_FREE) continue;
		if (p->flag != F_ALIVE) {
			p->flag = F_FREE;
			continue;
		}

		p->center.prev = p->center.cur;
		p->center.cur.x.v += p->center.velocity.x.v;
		p->center.cur.y.v += p->center.velocity.y.v;

		if ((unsigned int)(p->center.cur.x.v + (4 << 4)) >= (unsigned int)((PLAYFIELD_W + 8) << 4) ||
			(unsigned int)(p->center.cur.y.v + (4 << 4)) >= (unsigned int)((PLAYFIELD_H + 8) << 4)) {
			p->flag = F_REMOVE;
			continue;
		}

		p->center.velocity.y.v++;
		p->age++;
		if (p->age > 40) {
			p->flag = F_REMOVE;
		}
	}
}

void near sparks_render(void) {
	if (!VRAM_PLANE_B || !VRAM_PLANE_R || !VRAM_PLANE_G || !VRAM_PLANE_E) return;
	for (int i = 0; i < SPARK_COUNT; i++) {
		const spark_t *p = &sparks[i];
		if (p->flag != 1) continue;
		int sx = PLAYFIELD_LEFT + (p->center.cur.x.v >> 4);
		int sy = PLAYFIELD_TOP + (p->center.cur.y.v >> 4);
		ps3_grcg_pset(sx, sy);
		ps3_grcg_pset(sx + 1, sy);
		ps3_grcg_pset(sx, sy + 1);
		ps3_grcg_pset(sx + 1, sy + 1);
	}
}

void near tiles_bb_invalidate_raw(int) {}
void near tiles_render_all(void) {
#if !defined(__TURBOC__) && !defined(__MSDOS__)
	if (!VRAM_PLANE_B || !VRAM_PLANE_R || !VRAM_PLANE_G || !VRAM_PLANE_E) return;

	static int s_calls = 0;
	s_calls++;
	int copied = 0;
	int skipped = 0;

	for (int y = 0; y < 25; y++) {
		for (int x = 0; x < 24; x++) {
			uint16_t src_vo = tile_ring[y][x];
			uint16_t dst_vo = (y * 16) * 80 + 4 + (x * 2);

			for (int line = 0; line < 16; line++) {
				int s_off = src_vo + line * 80;
				int d_off = dst_vo + line * 80;

				if (s_off >= 0 && s_off < 32768 - 1 && d_off >= 0 && d_off < 32768 - 1) {
					VRAM_PLANE_B[d_off + 0] = VRAM_PLANE_B[s_off + 0];
					VRAM_PLANE_B[d_off + 1] = VRAM_PLANE_B[s_off + 1];
					VRAM_PLANE_R[d_off + 0] = VRAM_PLANE_R[s_off + 0];
					VRAM_PLANE_R[d_off + 1] = VRAM_PLANE_R[s_off + 1];
					VRAM_PLANE_G[d_off + 0] = VRAM_PLANE_G[s_off + 0];
					VRAM_PLANE_G[d_off + 1] = VRAM_PLANE_G[s_off + 1];
					VRAM_PLANE_E[d_off + 0] = VRAM_PLANE_E[s_off + 0];
					VRAM_PLANE_E[d_off + 1] = VRAM_PLANE_E[s_off + 1];
					copied++;
				}
				else {
					skipped++;
				}
			}
		}
	}

	if (s_calls <= 5 || (s_calls % 120) == 0) {
		printf("[TH04 PS3 Tiles] tiles_render_all #%d: acc=%d shown=%d copiadas=%d salteadas=%d ring[0]=%04X %04X %04X %04X ring[24]=%04X %04X %04X %04X\n",
			s_calls, s_vram_access_page, s_vram_shown_page, copied, skipped,
			(unsigned)tile_ring[0][0], (unsigned)tile_ring[0][1], (unsigned)tile_ring[0][2], (unsigned)tile_ring[0][3],
			(unsigned)tile_ring[24][0], (unsigned)tile_ring[24][1], (unsigned)tile_ring[24][2], (unsigned)tile_ring[24][3]);
	}
#endif
}
void near tiles_redraw_invalidated(void) {
#if !defined(__TURBOC__) && !defined(__MSDOS__)
	tiles_render_all();
#endif
}
void near tiles_bb_put_raw(int) {}

void near input_reset_sense(void) {
	key_det = 0;
	input_sense();
}

void near input_sense(void) {
#if defined(__PS3__) || defined(CELL_SDK) || defined(__CELLOS_LV2__) || defined(SN_TARGET_PS3)
	static int s_pad_inited = 0;
	if (!s_pad_inited) {
		printf("[TH04 PS3 Input] Initializing CellPad...\n");
		cellSysmoduleLoadModule(CELL_SYSMODULE_IO);
		int ret = cellPadInit(7);
		printf("[TH04 PS3 Input] cellPadInit(7) result = 0x%08X\n", (unsigned)ret);
		s_pad_inited = 1;
	}
	input_t k = 0;
	for (int p = 0; p < 7; p++) {
		CellPadData pad_data;
		if (cellPadGetData(p, &pad_data) == CELL_PAD_OK && pad_data.len > 0) {
			uint16_t d1 = pad_data.button[CELL_PAD_BTN_OFFSET_DIGITAL1] & 0xFF;
			uint16_t d2 = pad_data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] & 0xFF;

			if (d1 & CELL_PAD_CTRL_UP)    k |= INPUT_UP;
			if (d1 & CELL_PAD_CTRL_DOWN)  k |= INPUT_DOWN;
			if (d1 & CELL_PAD_CTRL_LEFT)  k |= INPUT_LEFT;
			if (d1 & CELL_PAD_CTRL_RIGHT) k |= INPUT_RIGHT;

			if (d2 & CELL_PAD_CTRL_CROSS)    k |= (INPUT_SHOT | INPUT_OK);
			if (d2 & CELL_PAD_CTRL_CIRCLE)   k |= (INPUT_SHOT | INPUT_OK);
			if (d2 & CELL_PAD_CTRL_SQUARE)   k |= (INPUT_BOMB | INPUT_CANCEL);
			if (d2 & CELL_PAD_CTRL_TRIANGLE) k |= (INPUT_BOMB | INPUT_CANCEL);

			if (d1 & CELL_PAD_CTRL_START)    k |= (INPUT_SHOT | INPUT_OK);
			if (d1 & CELL_PAD_CTRL_SELECT)   k |= INPUT_CANCEL;

			int lx = (int)(pad_data.button[CELL_PAD_BTN_OFFSET_ANALOG_LEFT_X] & 0xFF) - 128;
			int ly = (int)(pad_data.button[CELL_PAD_BTN_OFFSET_ANALOG_LEFT_Y] & 0xFF) - 128;
			if (lx < -50) k |= INPUT_LEFT;
			if (lx > 50)  k |= INPUT_RIGHT;
			if (ly < -50) k |= INPUT_UP;
			if (ly > 50)  k |= INPUT_DOWN;
		}
	}
	static input_t s_prev_k = 0;
	if (k != s_prev_k && k != 0) {
		printf("[TH04 Input] Controller pressed: 0x%04X\n", (unsigned)k);
	}
	s_prev_k = k;
	key_det = k;
#endif
}
void pascal near scoredat_encode(scoredat_section_t near *hi) {
	if (!hi) return;
	uint8_t *raw = (uint8_t *)hi;
	uint16_t sum = 0;
	for (int i = 0; i < 0xC0; i++) {
		sum += raw[4 + i];
	}
	hi->score_sum = sum;
	uint16_t r = (uint16_t)irand();
	hi->key1 = (uint8_t)(r & 0xFF);
	hi->key2 = (uint8_t)((r >> 8) & 0xFF);

	uint8_t dl = 0;
	for (int i = 0xC0 - 1; i >= 0; i--) {
		dl += hi->key1;
		raw[4 + i] -= dl;
		dl = raw[4 + i];
		dl = (uint8_t)((dl >> 3) | (dl << 5));
		dl ^= hi->key2;
	}
}

uint8_t pascal near scoredat_decode(scoredat_section_t near *hi) {
	if (!hi) return 1;
	uint8_t *raw = (uint8_t *)hi;
	for (int i = 0; i < 0xC0 - 1; i++) {
		uint8_t al = raw[4 + i + 1];
		al = (uint8_t)((al >> 3) | (al << 5));
		al ^= hi->key2;
		al += hi->key1;
		raw[4 + i] += al;
	}
	raw[4 + 0xC0 - 1] += hi->key1;

	uint16_t sum = 0;
	for (int i = 0; i < 0xC0; i++) {
		sum += raw[4 + i];
	}
	return (uint8_t)(hi->score_sum - sum);
}
void near egc_start_copy_noframe(void) {}

void near sub_12024(void) {}

static bool ps3_compat_null_bool(void) { return false; }
static void ps3_compat_null_void(void) {}

point_t tile_invalidate_box;
nearfunc_t_near bullet_template_tune = (nearfunc_t_near)ps3_compat_null_void;
void gather_point_render(int, int) {}
void score_update_and_render(void) { PS3_STUB_LOG("score_update_and_render"); }
void shot_level_update(void) {}
void bb_txt_put_8_raw(unsigned int, unsigned int) { PS3_STUB_LOG("bb_txt_put_8_raw"); }
int load_playchar_resources = 0;
void cdg_put_plane_roll_8(int, int, int, vram_plane_t, unsigned char*) { PS3_STUB_LOG("cdg_put_plane_roll_8"); }
void bullets_and_gather_invalidate(void) {}

uint16_t pascal near randring1_next16_mod(uint16_t divisor) {
	if (divisor == 0) return 0;
	uint16_t val = (uint16_t)(randring[randring_p & 0xFF] | (randring[(randring_p + 1) & 0xFF] << 8));
	randring_p = (uint16_t)((randring_p + 1) & 0xFF);
	return val % divisor;
}
uint16_t pascal near randring2_next16_mod(uint16_t divisor) {
	if (divisor == 0) return 0;
	uint16_t val = (uint16_t)(randring[randring_p & 0xFF] | (randring[(randring_p + 1) & 0xFF] << 8));
	randring_p = (uint16_t)((randring_p + 1) & 0xFF);
	return val % divisor;
}

// C++ BSS / Data definitions
boss_stuff_t boss;
midboss_stuff_t midboss;
bool boss_phase_timed_out = false;
SPPoint boss_hitbox_radius;
SPPoint shot_hitbox_center;
SPPoint shot_hitbox_radius;
func_t_near boss_update = (func_t_near)ps3_compat_null_void;
nearfunc_t_near boss_fg_render = (nearfunc_t_near)ps3_compat_null_void;
func_t_near boss_update_func = (func_t_near)ps3_compat_null_void;
nearfunc_t_near boss_bg_render_func = (nearfunc_t_near)ps3_compat_null_void;
nearfunc_t_near boss_fg_render_func = (nearfunc_t_near)ps3_compat_null_void;
nearfunc_t_near boss_backdrop_colorfill = (nearfunc_t_near)ps3_compat_null_void;
unsigned char boss_statebyte[16] = { 0 };

Palette8 Palettes;
unsigned int PaletteTone = 100;
bool palette_changed = false;

unsigned int stage_graze = 0;
unsigned char stage_point_items_collected = 0;

PlayfieldMotion player_pos;
bool player_is_hit = false;
unsigned char player_invincibility_time = 0;
uint8_t power = 0;
uint8_t shot_level = 0;

extern const short CosTable8[256] = {
	256, 256, 256, 255, 255, 254, 253, 252,
	251, 250, 248, 247, 245, 243, 241, 239,
	237, 234, 231, 229, 226, 223, 220, 216,
	213, 209, 206, 202, 198, 194, 190, 185,
	181, 177, 172, 167, 162, 157, 152, 147,
	142, 137, 132, 126, 121, 115, 109, 104,
	98, 92, 86, 80, 74, 68, 62, 56,
	50, 44, 38, 31, 25, 19, 13, 6,
	0, -6, -13, -19, -25, -31, -38, -44,
	-50, -56, -62, -68, -74, -80, -86, -92,
	-98, -104, -109, -115, -121, -126, -132, -137,
	-142, -147, -152, -157, -162, -167, -172, -177,
	-181, -185, -190, -194, -198, -202, -206, -209,
	-213, -216, -220, -223, -226, -229, -231, -234,
	-237, -239, -241, -243, -245, -247, -248, -250,
	-251, -252, -253, -254, -255, -255, -256, -256,
	-256, -256, -256, -255, -255, -254, -253, -252,
	-251, -250, -248, -247, -245, -243, -241, -239,
	-237, -234, -231, -229, -226, -223, -220, -216,
	-213, -209, -206, -202, -198, -194, -190, -185,
	-181, -177, -172, -167, -162, -157, -152, -147,
	-142, -137, -132, -126, -121, -115, -109, -104,
	-98, -92, -86, -80, -74, -68, -62, -56,
	-50, -44, -38, -31, -25, -19, -13, -6,
	0, 6, 13, 19, 25, 31, 38, 44,
	50, 56, 62, 68, 74, 80, 86, 92,
	98, 104, 109, 115, 121, 126, 132, 137,
	142, 147, 152, 157, 162, 167, 172, 177,
	181, 185, 190, 194, 198, 202, 206, 209,
	213, 216, 220, 223, 226, 229, 231, 234,
	237, 239, 241, 243, 245, 247, 248, 250,
	251, 252, 253, 254, 255, 255, 256, 256
};

extern const short SinTable8[256] = {
	0, 6, 13, 19, 25, 31, 38, 44,
	50, 56, 62, 68, 74, 80, 86, 92,
	98, 104, 109, 115, 121, 126, 132, 137,
	142, 147, 152, 157, 162, 167, 172, 177,
	181, 185, 190, 194, 198, 202, 206, 209,
	213, 216, 220, 223, 226, 229, 231, 234,
	237, 239, 241, 243, 245, 247, 248, 250,
	251, 252, 253, 254, 255, 255, 256, 256,
	256, 256, 256, 255, 255, 254, 253, 252,
	251, 250, 248, 247, 245, 243, 241, 239,
	237, 234, 231, 229, 226, 223, 220, 216,
	213, 209, 206, 202, 198, 194, 190, 185,
	181, 177, 172, 167, 162, 157, 152, 147,
	142, 137, 132, 126, 121, 115, 109, 104,
	98, 92, 86, 80, 74, 68, 62, 56,
	50, 44, 38, 31, 25, 19, 13, 6,
	0, -6, -13, -19, -25, -31, -38, -44,
	-50, -56, -62, -68, -74, -80, -86, -92,
	-98, -104, -109, -115, -121, -126, -132, -137,
	-142, -147, -152, -157, -162, -167, -172, -177,
	-181, -185, -190, -194, -198, -202, -206, -209,
	-213, -216, -220, -223, -226, -229, -231, -234,
	-237, -239, -241, -243, -245, -247, -248, -250,
	-251, -252, -253, -254, -255, -255, -256, -256,
	-256, -256, -256, -255, -255, -254, -253, -252,
	-251, -250, -248, -247, -245, -243, -241, -239,
	-237, -234, -231, -229, -226, -223, -220, -216,
	-213, -209, -206, -202, -198, -194, -190, -185,
	-181, -177, -172, -167, -162, -157, -152, -147,
	-142, -137, -132, -126, -121, -115, -109, -104,
	-98, -92, -86, -80, -74, -68, -62, -56,
	-50, -44, -38, -31, -25, -19, -13, -6
};

extern const int ITEM_PATNUM[IT_COUNT] = { 0 };
extern const Subpixel ITEM_MISS_VELOCITIES[MISS_FIELD_COUNT][2][ITEM_MISS_COUNT] = { 0 };
unsigned int items_spawned = 0;
unsigned int items_collected = 0;
unsigned int total_point_items_collected = 0;
unsigned int total_max_valued_point_items_collected = 0;
unsigned char item_playperf_lower = 0;
unsigned char item_playperf_raise = 0;
bool items_pull_to_player = false;
item_t items[ITEM_COUNT];

score_lebcd_t hiscore;
score_lebcd_t score;
unsigned int graze_score = 0;
unsigned char extends_gained = 0;
unsigned long score_delta = 0;

unsigned int bb_boss_seg = 0;
unsigned int tiles_bb_seg = 0;
unsigned char tiles_bb_col = 0;

func_t_near midboss_update = (func_t_near)ps3_compat_null_void;
nearfunc_t_near midboss_render = (nearfunc_t_near)ps3_compat_null_void;
func_t_near midboss_update_func = (func_t_near)ps3_compat_null_void;
nearfunc_t_near midboss_render_func = (nearfunc_t_near)ps3_compat_null_void;
int midboss_frames_until = 0;
unsigned char midboss_defeat_angle = 0;
unsigned char midboss1_angle = 0;
int midboss1_vram_y = 0;
unsigned char midboss2_pattern = 0;
unsigned char midboss2_direction = 0;
unsigned char midboss2_patterns_done = 0;
unsigned char midboss3_pattern = 0;
unsigned char midboss3_mirror = 0;
unsigned char midboss3_patterns_done = 0;
unsigned char MIDBOSS3_FLY_ANGLES[8] = { 0 };
unsigned char midboss4_pattern = 0;
unsigned char midboss4_aim_toggle = 0;
unsigned char midboss4_unknown_state = 0;
unsigned char midboss4_patterns_done = 0;
void (near pascal *midboss_invalidate)(void) = (void (near pascal *)(void))ps3_compat_null_void;
bool midboss_active = false;

char aSt00_bmt[] = "st00.bmt";
char aSt00bk_cdg[] = "st00bk.cdg";
char aSt00_bb[] = "st00.bb";
char aSt01_bmt[] = "st01.bmt";
char aSt01bk_cdg[] = "st01bk.cdg";
char aSt01_bb[] = "st01.bb";
char aSt02_bmt[] = "st02.bmt";
char aSt02bk_cdg[] = "st02bk.cdg";
char aSt02_bb[] = "st02.bb";
char aSt03_bmt[] = "st03.bmt";
char aSt03bk_cdg[] = "st03bk.cdg";
char aSt03bk2_cdg[] = "st03bk2.cdg";
char aSt03_bb[] = "st03.bb";
char aSt04bk_cdg[] = "st04bk.cdg";
char aSt04_bb[] = "st04.bb";
char aSt04_cdg[] = "st04.cdg";
char aSt05_bb[] = "st05.bb";
char st06_bft[] = "st06.bft";
char bss6_cd2[] = "bss6.cd2";
char st06_mpn[] = "st06.mpn";
char st05_bft[] = "st05.bft";
char bss5_cd2[] = "bss5.cd2";
char st05_mpn[] = "st05.mpn";
char bss4_cd2[] = "bss4.cd2";
char st04_bft[] = "st04.bft";
char st04_mpn[] = "st04.mpn";
char bss2_cd2[] = "bss2.cd2";
char st02_bft[] = "st02.bft";
char st02_mpn[] = "st02.mpn";
char bss1_cd2[] = "bss1.cd2";
char st01_bft[] = "st01.bft";
char st01_mpn[] = "st01.mpn";
char bss0_cd2[] = "bss0.cd2";
char st00_bft[] = "st00.bft";
char st00_mpn[] = "st00.mpn";
char st10_mpn[] = "st10.mpn";
char kao3_cd2[] = "kao3.cd2";
char kao2_cd2[] = "kao2.cd2";
char st03_bft[] = "st03.bft";
char st03_mpn[] = "st03.mpn";
char eye_rgb[] = "eye.rgb";
char miko_bft[] = "miko.bft";
char mari_bft[] = "mari.bft";
char mikod_bft[] = "mikod.bft";
char miko32_bft[] = "miko32.bft";
char miko16_bft[] = "miko16.bft";
static char stage_bgm_name_buf[16] = "ST00";
extern "C" char *stage_bgm_name = stage_bgm_name_buf;
int stage_faceset_count = 0;

nearfunc_t_near stage_render = (nearfunc_t_near)ps3_compat_null_void;
nearfunc_t_near stage_invalidate = (nearfunc_t_near)ps3_compat_null_void;
unsigned char stage_frame_mod2 = 0;
unsigned char stage_frame_mod4 = 0;
unsigned char stage_frame_mod8 = 0;
unsigned char stage_frame_mod16 = 0;
unsigned int stage_frame = 0;
int stage_id = 0;
func_t_near stage_vm = (func_t_near)ps3_compat_null_void;
int stage5_star_center_y = 0;
char STAGE_CLEAR_BONUS_DESC[] = "";
char gpCLEAR_BONUS[] = "";
char aBONUS_STAGE[] = "";
char aPOWERX50[] = "";
char aBONUS_DREAM[] = "";
char aGRAZEX50[] = "";
char aBONUS_POINT[] = "";
char aBONUS_TOTAL[] = "";
char aBOMB_EXTEND[] = "";
char gpCONGRATULATION[] = "";
char aALL_CLEAR[] = "";
char aPOWERX50_2[] = "";
char aBONUS_DREAM_2[] = "";
char aGRAZEX50_2[] = "";
char aPLAYER_REM_10000[] = "";
char aPLAYER_REM_30000[] = "";
char aBONUS_POINT_2[] = "";
char aBONUS_TOTAL_2[] = "";
int stage_bgm_title_len = 0;
int stage_title_id = 0;
static const char s_dummy_stage_title[] = "STAGE TITLE";
const char *STAGE_TITLES[8] = {
	s_dummy_stage_title, s_dummy_stage_title, s_dummy_stage_title, s_dummy_stage_title,
	s_dummy_stage_title, s_dummy_stage_title, s_dummy_stage_title, s_dummy_stage_title
};
int stage_title_len = 0;
static const char s_dummy_bgm_title[] = "BGM TITLE";
const char *BGM_TITLES[16] = {
	s_dummy_bgm_title, s_dummy_bgm_title, s_dummy_bgm_title, s_dummy_bgm_title,
	s_dummy_bgm_title, s_dummy_bgm_title, s_dummy_bgm_title, s_dummy_bgm_title,
	s_dummy_bgm_title, s_dummy_bgm_title, s_dummy_bgm_title, s_dummy_bgm_title,
	s_dummy_bgm_title, s_dummy_bgm_title, s_dummy_bgm_title, s_dummy_bgm_title
};
char gStage_1[] = "";
char gFINAL_STAGE[] = "";
char gEXTRA_STAGE[] = "";

unsigned char playchar = 0;
bool reimu_trail_visible = false;
unsigned char reimu_pattern_angle_delta = 0;
bool reimu_orbs_visible = false;
int orb_patnum_base = 0;
Shot shots[SHOT_COUNT];
int player_respawn_motion_time = 0;
SPPoint player_option_pos_prev[2];
Shot near *shot_ptr = 0;
char shot_last_id = 0;
unsigned int shot_laser_time = 0;
shot_laser_style_t shot_laser_style = SLS_2;
SPPoint player_option_pos_cur[2];
PlayfieldMotion shot_laser_bottomcenter;
uint8_t shot_laser_ring_cycle = 0;
unsigned char shot_time = 0;
unsigned char reimu_shot_cycle = 0;
unsigned char byte_259A7 = 0;
unsigned int shots_alive_count = 0;
shot_alive_t shots_alive[SHOT_COUNT];
unsigned char byte_25980 = 0;
int player_input_prev = 0;
nearfunc_t_near playchar_shot_func = (nearfunc_t_near)ps3_compat_null_void;
int player_option_patnum = 0;
nearfunc_t_near SHOT_FUNCS_REIMU_A[10] = { 0 };
nearfunc_t_near playchar_shot_funcs[10] = { 0 };
nearfunc_t_near SHOT_FUNCS_REIMU_B[10] = { 0 };
nearfunc_t_near player_bomb_func = (nearfunc_t_near)ps3_compat_null_void;
nearfunc_t_near playchar_bomb_func = (nearfunc_t_near)ps3_compat_null_void;
nearfunc_t_near SHOT_FUNCS_MARISA_A[10] = { 0 };
nearfunc_t_near SHOT_FUNCS_MARISA_B[10] = { 0 };
unsigned char player_state_unknown_0 = 0;
unsigned char player_state_unknown_1 = 0;
unsigned char player_state_unknown_2 = 0;
unsigned char player_state_unknown_3 = 0;
int player_miss_animation_frame = 0;

uint16_t randring_p = 0;
unsigned char randring[256] = { 0 };

unsigned char BOSS_ITEM_DROPS[8] = { 0 };
unsigned char ENEMY_DROPS[8] = { 0 };
int item_drop_cycle = 0;
int power_overflow = 0;
int POWER_OVERFLOW_BONUS = 0;
bool pointnum_times_2 = false;
int DREAM_SCORE_PER_ITEMS = 0;
int miss_time = 0;

unsigned char bullet_clear_time = 0;
unsigned char bullet_zap = 0;
bool bullet_zap_active = false;
BulletTemplate bullet_template;
bullet_special_u bullet_special = {};
bullet_special_angle_t bullet_template_special_angle = {};
bool bombing = false;
pixel_t playfield_shake_x = 0;
pixel_t playfield_shake_y = 0;
int slowdown_factor = 0;
bool bombing_disabled = false;
int playfield_shake_anim_time = 0;
int egc_shift_left_val = 0;
int egc_shift_right_val = 0;
int egc_shift_up_val = 0;
int egc_shift_down_val = 0;
int playfield_shake_redraw_time = 0;

resident_t far *resident = &s_resident;
unsigned char rank = 0;
int score_delta_frame = 0;
unsigned long dream_score = 0;
int dream_items_collected = 0;
int entered_place = 0;
#ifdef SCOREDAT_FN
#undef SCOREDAT_FN
#endif
char SCOREDAT_FN[] = "GENSOU.SCR";
char SCOREDAT_FN_0[] = "GENSOU.SCR";
char SCOREDAT_FN_1[] = "GENSOU.SCR";
char SCOREDAT_FN_2[] = "GENSOU.SCR";
char gCONTINUE_[] = "";

nearfunc_t_near overlay1 = (nearfunc_t_near)ps3_compat_null_void;
nearfunc_t_near overlay2 = (nearfunc_t_near)ps3_compat_null_void;
unsigned long overlay_popup_bonus = 0;
popup_id_t overlay_popup_id_new = POPUP_ID_HISCORE_ENTRY;
int overlay_fade = 0;
int titles_frame = 0;
int dissolve_sprite = 0;
int popup_id_cur = 0;
int popup_frame = 0;
int popup_shiftbuf = 0;
static const char s_dummy_popup[] = "POPUP";
const char *POPUP_STRINGS[8] = {
	s_dummy_popup, s_dummy_popup, s_dummy_popup, s_dummy_popup,
	s_dummy_popup, s_dummy_popup, s_dummy_popup, s_dummy_popup
};
int popup_gaiji_len = 0;
int popup_cur_tram_left = 0;
int popup_dest_tram_left = 0;
bool popup_dest_reached = false;
char PLAYFIELD_BLANK_ROW[] = "";
int HUD_POWER_COLORS[4] = { 0 };
char gHUD_HP_BLANK[] = "";
int HUD_HP_COLORS[4] = { 0 };
char gsENEMY[] = "\xEA\xEB\xEC";
int hud_bar_max = 0;
char gsHISCORE[] = "\xD6\xD7\xD8\xD9";
char gsSCORE[] = "\xD7\xD8\xD9";
char gsREIGEKI[] = "\xDA\xDB";
char gsBOMB[] = "\xE0\xE1";
char gsREIMU[] = "\xDC\xDD";
char gsPLAYER[] = "\xE2\xE3";
char gsREIRYOKU[] = "\xDE\xDF";
char gsPOWER[] = "\xE4\xE5";
char glEASY[] = "\xAE\xAA\xBC\xC2\x02\x02\x02\x00"
"\xB7\xB8\xB9\xBA\xBB\x02\x02\x00"
"\xB1\xAA\xB9\xAD\x02\x02\x02\x00"
"\xB5\xBE\xB7\xAA\xB2\xB3\x02\x00"
"\xAE\xC1\xBD\xB9\xAA\x02\x02\x00";
int hud_hp_bar_value_prev = 0;

bool quit = false;
SPPoint homing_target;
unsigned char elly_scythe_flag = 0;
PlayfieldMotion elly_scythe_motion;
Subpixel bg_shape_flyout_speed;
unsigned char yuuka6_bg_fade = 0;
yuuka6_bg_shape_t bg_shapes[64];
unsigned char yuuka6_bg_state = 0;
void (near pascal *near bg_shape_clip)(yuuka6_bg_shape_t near&) = 0;
unsigned char yuuka6_bg_palette_latch = 0;
main_patnum_t bg_shape_patnum = PAT_STAGE;
unsigned char elly_scythe_mode = 0;
int elly_scythe_frame = 0;
unsigned char elly_scythe_angle = 0;
SubpixelLength8 elly_scythe_speed;
char elly_scythe_turn = 0;
int elly_orbit_frame = 0;
vc_t circles_color = 0;
gather_template_t gather_template;
unsigned char elly_pattern_group = 0;
Explosion explosions_small[16];
Explosion explosions_big;
int explosion_big_frame = 0;
custom_t custom_entities[32];
thicklaser_t thicklaser_template;
int gengetsu_damage_flash_cycle = 0;
bool kurumi_special_turn_toggle = false;
unsigned char kurumi_unknown_state = 0;
int MARISA_BIT_HP = 0;
int marisa_bit_angle_speed = 0;
int bits_alive = 0;
int bit_center_x = 0;
int bit_center_y = 0;
bool bit_fire = false;
int marisa_pattern_variant = 0;
int marisa_palette_direction = 0;
int marisa_bitless_cycle = 0;
int marisa_prev_mode = 0;
int marisa_prev_bits_alive = 0;
int mugetsu_damage_flash_cycle = 0;
int mugetsu_gather_frame_offset = 0;
SPPoint mugetsu_anchor;
nearfunc_t_near mugetsu_transition_func = (nearfunc_t_near)ps3_compat_null_void;
int mugetsu_phase2_mode = 0;
int yuuka5_move_state = 0;
int yuuka5_sweep_x = 0;
int yuuka5_cloud_step = 0;
int yuuka5_cloud_accum = 0;
int yuuka5_palette_tone = 0;
unsigned char yuuka6_aux_flag = 0;
int yuuka6_damage_flash_cycle = 0;
unsigned char yuuka6_mirror_state = 0;
SPPoint yuuka6_mirror_pos;
unsigned char yuuka6_mirror_damage = 0;
int yuuka6_mirror_damage_flash_cycle = 0;
unsigned char YUUKA6_PHASE2_FLY_ANGLES[8] = { 0 };
int yuuka6_phase2_fly_path = 0;
unsigned char yuuka6_sprite_flag = 0;
unsigned char yuuka6_aux_state = 0;
SPPoint yuuka6_aux_pos;
int yuuka6_anim_frame = 0;
unsigned char yuuka6_pattern_prev = 0;

bullet_t bullets[440];
thicklaser_t thicklasers[2];
unsigned long circles[40];
gather_t gather_circles[16];
SPPoint drawpoint;
spark_t sparks[96];
enemy_t enemies[32];
pointnum_t pointnums[400];
int group_fixedspeed = 0;
int group_i_spread_angle = 0;
int group_i_absolute_angle = 0;

unsigned char DemoBuf[512] = { 0 };
char demo_fn[] = "";
input_t key_det = 0;
int shiftkey = 0;
int DEMOPLAY_BINARY_OP = 0;
bool gDEMO_PLAY = false;

static char eyename_buf[16] = "eye0.cdg";
extern "C" char *eyename = eyename_buf;
void* Ems = 0;
char EMS_NAME[] = "TH04EMS";
static char bbname_buf[16] = "bb0.cdg";
extern "C" char *bbname = bbname_buf;
char FACESET_REIMU_FN_0[] = "";
char FACESET_MARISA_FN_0[] = "";
cdg_slot_t cdg_slots[64] = {};

int frames_unused = 0;
int total_slow_frames = 0;
int total_frames = 0;
int page_front = 0;
int page_back = 0;
int total_std_frames = 0;
int enemies_gone = 0;
int enemies_killed = 0;
int gameover_fade_frame = 0;
bool gameover_erase_in = false;
bool gameover_erase_out = false;
char gGAMEOVER[] = "";
char maine_binary[] = "";
int continues_used = 0;
char gCONTINUE_QUESTION[] = "";
char gYES[] = "";
char gNO[] = "";
char gCREDIT[] = "";
void* dialog_p = 0;
static char dialog_fn_buf[32] = "_DM00.TXT";
extern "C" char *dialog_fn = dialog_fn_buf;
static char dialog_fn_yuuka5_defeat_bad_buf[32] = "_DM04B.TXT";
extern "C" char *dialog_fn_yuuka5_defeat_bad = dialog_fn_yuuka5_defeat_bad_buf;
int script_param_number_default = 0;
int dialog_side = 0;
unsigned char dialog_kanji_buf[64] = { 0 };
int dialog_cursor = 0;
char FACESET_REIMU_FN_1[] = "";
char FACESET_MARISA_FN_1[] = "";
int number_of_calls_to_this_function_during_extra = 0;
char FACESET_MUGETSU_DEFEAT_FN[] = "";
char FACESET_GENGETSU_DEFEAT_FN[] = "";
char BOMB_BG_REIMU_FN[] = "";
char BOMB_BG_MARISA_FN[] = "";
void near *std_enemy_scripts[32] = { 0 };
enemy_t* enemy_cur = 0;
uint8_t __seg *std_seg = 0;
unsigned int bb_txt_seg = 0;
char bb_txt_fn[] = "";
char bb_txt2_fn[] = "";
static char map_fn_buf[32] = "ST00.MAP";
extern "C" char *map_fn = map_fn_buf;
map_section_tiles_t __seg *map_seg = 0;
mpn_t mpn_slots[MPN_COUNT] = {};
bool mpn_show_palette_on_load = true;
int drawpoint_y = 0;
int drawpoint_x = 0;
vram_y_t scroll_line_on_page[2] = { 0 };
unsigned char byte_250FE = 0;
unsigned char byte_25104 = 0;
unsigned int word_25100 = 0;
signed char tile_row_in_section = 0;
int std_map_section_id = 0;
SubpixelLength8 near *std_scroll_speed = 0;
vram_offset_t tile_ring[25][32] = { 0 };
uint16_t spark_ring_offset = 0;
int checkerboard = 0;
bool halftiles_dirty[TILE_FLAGS_Y][TILES_MEMORY_X] = { false };
int carpet_lighting_cel = 0;
int carpet_light_level = 0;
int CARPET_TILE_IMAGE_VOS = 0;
uint8_t CARPET_LIGHTING_ANIM[8][24] = { 0 };
extern const uint16_t TILE_SECTION_OFFSETS[TILE_SECTION_COUNT_MAX] = {
	0 * 320, 1 * 320, 2 * 320, 3 * 320, 4 * 320, 5 * 320, 6 * 320, 7 * 320,
	8 * 320, 9 * 320, 10 * 320, 11 * 320, 12 * 320, 13 * 320, 14 * 320, 15 * 320,
	16 * 320, 17 * 320, 18 * 320, 19 * 320, 20 * 320, 21 * 320, 22 * 320, 23 * 320,
	24 * 320, 25 * 320, 26 * 320, 27 * 320, 28 * 320, 29 * 320, 30 * 320, 31 * 320
};
static char std_fn_buf[32] = "ST00.STD";
extern "C" char *std_fn = std_fn_buf;
void far *std_ip = 0;
int tile_render_all_time = 0;
char CFG_FN[] = "MIKO.CFG";
char main_pf_fn[] = "GENSOU.DAT";
char gaiji_fn[] = "GAMEFT.bft";
char se_fn[] = "PMD.BGM";
char op_fn[] = "OP.EXE";
nearfunc_t_near fp_23D90 = (nearfunc_t_near)ps3_compat_null_void;
nearfunc_t_near std_update = (nearfunc_t_near)ps3_compat_null_bool;
nearfunc_t_near bg_render_bombing = (nearfunc_t_near)ps3_compat_null_void;
nearfunc_t_near bg_render_not_bombing = (nearfunc_t_near)ps3_compat_null_void;
nearfunc_t_near bg_render_bombing_func = (nearfunc_t_near)ps3_compat_null_void;
unsigned char bgm_title_id = 0;
int boss_bomb_invincibility_frames = 0;
int bullet_special_turns_max = 0;
int bullet_special_speed_delta = 0;
int pellets_render_count = 0;
bool turbo_mode = false;
int max_valued_point_items = 0;
unsigned int bbufsiz = 0;
SubpixelLength8 scroll_speed;
Subpixel scroll_last_delta;
int score_unused = 0;
bool hiscore_popup_shown = false;
char hud_lives_extra[] = "";
char hud_bombs_extra[] = "";
int boss_bgm_frame = 0;
int boss_bgm_title_len = 0;
void* item_splashes = 0;
char item_splash_last_id = 0;
SubpixelLength8 scroll_subpixel_line;
static char bb_playchar_bb_fn_buf[16] = "bb0.bb";
extern "C" char *bb_playchar_bb_fn = bb_playchar_bb_fn_buf;
static char bb_playchar_cdg_fn_buf[16] = "bb0.cdg";
extern "C" char *bb_playchar_cdg_fn = bb_playchar_cdg_fn_buf;
unsigned int bb_playchar_seg = 0;
int bomb_frame = 0;
bool scroll_active = false;
Palette8 bomb_palette_color_backup;
int scroll_line = 0;
void* bomb_stars = 0;
Subpixel miss_explosion_radius;
unsigned char miss_explosion_angle = 0;
scoredat_section_t hi;
int boss_phase_state = 0;
int tile_ring_scroll_row_prev = 0;
int scroll_row_advance_current = 0;
int scroll_row_advance_previous = 0;
char aGAME_PAUSE_SPACES_1[] = "";
char aGAME_PAUSE_SPACES_2[] = "";
char aGAME_PAUSE_SPACES_3[] = "";
char gsCHUUDAN[] = "";
char gsSAIKAI[] = "";
char gsSHUURYOU[] = "";
int bgm_timer_divisor = 0;

PlayfieldPoint PlayfieldMotion::update_seg1() { PlayfieldPoint p; p.x.v = 0; p.y.v = 0; return p; }
PlayfieldPoint PlayfieldMotion::update_seg3() { PlayfieldPoint p; p.x.v = 0; p.y.v = 0; return p; }

bool shots_hittest_against_boss = false;
SPPoint shot_velocity_set(SPPoint*, unsigned char) { SPPoint p; p.x.v = 0; p.y.v = 0; return p; }
void pascal near pointnums_add_white(subpixel_t center_x, subpixel_t center_y, uint16_t points) {}
void pascal near pointnums_add_yellow(subpixel_t center_x, subpixel_t center_y, uint16_t points) {}
nearfunc_t_near bullets_add_regular = (nearfunc_t_near)ps3_compat_null_void;
nearfunc_t_near bullets_add_special = (nearfunc_t_near)ps3_compat_null_void;
void thicklaser_add(void) {}

#endif
