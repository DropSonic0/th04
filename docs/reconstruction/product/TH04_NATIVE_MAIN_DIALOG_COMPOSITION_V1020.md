# TH04 MAIN dialog physical composition

The maintained dialog fragments now have one local physical producer at
`src/main/dialog/dialog.cpp`. `src/main/dialog/fused.cpp` includes the local
file owner and then that producer in the historical `M4_RENDER_TEXT` /
`DIALOG_TEXT` order. The producer owns the shared dialog helpers and includes
the maintained render, fade, stage-transition, script-parameter, operation,
run, and animation fragments. `src/main/dialog/script_state.asm` is a
candidate `_BSS` owner for `_script_param_number_default`; its target DGROUP
offset and standalone link placement are not asserted.

The local product composition was exercised in the v1020 natural-source cold
tree. TC4J compiled the physical producer and the build log contains no
compiler errors. That candidate is intentionally not promoted to exact: its
physical composition changes the accepted historical object boundary and the
aggregate comparison fails raw/map/relocation equality until the local source
is independently reconciled with the target producer.

The source-closure inventory now reports no missing quoted paths or references:

```text
python3 scripts/probes/inventory_th04_native_main.py \
  --output .analysis/reconstruction/probes/native-main-inventory-dialog-compose-v1020-20260929/inventory.json
```

The inventory output SHA-256 is
`eeb97c5285333fac1426885984d471fc4640a8ceca2110d1f2c6e9575f2adfe4`.

Exact replay remains pinned to the attested historical scaffold. The manifest
adapter `th04-main-dialog-fused-v381-scaffold-inputs` rewrites only the cold
`th04/dlgfuse.cpp` copy from product `src/main/...` include spellings back to
`th04/f_dialog.cpp` and `th04/dialog.cpp`; maintained product files are never
rewritten. Two cold builds then pass all 275 selected units:

```text
python3 scripts/replay_th04_main_exact_units.py \
  --run-id gpt-5-6-sol-main-dialog-compose-1020-20260929-r4 --stage all
```

Receipt SHA-256:
`007800382cdce6c59ccc172bb267332cbd827e39c574981a4fb895dd98b848b4`.
Both diagnostic MAIN images are
`d51db833654d139b6e79c059a70be2859d4f83a3d6c9777e7fbc547f1d3c0bdb` and the
receipt has `failures=[]`.

This closes the maintained source-closure gap, not the product build. The
remaining blockers are a native MAIN object/link manifest, target DATA/BSS
ordering and offsets (including the candidate script parameter owner), and a
full PC-98 startup scenario. The historical scaffold replay is an Oracle for
accepted units, not standalone MAIN or runtime evidence.
