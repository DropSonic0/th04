#ifndef GAME
#define GAME 4
#endif

#include "src/shared/platform/ps3_compat.hpp"

#ifndef __TURBOC__

#include <stdio.h>
#include <string.h>

#if defined(__PS3__) || defined(CELL_SDK) || defined(__CELLOS_LV2__) || defined(SN_TARGET_PS3)
#include <PSGL/psgl.h>
#include <PSGL/psglu.h>
#include <sys/sys_time.h>
#include <cell/audio.h>
#include <cell/sysmodule.h>
#include <sys/ppu_thread.h>
#include <sys/timer.h>
#endif

#include "src/shared/runtime/api.hpp"
#include "src/shared/hardware/graphics.hpp"
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
        }
#endif
        g_ps3_audio_initialized = 1;
    }

    void ps3_audio_finish(void) {
#if defined(__PS3__) || defined(CELL_SDK) || defined(__CELLOS_LV2__) || defined(SN_TARGET_PS3)
        if (g_ps3_audio_initialized) {
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
    static GLuint g_psgl_texture = 0;
    static uint32_t g_psgl_rgba_buffer[640 * 400];
    static PSGLdevice* g_psgl_device = NULL;
    static PSGLcontext* g_psgl_context = NULL;
#endif

    void ps3_psgl_init(void) {
        if (g_psgl_initialized) return;
        printf("[TH04 PS3 PSGL] Initializing PSGL...\n");
#if defined(__PS3__) || defined(CELL_SDK) || defined(__CELLOS_LV2__) || defined(SN_TARGET_PS3)
        cellSysmoduleLoadModule(CELL_SYSMODULE_GCM_SYS);

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
            glGenTextures(1, &g_psgl_texture);
            glBindTexture(GL_TEXTURE_2D, g_psgl_texture);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 640, 400, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        }
#endif
        g_psgl_initialized = 1;
        printf("[TH04 PS3 PSGL] PSGL Initialized Successfully\n");
    }

    static inline uint8_t ps3_component_at_tone(uint8_t component, int tone) {
        int base = (component >> 4);
        if (tone <= 100) {
            return (uint8_t)((base * tone * 255) / (100 * 15));
        }
        return (uint8_t)((15 - (((15 - base) * (200 - tone)) / 100)) * 255 / 15);
    }

    void ps3_psgl_swap(void) {
#if defined(__PS3__) || defined(CELL_SDK) || defined(__CELLOS_LV2__) || defined(SN_TARGET_PS3)
        if (!g_psgl_initialized) return;

        if (psglGetCurrentContext() != g_psgl_context) {
            if (g_psgl_context && g_psgl_device) {
                psglMakeCurrent(g_psgl_context, g_psgl_device);
            }
        }
        if (!psglGetCurrentContext()) return;

        if (1) {
            uint32_t lut[16];
            int tone = (int)PaletteTone;
            if (tone < 0) tone = 0;
            else if (tone > 200) tone = 200;

            for (int c = 0; c < 16; c++) {
                uint8_t r = ps3_component_at_tone(Palettes[c].v[0], tone);
                uint8_t g = ps3_component_at_tone(Palettes[c].v[1], tone);
                uint8_t b = ps3_component_at_tone(Palettes[c].v[2], tone);
                lut[c] = (0xFFu << 24) | ((uint32_t)b << 16) | ((uint32_t)g << 8) | (uint32_t)r;
            }

            uint32_t* dst = g_psgl_rgba_buffer;
            for (int y = 0; y < 400; y++) {
                int row_off = y * 80;
                for (int byte_x = 0; byte_x < 80; byte_x++) {
                    int offset = row_off + byte_x;
                    uint8_t b_byte = VRAM_PLANE_B ? VRAM_PLANE_B[offset] : 0;
                    uint8_t r_byte = VRAM_PLANE_R ? VRAM_PLANE_R[offset] : 0;
                    uint8_t g_byte = VRAM_PLANE_G ? VRAM_PLANE_G[offset] : 0;
                    uint8_t e_byte = VRAM_PLANE_E ? VRAM_PLANE_E[offset] : 0;

                    for (int bit = 7; bit >= 0; bit--) {
                        uint8_t col = ((b_byte >> bit) & 1) |
                                      (((r_byte >> bit) & 1) << 1) |
                                      (((g_byte >> bit) & 1) << 2) |
                                      (((e_byte >> bit) & 1) << 3);
                        *dst++ = lut[col];
                    }
                }
            }

            glBindTexture(GL_TEXTURE_2D, g_psgl_texture);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 640, 400, GL_RGBA, GL_UNSIGNED_BYTE, g_psgl_rgba_buffer);

            glDisable(GL_DEPTH_TEST);
            glEnable(GL_TEXTURE_2D);

            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrthof(0.0f, 640.0f, 400.0f, 0.0f, -1.0f, 1.0f);

            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();

            static const GLfloat vtx[] = {
                0.0f,   0.0f,
                640.0f, 0.0f,
                0.0f,   400.0f,
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

            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            glDisableClientState(GL_VERTEX_ARRAY);

            psglSwap();
        }
#endif
    }

    // C-linkage API functions
    int TH04_PASCAL pf_open_member(const char far *path);
    unsigned TH04_PASCAL pf_read_member(void far *out, unsigned count);
    void TH04_PASCAL pf_seek_member(long offset, int origin);
    void TH04_PASCAL pf_close_member(void);

    FILE* g_dos_handles[MAX_DOS_HANDLES] = {0};
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
        0,            // bgm_mode
        0,            // stage
        '0',          // playchar_ascii
        '0',          // stage_ascii
        12345,        // rand
        0,            // se_mode
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
                } else {
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
        } else if (!g_current_file && strstr(filename, "GENSOU.SCR")) {
            printf("[TH04 PS3 I/O] Save file missing (%s), creating default zeroed file...\n", filename);
            FILE *fcreate = fopen(filename, "wb");
            if (!fcreate) {
                char path[512];
                snprintf(path, sizeof(path), "/app_home/%s", filename);
                fcreate = fopen(path, "wb");
            }
            if (fcreate) {
                char dummy[512] = {0};
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
            g_pf_active_handle = pf_handle;
            printf("[TH04 PS3 I/O] Successfully loaded archive member from GENSOU.DAT: %s (pf handle=%d)\n", filename, g_pf_active_handle);
            return 1;
        }

        printf("[TH04 PS3 I/O] ERROR: Could not open file or archive member: %s\n", filename);
        return 0;
    }

    int TH04_PASCAL file_read(void far *buf, unsigned wsize) {
        if (g_pf_active_handle) {
            unsigned read_bytes = pf_read_member(buf, wsize);
            printf("[TH04 PS3 I/O] pf file_read requested %u bytes, read %u bytes\n", wsize, read_bytes);
            return (int)read_bytes;
        }
        if (!g_current_file || !buf) {
            printf("[TH04 PS3 I/O] file_read failed: no active file or NULL buffer\n");
            return 0;
        }
        int read_bytes = (int)fread(buf, 1, wsize, g_current_file);
        printf("[TH04 PS3 I/O] file_read requested %u bytes, read %d bytes\n", wsize, read_bytes);
        return read_bytes;
    }

    long TH04_PASCAL file_size(void) {
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

    int TH04_PASCAL iatan2(int y, int x) { return 0; }
    int TH04_PASCAL isqrt(long x) { return 0; }
    int TH04_PASCAL ihypot(int x, int y) { return 0; }

    void TH04_PASCAL vsync_start(void) {
        printf("[TH04 PS3 Hardware] vsync_start()\n");
        ps3_psgl_init();
        ps3_audio_init();
        vsync_Count1++;
        vsync_Count2++;
    }
    void TH04_PASCAL vsync_end(void) {
        vsync_Count1++;
        vsync_Count2++;
        ps3_psgl_swap();
    }
    void TH04_PASCAL graph_start(void) {
        printf("[TH04 PS3 Graphics] graph_start() called\n");
        ps3_psgl_init();
        ps3_audio_init();
    }

    int TH04_PASCAL js_start() {
        printf("[TH04 PS3 JS] js_start() initialized\n");
        return 0;
    }
    void TH04_PASCAL js_end(void) {}
    int TH04_PASCAL js_sense(void) { return 0; }

    void TH04_PASCAL cdg_load_single(int slot, const char *fn, int image) {
        printf("[TH04 PS3 CDG] cdg_load_single(slot=%d, fn=%s, image=%d)\n", slot, fn ? fn : "NULL", image);
    }
    void TH04_PASCAL cdg_load_single_noalpha(int slot, const char *fn, int image) {
        printf("[TH04 PS3 CDG] cdg_load_single_noalpha(slot=%d, fn=%s, image=%d)\n", slot, fn ? fn : "NULL", image);
    }
    void TH04_PASCAL cdg_load_all(int slot_first, const char *fn) {
        printf("[TH04 PS3 CDG] cdg_load_all(slot_first=%d, fn=%s)\n", slot_first, fn ? fn : "NULL");
    }
    void TH04_PASCAL cdg_load_all_noalpha(int slot_first, const char *fn) {
        printf("[TH04 PS3 CDG] cdg_load_all_noalpha(slot_first=%d, fn=%s)\n", slot_first, fn ? fn : "NULL");
    }
    void TH04_PASCAL cdg_free(int slot) {
        printf("[TH04 PS3 CDG] cdg_free(slot=%d)\n", slot);
    }
    void TH04_PASCAL cdg_free_all(void) {
        printf("[TH04 PS3 CDG] cdg_free_all()\n");
    }
    void TH04_PASCAL cdg_put_8(screen_x_t left, vram_y_t top, int slot) {}
    void TH04_PASCAL cdg_put_noalpha_8(screen_x_t left, vram_y_t top, int slot) {}
    void TH04_PASCAL cdg_put_plane(screen_x_t left, vram_y_t top, int slot, int plane) {}

    void TH04_PASCAL grcg_circle(screen_x_t center_x, vram_y_t center_y, unsigned r) {}
    void TH04_PASCAL grcg_circlefill(int x, int y, unsigned r) {}
    void TH04_PASCAL grcg_setcolor(int mode, vc2 color) {}

    void TH04_PASCAL super_clean(int min_pat, int max_pat) {
        printf("[TH04 PS3 Super] super_clean(min=%d, max=%d)\n", min_pat, max_pat);
    }
    void TH04_PASCAL super_roll_put_1plane(int x, int y, int num, int pattern_plane, unsigned put_plane) {}
    void TH04_PASCAL super_put_1plane(int x, int y, int num, int pattern_plane, unsigned put_plane) {}
    void TH04_PASCAL super_wave_put(int x, int y, int num, int len, char amp, int ph) {}
    void TH04_PASCAL super_zoom(int x, int y, int num, int zoom) {}

    void TH04_PASCAL text_putsa(unsigned x, unsigned y, const char TH04_PTR *str, unsigned atrb) {
        if (str) printf("[TH04 PS3 Text] text_putsa(x=%u, y=%u, str=\"%s\")\n", x, y, str);
    }
    void TH04_PASCAL text_putca(unsigned x, unsigned y, unsigned ch, unsigned atrb) {}
    void TH04_PASCAL text_fillca(unsigned ch, unsigned atrb) {}

    void TH04_PASCAL key_beep_off(void) {
        printf("[TH04 PS3 Input] key_beep_off() called\n");
    }
    void TH04_PASCAL text_systemline_hide(void) {
        printf("[TH04 PS3 Text] text_systemline_hide() called\n");
    }
    void TH04_PASCAL text_cursor_hide(void) {
        printf("[TH04 PS3 Text] text_cursor_hide() called\n");
    }

    int TH04_PASCAL select_for_rank(int for_easy, int for_normal, int for_hard, int for_lunatic) { return for_normal; }

    unsigned super_patnum = 0;
    void __seg *super_buffer = 0;
    unsigned super_patdata[512] = {0};
    unsigned super_patsize[512] = {0};

    unsigned pferrno = 0;
    unsigned char pfkey = 0;
    int TH04_PASCAL pf_hook_install(void) { return 1; }
    void TH04_PASCAL pf_hook_remove(void) {}

    void pascal near tiles_invalidate_around(const SPPoint) {}
    void pascal vector2(int &ret_x, int &ret_y, unsigned char angle, int length) {}
    void pascal near vector2_near(SPPoint near &ret, unsigned char angle, subpixel_t length) {}
    void near playfield_fill(void) {}
    void near playfield_fillm_0_40_384_274(void) {}
    void near grcg_fill_playfield_rows(void) {}
    void near z_super_roll_put_tiny_16x16_raw(int) {}
    void near z_super_roll_put_tiny_32x32_raw(int) {}
    void near pellets_render_top(void) {}
    void near pellets_render_bottom(void) {}
    void near pellets_render(void) {}
    void TH04_PASCAL grcg_boxfill(int x1, int y1, int x2, int y2) {}
    int TH04_PASCAL grc_setclip(int xl, int yt, int xr, int yb) { return 0; }
    void near clear_dwords(void) {}
    void TH04_PASCAL text_clear(void) {}
    void pascal near pointnums_update(void) {}
    void pascal near pointnums_render(void) {}
    void pascal near pointnums_init(void) {}
    void near playperf_raise(int) {}
    void near playperf_lower(int) {}
    void TH04_PASCAL graph_clear(void) {}
    void TH04_PASCAL ems_free(unsigned) {}
    void TH04_PASCAL graph_hide(void) {}
    void pascal near hud_score_put(void) {}
    void TH04_PASCAL graph_400line(void) {}
    void TH04_PASCAL graph_scrollup(unsigned line) {}
    void near enemy_bullet_template_push(void) {}
    void near sub_3680(void) {}
    int near IRand(void) { return 0; }
    void near sub_11DE6(void) {}
    void near item_splash_dot_render(void) {}
    void near SHOT_LASER_PUT_RAW(void) {}
    void near sub_BAEE(void) {}
    void near tiles_fill_initial(void) {}
    void near bgm_timer_start(void) {}
    void near bgm_timer_stop(void) {}
}

