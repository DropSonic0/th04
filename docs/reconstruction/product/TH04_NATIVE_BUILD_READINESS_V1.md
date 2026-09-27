# TH04 native DOS build readiness

2026-09-27 compiler and source-graph observation. This is a product-build
investigation, not a new exactness or runtime acceptance claim. The two MAIN
authored-function residuals remain deferred for this lane.

## Acceptance model

The TH095 Windows i386 product build supplies the useful separation: close the
production source graph, compile every declared translation unit with the
pinned toolchain, link without unresolved symbols, verify the output format,
then validate the reconstructed executable with the same legal game data and
runtime scenarios as the original. Function-level raw matching has its own
ledger. TH04 needs this model with 16-bit OMF, DOS MZ, segment groups, real-mode
relocations, DIET containers, and PC-98 hardware behavior.

A playable TH04 build need not reproduce target file bytes or target relocation
table order. Its own relocation table must be structurally valid and must
resolve the intended code/data owners. Check MZ entry and stack, fixup sites,
segment/group layout, linker MAP ownership, and load-segment variation before
runtime acceptance. A successful TLINK exit alone cannot establish those facts.

## TH01 build reference, with separate evidence scope

The local ReC98 `Tupfile.lua` declares TH01's `zunsoft`, `op`, `reiiden`, and
`fuuin` as four explicit ordered object lists. Its link rule writes a separate
TLINK response file per artifact with the memory-model startup object,
ordered objects, output and MAP paths, then `emu.lib`, the matching math
library, and the matching C runtime library. The rule explains that letting
TCC spawn TLINK would reuse the fixed `turboc.$ln` response filename and
conflict with parallel builds. The local `build_dumb.bat` gives a serial
fallback, and ReC98's README reports that TH01 source reconstruction was
completed in 2022. We have inspected these build descriptions, not executed
or attested a TH01 output in this workspace.
In that TH01 graph, the large-model OP/REIIDEN/FUUIN object lists do not
name the shipped `masters.lib`; their required support is supplied through
declared source objects. The tiny ZUNSOFT link does name that library.

For TH04, reuse the explicit response-file and ordered-object method, and
keep TC4J/TASM32/TLINK work serial against shared outputs. The exact TH01
object lists, libraries, headers, and source topology are cross-game
corroboration only. TH04's manifest and data owners must be local and checked
against the pinned TH04 artifacts.

## Repository-only compiler probe

`python3 scripts/attest_toolchain.py` passed. An isolated private directory
contained a copy of this repository's `src/` only. The compiler runner and
installed TC4J/TASM32 binaries were pinned external tools; no ReC98 source or
header tree was copied into the probe. For OP/MAINE C++ the flags were
`-c -I. -O -b- -3 -Z -d -DGAME=4 -ml`, with the artifact's `BINARY` definition.
Each translation unit had a separate object output directory. The observation
is one serial compile pass, with strict OMF parsing of each emitted object.

| Source group | Observed result | Limit |
| --- | --- | --- |
| `src/op/**/*.cpp` | 48/48 compiled to valid OMF | No OP entry TU or product link yet |
| `src/maine/**/*.cpp` | Initial 26/26 compiled to valid OMF | The entry and SCORE EGC owners were added later; no product link yet |
| `src/shared/**/*.{cpp,c}` under OP flags | Initial 14/19 directly; current 19/19 with BGIMAGE assembly backend | Four formerly blocked sources now use TH04-owned headers |
| `src/shared/hardware/bgimage.cpp` | TC4J `-B` emitted symbolic ASM; pinned TASM32 assembled valid OMF | The assembler reported one segment-alignment warning, as in the existing focused producer workflow |

The private compiler receipts are
`.analysis/native-product-probe-20260927/{op-compile,maine-compile,shared-op-compile}.json`.
Their SHA-256 values are respectively
`d61930ae38017a386de3dfca92d6647edaae5f691c45b3ffd6eeba74b3a94d05`,
`c79bb00d6504627a2519d31d82b6371732897a99d147710103db3d0487e`, and
`21ffec05e1f5c67168b656db27cb3980a25883bfd93fc28d65fa5664f3169a21`.
They are ignored host-local diagnostics, not checked-in build receipts. A
checked-in product builder must reproduce this test before accepting it.

TC4J maps long source basenames to DOS 8.3 aliases and emits object files with
those aliases. The product builder must isolate output per input and record the
actual generated object name; deriving it from the long source name produced a
false failure in the first diagnostic pass.

