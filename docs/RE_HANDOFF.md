# TH04 reconstruction handoff

Updated 2026-09-27 after the MAINE native-link and accepted-function replay. This is the
current resume index; use
`python3 scripts/status.py`, `config/units.csv`, and the function-boundary and
decoded-acceptance ledgers for live counts. `docs/RE_ROADMAP.md` gives the next
work order; `docs/reconstruction/README.md` routes focused evidence. Versioned
notes, historical receipt paths, `config/evidence.csv`, and version-tagged
`config/knowledge.csv` rows are chronological snapshots, not current acceptance.
When an old row says a function is blocked, check for later evidence before
acting.

## Resume checks

```sh
git status --short --branch
python3 scripts/preflight.py
python3 scripts/status.py
python3 scripts/audit_compat_dependencies.py --check
python3 scripts/boundary_review/validate_function_boundary_ledger.py
```

The four local targets pass size, SHA-256, format, and MZ-structure checks.
Their provenance is still `candidate-local-attested`, not proof of pristine
release media. Re-attest the active Ghidra database before new target
observations. Run only one writable Borland/Wine replay at a time and limit CPU
use; this host's focused replays use `taskset -c 0,1 nice -n 10`.

## Accepted state and remaining blockers

| Artifact | Reviewed authored boundaries | Exact authored functions | Blocked | Original-ASM observations |
| --- | ---: | ---: | ---: | ---: |
| OP.EXE | 93 / 93 | 93 | 0 | 16 reviewed |
| MAIN.EXE | 495 / 495 | 493 | 2 | 73 attestation entries; 6 provisional |
| MAINE.EXE | 72 / 72 | 72 | 0 | 15 reviewed |
| ZUN.COM | 3 / 3 | 3 | 0 | 12 reviewed |

MAIN's reviewed file-backed authored extent has 83,444 / 83,469 exact bytes.
Its 25-byte remainder belongs only to `carpet_lighting_put_new` (23) and
checkerboard (2). v836 migrates MAIN `SND_LOAD` from historical partial owners
to one exact 234-byte maintained owner. OP has 14,692 accepted decoded
source-owner bytes of 14,692 tracked and ZUN
has 442 / 442. MAINE decoded owner coverage is reported by the live status and
decoded-acceptance ledgers. None of these gives a packed-file byte denominator
or whole-artifact exactness.
Original-ASM observations are outside the authored C/C++ counts.

OP and MAINE now both have closed authored-function queues; MAINE canonical v835 passes all 72/72 accepted slices raw-zero. v821 closes OP `scoredat_decode` and `scoredat_encode`
with maintained hybrid source: all control/data flow stays C++, while the
single 8-bit ROR primitive is independently corroborated by pre-decompilation
TH03 OP/MAINL codec bodies. Focused v821 replay and the archived canonical
current-ledger replay are raw-zero; the latter checks all 87 accepted OP slices.
This does not establish original-source spelling or packed-file exactness.

v827 closes `SND_SE_PLAY` and `_snd_se_update`, superseding v822's conservative
source-acceptance verdict while retaining its provenance warning. Restored TH05
OP and MAINE release targets each contain exactly one complete fixed 0x86
`th04/snd_se.cpp` producer after masking only the same 20 legal OMF link operands.
This independently corroborates the frame-free parameter primitive and both
BL/XOR-BH current-index sites. The surrounding selection/update logic remains
maintained C++, and no emitted target bytes or post-build patching is used. Two
cold TH04 links reproduce both function bodies, the full producer, accepted OP
EXE/MAP, and all 804 relocations. Canonical v827 checks all 92 accepted OP
slices raw-zero. Focused receipt SHA-256
`33962357744f6b3e3983c37622de9f474bc283f67710a9dd0fa77549f898c002`;
canonical receipt SHA-256
`c0d1684341c87dcf156907e2b8304a4d1918ce8c2b69da6f3f4827471aa2aa98`.
Literal original-source spelling and packed-file exactness are not claimed.