// C++ linkage VRAM plane pointers and allocated buffers (32KB each for PC-98 640x400 / 8)
static uint8_t s_vram_b[32768] = {0};
static uint8_t s_vram_r[32768] = {0};
static uint8_t s_vram_g[32768] = {0};
static uint8_t s_vram_e[32768] = {0};

uint8_t far *VRAM_PLANE_B = s_vram_b;
uint8_t far *VRAM_PLANE_R = s_vram_r;
uint8_t far *VRAM_PLANE_G = s_vram_g;
uint8_t far *VRAM_PLANE_E = s_vram_e;

// C++ linkage helper stubs and data
void near grcg_setmode_tdw(void) {}
void near grcg_setmode_rmw(void) {}
void near grcg_setcolor_direct_raw(void) {}
void near grcg_vline(int, int, int) {}
void near grcg_line(int, int, int, int) {}

void TH04_PASCAL egc_shift_left(screen_x_t x1, vram_y_t y1, screen_x_t x2, vram_y_t y2, pixel_t dots) {}
void TH04_PASCAL egc_shift_right(screen_x_t x1, vram_y_t y1, screen_x_t x2, vram_y_t y2, pixel_t dots) {}
void TH04_PASCAL egc_shift_up(screen_x_t x1, vram_y_t y1, screen_x_t x2, vram_y_t y2, pixel_t dots) {}
void TH04_PASCAL egc_shift_down(screen_x_t x1, vram_y_t y1, screen_x_t x2, vram_y_t y2, pixel_t dots) {}