The four initial direct shared compile blockers were `vram_planes.cpp`
(`planar.h`), `determine_modes.cpp`, `pmd_resident.c`, and `mmd_resident.c`
(`x86real.h` and `th04/snd/snd.h`). They now include the existing TH04-owned
PC-98, x86, and sound interfaces. `vram_planes.hpp` declares only the four far
VRAM pointers needed by that producer. Two cold OP and MAINE scaffold links
preserve zero raw differences in each of the four accepted function slices;
these replays are function Oracles, not standalone product links. The MMD replay
continues to record one aggregate image difference in each artifact outside
its accepted function slice.

`python3 scripts/probes/probe_th04_native_source_compile.py --output-dir
.analysis/reconstruction/probes/native-th04-source-compile-20260927` then
compiled a fresh copy of only this repository's `src/`: 48 OP, 26 MAINE, and
19 shared C/C++ translation units, all producing valid OMF. BGIMAGE took the
verified TC4J `-B`/TASM32 producer path. The private receipt SHA-256 is
`b1d7a182c0803eeab92bd20effb383c783b1cbc4979237d9e6f9ccb6da42cfd0`.
The pinned MS-DOS runner and installed Borland/TASM tools are external tool
inputs; no ReC98 source or headers enter this compile snapshot.

A second cold source-only compile has receipt SHA-256
`9b9ea408617c7c88aa89f31994b0c8136ec35b1ec3955c4a290b8947034c1255`.
`scripts/probes/compare_th04_native_source_compile.py` compares both receipts
and their objects: 92/93 raw OMF files match; BGIMAGE differs only in TASM's
dependency timestamp comment. All 93 timestamp-normalized objects and all 93
link-relevant OMF record sequences match. The compiler evidence input digest
hashes ordered lines of `source path`, one space, `source_sha256`, and newline
from the first receipt.

## MAINE link diagnosis and reusable Borland lessons

`src/maine/end/entry.cpp` now includes the MAINE entry body and owns its
resident pointer. `src/maine/score/egc_copy.cpp` includes the SCORE EGC setup
and rectangle-copy bodies as one TH04-only translation unit. Both compile with
the pinned TC4J flags above and pass OMF integrity parsing. Their local object
SHA-256 values are `547be4f2446453ea843ea2f0fb15d3876b025bd46be01bb4b4ee5fca3ceac991`
and `eead836896e4b428c596441e1cce7a70c82b5d3cb41f979075ac0a112b69814a`.

An isolated TLINK diagnostic under
`.analysis/reconstruction/probes/native-th04-maine-link-20260927/` uses
28 MAINE C++ objects, 19 shared C/C++ objects, and 25 repository-owned ASM
objects. It uses the pinned startup and runtime libraries plus the external
`_reference/ReC98/bin/masters.lib` as a **calibration support library** (SHA-256
`6be41dbcfcf4504977165ccc44443525a29a01f85a1580e6ad0c620bf802faf6`).
This is not a repository-only product dependency closure. Replay it with
`python3 scripts/probes/probe_th04_native_maine_link.py --output-dir
.analysis/reconstruction/probes/<new-run-id>`. The script copies only this
repository's `src/`, validates every OMF, writes the ordered TLINK response,
and records the unresolved-symbol vector. The final diagnostic
`maine-link-after-score-egc.log` (SHA-256
`cdc95608a6a9c120210e7cc9f4f2328d216fb52e496704e98ef4f931a983e028`)
fails with 45 unresolved symbols. They are principally TH04-owned state plus
unattached cutscene/end/registration bodies. TLINK also warns that the
calibration library has an invalid extended dictionary, which it ignores.
TLINK wrote an incomplete MZ despite its failure: its header parses, and it
lists 204 relocation entries, but unresolved addresses make it unusable.
Neither MZ-header validity nor this relocation count is product or runtime
acceptance.

Two fresh script runs have receipt SHA-256
`847a894387ce866bd1ec957bb2788515d68e33179f77d007c5511fe05a00595c`
and `01208ca1b7be5b1920c1692620c3a50680f22b385f943eededdef7a18efa681b`.
All 72 source hashes and all 72 link-relevant OMF object records agree. Of the
raw OMF files, 71/72 agree; BGIMAGE differs only in non-link comment metadata.
Both links report the same 45 unresolved symbols and warning and write the
same incomplete MZ SHA-256
`ce9c023058be27bccd0d60da6c33ffb9666e77aed8cb21edce5411083050e9c9`.
This repeatability makes the frontier reproducible; it does not make that MZ
usable. The link gate must check TLINK exit status and unresolved externals
before interpreting the MZ relocation table.