v824 closes the internal 63-byte OP `egc_start_copy` helper. v825 then closes
the adjacent 111-byte `egc_copy_rect_1_to_0_16` outer body with a narrowed
hybrid source: ordinary TC4J owns parameter loads and rectangle arithmetic,
while only bounded CLD/SHL/immediate-page-OUT/STOSW/LOOP primitives remain
symbolic. TH05 OP/MAINE descendant bodies and earlier release-target machine
code provide independent mechanism evidence. Two cold v825 links reproduce the
complete `0xB0` producer, accepted OP EXE/MAP, and all 804 relocations.
Canonical v825 checks all 89 accepted OP slices raw-zero. v825 focused receipt
SHA-256 `984291bce653fe242c33003e85eedd1572c71e062f7e0cec02c3840a52ec649d`;
canonical receipt SHA-256
`5df5f441a94e5ce3aadfdd102b84abdba1ffcb4730e92bcdd329b91429b65eca`.

v826 closes `nopoly_b_put`. TH03, TH04, and TH05 OP release targets each
contain exactly one copy of the same normalized 30-byte GAME>=3 body after
masking only the linked `_nopoly_B` word; TH02 is a nonmatching GAME<3 control.
The maintained hybrid keeps segment values/count in ordinary TC4J and limits
symbolic source to DS save/restore, the cross-game XOR encoding direction, and
REP MOVSW. Two cold links preserve the complete OP_MUSIC_TEXT producer and all
804 relocations. Canonical v826 checks all 90 accepted OP slices raw-zero.
Focused receipt SHA-256
`26405281b33c09142e3eec074fe62791614717bd25ef12e27178c4a5232102ca`;
canonical receipt SHA-256
`cab1b089612158f70a983e9f277e3c9146b9b937c6d5c515785257fcd70adcde`.

v823 first closes the code-generation mechanism for the 234-byte SND_LOAD:
natural and target differ only at `0xDE8B..0xDE8C`, and TC4J integrated inline
`mov bx,ax` makes the complete function raw-zero without moving the MAP owner or
804 relocations. v828 supplies the missing acceptance precedent. The already
accepted v394 `dialog_face_unput_8` hybrid uses the same register-direction
primitive, and TH04/TH05 release targets independently preserve the common
`8B 46 04 89 C3` sequence. The maintained complete SND_LOAD source keeps only
this two-byte primitive symbolic. Focused v828 SHA-256
`0346f3f2d2e89121b1b89aaf7cab25f87cc3bc64540ff4de0e24d62bfe2c5dad`;
canonical v828 SHA-256
`50928bc1bcef1ea614574e94ccb9538f1e7167e7eb7ec92cafc6cbe806f22cc5`.
Canonical v828 checks **93/93 OP authored function slices raw-zero**.

OP has no remaining authored-function blockers. v820's ordinary TC4J / `-B`
negatives remain useful historical controls, while v821 and v824-v828 record
the independently corroborated bounded hybrid exceptions.

v829 closes MAINE `SND_LOAD` artifact-locally: two cold MAINE links reproduce
the full 234-byte body and all 559 ordered relocations; canonical v829 checks
64/64 accepted MAINE slices raw-zero.

v830 then closes MAINE `SND_SE_PLAY` and `_snd_se_update` artifact-locally.
Two cold links reproduce the complete 0x86 `th04/snd_se.cpp` producer, the
retained MAINE EXE/MAP, and all 559 relocations; canonical v830 checks 66/66
accepted MAINE slices raw-zero.

v832 closes MAINE `scoredat_decode` and `scoredat_encode` artifact-locally.
Maintained C++ keeps all codec logic except the TH03 release-backed byte-ROR
primitive. Two cold links reproduce complete 0xB30 SCORE_TEXT, all 203 ordered
OMF fixups, retained EXE/MAP identity, and all 559 MZ relocations. Canonical
v832 checks 68/68 accepted MAINE slices raw-zero.

v833 closes cutscene `egc_start_copy` and `box_1_to_0_masked` artifact-locally.
The complete 42-byte AX=value -> DX=port -> OUT DX,AX EGC setup core is
independently preserved in TH02/TH03/TH04/TH05 release targets. Two cold MAINE
links reproduce the complete 0xC3E CUTSCENE_TEXT producer, all 214 OMF fixups,
retained EXE/MAP, and all 559 relocations; canonical v833 checks 70/70 accepted
MAINE slices raw-zero.

v834 closes SCORE `_egc_start_copy_inlined` without `_outportb_` or `keep_0`;
the only object-format change removes the artificial `_address_0` linker-zero
fixup while final EXE bytes and all 559 relocations remain unchanged. v835
then closes the 924-byte `regist_menu` without `optimization_barrier`: an
ordinary semantic conditional expression naturally emits the target
direct-CMP/JZ/JMP frontier under pinned TC4.02. Literal original-source
spelling is not claimed.

