// Compiler-routing helper for the historical tile.cpp umbrella.
//
// The target's REP STOSD producer is maintained separately as an
// evidence-backed assembler owner.  This natural C++ helper only supplies the
// source closure needed by the umbrella TU; it is not an exactness claim.
static void near tiles_invalidate_set_all(uint32_t value)
{
	const bool dirty = (value != 0);
	for(int y = 0; y < TILE_FLAGS_Y; y++) {
		for(int x = 0; x < TILES_MEMORY_X; x++) {
			halftiles_dirty[y][x] = dirty;
		}
	}
}