The 45-symbol vector currently has four unattached C++ bodies
(`cutscene_animate`, `verdict_animate`, `regist_menu`, and
`staffroll_animate`), 39 underscore-decorated state/data symbols, and two
public ASM data names (`DOTS16_MASK_UNALIGNED`, `cdg_images_to_load`). The
receipt gives each requesting object. The next ownership pass must establish
real definitions and initialization from TH04 evidence; merely adding storage
to silence TLINK would not establish working behavior.

The diagnostic exposed three rules worth replaying in TH05 rather than
assuming its source or binary has the same layout:

1. **C++ parameter identity and C/Pascal linkage matter at link time.**
   MAINE's `box_1_to_0_masked()` caller used a 16-bit enum while its owner
   used `unsigned int`; equal storage width did not make their decorated names
   equal. One TH04-local `box_mask_t` declaration now serves both. The caller
   also used a C++ declaration for `egc_off`, while the hardware header's
   `extern "C"`/Pascal interface names the actual provider. These two repairs
   remove two unresolved symbols. The focused v833 cutscene EGC replay still
   passes its complete accepted body and all 559 ordered MAINE relocations
   (receipt SHA-256 `4b0cbfd0f6746105843b4de8b7b566690500e91da0a96b07be272253bcd8f23a`).
2. **TC4J binds a function to the segment at its first declaration.** A first
   attempt declared `box_1_to_0_masked()` in a header before `#pragma codeseg`.
   It compiled but moved the producer out of `CUTSCENE_TEXT`, so the focused
   v833 standalone producer failed the segment-byte check. Keeping the shared
   enum in the header and the caller's function prototype in its wrapper
   restored the 134-byte `CUTSCENE_TEXT` producer. Check OMF `SEGDEF`, `PUBDEF`,
   and `FIXUPP` after moving prototypes; source order is a link input.
3. **One named code segment needs one group assignment.** Seven independent
   cutscene/SCORE wrappers previously supplied different group names for
   `CUTSCENE_TEXT` and `SCORE_TEXT`. TLINK reported 11 cross-group warnings.
   Assigning `cutscene_01` and `score_01` consistently removes all 11 without
   changing the remaining unresolved count. A TH05 product graph must verify
   its own segment/group pairing before accepting its MAP or relocations.

After these wrapper changes, `python3 scripts/decoded_function_acceptance.py
--artifact th04-maine --output-dir
.analysis/reconstruction/probes/native-th04-maine-accepted-after-group-20260927`
cold-replayed all 72 current MAINE accepted decoded slices at zero difference.
The receipt SHA-256 is
`a6d345acd4d49b413ead40bb7dca18d7c6a4430a5d500d980def71a01bd26973`.
This preserves their existing slice acceptance, not packed-file or native
product acceptance.

The failure count is a closure metric, not an exactness metric: the initial
71-object link had 99 unresolved symbols, adding the calibration library left
48, the ABI repair left 46, and the SCORE EGC owner left 45. The library step
establishes only which declarations it offers; it does not attest TH04 product
ownership. Each newly attached `.inl` must be chosen by an actual owner and
ABI review.

### Cutscene state owner

`src/maine/cutscene/state.hpp` and `state.cpp` now declare and define the
cutscene script buffer/pointer, cursor, text timing, fast-forward flag,
background pointer, default script parameter, and five EGC box masks. The
cursor and background-plane structures have explicit 4-byte and 8-byte
compiler size checks. The `BOX_MASKS` table is a semantic 5-by-4 word mask:
`scripts/probes/probe_th04_maine_box_masks.py` finds its exact 40 bytes only
once in the attested decoded MAINE image at load offset `0xEB5C` (SHA-256
`82be2a7b8e42f643e7912bbd106d55480fa74435a0a5c1fdf58c0fd8f5ca236c`).
The candidate MAP's `_BOX_MASKS` at `0E53:062C` corroborates the address but
does not establish the original source name. The probe receipt SHA-256 is
`e50f0ea6b9c84e85f3c19e9447ec58110a95717621ac297ecd97de68b0c0e71f`.

