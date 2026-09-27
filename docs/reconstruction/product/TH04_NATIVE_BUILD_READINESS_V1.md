# TH04 native DOS build readiness

2026-09-27 compiler and source-graph observation. This is a product-build
investigation, not a new exactness or runtime acceptance claim. The two MAIN
authored-function residuals remain deferred for this lane.

## Acceptance model

The TH095 Windows i386 product build supplies the useful separation: close the
production source graph, compile every declared translation unit with the
pinned toolchain, link without unresolved symbols, verify the output format,
then validate the reconstructed executable with the same legal game data and
runtime scenarios as the original. Function-level raw matching has its own
ledger. TH04 needs this model with 16-bit OMF, DOS MZ, segment groups, real-mode
relocations, DIET containers, and PC-98 hardware behavior.

A playable TH04 build need not reproduce target file bytes or target relocation
table order. Its own relocation table must be structurally valid and must
resolve the intended code/data owners. Check MZ entry and stack, fixup sites,
segment/group layout, linker MAP ownership, and load-segment variation before
runtime acceptance. A successful TLINK exit alone cannot establish those facts.

## Repository-only compiler probe

`python3 scripts/attest_toolchain.py` passed. An isolated private directory
contained a copy of this repository's `src/` only. The compiler runner and
installed TC4J/TASM32 binaries were pinned external tools; no ReC98 source or
header tree was copied into the probe. For OP/MAINE C++ the flags were
`-c -I. -O -b- -3 -Z -d -DGAME=4 -ml`, with the artifact's `BINARY` definition.
Each translation unit had a separate object output directory. The observation
is one serial compile pass, with strict OMF parsing of each emitted object.

| Source group | Observed result | Limit |
| --- | --- | --- |
| `src/op/**/*.cpp` | 48/48 compiled to valid OMF | No OP entry TU or product link yet |
| `src/maine/**/*.cpp` | 26/26 compiled to valid OMF | No MAINE entry TU or product link yet |
| `src/shared/**/*.{cpp,c}` under OP flags | 14/19 compiled directly | Four require missing headers; BGIMAGE uses the assembly backend |
| `src/shared/hardware/bgimage.cpp` | TC4J `-B` emitted symbolic ASM; pinned TASM32 assembled valid OMF | The assembler reported one segment-alignment warning, as in the existing focused producer workflow |

The private compiler receipts are
`.analysis/native-product-probe-20260927/{op-compile,maine-compile,shared-op-compile}.json`.
Their SHA-256 values are respectively
`d61930ae38017a386de3dfca92d6647edaae5f691c45b3ffd6eeba74b3a94d05`,
`c79bb00d6504627a2519d31d82b6371732897a99d147710103db3d0487e`, and
`21ffec05e1f5c67168b656db27cb3980a25883bfd93fc28d65fa5664f3169a21`.
They are ignored host-local diagnostics, not checked-in build receipts. A
checked-in product builder must reproduce this test before accepting it.

TC4J maps long source basenames to DOS 8.3 aliases and emits object files with
those aliases. The product builder must isolate output per input and record the
actual generated object name; deriving it from the long source name produced a
false failure in the first diagnostic pass.

The four direct shared compile blockers are `vram_planes.cpp` (`planar.h`),
`determine_modes.cpp`, `pmd_resident.c`, and `mmd_resident.c` (the latter three
require `x86real.h` and `th04/snd/snd.h`). `bgimage.cpp` is the fifth direct
TCC failure and instead has the verified `-B`/TASM32 producer path. These
missing declarations must be recovered or routed through the repository's
attested compatibility boundary; they cannot be silently supplied by a ReC98
source include path.

## Current build-graph gaps

- MAIN C/C++ source has 764 quoted include sites whose paths do not resolve
  inside this repository, covering 102 distinct header names. This is a
  source-graph observation, not a count of missing semantic declarations.
- No checked-in OP or MAINE product translation unit includes their respective
  `src/op/main/main.inl` or `src/maine/end/main.inl` entry body. Other bounded
  `.inl` fragments also lack a product TU; some are historical overlapping
  replay fragments and must be selected by ownership rather than bulk-included.
- The source tree has no four-artifact product link manifest that assigns every
  C/C++/ASM TU, object order, startup object, library, segment, and output.
  ReC98 linker responses are calibration evidence, not this manifest.
- OP, MAINE, and ZUN still need a product DIET/container route. ZUN's source
  composite additionally retains the documented usage-asset input.
- The current PC-98 smoke script boots the original private disk image. It
  does not install and exercise a candidate build.

## Build lane

Start with an explicit OP/MAINE source and object graph, then attach the
missing entry bodies and shared producers. Compile all declared TUs serially
with TC4J or TC4J `-B` plus TASM32 as required; validate each OMF record and
fixup. Link with TLINK without unresolved externals, duplicate storage, fake
stubs, or target-byte patches. Record flags, ordered objects/libraries, MAP,
MZ header, entry/stack, relocation sites and resolved symbol owners in a
repeatable receipt. The output may differ bytewise from the target.

After the unpacked programs link, package the three DIET artifacts and test
each candidate in a disposable copy of the legal game image. Boot the original
first under the same emulator configuration. Compare startup, menu, gameplay,
input, sound, save/config state, and transitions using memory, event, VRAM,
palette, and audio checkpoints. Repeat with a changed DOS load segment where
the emulator supports it. Freeze a runnable candidate by source revision and
artifact hashes; later source changes need fresh runtime validation.
