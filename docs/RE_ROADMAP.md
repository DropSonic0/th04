# TH04 reconstruction roadmap

Updated 2026-09-27 for the native product-build lane. This is the
current next-work map. Use
`python3 scripts/status.py` and the live ledgers for counts. Versioned notes,
`config/evidence.csv`, and version-tagged `config/knowledge.csv` rows are a
chronological evidence history, not current-state authority; later accepted
rows may supersede earlier blocker language.

## Current baseline

| Artifact | Exact authored functions | Remaining | Reviewed boundaries |
| --- | ---: | ---: | ---: |
| OP.EXE | 93 / 93 | 0 | 93 / 93 |
| MAIN.EXE | 493 / 495 | 2 blocked | 495 / 495 |
| MAINE.EXE | 72 / 72 | 0 | 72 / 72 |
| ZUN.COM | 3 / 3 | 0 | 3 / 3 |

OP has 14,692 accepted decoded source-owner bytes out of 14,692 tracked; ZUN
has 442 out of 442. MAINE decoded owner coverage is reported by the live status
and acceptance ledgers rather than a hand-maintained packed-file denominator. MAIN's 83,444 / 83,469 exact authored C/C++
bytes cover only its reviewed file-backed owner extents.

ZUN now has all three reviewed authored functions decoded-exact. OP and MAINE
have completed their current original-ASM boundary reviews. v820 remains a valid
negative for the ordinary TC4J / `-B` compiler paths, while v821 and v824-v827
establish bounded hybrid exceptions using independent release-target evidence.

v828 closes the last OP blocker, the 234-byte `SND_LOAD`. v823 had already
shown that the complete function becomes raw-zero when its sole `8B D8` natural
residual is expressed as the target-equivalent `89 C3` register direction.
v828 supplies the missing independent precedent: the already accepted v394
`dialog_face_unput_8` hybrid uses the same `MOV BX,AX` primitive, and TH04/TH05
release targets independently preserve the common `8B 46 04 89 C3` sequence.
The maintained complete loader reuses the accepted natural/v391 fragments and
keeps only that two-byte register-direction primitive symbolic. Two cold OP
links reproduce the full function and all 804 relocations; canonical v828
passes **93/93** accepted OP slices raw-zero. OP's authored-function queue is
therefore closed.

Do not reopen the accepted OP producers without evidence that changes their
boundaries or source-policy basis.

v829 independently closes MAINE's 234-byte SND_LOAD using MAINE-local cold
compile/link evidence and all 559 ordered relocations. v830 then independently
closes MAINE SND_SE_PLAY and _snd_se_update by replaying the complete 0x86
shared-sound producer inside MAINE. v832 closes scoredat_decode and
scoredat_encode with maintained C++ plus the TH03 release-backed single-byte
ROR primitive. v833 closes cutscene egc_start_copy plus the 134-byte
box_1_to_0_masked using a release-backed EGC word-write primitive. v834 removes
the SCORE EGC helper's decomp-only `_outportb_` / `keep_0` mechanisms while
preserving exact SCORE code, final EXE bytes, and 559 relocations. v835 then
removes `optimization_barrier()` from the 924-byte `regist_menu`: materially
new pinned-TC4.02 evidence shows an ordinary semantic conditional expression
naturally emits the target direct-CMP/JZ/JMP frontier. Canonical v835 checks
**72/72** MAINE accepted slices raw-zero. MAINE's authored-function queue is closed.

v831 remains a useful historical negative: switch source preserved target
JZ+JMP but used MOV/OR, while direct if/goto emitted direct CMP but merged the
JMP. v835 supersedes that frontier with materially new pinned-compiler evidence;
do not reintroduce the old barrier or repeat the closed matrix.

ZUN, OP, and MAINE authored-function queues are closed. v836 also closes
MAIN `SND_LOAD` as one complete 234-byte owner, superseding the historical
232/234 partial-owner frontier. MAIN is now **493/495 exact** with only
25 residual authored bytes: 23 in `carpet_lighting_put_new` and 2 in
`playfield_checkerboard_grcg_tdw_`.