MAIN's two reviewed blockers / 25 bytes remain
`carpet_lighting_put_new` (23 residual bytes) and
`playfield_checkerboard_grcg_tdw_` (2 residual bytes). They are deferred while
build readiness is examined. The default MAIN replay manifest now selects the
complete exact `SND_LOAD` at `SHARED:03B6` and excludes its historical partial
owners. `python3 scripts/replay_th04_main_exact_units.py --run-id gpt-6-sol-main-aggregate-fixed-20260927`
cold-built the selected 275 units
twice with raw-zero accepted extents; receipt SHA-256
`40f18d358cdfdd970e841baeb93da4a83567e17344c32327730864b4fc40b6c4`.
The scaffold-built MAIN.EXE is 152,974 bytes versus the 156,258-byte target;
the complete MZ comparison rejects raw identity. All 1,136 relocation sites
and site values match, but their entry order differs. This remains a
diagnostic ReC98 scaffold build, not a standalone TH04 product or a
runtime-tested replacement. Next
work is packed-container ownership, DIET/link-layout closure, standalone
product construction, and a candidate runtime scenario; the two MAIN bodies
remain unresolved exactness work.

The [native product-build readiness note](reconstruction/product/TH04_NATIVE_BUILD_READINESS_V1.md)
records the current TH04-only compile and link lane. A fresh `src/` snapshot
compiles 97 MAINE and shared C/C++/ASM translation units under the pinned
TC4J/TASM32 toolchain. The MAINE entry, staffroll, registration, verdict,
cutscene, sound-effect update, and unclipped PI writer now have product owners.
The cutscene PI masks and adjacent box/glyph data match the attested target
in their compiled OMF owners. Two cold no-archive links agree on every
link-relevant and timestamp-normalized object; BGIMAGE differs only in a
dependency timestamp. Those 97-object links fail TLINK with 61 support names
and zero warnings. Their parsable MZ files and 432 relocation entries are
incomplete diagnostics, not runnable candidates.
The affected MAINE cold aggregate passes all 72 accepted function slices
raw-zero after the cutscene additions; receipt SHA-256
`a1fff45cf81442a1aa3e0a6ae6c253c958a283e4c1aa1be028dc841fcd118471`.

The historical `masters.lib` remains an ABI-incompatible calibration input.
The prior 61 unresolved names route to 49 historical archive members by public
name. A product-only C++ compile branch now puts the four MAINE near-code
segments in `GROUP_01`; the exact replay branch retains its original groups.
The historical-library calibration link consequently exits 0 with no unresolved
names or near-call fixup errors. Its valid MZ has 573 relocations, all unique,
nonoverlapping, in-image, and statically safe at two DOS load segments; the
archive's extended dictionary still warns. This remains a calibration artifact.
The product-group wrapper edit passed a focused SCORE codec replay and the
full cold MAINE aggregate: all 72 accepted function slices remain raw-zero
(aggregate receipt SHA-256
`fda5aab5ce3d50fba8f8693f34915896657cd95999b55e9b86bd96e25ceefe03`).
TH04-local display controls now own `GRAPH_HIDE`, `GRAPH_SHOW`, `GRCG_OFF`,
and `GRCG_SETCOLOR`. The current 98-object no-archive link has 57 unresolved
names and zero warnings; two cold rounds agree. The historical-library
calibration still links, and its 573 MZ relocation sites pass the static audit.
Product closure needs the remaining local runtime providers, a successful
TH04-only TLINK link, its own MAP/MZ relocation audit, DIET packaging, and a
candidate PC-98 scenario. OP lacks an entry TU; MAIN
still has 102 distinct unresolved
quoted include paths. The remaining two nonexact MAIN function slices are
outside this build lane. Source ownership and runtime correctness are separate
from the already accepted raw-zero function ledgers.

