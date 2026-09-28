#ifndef TH04_MAIN_QUIT_HPP
#define TH04_MAIN_QUIT_HPP

#include "src/shared/platform/types.hpp"

typedef enum {
	Q_KEEP_RUNNING = 0,
	Q_QUIT_TO_OP = 1,
	Q_NEXT_STAGE = 2,
} quit_t;

extern quit_t quit;

#endif
