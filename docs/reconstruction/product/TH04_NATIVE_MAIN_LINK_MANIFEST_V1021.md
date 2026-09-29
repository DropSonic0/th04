# TH04 native MAIN link frontier v1021

The pinned ReC98 `Tupfile.lua` contains 60 ordered MAIN inputs after the
`th04_main.asm` scaffold. `config/native_main_sources.toml` freezes that list
and classifies each input as a direct maintained owner, a fused physical owner,
or an explicit unresolved routing umbrella. The checker is
`scripts/probes/probe_th04_native_main_manifest.py`.

On 2026-09-29 the manifest check passed without running Borland:

```text
python3 scripts/probes/probe_th04_native_main_manifest.py \
  --check \
  --output .analysis/reconstruction/probes/native-main-link-manifest-v1021k-20260929/manifest.json
```

It reports 56/60 reference inputs mapped and four unresolved historical owners:
`tile.cpp`, `stages.cpp`, `boss_4r.cpp`, and `boss_x2.cpp`. The local files all
exist for the mapped entries, and the scaffold path is present. The result is
therefore
`ready_for_native_link = false`; no linker or runtime claim is made.

The fused classifications are deliberate: `f_dialog.cpp` + `dialog.cpp` use
the maintained `src/main/dialog/fused.cpp`, while `midboss.cpp` + `hud_hp.cpp`
+ `mb_dft.cpp` share `src/main/midboss/midboss.cpp`. A future native build must
materialize one physical object for each fused group and must account for the
remaining DATA/BSS owners before replacing the scaffold. This manifest is a
control-plane frontier, not exactness evidence.

## Compiler frontier

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
  --output-dir .analysis/reconstruction/probes/native-main-compile-frontier-v1021k-20260929
```

It produced 53 unique direct/fused owners and 53 valid OMF objects. The
historical `th04/bullet_u.cpp` route now uses the checked-in physical
composition `src/main/bullet/update.cpp`, which includes the exact recovered
`update_prefix.inl` and `update_body.inl` owners. In particular,
`src/main/spark.asm` compiles as `th04/spark_a.asm`, so TASM emits
the expected `SPARK_A_TEXT` segment without changing the maintained body. The
historical `th04/it_spl_u.cpp` route likewise uses
`src/main/item/splash_u.cpp`, composing the reviewed init and add/update
fragments with the three reference-attested radius constants. The
receipt records `link_performed=false` and `runtime_performed=false`; this is
compiler-observed frontier evidence, not standalone MAIN readiness.
