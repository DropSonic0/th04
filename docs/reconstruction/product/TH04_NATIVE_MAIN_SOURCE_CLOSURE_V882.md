# Native MAIN source closure frontier

MAIN is the remaining large TH04 product-build graph. This is a read-only
inventory of maintained `src/main` C/C++ and `.inl` files, not a standalone
build claim. It does not revisit the two deferred non-exact MAIN functions.
Replay the inventory with
`python3 scripts/probes/inventory_th04_native_main.py`.
The current v900 read-only result is retained at
`.analysis/reconstruction/probes/native-main-inventory-v900-20260928/inventory.json`.

| Missing quoted include class | Unique paths | References |
| --- | ---: | ---: |
| `.h` / `.hpp` declarations | 64 | 660 |
| `.cpp` composite fragments | 35 | 35 |
| `.inl` composite fragments | 2 | 2 |
| **Total** | **101** | **697** |

The heaviest header edges are `th04/snd/snd.h` (68 references),
`th04/sprites/main_pat.h` (64), `th04/main/frames.h` (62), and the
player/bullet headers (41 each). Five other missing platform/header names
have no `th04/` prefix: `platform.h`, `x86real.h`, `planar.h`, `decomp.hpp`,
and `shiftjis.hpp`. These names denote needed declarations, not approval to
reuse cross-game product headers.

Eight maintained physical producers include `.cpp` fragments by historical
`th04/` paths: `boss_bg_main01.cpp`, `yuuka6_main034.cpp`,
`kurumi_update.cpp`, `elly_update.cpp`, `yuuka5.cpp`,
`mugetsu_main033.cpp`, `demo_prefix.cpp`, and `dialog/fused.cpp`. Most of
their included names are absent even from the pinned local ReC98 tree.
Related function bodies already exist as separate maintained `src/main`
translation units, but their composition and near-call segment ownership must
be reconciled before adding them to a native MAIN link manifest. Compiling
both a composite and its included child as objects would duplicate publics.
The local ReC98 reference has files at all 65 missing header paths, but only
two of the 35 missing `.cpp` paths and neither missing `.inl` path. This is a
source-location observation, not evidence that those headers are ready for a
TH04-owned product build. The existing maintained function bodies must be
composed into their physical translation units and their state owners found.
The inventory now joins historical include paths to all three exact-replay
composition surfaces: ordinary unit overlays, build inserts, and source
splits. Maintained source maps 36 of 37 missing body fragment paths. The four
v898 false negatives are `th04/gsinit.cpp`, `th04/m4tail.inl`,
`th04/main/dialog/init_exit.inl`, and `th04/y5p2.cpp`; their checked-in replay
rules already bind them to maintained source. Only `th04/dialog.cpp` remains
without a complete local physical composition. It is the historical root
wrapper for the dialog producer, whose accepted bodies currently enter
through several fragment and scaffold patch surfaces.

## MAIN frame-state closure

The first declaration/data batch localizes the former
`th04/main/frames.h` dependency at
`src/main/include/th04/main/frames.h`. This preserves the historical include
spelling through the product-owned `-Isrc/main/include` root and closes 62
quoted-include references without changing accepted TU source composition.
`src/main/core/frame_state.asm` owns the ten public frame/slowdown symbols in
`_DATA` and `_BSS`; initialized totals begin at zero while stage-relative and
slowdown state remains uninitialized until normal game setup.

`scripts/probes/probe_th04_native_main_frames.py` attests the pinned toolchain,
compiles a large-model TC4J consumer, assembles the owner with TASM32, validates
the single-module OMF producer and ten PUBDEF records, links a valid MZ, and
runs it under the pinned MS-DOS Player. Two fresh directories both print
`FRAMES_PASS`; the complete MZ is identical in both runs at SHA-256
`cbdedb41ae7b7073c8483f21b6cb572ef5b6efd4902642ddaea3baf1352d1047`
with 217 relocations, and the frame-state OMF is raw-identical at SHA-256
`9626d47ea08919bf316d21bf2214f16ffb43e829f0fa29522a72bc963a556fe7`.
This is runtime-observed declaration/storage behavior for an isolated DOS
probe. Target DATA/BSS offsets, full native MAIN linking, and PC-98 execution
remain open.

This include inventory is only the first frontier. The frame declaration and
storage batch demonstrates the required pairing, but the other 64 missing
header paths still need product-owned declarations and their data/BSS owners.
A native link manifest must account for those owners and not infer completeness
from closing the include list alone.

Recover TH04-specific declarations under `src/main` or proved `src/shared`
ownership, with a build-time include projection if an accepted historical
include path must remain for strict replay. The ReC98 headers are candidate
material only; a forwarding layer is a temporary migration boundary, not
standalone product closure. A first native MAIN compile frontier should use
a strict ordered source manifest and report its exact unresolved/include
set before changing shared ABI or segment groups. Rebuild every affected
accepted unit after common declaration or layout changes.
