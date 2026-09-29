# TH04 native MAIN product-source frontier (v1025)

This note records the 2026-09-29 compiler boundary after the native MAIN
routing batch. It is source/buildability evidence, not an exactness or startup
claim.

## Product-only C/C++ closure

`scripts/probes/probe_th04_native_main_product_sources.py` copies only the
checked-in `src/` tree and compiles every `.c`/`.cpp` under `src/main/` and
`src/shared/` with the pinned TC4J runner and product include root. TC4J has a
DOS-visible 8.3 intermediate-name limit; therefore each invocation uses a
short `uNNN` alias in the original directory. The receipt retains the
original path, source hash, alias hash, object hash, OMF record summary, and
the deterministic source-tree digest, so the alias is not a source identity
substitution.

Receipt:

```text
.analysis/reconstruction/probes/native-main-product-sources-v1025-20260929/receipt.json
SHA-256 3deba2f34c79d9c53f3d319963d17fee348f05741a592dc3465fbc7fe191c068
source_count=235  compile_pass=235  compile_fail=0
source_tree_sha256=e0324fe1ea8b6df1cd7ca8adec716877fd8355136167e6d4555e6fc58c9c9585
```

The probe performs no ASM assembly, TLINK, MZ comparison, relocation/layout
check, or runtime/startup test. The result is `compiler-observed` source
closure only.

## Physical DATA/BSS owner closure

`scripts/probes/probe_th04_native_main_state_owners.py` assembles the eight
maintained physical state owners that are not distinct historical C/C++ inputs:

```text
src/main/core/frame_state.asm
src/main/core/quit_state.asm
src/main/core/slowdown_state.asm
src/main/dialog/data.asm
src/main/dialog/script_state.asm
src/main/dialog/state.asm
src/main/scroll/page_state.asm
src/main/scroll/state.asm
```

The TASM32 receipt records `assemble_pass=8`, valid OMF structure, producer
comments, and source/object hashes:

```text
.analysis/reconstruction/probes/native-main-state-owners-v1025-20260929/receipt.json
SHA-256 48801b24a188977c3d213ef929ccf8b024806eb2df8465ac5d1324bcae1bb806
```

This closes the physical producer inventory, not its final placement. DGROUP
order, BSS offsets, TLINK fixups, complete MAIN MZ layout, and PC-98 startup
remain unverified.

## Product include repairs

The frontier required four source-level closure repairs:

- `src/main/stage/stages.cpp` now includes the product boss header through the
  product source path;
- `src/main/formats/cdg.hpp` owns the Stage 5 plane-roll declaration and its
  planar types locally;
- `src/main/hud/hud.cpp` uses the installed product HUD header path;
- `src/shared/hardware/bgimage.cpp` uses the product `REP MOVSD` macro and
  removes a non-emitting TC4J-rejected segment-alignment block.

These repairs establish compiler compatibility only. They must not be promoted
to exact shared-unit claims without affected-unit cold replay, relocation/layout
agreement, and raw comparison.

The independent quoted-include inventory was refreshed after the repairs:

```text
.analysis/reconstruction/probes/native-main-inventory-v1025-20260929/inventory.json
SHA-256 eeb97c5285333fac1426885984d471fc4640a8ceca2110d1f2c6e9575f2adfe4
unique_missing=0
```
