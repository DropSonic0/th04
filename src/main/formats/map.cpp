#pragma option -zCEND_TEXT -zPmain_01

#include "src/shared/runtime/api.hpp"
#include "src/main/formats/map.hpp"
#include "src/shared/config/resident.hpp"

extern char *map_fn;

void near map_load(void)
{
    map_header_t mh;

    map_fn[3] = resident->stage_ascii;
    file_ropen(map_fn);
    file_read(&mh, sizeof(mh));

    uint16_t real_size = (uint16_t)(((const uint8_t *)&mh.size)[0] | (((const uint8_t *)&mh.size)[1] << 8));

    map_free();
    map_seg = reinterpret_cast<map_section_tiles_t __seg *>(hmem_allocbyte(real_size));
    file_read(map_seg, real_size);
    file_close();

    uint16_t *words = reinterpret_cast<uint16_t *>(map_seg);
    size_t num_words = real_size / sizeof(uint16_t);
    for (size_t i = 0; i < num_words; i++) {
        uint8_t *p = reinterpret_cast<uint8_t *>(&words[i]);
        words[i] = (uint16_t)(p[0] | (p[1] << 8));
    }
    printf("[TH04 PS3 Map] map_load: loaded and byte-swapped %u words for stage %c\n",
        (unsigned)num_words, resident->stage_ascii);
}

void near map_free(void)
{
    if(map_seg) {
        hmem_free(map_seg);
        map_seg = nullptr;
    }
}