A fresh 73-object native diagnostic compiles this source and reduces the
unresolved vector from 45 to 37, resolving precisely `_BOX_MASKS`, `_box_bg`,
`_cursor`, `_fast_forward`, `_script`, `_script_p`,
`_script_param_number_default`, and `_text_interval` with no new undefineds.
The new 37-symbol frontier is still a failed link; TLINK again writes an
incomplete MZ with 204 relocation entries. Its receipt SHA-256 is
`17f012a22b7d8f1a9699c346878f6ddde7ae1bcb467b415dbe4a92ca4dbaea0a`.
The BSS initialization and cutscene behavior still require runtime checks.
A second cold 73-object build has receipt SHA-256
`8c4ac21cc00176345e4f0eed91f4af30464ac6459061faebc09351ca0e38e569`.
Both rounds have identical source hashes, all 73 link-relevant OMF records,
the 37-symbol unresolved vector, warning vector, and incomplete MZ. BGIMAGE
is the sole raw OMF difference, again confined to non-link comments.
`scripts/probes/compare_th04_native_maine_link.py` checks the two receipts,
including the ordered source/object identities, unresolved vector, linker
result, warning vector, and incomplete MZ hash.

The remaining 37 symbols split into four unattached cutscene/end/score C++
bodies, eight graphics/input state symbols (four VRAM pointers, BGIMAGE,
`key_det`, `shiftkey`, and the flip LUT), nine sound state symbols, and sixteen
MAINE score/PI/CDG/memory data symbols. This groups owner searches; it does
not authorize data definitions without width, initialization, and consumer
checks. The link receipt retains every requesting object and exact symbol.

One integration attempt moved existing cutscene wrappers' declarations into
the new state header. The native all-source compile passed, but the v585
accepted `box_bg_free` Oracle failed before comparison: its historical
isolated-source replay copied the wrapper, body, and HMem header, not the new
header (`compile-v585-box-bg-free-standalone-a.log` SHA-256
`92f09d692ba35ca984f41f83b2b523208b923984eb156f7b56f9ddcdef348b19`).
The state owner retains its local header, while established accepted wrappers
retain their self-contained declarations. Focused v585 A/B replay then
raw-matched the complete 31-byte function with 559 ordered relocations
(receipt SHA-256 `3100e6d3ec70c392562fdd3988dcd60db5e8a4206d19b2f84e180bdd3be0ab7f`).
For a future TH05 declaration migration, update the isolated replay's
transitive source-copy closure and rerun accepted extents; a product compile
alone cannot establish that an accepted unit remains reproducible.

After the cutscene-state wrapper composition stabilized, a fresh cold
`decoded_function_acceptance.py --artifact th04-maine` run again passed all
72 accepted MAINE slices. Receipt SHA-256:
`80e101f5308e2755c9225035cc83fb66371c55e1501f37db0216c12872238ad5`.
This run precedes the subsequent hardware, sound, score, and CDG data owners;
their layout changes require another aggregate replay before promotion.

### TH04 state closure and the DGROUP trap

Four TH04-local ASM state owners now provide the four far VRAM plane pointers,
the 8-byte BGIMAGE segment set, the keyboard detection/shift state, and the
256-byte generated bit-flip lookup table. Their widths and consumer order are
declared in this repository's hardware headers and ASM users. A 77-object
TC4J/TASM32 diagnostic removed precisely eight unresolved hardware symbols,
37 to 29, without new undefineds (receipt SHA-256
`7edac4c567adb598b5f57a92334c8bb5eab986ce38424acc1f540d835d9970fb`).

`src/shared/sound/state.cpp` supplies the driver flags, SE playing/frame
state, 17-entry priority and frame tables, 13-byte load filename, and the
four extension pointers. The 34 table bytes occur uniquely at decoded MAINE
load offset `0xEA8E`; the target has `FF 00` at SE playing/frame offset
`0xEB30`, and the extension strings occur uniquely at `0xEAE4`. The target
probe receipt SHA-256 is
`c51ec3721f819bc3a1abd5c3e29ea3ad1e9ff65a578bd64de1ac23c0fb366aed`.
The 78-object native link reduced 29 unresolved symbols to 20 (receipt
SHA-256 `91f3d4740f1fedd4f5765b593396ce561e9ec110a16b49e21146721bbe12e0ee`).
The same 34 table bytes occur uniquely in the independently attested OP
decoded load image at `0xFCFE`; OP also has `FF 00` at `0xFD80` and the same
extension strings uniquely at `0xFD34`. Its target-only probe receipt SHA-256
is `17df17d971985aa32c2d53348d5b3b57d6c3b394e6ffa02c71ab6546c2caa97f`.
This supports a shared TH04 sound-state definition without claiming an OP
product link or runtime behavior.

