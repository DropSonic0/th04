# TH04 native MAIN link frontier v1021

The pinned ReC98 `Tupfile.lua` contains 60 ordered MAIN inputs after the
`th04_main.asm` scaffold. `config/native_main_sources.toml` freezes that list
and classifies each input as a direct maintained owner, a fused physical owner,
or an explicit unresolved routing umbrella. The checker is
`scripts/probes/probe_th04_native_main_manifest.py`.

The current compiler-routing continuation was run on 2026-09-29:

```text
python3 scripts/probes/probe_th04_native_main_manifest.py \
  --check \
  --output .analysis/reconstruction/probes/native-main-link-manifest-v1021m-20260929/manifest.json
```

It reports all 60/60 reference inputs mapped to maintained direct or fused
physical owners, including the new `src/main/tile/tile.cpp` and
`src/main/stage/stages.cpp` compiler-routing wrappers. The manifest now has
`unmapped = []` and `ready_for_native_link = true`; this is only a routing
precondition, not a link or exactness claim.

The compiler-only continuation was then run with:

```text
python3 scripts/probes/probe_th04_native_main_compile_frontier.py \
  --output-dir .analysis/reconstruction/probes/native-main-compile-frontier-v1021n-20260929 \
  --require-all
```

It produced 57 unique physical owners and 57 valid OMF objects. The receipt
records `compile_fail = 0`, `link_performed = false`, and
`runtime_performed = false`. The tile invalidation helper is natural C++ for
source closure only; the target REP-STOSD owner and Stage 4 carpet/checkerboard
low-level residuals remain separately classified and are not promoted.

The previous v1021l manifest was:

```text
python3 scripts/probes/probe_th04_native_main_manifest.py \
  --check \
  --output .analysis/reconstruction/probes/native-main-link-manifest-v1021l-20260929/manifest.json
```

It reported 58/60 reference inputs mapped and two unresolved historical owners:
`tile.cpp` and `stages.cpp`. The local files all exist for the mapped entries,
and the scaffold path is present. The result is therefore
`ready_for_native_link = false`; no linker or runtime claim is made.

The fused classifications are deliberate: `f_dialog.cpp` + `dialog.cpp` use
the maintained `src/main/dialog/fused.cpp`, while `midboss.cpp` + `hud_hp.cpp`
+ `mb_dft.cpp` share `src/main/midboss/midboss.cpp`. A future native build must
materialize one physical object for each fused group and must account for the
remaining DATA/BSS owners before replacing the scaffold. This manifest is a
control-plane frontier, not exactness evidence.

## Compiler frontier (historical v1021l snapshot)

The follow-up compiler-only probe stages the pinned ReC98 revision, copies the
current `src/` tree, and compiles each unique mapped physical owner under its
historical input basename. This basename matters to TASM: `.code` derives the
default segment name from the filename. The manifest therefore records two
exact-unit build aliases (`th04/vectorfar.asm` and `th04/cdgpna.asm`) for
maintained ASM owners whose historical Tupfile entries are named `.cpp`.
The sound-effect routes now also have explicit local owners: `se_reset.cpp`
for `th02/snd_se_r.cpp` and the checked-in `src/main/sound/se.cpp` physical
composition for `th04/snd_se.cpp`.

```text
python3 scripts/probes/probe_th04_native_main_compile_frontier.py \
  --output-dir .analysis/reconstruction/probes/native-main-compile-frontier-v1021l-20260929
```

It produced 55 unique direct/fused owners and 55 valid OMF objects. The
historical `th04/bullet_u.cpp` route now uses the checked-in physical
composition `src/main/bullet/update.cpp`, which includes the exact recovered
`update_prefix.inl` and `update_body.inl` owners. In particular,
`src/main/spark.asm` compiles as `th04/spark_a.asm`, so TASM emits
the expected `SPARK_A_TEXT` segment without changing the maintained body. The
historical `th04/it_spl_u.cpp` route likewise uses
`src/main/item/splash_u.cpp`, composing the reviewed init and add/update
fragments with the three reference-attested radius constants. The historical
`th04/boss_4r.cpp` route now uses `src/main/boss/reimu4_state.cpp` for the
target-attested orb-template layout and Reimu state cells, while
`th04/boss_x2.cpp` uses `src/main/boss/gengetsu6_state.cpp` for the Gengetsu
wave state and spawn-column layout. These are source-backed state owners only;
they do not establish target DATA/BSS placement or exactness. The receipt
records `link_performed=false` and `runtime_performed=false`; this is
compiler-observed frontier evidence, not standalone MAIN readiness.
