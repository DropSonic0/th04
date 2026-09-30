// TH04 large-model palette_entry_rgb support.
//
// The pinned ReC98 archive member uses the small/near model and returns with
// RET after consuming a near filename.  MAIN declares this entry point as a
// far Pascal function.  Keep the RGB-file transformation local while using
// the maintained large-model file owners for the far pointer and return ABI;
// this is not an authored MAIN exactness claim.

#include "src/shared/hardware/graphics.hpp"
#include "src/shared/runtime/api.hpp"

int TH04_PASCAL palette_entry_rgb(const char TH04_PTR *filename)
{
    if(!file_ropen(filename)) {
        return FileNotFound;
    }

    unsigned char raw[48];
    const int got = file_read(reinterpret_cast<void far *>(raw), sizeof(raw));
    file_close();
    if(got != sizeof(raw)) {
        return InvalidData;
    }

    for(int i = 0; i < sizeof(raw); i++) {
        const int color = i / COMPONENT_COUNT;
        const int component = i % COMPONENT_COUNT;
        Palettes.colors[color].v[component] =
            static_cast<unsigned char>((raw[i] << 4) | raw[i]);
    }
    return NoError;
}
