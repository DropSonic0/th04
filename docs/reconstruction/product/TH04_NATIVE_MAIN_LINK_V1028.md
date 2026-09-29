# TH04 native MAIN link diagnostic v1028

The checked-in native-link probe now cold-runs the complete maintained product
source tree instead of only the C/C++ frontier:

```text
python3 scripts/probes/probe_th04_native_main_link.py \
  --output-dir .analysis/reconstruction/probes/native-main-link-v1028-20260929
```

Receipt SHA-256: `66a29b2da65e154eff25c92e49ba070fd1203a215a65dc5ea512d5d0fd4e7c80`.
The pinned TC4J/TASM32 run compiles all 191 C/C++ physical roots, assembles all
8 dedicated DATA/BSS owners, and scans the other 84 ASM sources. The result is
191/191 C/C++ roots, 8/8 state owners, and 64/84 other ASM owners; the 20 ASM
failures are recorded per source in the receipt rather than hidden.

`main_code_order_anchor.asm` is placed first in the TLINK response. This keeps
the configured `MAIN_01`/`MAIN_03` group order below 64 KiB and removes the
duplicate-public frontier: `group_overflows=0` and `duplicate_errors=0`.
The diagnostic still exits TLINK with 480 unresolved symbols and 3 support
library fixup overflows. It emits a partial MZ (148,860 bytes, 1,098
relocations, header paragraphs 384), which is a link-frontier observation only;
it is not a complete MAIN build, raw match, or startup result.

The remaining 20 ASM failures split between source fragments that require the
historical scaffold's active segment/macro context and units whose includes
(`th03/arg_bx.inc`, `libs/master.lib/master.inc`, `ReC98.inc`, or game include
trees) are intentionally not copied into the product tree. The next bounded
work is to close those wrapper/include contexts and then reconstruct the
unresolved DATA/BSS owners. Do not treat a partial MZ or support-library
warnings as evidence of runtime readiness.

The private v1027/v1028 object trees were pruned after the receipt and ledger
updates; the checked-in probe is the replay input, while the receipt is the
durable frontier summary.