The two MAIN producers remain exactness work. The current user-directed build
lane defers them while closing TH04-only source dependencies, standalone
product construction, relocation checks, DIET packaging, and runtime
validation. Decoded-function exactness is not packed-file exactness.

## ZUN.COM: authored-function source authority closed

The natural Tiny-model MEMCHK `_main` at payload `0x26A7` is the first exact
ZUN authored function. Together with maintained shared `DOS_PUTS2` and
`DOS_MAXFREE`, it cold-links a raw-identical 4,066-byte MEMCHK component. The
helpers are library support, not authored-function credit. See
`docs/reconstruction/zun/TH04_ZUN_MEMCHK_NATURAL_EXACT_V773.md`.

The complete 1,141-byte ZUNINIT component now cold-links raw-identically from
six maintained symbolic original-style ASM units. The 223-byte launcher
selector also cold-links raw-identically from maintained ASM. These decoded
component results do not grant packed `ZUN.COM` exactness. The 8-byte selector
mover and 68-byte outer customization stub also have raw-identical maintained
ASM. The embedded 926-byte ONGCHK third-party library component now cold-links
raw-identically from maintained symbolic ASM. The generated selector directory,
external usage asset and DIET product build remain on the
artifact-closure path. The checked-in composite builder reproduces the
directory and complete decoded flat payload with maintained ONGCHK. A serial
source-driven replay now rebuilds all five maintained component groups before
the same raw-identical flat comparison; the external usage asset and older
resident-candidate integration path keep that composite replay diagnostic. Pinned
DIET 1.45f repacked the earlier mixed flats to byte-identical MZ targets; its
source-acceptance verdict remains none. Treat historical
disassembler-generated assembly as a candidate, not original-source evidence.

Resident cfg_init and _main are now decoded-exact. v817 found the missing
producer mechanism without importing inert barriers: maintained _main makes
the real /R not-resident path jump to the no-space failure return, and TC4J's
-B assembly backend followed by pinned TASM32 emits the target 252-byte
selective-tail body. Direct TC4J remains a 246-byte negative control. Two cold
v817 rounds link the complete 6,360-byte resident component raw-identically,
which also resolves every cfg_init FIXUPP value. v819 canonical decoded
acceptance raw-compares cfg_init, resident _main, and MEMCHK _main at zero
differences. This closes ZUN's authored-function queue, not whole ZUN.COM:
packed byte ownership, the composite product route, DIET closure, and runtime
validation remain separate artifact-closure work.

## OP.EXE and MAINE.EXE: strict source frontiers

OP's `CDG_PUT_NOCOLORS_8` now has a target-reviewed 0x51-byte FAR body and
an OP-local maintained original-style ASM unit whose complete 0x52-byte
contribution cold-links raw-zero. Its final byte is alignment and its ReC98
starting source retains candidate provenance. See the v806 focused note.

The complete `CDG_LOAD` and `CDG_PUT_8` original-ASM contributions are now
backed by maintained sources under `src/shared/formats/`. MAIN, OP, and MAINE
cold-link the respective 0x164-byte loader and 0x9E-byte renderer raw-identically,
with all ordered relocation positions preserved. OP/MAINE have decoded
`source-present` units; this does not alter authored-function counts or prove
packed-file offsets. See the v797 and v799 focused CDG notes under
`docs/reconstruction/op-maine/`. All 16 OP and 15 MAINE original-ASM
function-like observations now have reviewed boundaries and source-backed
modules; this does not grant authored C++ or packed-file credit.

The 0x10A-byte `INPUT_S` shared original-ASM contribution also cold-links
raw-identically across MAIN, OP, and MAINE from
`src/shared/hardware/input_s.asm`; see the v802 input note in the same
directory. The OP/MAINE rows remain decoded `source-present` units.

