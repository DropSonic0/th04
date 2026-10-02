#ifndef TH04_MAIN_EMS_HPP
#define TH04_MAIN_EMS_HPP

// EMS cache area.  It stores situational CDG images which are copied into
// conventional memory only when the corresponding screen needs them.

#include "src/main/playfld.hpp"
#include "src/main/sprites/main_cdg.hpp"
#include "src/main/formats/cdg.hpp"
#include "src/shared/runtime/api.hpp"
#include <stddef.h>

#if (GAME == 5)
#define EMS_NAME "GENSOEMS"
#else
#define EMS_NAME "KAIKIEMS"
#endif

// Layout constants are intentionally explicit: TC4J narrows enum-backed
// 32-bit values in this header, so an enum cannot represent these offsets.
#define sizeof_planar_rect(w, h) \
    ((((w) / BYTE_DOTS) * h) * static_cast<uint32_t>(PLANE_COUNT))

static const uint32_t EMS_EYECATCH_OFFSET = 0;
static const uint32_t EMS_EYECATCH_END = (
    EMS_EYECATCH_OFFSET + sizeof_planar_rect(EYECATCH_W, EYECATCH_H)
);
static const uint32_t EMS_PLAYCHAR_BOMB_BG_OFFSET = 34000;
static const uint32_t EMS_PLAYCHAR_BOMB_BG_END = (EMS_PLAYCHAR_BOMB_BG_OFFSET +
    sizeof_planar_rect(BOMB_BG_W_MAX, BOMB_BG_H_MAX)
);
static const uint32_t EMS_FACESET_PLAYCHAR_OFFSET = (
    (GAME == 5) ? 100000 : 94000
);
static const uint32_t EMS_FACESET_PLAYCHAR_END = (EMS_FACESET_PLAYCHAR_OFFSET +
    (FACESET_PLAYCHAR_COUNT * sizeof_planar_rect(FACE_W, FACE_H))
);
#if (GAME == 5)
static const uint32_t EMS_FACESET_BOSS_OFFSET = 200000;
static const uint32_t EMS_FACESET_BOSS_END = (EMS_FACESET_BOSS_OFFSET +
    (FACESET_BOSS_COUNT * sizeof_planar_rect(FACE_W, FACE_H))
);
static const uint32_t EMSSIZE = 320000;
#else
static const uint32_t EMSSIZE = 180000;
#endif

extern seg_t Ems;

void near ems_allocate_and_preload_eyecatch(void);
void near bomb_bg_load__ems_preload_playchar_cdgs(void);

#if (GAME == 5)
void pascal near ems_preload_boss_faceset(const char *fn);
#endif

void near eyecatch_animate(void);

// Helper functions
// ----------------

#ifdef __TURBOC__
#define allocate_and_load_from_ems(dst_seg, src_off, size) { \
	reinterpret_cast<void __seg *>(dst_seg) = hmem_allocbyte(size); \
	ems_read(Ems, src_off, dst_seg, size); \
}
#else
#define allocate_and_load_from_ems(dst_seg, src_off, size) { \
	(void *&)dst_seg = hmem_allocbyte(size); \
	ems_read(Ems, src_off, dst_seg, size); \
}
#endif

// Assumes [Ems] to be non-null.
inline void playchar_bomb_bg_load_from_ems(void) {
    size_t size = (cdg_slots[CDG_BG_PLAYCHAR_BOMB].bitplane_size * PLANE_COUNT);
    allocate_and_load_from_ems(
        cdg_slots[CDG_BG_PLAYCHAR_BOMB].seg_colors(),
        EMS_PLAYCHAR_BOMB_BG_OFFSET,
        size
    );
}

#endif