Score state, six PI slots, the 16-bit allocation-limit variable, and CDG
storage/mask owners then removed the other sixteen data symbols. The target
probe uniquely locates the 51-byte registration alphabet at `0xED5C`, four
consecutive `GENSOU.SCR` strings at `0xED8F`, and the 32-byte first-word CDG
mask table at `0xEAB0` (receipt SHA-256
`1a4f688275cbea794867cdfcc209fa95f330ecd90f949de2f2d65f392020a3a1`).
The v489 candidate MAP labels these positions as `0E53:082C` alphabet,
`0E53:085F..0880` score filenames, and `0E53:0580` CDG masks; those names
and segment assignments are corroboration, while the unique decoded bytes
are the target observations.
The score section remains a 196-byte BSS structure with compiler-checked
field offsets; uniqueness of the initialized bytes does not attest its BSS
runtime contents.

The first 83-object link reported **zero** unresolved symbols but failed with
`Group DGROUP exceeds 64K`, produced no MZ, and exited 2 (receipt SHA-256
`4332ba7e6c3969c80d81a43a83004202e5629fda9a87d924bc81aaceebd3c247`).
The cause was ASM `DGROUP group _TEXT, ...` declarations: Borland combines
all like-named `_TEXT` contributions before applying group membership, which
pulled program code into the 64 KB data group. This was latent while
unresolved-symbol errors kept the earlier links incomplete. The local
historical MASTER archive's 40 candidate support objects declare 921 bytes
of explicit DGROUP data and **zero** `_TEXT` members; the candidate MAINE MAP
also lists `_TEXT` outside any group. Correct large-model product declarations
group only `_DATA` and `_BSS`. This is a segment ownership requirement, not
a cosmetic linker-warning fix.

Three older TH04 ZUN support modules (`master_version.asm`, `graph_state.asm`,
`file_state.asm`) had accepted OMF identities with `_TEXT` grouped for their
resident replay. A direct edit to their group declaration failed that
replay's VERSION OMF check. They now select data-only DGROUP when
`TH04_LARGE_PRODUCT` is defined by the native product assembler and retain
their earlier declaration otherwise. A cold ZUN file-state replay restored
the accepted 6360-byte component SHA-256
`a15ee1e7eac9616e9cd60656a00fb5700c7e20299efc2c0ab44bce6c27cf1dab`
(receipt SHA-256 `28cca84065f20929c50719b2c1e4e6874012633d4aefa49ba1591d2ffbe1b272`).
The first data-only group diagnostic, before the explicit macro was added,
linked without overflow and left exactly four unresolved C++ bodies. Its
parsable MZ has 369 relocations but is incomplete because TLINK exited 1
(receipt SHA-256 `a721b0ae176bce78ed18e95352ef953c34738854a83a6b970167cd6fa943699c`).
Two further cold builds with the explicit large-product assembler flag
reproduce this four-symbol frontier. All 83 link-relevant and
timestamp-normalized OMF objects agree; BGIMAGE's raw OMF alone contains a
dependency timestamp difference. The unresolved vector, warning vector,
linker response, incomplete MZ SHA-256, and 369 relocation entries are the
same. A/B receipt SHA-256 values are
`2c7b4ce38b6b9b86a591dd4d18623f41d69e930efa38d29837410d0242402b74`
and `b9d72312efa3fe55b730fcc84b30c7113d08efeab9b23f8448015b83f03c1a54`.
TLINK's nonzero exit still forbids interpreting that MZ as runnable.

Dropping the external `masters.lib` from that same 83-object response reveals
55 unresolved symbols: the four C++ bodies plus 51 PC-98 runtime/library
names supplied by 40 historical archive members. The no-library link log
SHA-256 is `f00147c200a300cd611b2382e7fdb2089b6ed8b69bc20aa6dc12eeb9e069834f`.
Earlier archive analysis also found that this support library's extracted
objects use an ABI unsuitable as a drop-in large-model product dependency.
It remains a link-frontier diagnostic; TH04's runnable product still needs
local, ABI-checked runtime owners.

The native link probe now has `--without-support`, which performs the entire
serial TH04 source/OMF build and omits the historical archive from the TLINK
response. Its fresh 83-object run reproduces the 55-name frontier with zero
warnings and a nonzero linker exit (receipt SHA-256
`6211764483f4996991d6d11ee91f4e13cbf521322364bf84b13b9979adbcf17f`).
The receipt records `support_lib_sha256 = null`, so the omitted archive is
visible in the replay identity rather than inferred from a manually edited
response.