See the [v821 SCORE codec closure](reconstruction/op-maine/TH04_OP_SCORE_CODECS_HYBRID_V821.md),
[v822 shared-sound provenance bound](reconstruction/op-maine/TH04_OP_SND_SE_SHARED_V822.md),
[v827 shared-sound cross-game hybrid closure](reconstruction/op-maine/TH04_OP_SND_SE_CROSSGAME_V827.md),
[v823 SND_LOAD provenance bound](reconstruction/op-maine/TH04_OP_SND_LOAD_PROVENANCE_V823.md),
[v828 SND_LOAD maintained-hybrid closure](reconstruction/op-maine/TH04_OP_SND_LOAD_HYBRID_V828.md),
[v829 MAINE SND_LOAD artifact-local closure](reconstruction/op-maine/TH04_MAINE_SND_LOAD_HYBRID_V829.md),
[v830 MAINE shared-sound artifact-local closure](reconstruction/op-maine/TH04_MAINE_SND_SE_CROSSGAME_V830.md),
[v831 MAINE regist_menu current-v489 compiler frontier](reconstruction/op-maine/TH04_MAINE_REGIST_FRONTIER_V831.md),
[v832 MAINE SCORE codec artifact-local closure](reconstruction/op-maine/TH04_MAINE_SCORE_CODECS_HYBRID_V832.md),
[v833 MAINE cutscene EGC artifact-local closure](reconstruction/op-maine/TH04_MAINE_CUTSCENE_EGC_HYBRID_V833.md),
[v834 MAINE SCORE EGC helper closure](reconstruction/op-maine/TH04_MAINE_SCORE_EGC_HYBRID_V834.md),
[v835 MAINE regist_menu ordinary-C++ closure](reconstruction/op-maine/TH04_MAINE_REGIST_TERNARY_V835.md),
[v824 EGC-start hybrid closure](reconstruction/op-maine/TH04_OP_EGC_START_HYBRID_V824.md),
[v825 EGC rectangle-copy hybrid closure](reconstruction/op-maine/TH04_OP_EGC_COPY_HYBRID_V825.md),
[v826 nopoly_B_put hybrid closure](reconstruction/op-maine/TH04_OP_NOPOLY_HYBRID_V826.md),
[OP strict frontier](reconstruction/op-maine/TH04_OP_STRICT_FRONTIER_V766.md),
and [MAINE strict frontier](reconstruction/op-maine/TH04_MAINE_STRICT_FRONTIER_V732.md).
Do not substitute target-derived inline ASM, explicit register forcing, or
inert optimizer barriers without independent provenance. All 16 OP and 15
MAINE original-ASM function-like entries now have reviewed boundaries and
source-backed raw-identical modules; the prior MAINE provisional-cut statement
is superseded by v815.

| Shared/original-style ASM source | OP load segment:offset | MAINE load segment:offset | Focused evidence |
| --- | --- | --- | --- |
| `CDG_LOAD`, `0x164` | `0DA1:0B6A` | `0CC7:0B08` | [v797 and v815](reconstruction/op-maine/TH04_SHARED_CDG_LOAD_V797.md) |
| `CDG_PUT_8`, `0x9E` | `0DA1:05FE` | `0CC7:06E6` | [v799](reconstruction/op-maine/TH04_SHARED_CDG_PUT_V799.md) |
| `INPUT_S`, `0x10A` | `0DA1:07CC` | `0CC7:081A` | [v802](reconstruction/op-maine/TH04_SHARED_INPUT_V802.md) |
| `BGIMAGE_PUT_RECT_16`, `0x82` | `0DA1:0AE8` | `0CC7:0A86` | [v805](reconstruction/op-maine/TH04_SHARED_BGIMAGER_V805.md) |
| `_hflip_lut_generate`, `0x1E` | `0DA1:0134` | `0CC7:01EC` | [v808 and v812](reconstruction/op-maine/TH04_OP_HFLIP_LUT_V808.md) |
| `GRAPH_PUTSA_FX`, `0x15A` code + `0x40` data | `0DA1:04A4`, data `0F34:0A00` | `0CC7:058C` | [v809 and v813](reconstruction/op-maine/TH04_OP_GRAPH_PUTSA_FX_V809.md) |

OP also has its local `CDG_PUT_NOCOLORS_8` (`0DA1:0282`, 0x52-byte module)
and shared `CDG_PUT_NOALPHA_8` (`0DA1:0766`, 0x66-byte module). MAINE has local
`CDG_PUT_PLANE` (`0CC7:0408`, 0x9A-byte module). Their [v806](reconstruction/op-maine/TH04_OP_CDG_NOCOLORS_V806.md),
[v807](reconstruction/op-maine/TH04_OP_CDG_NOALPHA_V807.md), and
[v814](reconstruction/op-maine/TH04_MAINE_CDG_PLANE_V814.md) focused A/B links
match complete decoded module bytes and ordered relocations. All OP/MAINE
packed-file offsets remain unknown. The shared CDG and input modules also
passed focused MAIN cold replay; source placement and include composition are
replay inputs, so changes require affected-unit cold replay.

