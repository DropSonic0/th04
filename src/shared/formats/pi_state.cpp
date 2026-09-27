#include "src/shared/formats/pi.hpp"

// Six independent TH04 PI slots. pi_load() fills each header and far buffer.
PiHeader pi_headers[PI_SLOT_COUNT];
void far *pi_buffers[PI_SLOT_COUNT];