For the next body-owner pass, a target-only probe attests 21 contiguous
staffroll resource filenames at MAINE load `0xEB88` and the verdict image
name `ude.pi` uniquely at `0xED54` (receipt SHA-256
`bd5a62aca5c3c1ca2255e1614452e4e032b683ca6ec6e355fecedcdcec27e1e7`).
The v489 MAP supplies candidate names and segment addresses for these bytes;
their source owners and successful asset loads remain open.

The first TH04-local body-owner pass attaches the seven staffroll dissolve
helpers and `staffroll_animate` in `src/maine/end/staff.cpp`. A new local CDG
header checks the 16-byte slot structure against the TH04 ASM field offsets.
`staff_resources.cpp` owns 21 addressable filenames; their concatenated
193-byte block appears once in the freshly compiled OMF and equals the
target-observed block hash
`a3228d034412eb1d9611eb6a136cd7f30a98a0b1c4f818b691ae0993798db55a`.
The first staff compile exposed a missing declaration for the already owned
`BGIMAGE_PUT_RECT_16` ASM routine. A local FAR Pascal four-word declaration,
checked against its `proc far` and argument list, closed that compile error.
With no historical support archive, the 85-object link still has 55 undefined
names: `staffroll_animate()` disappears and `GRCG_SETCOLOR` appears as an
additional runtime dependency. Comparing only the count would hide this
progress; compare symbol identities without the `in module` suffix because TLINK also changes
which referencing module it names when object order changes. TLINK still exits
nonzero, so this is not a runnable or relocation-accepted product.
Two fresh 85-object no-archive builds match in every link-relevant and
dependency-timestamp-normalized OMF record, unresolved vector, warning vector,
and incomplete MZ. Their receipt SHA-256 values are
`14e158222412736dce205941083dc512cfe3801ccab4da76f3fdda31928bef39`
and `3b39e4efba3e6189d607e37442283938b785dd8934d3729a1f50ebc3a11d1af1`.
Only BGIMAGE's raw OMF dependency timestamp differs. This is a deterministic
link failure, not product acceptance.
With the same 85 TH04 objects and the pinned historical archive added solely
for calibration, TLINK reports the three remaining C++ bodies:
`cutscene_animate`, `verdict_animate`, and `regist_menu`. Its nonzero exit also
reports the archive's invalid extended-dictionary warning. The partial MZ has
468 relocation entries, which cannot enter load-segment acceptance. Calibration
receipt SHA-256:
`b77239850bc753f6972bbae6dc20634a69808ab32c00c70707620c02d83c7c77`.

The next local owner is `src/maine/score/regist.cpp`, which includes the
accepted 924-byte `regist_menu` body in `SCORE_TEXT` with TH04 declarations.
A target-only probe identifies one 126-byte registration resource block at
MAINE decoded load `0xEDBB`: `hi01.pi`, `scnum2.bft`, two separately
addressable copies of the 50-byte Shift-JIS slow-mode notice, and `name`.
`regist_resources.cpp` emits the same contiguous block once in its freshly
compiled OMF. The local input flag values agree with owned `input_s.asm`, and
the `graph_putsa_fx` declaration follows the owned FAR Pascal ASM's `RETF 10`.
TC4J confirms `playchar_t` occupies one byte in the current profile; the
product state now uses that type. The first focused compile failed because
`resident.hpp` relies on `types.hpp` for the 16-bit Borland `bool`; the new
wrapper includes the latter first and then compiles. Keep this include order
when migrating TH05 wrappers, or explicitly update and cold-replay all old
isolated snapshot closures before changing a shared header.