ZUN's natural Tiny-model MEMCHK `_main` at COM payload `0x26A7` was its first
exact authored function (38 bytes). Its complete 4,066-byte component cold-links
raw-identically with maintained `DOS_PUTS2` and `DOS_MAXFREE`; those helpers are
library support, not authored-function credit. See the [MEMCHK note](reconstruction/zun/TH04_ZUN_MEMCHK_NATURAL_EXACT_V773.md).

Maintained symbolic original-style ASM cold-links the 1,141-byte ZUNINIT
component at decoded `0x6F3..0xB67`, 223-byte launcher selector, 8-byte mover,
68-byte outer stub, and 926-byte ONGCHK component. The source-driven composite
replay rebuilds these groups and reproduces the 13,422-byte decoded flat
payload. That older composite integration still uses an external usage asset and a
resident-candidate path, so it remains a diagnostic flat comparison even
though the resident component is independently exact now. Earlier mixed flats repacked with pinned DIET 1.45f to a
byte-identical 7,754-byte MZ target; that does not make the product source
exact. See [ZUNINIT](reconstruction/zun/TH04_ZUNINIT_SYMBOLIC_COMPONENT_V785.md)
and the [composite note](reconstruction/zun/TH04_ZUN_MIXED_COMPOSITE_V788.md).

Resident cfg_init at COM payload 0xDCF and _main at 0xE67 are now
decoded-exact. v817 found a real compiler/source mechanism rather than an inert
barrier: the maintained /R not-resident branch jumps to the no-space failure
return, while TC4J -B plus pinned TASM32 emits the target 252-byte selective
tail. Direct TC4J still emits 246 bytes and remains the negative control. Two
cold resident links produce the target-identical 6,360-byte component, so
cfg_init also links raw-zero. v819 canonical acceptance then raw-compares all
three ZUN authored functions at zero differences. The ZUN resident notes carry
the focused producer and canonical receipt details.

## Private inputs and cleanup

The canonical cold aggregate receipts in
`.analysis/reconstruction/receipt-archive/` remain verified:

| Artifact | Receipt | SHA-256 |
| --- | --- | --- |
| OP | `v828-op-snd-load-canonical-receipt.json` | `50928bc1bcef1ea614574e94ccb9538f1e7167e7eb7ec92cafc6cbe806f22cc5` |
| MAINE | `v835-maine-regist-canonical-receipt.json` | `a8cc9a62e9d223c1b6a372a4c109139904898874ddc995e44560ffb2a8362092` |
| ZUN | v819-zun-resident-canonical-receipt.json | fe59d4624229bdc111a427d41144b6b6ca93973d729f570831c28805bba03508 |

Preserve `.analysis/targets/`, `.analysis/toolchain/`, `.analysis/ghidra/`,
`.analysis/runtime/images/zun.hdi`, the v401/v402/v489 source snapshots,
DIET replay inputs, and the configured v546 ZUN runtime inventory. All recordable expanded probe and exact-unit replay worktrees present at this
cleanup were checksum-verified into
`replay-heads-v837-20260927.tar.zst` with an adjacent file manifest and archive
checksum (`87f4805bacc4c93c78a3dc0457931df7b882642b053061d83af08791d3019681`), then pruned. Pure scratch directories with no result files or
receipts were deleted rather than archived. Probe scratch now retains only the
configured `v546-zun-runtime-inventory-001` live input; expanded
`exact-unit-replay` worktrees are empty. Stable focused/canonical receipts
remain directly under `.analysis/reconstruction/receipt-archive/`.
These private archives are ignored and exist only on this workspace; a fresh
clone must regenerate evidence from checked-in commands. Historical paths into
pruned worktrees are provenance, not live input promises. Use
`scripts/prune_analysis.py` in dry-run mode before future cleanup; its
probe/exact-replay apply paths verify archive coverage before deletion.

Finish future changes with the focused comparison, `python3 scripts/ci.py`,
`git diff --check`, and an updated handoff when phase or blockers change.
