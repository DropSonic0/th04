# Native MAIN composite-source localization

The MAIN source-closure inventory was replayed after localizing the 36
historical composite body includes that already have checked-in product owners.
The changed producers are the Yuuka 5/6, Elly, Kurumi, Marisa, Mugetsu, DEMO,
and dialog-fused translation units. The physical `th04/dialog.cpp` wrapper is
deliberately still open; no scaffold implementation was copied into product
source.

`scripts/replay_th04_main_exact_units.py` now freezes product-owned `.c`,
`.cpp`, and `.asm` inputs reached through `src/` include closure. It also
records explicit cold-tree-only include rewrites where a historical source
transform or the PC-98 integrated Mugetsu producer requires the old `th04/`
spelling. The maintained source is never rewritten. The replay receipt records
the rewrite hashes in each build materialization.

Evidence:

- `python3 scripts/probes/inventory_th04_native_main.py --output .analysis/reconstruction/probes/native-main-inventory-v1013b-composition-20260929/inventory.json`
  reports one missing quoted body path, one reference, and one unmapped path:
  `th04/dialog.cpp`.
- `python3 scripts/replay_th04_main_exact_units.py --run-id gpt-5-6-sol-main-composition-localization-1013c-20260929-r3 --stage all`
  passes two isolated cold builds for all 275 selected exact units. Both
  candidate MAIN images are identical at SHA-256
  `d51db833654d139b6e79c059a70be2859d4f83a3d6c9777e7fbc547f1d3c0bdb`, with
  `failures=[]`. The replay receipt SHA-256 is
  `bf96f2a13a8430d6fe16e5f2cd3b1eec568c928633aa0e4c86571389fbb574af`.

This is source-closure and exact-unit replay evidence only. It does not claim a
standalone MAIN source manifest, whole-image equality, target DATA/BSS
placement, or PC-98 startup. The next bounded task is to recover the physical
dialog composition from the maintained dialog fragments and shared declaration
surface, then define the standalone MAIN link inputs and runtime checkpoints.