The fresh 87-object no-archive link resolves `regist_menu()` and exposes
`SUPER_ENTRY_BFNT` and `SUPER_FREE`; the total unresolved vector is now 56
with zero warnings and TLINK exit 1 (receipt SHA-256
`a6b3ee7260f06bc59979bd187d07c171e8723e33b24baca4a825f9ca821e94cd`).
No partial MZ or relocation count from this failed link is product evidence.
The affected MAINE 72-slice cold aggregate passed all accepted raw-zero
comparisons after the input/playchar/score declaration changes (receipt
SHA-256 `871e1398afab56d030e7d0ea04688ca497cc0f40bc01df466cdd00a5ed2726be`).
An independent second 87-object no-archive build agrees on every
link-relevant and timestamp-normalized OMF object, the 56-name unresolved
vector, warning vector, and incomplete MZ. Its receipt SHA-256 is
`ae12c7fe1ce0fd1b7d056937671b0abf4eb369cee3eb15a66f27b9b547c91a31`;
only BGIMAGE's raw OMF dependency timestamp differs. This remains a
deterministic failed product link.
An independently recorded TLIB public-name inventory maps 54 of those 56
names to 43 members of the pinned historical archive. Only the two remaining
C++ bodies have no archive public-name provider. This gives a bounded local
runtime porting queue; a public-name match does not attest memory model,
calling convention, fixups, or suitability for a standalone product.
`scripts/probes/inventory_th04_maine_support.py` checks the no-archive receipt,
archive digest, and TLIB listing digest before reporting this mapping.

For the following verdict owner, a target-only probe identifies a unique
274-byte display data block at MAINE decoded load `0xEC4A` (candidate MAP
`0E53:071A`). It checks eight CP932 label/file anchors, including `_ude.txt`
and the slowdown verdict text; SHA-256 is
`6d588142d2da59f6b695df4bf51f6e77ca1fc3ca36e206278ae2f5774324a8aa`.
This prepares an ownership boundary only. The verdict's helper code, state,
resource arrays, and runtime reads have not yet been attached to the product.

The verdict owner now compiles seven TH04-local bodies in `verdict.cpp`, with
separate state and named CP932 resources. Its text-buffer terminator is an
alias of byte 28 inside the 30-byte buffer, matching the candidate v489 MAP's
overlapping addresses; it is not a second allocation. A focused OMF Oracle checks
all 25 resource public names and the complete 271 semantic bytes of gaiji
labels and strings against the attested target, excluding three zero-valued
state/alignment bytes from the target's 274-byte block (receipt SHA-256
`ca2d417736b1c8fcf25d2303cc65582d37b75c82e41004797e1cec9b3f66e948`).
The first wrapper compile found `V_WHITE` missing; including the existing
TH04 color owner closed the compiler error. The current 90-object no-archive
link resolves `verdict_animate()` and exposes `_random_seed`; it still has 56
unresolved names, zero warnings, and TLINK exit 1 (receipt SHA-256
`84bf73b78043832507c2cb2e786784fa3bc16aa90e7097b32906beaad20f4604`).
Its 407-entry partial-MZ relocation table is diagnostic only. The pinned TLIB
public-name inventory now maps 55 of the 56 names to 44 historical members;
`cutscene_animate()` alone lacks an archive public-name provider. These
members remain ABI-unaccepted calibration inputs.
Two 90-object no-archive cold builds agree in every link-relevant and
timestamp-normalized OMF object, the 56-name unresolved vector, warnings,
and incomplete MZ. The second receipt SHA-256 is
`a2226805c0f0c149dab115d531e6c1cdb194917b69b16d7abaf2afa9cbd250e6`;
BGIMAGE's raw OMF differs only in a dependency timestamp.

The cutscene product owner now compiles the maintained
`cutscene_animate.inl`, `script_op.inl`, `box_bg_snap.inl`, and
`pic_put_both_masked.inl` through TH04-local translation units. The accepted
function bodies themselves were not edited. A target and OMF probe checks
`PI_MASKS` at MAINE `0E53:060C` (decoded load `0xEB3C`), the adjacent
`BOX_MASKS` at `0E53:062C`, and the initial `CUTSCENE_KANJI` buffer at
`0E53:0654`: the PI mask object matches all 32 target bytes, while the state
object matches 44 contiguous mask and glyph bytes with publics at offsets 0
and 40. Its receipt SHA-256 is
`07c4ede8d1812ae9346f51e14d41320217536f66deaeea6ca016931bf034a6cd`.
The buffer begins with two ASCII spaces and two zero bytes; the target's
decoded load, rather than the candidate MAP alone, establishes those bytes.