The separate 0x82-byte `BGIMAGE_PUT_RECT_16` ASM contribution now cold-links
raw-identically in OP and MAINE from `src/shared/hardware/bgimager.asm`.
Its ReC98 source origin remains candidate provenance; the local v805 replay
establishes artifact-specific bytes and layout. It does not change the C++
BGIMAGE producer or its accepted function count.

All authored candidate boundaries in both artifacts are reviewed. OP is
93/93 decoded-exact as of v828. MAINE is **72/72 decoded-exact** as of v835:
v829 closes `SND_LOAD`, v830 closes shared sound, v832 both SCORE codecs, v833
the cutscene EGC pair, v834 the SCORE EGC helper, and v835 `regist_menu`.

For OP, v820 remains a useful negative control for ordinary TC4J / `-B`
lowering. v821 and v824-v828 record the bounded hybrid exceptions backed by
independent release-target evidence. v828 closes SND_LOAD by reusing the
already accepted v394 TH04/TH05 `dialog_face_unput_8` register-direction
precedent for its sole two-byte `MOV BX,AX` residual. OP has no remaining
authored-function blockers; literal original-source spelling and packed-file
exactness remain separate claims.

See
`docs/reconstruction/op-maine/TH04_OP_SCORE_CODECS_HYBRID_V821.md`,
`docs/reconstruction/op-maine/TH04_OP_SND_SE_SHARED_V822.md`,
`docs/reconstruction/op-maine/TH04_OP_SND_SE_CROSSGAME_V827.md`,
`docs/reconstruction/op-maine/TH04_OP_EGC_START_HYBRID_V824.md`,
`docs/reconstruction/op-maine/TH04_OP_EGC_COPY_HYBRID_V825.md`,
`docs/reconstruction/op-maine/TH04_OP_NOPOLY_HYBRID_V826.md`,
`docs/reconstruction/op-maine/TH04_OP_SND_LOAD_PROVENANCE_V823.md`,
`docs/reconstruction/op-maine/TH04_OP_SND_LOAD_HYBRID_V828.md`,
`docs/reconstruction/op-maine/TH04_OP_STRICT_FRONTIER_V766.md`,
`docs/reconstruction/op-maine/TH04_MAINE_SCORE_CODECS_HYBRID_V832.md`,
`docs/reconstruction/op-maine/TH04_MAINE_SCORE_EGC_HYBRID_V834.md`,
`docs/reconstruction/op-maine/TH04_MAINE_REGIST_TERNARY_V835.md`, and
`docs/reconstruction/op-maine/TH04_MAINE_STRICT_FRONTIER_V732.md`.

For a new exact function, review the complete physical owner, build its
natural source in isolated cold rounds, compare producer OMF/layout/ordered
relocations and every raw byte, bind the backend to the maintained source,
then replay the affected aggregate. A decoded match is not a packed-file match.

## Artifact closure

After source ownership and function queues close:

1. Cold-replay all maintained producers and affected shared dependencies.
2. Establish honest file-backed authored-source coverage for packed artifacts.
3. Reconstruct and compare complete MZ containers, DIET packing, relocation
   order, runtime/library bytes, padding, and overlays.
4. Check deterministic PC-98 runtime invariants under pinned scenarios.
5. Remove remaining calibration-scaffold build dependencies before claiming a
   standalone TH04 build.

The four original files and toolchain are locally attested candidates, not
independently proven pristine release media. Function exactness never implies
whole-artifact exactness.

## Handoff and hygiene

`docs/RE_HANDOFF.md` is the concise current-state index.
`docs/PROGRESS.md` and `docs/BOUNDARY_REVIEW.md` are generated views.
`docs/reconstruction/README.md` routes bounded historical evidence and
negative results. Private probe worktrees are disposable after required
receipts and digests are retained; preserve pinned targets, tools, databases,
runtime images, and configured replay inputs.
