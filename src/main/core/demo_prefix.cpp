#pragma option -zCDEMO_TEXT -zPmain_01

#define TH04_DEMO_PREFIX_COMBINED 1

// Original physical DEMO_TEXT producer prefix: main(), gameplay_loop(), and
// gameplay_session_init() share one TC4J translation unit. The session source
// enables word alignment only after all of its headers/declarations, allowing
// TC4J to word-align its generated switch jump table naturally.
#include "src/main/core/main.cpp"
#include "src/main/core/gameplay_loop.cpp"
#include "src/main/core/gameplay_session_init.cpp"

#undef TH04_DEMO_PREFIX_COMBINED
