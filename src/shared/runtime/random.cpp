#include "src/shared/runtime/api.hpp"

// TC4J's unsigned arithmetic gives the intended modulo-2^32 state update.
// The multiplier matches the historical PC-98 master-library LCG family;
// this product owner makes no target-byte claim.
long __cdecl random_seed = 1;

int TH04_PASCAL irand(void)
{
	const unsigned long next =
		((unsigned long)random_seed * 0x015A4E35UL) + 1UL;
	random_seed = (long)next;
	return (int)((next >> 16) & 0x7FFFUL);
}
