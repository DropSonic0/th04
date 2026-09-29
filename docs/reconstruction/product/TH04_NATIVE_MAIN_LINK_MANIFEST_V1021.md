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
  --output .analysis/reconstruction/probes/native-main-link-manifest-v1021-20260929/manifest.json
```

It reports 52/60 reference inputs mapped and eight unresolved historical
owners: `tile.cpp`, `stages.cpp`, `snd_se_r.cpp`, `snd_se.cpp`, `it_spl_u.cpp`,
`bullet_u.cpp`, `boss_4r.cpp`, and `boss_x2.cpp`. The local files all exist for
the mapped entries, and the scaffold path is present. The result is therefore
`ready_for_native_link = false`; no linker or runtime claim is made.

The fused classifications are deliberate: `f_dialog.cpp` + `dialog.cpp` use
the maintained `src/main/dialog/fused.cpp`, while `midboss.cpp` + `hud_hp.cpp`
+ `mb_dft.cpp` share `src/main/midboss/midboss.cpp`. A future native build must
materialize one physical object for each fused group and must account for the
remaining DATA/BSS owners before replacing the scaffold. This manifest is a
control-plane frontier, not exactness evidence.

## Compiler frontier

The follow-up compiler-only probe stages the pinned ReC98 revision and copies
the current `src/` tree before compiling each unique mapped physical owner:

```text
python3 scripts/probes/probe_th04_native_main_compile_frontier.py \
  --output-dir .analysis/reconstruction/probes/native-main-compile-frontier-v1021c-20260929
```

It produced 49 unique direct/fused owners: 48 valid OMF objects and one
assembly failure. `src/main/spark.asm` cannot be assembled as an independent
object because it expects the scaffold-declared `SPARK_A_TEXT` segment; TASM
reports one undefined segment followed by ten `CS unreachable` diagnostics.
The native link must therefore materialize that owner through an explicit
segment wrapper/include decision. The receipt records `link_performed=false`
and `runtime_performed=false`; this is compiler-observed frontier evidence, not
standalone MAIN readiness.