void near reimu_marisa_backdrop_colorfill(void) {}
void near yuuka5_backdrop_colorfill(void) {}

void near super_roll_put(int, int, int) {}
void near super_large_put(int, int, int) {}
void near z_super_put_16x16_mono_raw(int) {}
int TH04_PASCAL super_convert_tiny(int num) { return 0; }

void near playfield_checkerboard_grcg_tdw_(void) {}
void near pointnums_invalidate(void) {}

void pascal near sparks_add_circle(Subpixel center_x, Subpixel center_y, subpixel_t distance, int count) {}
void pascal sparks_add_random(Subpixel center_x, Subpixel center_y, subpixel_t radius_min, int count) {}
void near sparks_update(void) {}
void near sparks_render(void) {}

void near tiles_bb_invalidate_raw(int) {}
void near tiles_redraw_invalidated(void) {}
void near tiles_render_all(void) {}
void near tiles_bb_put_raw(int) {}

void near input_reset_sense(void) {}
void near input_sense(void) {}
void pascal near scoredat_encode(scoredat_section_t near *hi) {}
uint8_t pascal near scoredat_decode(scoredat_section_t near *hi) { return 0; }
void near egc_start_copy_noframe(void) {}

void near sub_12024(void) {}

static bool ps3_compat_null_bool(void) { return false; }
static void ps3_compat_null_void(void) {}

