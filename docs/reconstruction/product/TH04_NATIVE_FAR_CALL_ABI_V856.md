# TH04 native far-call ABI trap and repair

2026-09-27. This note concerns the **unpacked MAINE product-build lane**.
The executable under test is a private historical-library calibration link,
not a TH04-only product or runtime-accepted replacement. No target-byte
exactness follows from this work.

## Rejecting observation

The earlier 108-object calibration MZ linked successfully and passed ordinary
MZ relocation checks. Its MAP placed `FILE_ROPEN` at MAINE
`_TEXT:0000:04A6` and `cutscene_script_load` at `CUTSCENE_TEXT:09CE:07A3`.
The latter contains opcode `9A A6 04 00 00`, a far call to `FILE_ROPEN`.
The callee instead ended in `C2 02 00`, a near return that also removed only
the two-byte near filename argument. MAINE's large-model C++ caller passes
a four-byte far filename pointer. This combination necessarily corrupts
the return stack and discards half the pointer. TLINK and the MZ relocation
auditor accepted it because neither gate checked call/return distance or
argument layout.

The same pre-repair MZ had 38 far calls into seven near-return `FILE_*`
owners and six far calls into near-return `GRAPH_CLEAR`. The raw-call scan
also confirmed each call's segment word had an MZ relocation entry, which
shows why relocation validity alone was insufficient. The old MZ SHA-256 is
`7ef632fa9548be660820d30b3c503384dc343eb8c8ace0ae28f1f60cd38275b2`.

## Product branch

`TH04_LARGE_PRODUCT` now selects far public entry/return instructions and
far-call stack offsets in the seven existing `src/shared/dos/file_*.asm`
owners. `FILE_ROPEN`, `FILE_CREATE`, and `FILE_APPEND` pass the caller's
far filename directly to DOS using DS:DX and restore DS afterward. The
unchanged branch retains the original near ABI for focused exact replays.
The new `src/shared/dos/file_size.asm` obtains size through DOS seek-to-end
and restores the physical position. `GRAPH_CLEAR` similarly selects a far
product entry/return and retains its near replay branch. This is semantic
product source, not an exact-match rewrite.

`scripts/probes/audit_th04_native_maine_call_abi.py` binds the calibration
link receipt, MZ digest, and TLINK MAP. It uses ndisasm to check the first
return instruction and cleanup count at each of nine known public entry
points, inventories every raw far-call target, requires a relocation entry
for each far-call segment word, and rejects any newly seen far call to a
near-declared source owner outside its audited set. The pre-repair MZ fails
on `FILE_APPEND: ret 2`; the file-repaired, pre-graph-repair MZ fails on
`GRAPH_CLEAR: ret`.

## Current observations

- The 109-TU no-archive MAINE link has 32 unresolved names and no warnings.
  Two cold builds agree on all link-relevant and timestamp-normalized OMF
  objects; only BGIMAGE's raw dependency timestamp differs. Receipt SHA-256:
  `bb7c6cc886478d2a1ea6a379ae5045b85bfae86cfdfccc4281dcc769071abe94`
  and `be94dfe37e8dc1993b9158a84ba26b1e8e36394bfd2f91ddece4dec3ae16137e`.
- Historical-library calibration TLINK exits 0, with its existing invalid
  extended-dictionary warning. Its MZ has 569 relocation sites; independent
  site, entry, stack, and two DOS load checks pass. Link/audit receipt SHA-256:
  `aca09e267b469e07b32b83918b57a2c1b000fb0e5884b63cd5888664c0614e6b`
  and `077a2c165f7fede5432d53509b20b2c59a0c44a5080c9b896b93029e7043ada0`.
- At MAINE `0000:04B2`, the current `FILE_ROPEN` first return is `retf 4`;
  at `0000:06CE`, `GRAPH_CLEAR` first returns with `retf`.
  The call ABI receipt SHA-256 is
  `5ed456c8bbc978bd650a3c9f855c7fdc73d52ab2d0f1371dcf0fbb23bd1290f1`:
  nine entries, 44 relocated far calls.
- A cold MAINE accepted-function replay after the file branch change checked
  all 72 accepted slices at raw zero (receipt SHA-256
  `554eee9f9e06d120bd3a43a34f0a2d73d420e129aa7f15f8522f31edd475e504`).
  A focused ZUN replay after the graph branch change preserves the 36-byte
  `GRAPH_CLEAR` accepted slice at decoded ZUN.COM `_TEXT:0xF64` raw-zero;
  its wider 6,360-byte diagnostic component still has 4,241 known differences
  and is not an exact claim (receipt SHA-256
  `caab8bdb59722c19d7d44aed2fce1b67999bb75532a856f00ce1a9a5662261fa`).

## Replay and remaining limits

Run `probe_th04_native_maine_link.py` twice with `--without-support`, then
`compare_th04_native_maine_link.py`. Link a third time with the historical
support library only for calibration. Run both
`audit_th04_native_maine_mz.py` and
`audit_th04_native_maine_call_abi.py` on that link receipt. The private
directories and receipt hashes above identify the observed runs; see the
product readiness note for source-manifest and toolchain flags.

The audit establishes call/return distance, cleanup width, and segment-word
relocation presence. It does not prove every pointer value, DOS I/O effect,
or hardware behavior. MAINE still needs 32 local symbols before a TH04-only
link, and the original OP → MAIN → MAINE transition remains the proper
runtime route. For TH05, add an equivalent ABI gate before interpreting a
valid MZ relocation table as a runnable product.
