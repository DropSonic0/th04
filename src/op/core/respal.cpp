#ifdef __TURBOC__
#include <dos.h>
#endif

#include "src/shared/runtime/api.hpp"

// OP calls the resident-palette API only as a setup/teardown pair. Keep the
// DOS paragraph allocation real in the standalone build; the palette itself
// is never read or written by TH04 OP.
static unsigned respal_segment;

extern "C" int TH04_PASCAL respal_create(void)
{
    if(respal_segment) return 2;
#ifdef __TURBOC__
    if(_dos_allocmem(4, &respal_segment) != 0) {
        respal_segment = 0;
        return 0;
    }
#else
    respal_segment = 1;
#endif
    return 1;
}

extern "C" void TH04_PASCAL respal_free(void)
{
    if(respal_segment) {
#ifdef __TURBOC__
        _dos_freemem(respal_segment);
#endif
        respal_segment = 0;
    }
}