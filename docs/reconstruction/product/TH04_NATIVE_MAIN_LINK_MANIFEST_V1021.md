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
