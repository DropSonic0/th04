#include "src/shared/platform/types.hpp"
#include "src/op/score/scoredat.hpp"

// High-score menu state is shared with the character selection menu.
op_scoredat_section_t hi;
op_scoredat_section_t hi2;
unsigned char rank;
unsigned char cleared_with[2][5];
bool extra_unlocked;