The first 92-object product probe attached the glyph loop and background
snapshot, replacing the unresolved `cutscene_animate()` name with
`script_op(unsigned char)`. Adding the local script interpreter, PI masks,
and masked blit yields 95 objects. The no-archive link now has 62 distinct
unresolved names, no warnings, TLINK exit 1, and a parsable but incomplete MZ
with 430 relocation entries (receipt SHA-256
`669aa2d33aa0c90bbbbedb12611a6609771f158299b94c1d36200d2e64cc525d`).
The seven new names are the previously unreachable masked-pixel writer,
scroll, byte-box fill, GRCG disable, white fades, and SE update; the cutscene
C++ bodies themselves are resolved. A second cold build agrees on all 95
link-relevant and timestamp-normalized OMF objects and the full failure
signature (receipt SHA-256
`72b724ec69114d9650f8ae6965e6b6e00e74e847d9f2036bde231466272e5ba4`).
Only BGIMAGE's raw object dependency timestamp differs. A pinned archive
public-name inventory routes 60 of the 62 remaining names to 48 historical
members. `GRAPH_PACK_PUT_8_NOCLIP` and `_snd_se_update` have no archive
provider; the first has a separate upstream candidate assembly implementation,
and the second has a TH04 OP accepted source body. Neither is yet a MAINE
product owner. The archive remains calibration evidence, not a product input.
After restoring the accepted box-background wrapper composition, the complete
MAINE cold aggregate passes all 72 accepted function slices raw-zero (receipt
SHA-256 `a1fff45cf81442a1aa3e0a6ae6c253c958a283e4c1aa1be028dc841fcd118471`).

This exposes two useful TH05 checks: an accepted body can reveal additional
support calls only after its real caller and command parser are linked, so
compare symbol identities across cold links; and initialized masks next to
mutable glyph storage should be checked through both target bytes and OMF
public offsets before merging them into one data owner. This TH04 cutscene
wrapper is TH04-only; ReC98's shared TH01–TH05 build response supplied no
standalone artifact manifest.

The first aggregate repeated the previously recorded v839 isolated-header
closure failure after a temporary `state.hpp` include in two accepted wrappers.
Those changes were reverted; both wrapper backends and the complete aggregate
then passed. The v839 knowledge row remains the reusable TH05 warning.
The two link receipts above were generated again from this final wrapper
composition: all 95 link-relevant objects, the 62-name unresolved vector,
warnings, and failed-link MZ agree. The prior v844 receipts remain historical
diagnostics with a different wrapper source hash.

## Current build-graph gaps

- MAIN C/C++ source has 764 quoted include sites whose paths do not resolve
  inside this repository, covering 102 distinct include paths, including
  headers and source fragments. This is a source-graph observation, not a
  count of missing semantic declarations.
- No checked-in OP product translation unit includes `src/op/main/main.inl`.
  MAINE now has `src/maine/end/entry.cpp` for `main.inl`; other bounded
  `.inl` fragments also lack a product TU; some are historical overlapping
  replay fragments and must be selected by ownership rather than bulk-included.
- The source tree has no four-artifact product link manifest that assigns every
  C/C++/ASM TU, object order, startup object, library, segment, and output.
  ReC98 linker responses are calibration evidence, not this manifest.
- OP, MAINE, and ZUN still need a product DIET/container route. ZUN's source
  composite additionally retains the documented usage-asset input.
- The current PC-98 smoke script boots the original private disk image. It
  does not install and exercise a candidate build.

## Build lane

TH095's local `scripts/build-whole.py` checks the manifest against all source
TUs before a real link and reports unique unresolved names separately from
their diagnostic line count. Its `audit-owner-relocations.py` inventories
relocation targets in writable and zero-fill data. The TH04 equivalent should
retain these ownership gates but use OMF PUBDEF/EXTDEF/FIXUPP, TLINK MAP, MZ
segment:offset relocation sites, and two DOS load segments rather than PE
DIR32/import-section rules. The TH095 scripts are method evidence only; their
addresses and libraries are unrelated to TH04.

Start with an explicit OP/MAINE source and object graph, then attach the
missing entry bodies and shared producers. Compile all declared TUs serially
with TC4J or TC4J `-B` plus TASM32 as required; validate each OMF record and
fixup. Link with TLINK without unresolved externals, duplicate storage, fake
stubs, or target-byte patches. Record flags, ordered objects/libraries, MAP,
MZ header, entry/stack, relocation sites and resolved symbol owners in a
repeatable receipt. The output may differ bytewise from the target.

After the unpacked programs link, package the three DIET artifacts and test
each candidate in a disposable copy of the legal game image. Boot the original
first under the same emulator configuration. Compare startup, menu, gameplay,
input, sound, save/config state, and transitions using memory, event, VRAM,
palette, and audio checkpoints. Repeat with a changed DOS load segment where
the emulator supports it. Freeze a runnable candidate by source revision and
artifact hashes; later source changes need fresh runtime validation.
