# Native MAIN source closure frontier

MAIN is the remaining large TH04 product-build graph. This is a read-only
inventory of maintained `src/main` C/C++ and `.inl` files, not a standalone
build claim. It does not revisit the two deferred non-exact MAIN functions.
Replay the inventory with
`python3 scripts/probes/inventory_th04_native_main.py`.
The v898 read-only result is retained at
`.analysis/reconstruction/probes/native-main-inventory-v898-20260928/inventory.json`.

| Missing quoted include class | Unique paths | References |
| --- | ---: | ---: |
| `.h` / `.hpp` declarations | 65 | 722 |
| `.cpp` composite fragments | 35 | 35 |
| `.inl` composite fragments | 2 | 2 |
| **Total** | **102** | **759** |

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
The inventory now joins historical include paths to the local exact-unit
replay ledger. It identifies maintained source for 32 of 37 missing body
fragment paths. Five have no direct replay mapping:
`th04/dialog.cpp`, `th04/gsinit.cpp`, `th04/m4tail.inl`,
`th04/main/dialog/init_exit.inl`, and `th04/y5p2.cpp`. Local files with
plausible related names exist, but composition must be checked against the
target producer and accepted replay before claiming ownership.

This include inventory is only the first frontier. For example, the 62
`th04/main/frames.h` references need declarations for frame counters, while
their original storage lives in separate `frames[data].asm` and
`frames[bss].asm` producers in the reference build. No corresponding owned
MAIN state producers are present yet. A native link manifest must account for
data/BSS and not infer completeness from closing the include list alone.

Recover TH04-specific declarations under `src/main` or proved `src/shared`
ownership, with a build-time include projection if an accepted historical
include path must remain for strict replay. The ReC98 headers are candidate
material only; a forwarding layer is a temporary migration boundary, not
standalone product closure. A first native MAIN compile frontier should use
a strict ordered source manifest and report its exact unresolved/include
set before changing shared ABI or segment groups. Rebuild every affected
accepted unit after common declaration or layout changes.