point_t tile_invalidate_box;
nearfunc_t_near bullet_template_tune = (nearfunc_t_near)ps3_compat_null_void;
void gather_point_render(int, int) {}
void score_update_and_render(void) {}
void shot_level_update(void) {}
void bb_txt_put_8_raw(unsigned int, unsigned int) {}
int load_playchar_resources = 0;
void cdg_put_plane_roll_8(int, int, int, vram_plane_t, unsigned char*) {}
void bullets_and_gather_invalidate(void) {}

uint16_t pascal near randring1_next16_mod(uint16_t divisor) { return 0; }
uint16_t pascal near randring2_next16_mod(uint16_t divisor) { return 0; }

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
unsigned char boss_statebyte[16] = {0};

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

extern const short CosTable8[256] = {0};
extern const short SinTable8[256] = {0};

extern const int ITEM_PATNUM[IT_COUNT] = {0};
extern const Subpixel ITEM_MISS_VELOCITIES[MISS_FIELD_COUNT][2][ITEM_MISS_COUNT] = {0};
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
unsigned char MIDBOSS3_FLY_ANGLES[8] = {0};
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
char STAGE_TITLES[8][32] = {0};
int stage_title_len = 0;
char BGM_TITLES[16][32] = {0};
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
nearfunc_t_near SHOT_FUNCS_REIMU_A[10] = {0};
nearfunc_t_near playchar_shot_funcs[10] = {0};
nearfunc_t_near SHOT_FUNCS_REIMU_B[10] = {0};
nearfunc_t_near player_bomb_func = (nearfunc_t_near)ps3_compat_null_void;
nearfunc_t_near playchar_bomb_func = (nearfunc_t_near)ps3_compat_null_void;
nearfunc_t_near SHOT_FUNCS_MARISA_A[10] = {0};
nearfunc_t_near SHOT_FUNCS_MARISA_B[10] = {0};
unsigned char player_state_unknown_0 = 0;
unsigned char player_state_unknown_1 = 0;
unsigned char player_state_unknown_2 = 0;
unsigned char player_state_unknown_3 = 0;
int player_miss_animation_frame = 0;

uint16_t randring_p = 0;
unsigned char randring[256] = {0};

unsigned char BOSS_ITEM_DROPS[8] = {0};
unsigned char ENEMY_DROPS[8] = {0};
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
char POPUP_STRINGS[8][32] = {0};
int popup_gaiji_len = 0;
int popup_cur_tram_left = 0;
int popup_dest_tram_left = 0;
bool popup_dest_reached = false;
char PLAYFIELD_BLANK_ROW[] = "";
int HUD_POWER_COLORS[4] = {0};
char gHUD_HP_BLANK[] = "";
int HUD_HP_COLORS[4] = {0};
char gsENEMY[] = "";
int hud_bar_max = 0;
char gsHISCORE[] = "";
char gsSCORE[] = "";
char gsREIGEKI[] = "";
char gsBOMB[] = "";
char gsREIMU[] = "";
char gsPLAYER[] = "";
char gsREIRYOKU[] = "";
char gsPOWER[] = "";
char glEASY[] = "";
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
unsigned char YUUKA6_PHASE2_FLY_ANGLES[8] = {0};
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

unsigned char DemoBuf[512] = {0};
char demo_fn[] = "";
int key_det = 0;
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
int playperf = 0;
int playperf_min = 0;
int playperf_max = 0;
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
unsigned char dialog_kanji_buf[64] = {0};
int dialog_cursor = 0;
char FACESET_REIMU_FN_1[] = "";
char FACESET_MARISA_FN_1[] = "";
int number_of_calls_to_this_function_during_extra = 0;
char FACESET_MUGETSU_DEFEAT_FN[] = "";
char FACESET_GENGETSU_DEFEAT_FN[] = "";
char BOMB_BG_REIMU_FN[] = "";
char BOMB_BG_MARISA_FN[] = "";
void* std_enemy_scripts = 0;
enemy_t* enemy_cur = 0;
unsigned int std_seg = 0;
unsigned int bb_txt_seg = 0;
char bb_txt_fn[] = "";
char bb_txt2_fn[] = "";
static char map_fn_buf[32] = "ST00.MAP";
extern "C" char *map_fn = map_fn_buf;
unsigned int map_seg = 0;
int mpn_slots = 0;
bool mpn_show_palette_on_load = false;
int drawpoint_y = 0;
int drawpoint_x = 0;
vram_y_t scroll_line_on_page[2] = {0};
unsigned char byte_250FE = 0;
unsigned char byte_25104 = 0;
unsigned int word_25100 = 0;
int tile_row_in_section = 0;
int std_map_section_id = 0;
int std_scroll_speed = 0;
int tile_ring = 0;
uint16_t spark_ring_offset = 0;
int checkerboard = 0;
int halftiles_dirty = 0;
int carpet_lighting_cel = 0;
int carpet_light_level = 0;
int CARPET_TILE_IMAGE_VOS = 0;
uint8_t CARPET_LIGHTING_ANIM[8][24] = {0};
static char std_fn_buf[32] = "ST00.STD";
extern "C" char *std_fn = std_fn_buf;
int std_ip = 0;
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
int hud_lives_extra = 0;
int hud_bombs_extra = 0;
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
int TILE_SECTION_OFFSETS[16] = {0};
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
