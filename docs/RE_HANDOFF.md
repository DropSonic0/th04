# TH04 reconstruction handoff

Updated 2026-09-29 after the thirty-first MAIN declaration closure batch. This is the
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
The later v905 aggregate redirects all 77 staged `th04/snd/snd.h` include
occurrences to the maintained shared sound API. Two cold builds again preserve
all 275 accepted extents, MAP ownership, and relocations; receipt SHA-256
`c5ed61a81376b3cf3873f688d78106f8adc6b17522cd194d8879539a2ac60124`.
The v909 aggregate redirects 68 staged `th04/sprites/main_pat.h` occurrences
to the TH04-owned pattern/cel tables. The first attempt caught a missing
TH05-shared `PAT_ENEMY_KILL_last` declaration; after closing that cross-game
consumer, both cold builds preserve all 275 accepted extents, MAP ownership,
and relocations. Receipt SHA-256
`39830c44554d42b25933c113c5de32596e7e483cc5204a6e0475f94e32f22c4c`.
The v913 aggregate redirects 27 staged `th04/math/vector.hpp` occurrences to
the artifact-local vector API. Its receipt explicitly contains the new header
snapshot and rewrite records; both cold builds preserve all 275 accepted
extents, MAP ownership, and relocations. Receipt SHA-256
`075ee33b00435a57dd2a0b290c17b8b749f22df13f4ac7914418f919f3d1e1dd`.
An earlier v911 green aggregate omitted the configured rewrite because its
new header was not frozen; it is retained only as replay-control negative
evidence, not as vector closure.
The v915 aggregate freezes the product-owned scroll API and redirects 29 staged
`th04/main/scroll.hpp` occurrences. Both cold builds preserve all 275 accepted
extents, MAP ownership, and relocations. Receipt SHA-256
`cf41672289dee812d46bb6fa15ad94201b506489d40ae71150fa9cb55766a532`.
The scaffold-built MAIN.EXE is 152,974 bytes versus the 156,258-byte target;
the complete MZ comparison rejects raw identity. All 1,136 relocation sites
and site values match, but their entry order differs. This remains a
diagnostic ReC98 scaffold build, not a standalone TH04 product or a
runtime-tested replacement. Next
work is packed-container ownership, DIET/link-layout closure, standalone
product construction, and a candidate runtime scenario; the two MAIN bodies
remain unresolved exactness work.

## Native build handoff

The [native OP note](reconstruction/product/TH04_NATIVE_OP_LINK_V875.md) and
`config/native_op_sources.toml` now define a source-only 156-object OP build
(110 C/C++, 46 ASM). Two cold pinned TC4J/TASM32/TLINK links without
`masters.lib` produce the same complete 75,212-byte MZ SHA-256
`a21afefaf14c9530c2e6b85c4fd322bfe8b8c25bbd92c063c2534655f1f41180`,
with zero unresolved names, errors, or warnings. The static audit validates
799 relocation sites at DOS loads `0x2000` and `0x6000`; the ABI audit checks
17 far-return ASM owners, 82 relocated direct far calls, and three same-CS
calls. The ZUN/Tiny near `DOS_PUTS2` remains unchanged; OP uses its own far
Pascal four-byte-pointer provider. This is compiler/linker and binary-structure
evidence, not a whole-artifact exact claim.
After the CDG ES repair, two further cold links agree on the complete
75,212-byte MZ SHA-256
`1abb7acf3d89e01f97262d2e407655a87f9d9a8c1a08b1397fb3c8f4ab19fb23`;
the same static relocation and far-call audits pass. Four raw OMF files differ
only in narrowly framed Borland source/dependency timestamp comments, while
all link-relevant OMF records agree.

The native OP title is visible at 60 and 100 seconds, while the original-image
control reaches demo gameplay by 45 seconds. A private stage trace reached
the main-menu loop; a later CDG-slot dump found the second cursor image
missing. The TH04-large-product `CDG_LOAD_ALL` path now resets `ES=DS`
before each header copy. After the change, the private trace passed the first
menu frame and all three checked CDG slots were populated. A disposable
TC4J MAIN entry shim wrote `MAINHIT.TXT = H` on the normal `GAME.BAT` route:
native OP has transferred control to `MAIN.EXE`. This does not establish
original MAIN or MAINE execution. The strict shared CDG loader still gives
raw-zero decoded modules and matching relocations for both OP and MAINE in
two cold replay links. See the native OP note for receipts and limits. MAIN has no
standalone build yet: its maintained sources refer to 73 missing quoted
include paths: 36 declarations (117 references), 35 composite `.cpp`
fragments, and two `.inl` fragments. See the
[MAIN source-closure note](reconstruction/product/TH04_NATIVE_MAIN_SOURCE_CLOSURE_V882.md)
for the eight affected composite producers and missing data/BSS owners.
Recover TH04 declarations under the local product tree rather than importing
ReC98 headers wholesale. The corrected inventory joins unit overlays, build
inserts, and splits: 36/37 missing body fragment paths map to maintained
source; only the physical `th04/dialog.cpp` composition remains open. The new
product-owned `frames.h` closes 62 include references, and a local ASM owner
provides its ten frame/slowdown DATA/BSS symbols. Two isolated TC4J/TASM/TLINK
DOS probes pass with identical frame OMF and final MZ; this does not yet place
the owner in a standalone MAIN link or attest target DATA/BSS offsets.
The historical `th04/snd/snd.h` include path now resolves to the maintained
shared sound API and closes 68 references. A TC4J reference/local ABI probe
passes, and the v905 aggregate cold-replays all accepted units while actually
using that local API. The first aggregate attempt also caught and corrected
the missing transitive `PF_FN_LEN` macro. This remains declaration closure,
not standalone linking or full MAIN startup.
The historical `th04/sprites/main_pat.h` path now resolves to complete local
TH04 pattern/cel tables and closes 64 maintained-source references. A pinned
TC4J reference/local probe gives identical link-semantic OMF for every TH04
constant. The v909 aggregate rewrites 68 staged occurrences and preserves all
275 accepted units in two cold builds; its first attempt caught the TH05
midboss fragment's dependency on `PAT_ENEMY_KILL_last`. Pattern closure does
not supply any of the remaining state owners or complete the MAIN link.
The historical `th04/math/vector.hpp` path now resolves to an artifact-local
four-entry vector API without widening the narrower OP/MAINE shared header.
A TC4J reference/local ABI probe matches link-semantic OMF, and corrected v913
replay rewrites 27 staged occurrences while preserving all 275 accepted units.
The replay driver now freezes headers introduced solely by tree rewrites; this
fix prevents a configured rewrite from being silently skipped. Vector closure
does not provide remaining MAIN data owners or a standalone link.
The historical `th04/main/scroll.hpp` path now resolves to a complete
artifact-local scroll API. Reference/local TC4J probes agree for both GAME 4
and GAME 5, and two TASM/TLINK/DOS runs validate separate five-symbol
playfield-scroll and one-symbol per-page BSS owners with one deterministic
217-relocation MZ. The v915 aggregate actually rewrites 29 staged files and
preserves all 275 accepted extents. The owners are not yet placed at their
target BSS offsets or integrated into a standalone MAIN link.

The historical `th04/main/circle.hpp` path now resolves to the artifact-local
circle API. A TC4J reference/local probe exercises all four entry points and
the `circles_color` declaration with matching link-semantic OMF. The focused
circle replay remains raw-zero, and the strict aggregate freezes the local
header and rewrites all 20 staged occurrences while preserving all 275
accepted extents in two cold builds. Circle BSS ownership and standalone MAIN
placement remain open.

The historical `th04/main/player/player.hpp` path now resolves to the
artifact-local player API through a product include wrapper. The v922 TC4J
reference/local probe matches link-semantic OMF for the player calls, globals,
constants, and motion layout. The first aggregate compile caught and closed
the transitive playfield shake and `SHOT_W/H` declarations; the focused player
owner is raw/MAP/relocation exact, and the strict aggregate freezes the local
header and rewrites 43 staged occurrences while preserving all 275 accepted
extents. Player/playfield BSS ownership and standalone MAIN placement remain
open.

The historical `th04/main/bullet/bullet.hpp` path now resolves to the
artifact-local bullet API through a product include wrapper. The v928 TC4J
probe matches link-semantic OMF for the GAME 4/GAME 5 bullet constants,
unions, template layout, and add-entry ABI. After preserving the attested TH04
playfield/rank declarations to satisfy PC-98 IDE path and duplicate-rank
constraints, the focused `bullet_a.cpp` owner is raw/MAP/relocation exact at
2139 bytes and the strict aggregate rewrites 44 staged occurrences while
preserving all 275 accepted extents in two cold builds. Bullet BSS ownership,
the remaining 94-path source closure, standalone MAIN placement, and PC-98
startup remain open.

The historical `th04/main/null.hpp` path now resolves to the artifact-local
near/far Pascal null-callback API through a product wrapper. The v957 TC4J
probe matches reference/local link-semantic OMF
(`35fce2163d270dde89609cd884e6217648b40a4696423b8a26123561a26c531b`), and
the focused `std_run` owner remains raw/MAP/relocation exact at 0x6D bytes.
The v957 aggregate preserves all 275 accepted extents in two cold builds while
rewriting 11 staged occurrences. Null callback implementation ownership, the
remaining 83-path source closure, standalone MAIN placement, and PC-98 startup
remain open.

The historical `th04/main/phase.hpp` path now resolves to artifact-local GAME 4
phase constants through a product wrapper. The v959 TC4J probe matches
reference/local link-semantic OMF
(`212b0f77eb40972c801634cc27ea6f485ba8d0b74f21dd238aa8c608a315566c`), and
the focused `midboss4_render` owner remains raw/MAP/relocation exact at 0x8D
bytes. The v959 aggregate preserves all 275 accepted extents in two cold builds
while rewriting 11 staged occurrences. Phase DATA/BSS ownership, the remaining
82-path source closure, standalone MAIN placement, and PC-98 startup remain open.

The historical `th04/main/slowdown.hpp` path now resolves to an artifact-local
slowdown API and a maintained `_turbo_mode` BSS owner through product wrappers.
The v962 TC4J/TASM/TLINK/DOS probe passes the byte/word declaration and storage
ABI in two independent runs, and the focused `slowdown_frame_delay` owner remains
raw/MAP/relocation exact at 0x1A bytes. The v962 aggregate preserves all 275
accepted extents in two cold builds while rewriting 9 staged occurrences. The
v963 inventory is now 81 missing paths / 226 references: 44 headers (189
references), 35 `.cpp` fragments, and two `.inl` fragments; only
`th04/dialog.cpp` remains unmapped. Target BSS ordering, standalone MAIN
placement, and PC-98 startup remain open.

The historical `th04/main/quit.hpp` path now resolves to an artifact-local
one-byte `quit_t` enum and `_quit` BSS owner through product wrappers. The v964
TC4J/TASM/TLINK/DOS probe passes `QUIT_PASS` in two independent runs with a
valid 217-relocation MZ. The focused `stage_state_init` owner is raw/MAP/
relocation exact at 0xCB bytes, and the v964 aggregate preserves all 275
accepted extents in two cold builds while rewriting 8 staged occurrences. The
v965 inventory is now 80 missing paths / 218 references: 43 headers (181
references), 35 `.cpp` fragments, and two `.inl` fragments; only
`th04/dialog.cpp` remains unmapped. Quit-state target BSS ordering, standalone
MAIN placement, and PC-98 startup remain open.

The historical `th04/playchar.h` path now resolves to the artifact-local
GAME=4 play-character API through `src/main/include/th04/playchar.h` and
`src/main/playchar.hpp`. The v966 TC4J probe matches reference/local semantic
OMF SHA-256
`dd123fa70e950c5c93493b3d0241434a705a2b3c07dca432eeafca9c6da7f0ec`; the
focused `gameplay_session_init` owner is raw/MAP/relocation exact at 0x1CD
bytes. The v966 aggregate preserves all 275 accepted extents in two cold
builds while rewriting 16 staged occurrences. Inventory v967 is now 79 missing
paths / 210 references: 42 headers (173 references), 35 `.cpp` fragments, and
two `.inl` fragments; only `th04/dialog.cpp` remains unmapped. Play-character
target DATA/BSS ownership, standalone MAIN placement, and PC-98 startup remain
open.

The historical `platform.h` and `x86real.h` paths now resolve to artifact-local
wrappers over `src/shared/platform/types.hpp` and `src/shared/platform/x86.hpp`.
The v968 TC4J probe matches reference/local semantic OMF SHA-256
`6a0b4ab3ddee2bee11b29d6adb14137aa4499d0ff243d6e4cc1c8dcf0f84e8d2`.
Focused `main-entry` (0x7C bytes at 0xC30C) and `bullets_render` (0x10B bytes
at 0x144E5) owners remain raw/MAP/relocation exact. The v968 aggregate
preserves all 275 accepted extents in two cold builds while rewriting 23
platform and 18 x86real occurrences. Inventory v969 is now 77 missing paths /
186 references: 40 headers (149 references), 35 `.cpp` fragments, and two
`.inl` fragments; only `th04/dialog.cpp` remains unmapped. Platform/data/BSS
ownership, standalone MAIN placement, and PC-98 startup remain open.

The historical `th04/common.h` path now resolves to the artifact-local
`MAIN_STAGE_COUNT`/`STAGE_EXTRA` macro surface through
`src/main/include/th04/common.h` and `src/main/common.hpp`. The v970 TC4J probe
matches reference/local semantic OMF SHA-256
`919b53d19d4ae0ae919c6957738ce61aaab87a4600007b55deb50acd73367d81`.
The focused EMS owner is raw/MAP/relocation exact at 0x1FA bytes at 0xCC88.
The v970 aggregate preserves all 275 accepted extents in two cold builds while
rewriting 14 staged occurrences. Inventory v971 is now 76 missing paths / 177
references: 39 headers (140 references), 35 `.cpp` fragments, and two `.inl`
fragments; only `th04/dialog.cpp` remains unmapped. Stage-state/data-BSS
ownership, standalone MAIN placement, and PC-98 startup remain open.

The historical `th04/main/bullet/clearzap.hpp`, `th04/main/hud/hud.hpp`, and
`th04/sprites/main_cdg.h` paths now resolve to artifact-local combat, HUD, and
MAIN CDG APIs through product wrappers. The v977 TC4J probe matches semantic
OMF SHA-256 `f63c46cce7df656b34be719f1013a0edb0d936e79c64e40337449b64e5cfcb19`.
Focused `bullets_render` (0x10B bytes at 0x144E5), `midboss_hud_defeat_tu`
(0x1CB bytes at 0x642C), and `BOSS_BD_TEXT` (0x27 bytes at 0x7667) replays
remain raw/MAP/relocation exact. The v977 aggregate preserves all 275 accepted
extents in two cold builds while staging 8, 10, and 11 rewrites; its diagnostic
MAIN SHA-256 is `d51db833654d139b6e79c059a70be2859d4f83a3d6c9777e7fbc547f1d3c0bdb`.
Inventory v978 is now 73 missing paths / 154 references: 36 headers (117
references), 35 `.cpp` fragments, and two `.inl` fragments; only
`th04/dialog.cpp` remains unmapped. Header closure is compiler-observed only:
combat/HUD/CDG backing storage, target DATA/BSS ownership, standalone MAIN
placement, and PC-98 startup remain open.

The historical `th04/main/player/bomb.hpp`, `th04/main/player/shot.hpp`, and
`th04/formats/super.h` paths now resolve to artifact-local player bomb, shot,
and super-sprite APIs through product wrappers. The v979 TC4J probe matches
semantic OMF SHA-256
`5e0d628d73737cd418324e7cb1ae95dd157febf84740476b2fc721106959790a`.
Selecting trigger owners `th04-main-bomb-stars-v178` and
`th04-main-enemies-render-v177` replays bomb core (0x3D9 bytes at file
0x11734), bomb-star renderer (0x11D at 0x11B0D), shot producer (0x2E9 at
0x11C2A), and enemy renderer (0xCF at 0x11F13) raw/MAP/ordered-relocation
exact in two cold builds. The 275-owner aggregate remains deterministic with
7, 8, and 7 staged rewrites and diagnostic MAIN SHA-256
`d51db833654d139b6e79c059a70be2859d4f83a3d6c9777e7fbc547f1d3c0bdb`.
Inventory v979 is now 70 missing paths / 135 references: 33 headers (98
references), 35 `.cpp` fragments, and two `.inl` fragments; only
`th04/dialog.cpp` remains unmapped. Combat declaration closure is
compiler-observed only: backing DATA/BSS ownership, standalone MAIN placement,
and PC-98 startup remain open.

The historical `th04/main/enemy/enemy.hpp` path now resolves to the
artifact-local enemy API through `src/main/include/th04/main/enemy/enemy.hpp`.
The v980 TC4J probe matches semantic OMF SHA-256
`386f9e1c92c7cd65bc1ccd05866fdb7eb344ec71b4f845a4a9d4da71cc430393`.
Selecting the downstream `th04-main-enemies-render-v177` trigger proves
`enemies_add` (0xDE at file 0x194F3), `enemies_update` (0x1D6 at 0x19659),
and `enemies_render` (0xCF at 0x11F13) raw/MAP/ordered-relocation exact in two
cold builds. The v980 aggregate preserves all 275 accepted extents with
identical diagnostic MAIN SHA-256
`d51db833654d139b6e79c059a70be2859d4f83a3d6c9777e7fbc547f1d3c0bdb` while
staging six enemy-header rewrites. Inventory v980 is now 69 missing paths /
129 references: 32 headers (92 references), 35 `.cpp` fragments, and two
`.inl` fragments; only `th04/dialog.cpp` remains unmapped. Enemy declaration
closure is compiler-observed only: enemy DATA/BSS ownership, standalone MAIN
placement, and PC-98 startup remain open.

The historical `th04/hardware/input.h` path now resolves to a MAIN-local input
API through `src/main/include/th04/hardware/input.h`, backed by
`src/main/hardware/input.hpp`. The v981 TC4J probe matches semantic OMF
SHA-256 `4ad85e3888b72d31bb17af992df891b9858480c2742fa223a54222af25d0212f`.
The focused `th04-main-input-wait-for-change` owner is raw/MAP exact at 0x56
bytes (file 0x14A13) with empty relocation overlap. The v981 aggregate
preserves all 275 accepted extents with identical diagnostic MAIN SHA-256
`d51db833654d139b6e79c059a70be2859d4f83a3d6c9777e7fbc547f1d3c0bdb` while
staging five input-header rewrites. Inventory v981 is now 68 missing paths /
124 references: 31 headers (87 references), 35 `.cpp` fragments, and two
`.inl` fragments; only `th04/dialog.cpp` remains unmapped. Input declaration
closure is compiler-observed only: key-state DATA/BSS ownership, standalone
MAIN placement, and PC-98 startup remain open. The MAIN-local surface is kept
separate from the shared OP/MAINE input header.

The historical root `planar.h` path now resolves to the product-owned MAIN
planar surface through `src/main/include/planar.h`, backed by
`src/main/hardware/planar.hpp`. The v982 TC4J reference/local probe matches
semantic OMF SHA-256 `113841b4368c98e426fe3456e601cff139cf4fac55cdaf1e35d527db8434695b`.
The focused `th04-main-elly-backdrop-v200` owner is raw/MAP/ordered-relocation
exact at 0x0E bytes (file 0xD6CC), slice SHA-256
`6e1968ec26ed949a9659f9f56232985154f34111f607b4a0b8d51debb145688b`.
The v982 aggregate preserves all 275 accepted extents in two cold builds with
identical diagnostic MAIN SHA-256
`d51db833654d139b6e79c059a70be2859d4f83a3d6c9777e7fbc547f1d3c0bdb` while
staging eleven planar-header occurrences. Inventory v982 is now 67 missing
paths / 119 references: 30 headers (82 references), 35 `.cpp` fragments, and
two `.inl` fragments; only `th04/dialog.cpp` remains unmapped. The full local
surface keeps the historical `PLANAR_H` guard so untouched scaffold consumers
do not redeclare planar templates; declaration closure is compiler-observed
only, not MAIN DATA/BSS ownership, standalone placement, or PC-98 startup.

The historical `th04/main/drawp.hpp` path now resolves to the product-owned
MAIN drawpoint declaration through `src/main/include/th04/main/drawp.hpp`,
backed by `src/main/player/drawp.hpp`. The v983 TC4J reference/local probe
matches semantic OMF SHA-256
`be3864707b1601a673d64373aaf28b67ebd017b8aab918953e64f04d687c04c0`.
The focused `th04-main-player-invalidate-v176` owner is raw/MAP/relocation
exact at 0xB6 bytes (file 0x11FE2), slice SHA-256
`13b2ea1657c35eefbd27fb1eaa66c637f48815877175dfbe541e162d0e7a218b`, including
the existing drawpoint BSS field split. The v983 aggregate preserves all 275
accepted extents in two cold builds with identical diagnostic MAIN SHA-256
`d51db833654d139b6e79c059a70be2859d4f83a3d6c9777e7fbc547f1d3c0bdb` while
staging six drawpoint-header occurrences. Inventory v983 is now 66 missing
paths / 114 references: 29 headers (77 references), 35 `.cpp` fragments, and
two `.inl` fragments; only `th04/dialog.cpp` remains unmapped. This closes the
drawpoint declaration edge only; its target DATA/BSS placement, standalone MAIN
placement, and PC-98 startup remain open.

The historical `th04/main/gather.hpp` path now resolves to the artifact-local
gather API through a product include wrapper. The v930 TC4J probe matches
link-semantic OMF for the GAME 4/GAME 5 gather-circle and gather-template
layouts, constants, initializer, and entry declarations. The focused
`gather.cpp` owner is raw/MAP/relocation exact at 587 bytes, and the strict
aggregate rewrites 24 staged occurrences while preserving all 275 accepted
extents in two cold builds. Gather BSS ownership, the remaining 93-path source
closure, standalone MAIN placement, and PC-98 startup remain open.

The historical `th04/main/score.hpp` path now resolves to the artifact-local
score API through a product include wrapper. The v931 TC4J probe matches
link-semantic OMF for `score_lebcd_t`, high-score/graze/extend/score-delta
state, and both entry ABIs. The focused `ranking.cpp` owner is raw/MAP/relocation
exact at 731 bytes, and the strict aggregate rewrites 21 staged occurrences
while preserving all 275 accepted extents in two cold builds. Score DATA/BSS
ownership, the remaining 92-path source closure, standalone MAIN placement,
and PC-98 startup remain open.

The historical `th04/main/custom.hpp` path now resolves to the artifact-local
custom-entity API through a product include wrapper. The v933 TC4J probe matches
link-semantic OMF for the GAME 4 26-byte `custom_t`, 32-entity storage surface,
field layout, and `custom_assert_count`; the focused `chasecrosses_add.cpp`
owner is raw/MAP/relocation exact at 1206 bytes. The strict aggregate rewrites
23 staged occurrences while preserving all 275 accepted extents in two cold
builds. Custom BSS ownership, the remaining 91-path source closure, standalone
MAIN placement, and PC-98 startup remain open.

The historical `th04/main/spark.hpp` path now resolves to the artifact-local
spark API through a product include wrapper. The v935 TC4J probe matches
link-semantic OMF for `spark_t`, the GAME 4 spark count, ring offset, add APIs,
and lifecycle declarations; the focused `sparks_init` owner is raw/MAP/relocation
exact at 30 bytes. After removing a redundant hash-bound entity rewrite exposed
by the first closed replay, the strict aggregate rewrites 19 staged occurrences
while preserving all 275 accepted extents in two cold builds. Spark BSS
ownership, the remaining 90-path source closure, standalone MAIN placement,
and PC-98 startup remain open.

The historical `th04/main/tile/tile.hpp` path now resolves to the artifact-local
tile API through a product include wrapper. The v937 TC4J probe matches
link-semantic OMF for tile-ring storage, image-VO arithmetic, dirty flags,
invalidation state, and render/add declarations; the focused `tile_ring_set_vo`
owner is raw/MAP/relocation exact at 79 bytes. The first focused replay exposed
a cross-game duplicate `entity_flag_t` declaration, and the first aggregate
hit the old hash-bound spark rewrite; the canonical TH02 entity guard in the
product header and removal of that redundant rewrite fixed both controls. The
strict aggregate rewrites 22 staged occurrences while preserving all 275
accepted extents in two cold builds. Tile DATA/BSS ownership, the remaining
89-path source closure, standalone MAIN placement, and PC-98 startup remain
open.

The historical `th04/main/playfld.hpp` path now resolves to the artifact-local
playfield API through a product wrapper and a DOS-8.3-compatible
`src/main/playfld.hpp` path. The v944 TC4J probe matches link-semantic OMF
(`0c5117830fecd196fe5beebda0232068f823cce32206dc1f2943ccfae7750206`) for
the inherited VRAM/TRAM extents, clipping/enclosure macros, point conversion,
scroll linkage, motion ABI, and shake declarations. The focused
`playfield_shake_update_and_render` owner is raw/MAP/relocation exact at 208
bytes, and the strict aggregate rewrites 28 staged occurrences while
preserving all 275 accepted extents in two cold builds. Playfield DATA/BSS
ownership, the remaining 88-path source closure, standalone MAIN placement,
and PC-98 startup remain open.

The historical `th04/main/item/item.hpp` path now resolves to the artifact-local
item API through a product include wrapper. The v946 TC4J probe matches
link-semantic OMF (`ec1897de6e3908a4a10c51def070e411a0cffa7c35088cb9f0de9c901cc89447`)
after preserving the target-bound far/`extern "C"` and byte-width corrections.
The first focused replay caught the pristine near/C++ declaration mismatch;
the corrected `items_update` owner is raw/MAP/relocation exact at 0x546 bytes,
and the strict aggregate rewrites 16 staged occurrences while preserving all
275 accepted extents in two cold builds. Item DATA/BSS ownership, the
remaining 87-path source closure, standalone MAIN placement, and PC-98 startup
remain open.

The historical `th04/main/midboss/midboss.hpp` path now resolves to the
artifact-local midboss API and closes 19 staged references. The v950 TC4J probe
matches reference/local link-semantic OMF (`dd141e4f...`), and the focused
`midboss4_render` owner remains raw/MAP/relocation exact at 0x8D bytes. The v950
aggregate preserves all 275 accepted extents in two cold builds. Midboss
DATA/BSS ownership, standalone MAIN placement, and PC-98 startup remain open.

The historical `th04/main/rank.hpp` path now resolves to the artifact-local
rank API and closes 14 staged references. The v953 TC4J probe matches
reference/local link-semantic OMF (`83f8091d...`); its include-order guard
prevents duplicate rank enum declarations. The accepted full 275-owner
aggregate preserves both cold builds, and `stage_bonus` at 0x1EC8E/0x58D
remains raw/MAP/relocation exact. Rank DATA/BSS ownership, the remaining
84-path source closure, standalone MAIN placement, and PC-98 startup remain
open. Minimal focused controls are recorded as failed dependency/cohort
controls, not exact evidence.

The historical `th04/main/stage/stage.hpp` path now resolves to the
artifact-local stage callback API through a product wrapper. The v955 TC4J
probe matches reference/local link-semantic OMF
(`b2d6a74eae9046ad2ab910bd0674eddcad26a70d5af860f877e1c77137a86d75`), and
the focused `demo-session` owner remains raw/MAP/relocation exact at 0x51E
bytes. The v955 aggregate preserves all 275 accepted extents in two cold
builds while rewriting 17 staged occurrences. Stage DATA/BSS ownership, the
remaining 84-path source closure, standalone MAIN placement, and PC-98
startup remain open.

The [native ZUN note](reconstruction/zun/TH04_ZUN_NATIVE_SOURCE_BUILD_V880.md)
records a TH04-only 18-object Tiny resident COM, cold 13,356-byte flat launcher,
and deterministic 7,723-byte DIET-packed MZ with zero relocations. The packed
candidate is not target-byte-exact. In a disposable HDI with original OP,
MAIN, and MAINE, the normal `GAME.BAT` path reaches demo gameplay by 45
seconds. This is runtime evidence for the source-only ZUN launch path; other
launcher options and the combined candidate product chain remain to test.

The [native product-build readiness note](reconstruction/product/TH04_NATIVE_BUILD_READINESS_V1.md)
and `config/native_maine_sources.toml` define the current TH04-only MAINE
source order. Pinned TC4J/TASM32/TLINK compiles 130 TH04-owned translation
units (83 C/C++, 47 ASM) and links MAINE without `masters.lib`: two cold
links agree on the complete MZ SHA-256
`6a20e946ce6ed8c6865887ba90fef31e4cbcb34937dd22e6829b30c21a5435f2`,
with zero unresolved symbols and zero warnings. All 130 link-relevant and
timestamp-normalized OMF objects agree; BGIMAGE's raw dependency timestamp
comment differs. The static MZ auditor checks 656 relocation sites at DOS
load segments `0x2000` and `0x6000`, entry `0000:0000`, and stack
`12C3:0080`. The call auditor checks 36 far-return owners, 148 relocated
direct far calls, and one same-CS far call. These are compiler/linker and
static binary observations of a TH04-only MAINE build candidate, not target
byte exactness or full PC-98 runtime acceptance.
The native-only CDG ES correction changes the standalone MAINE MZ to SHA-256
`adaea486a9931ae1dcead561c8b5ea2bbfab9619f13c3ace699e678c06b23aa2`.
Its fresh static audit still validates 656 relocations, 36 far-return owners,
148 direct far calls, and one same-CS call. Two cold links agree on the full
MZ and all 130 link-relevant/timestamp-normalized OMF objects; only BGIMAGE's
raw timestamp comment differs.

The [BGM/link note](reconstruction/product/TH04_NATIVE_BGM_V871.md) records
the pinned `MIKO.EFS` historical-library differential, 15 effects/1496 words,
DOS software-`INT 8` test, final cold-link commands, and the critical far-call
fixup counterexample. Plain TASM `call` and even `call far ptr` reached the
wrong full-link address while TLINK reported success. Symbolic `9A` far-call
encoding plus a relocation-aware linear-target audit fixes that edge. The
private final link, MZ, and call receipts are at
`.analysis/reconstruction/probes/native-maine-bgm-v3-{a,mz,call}-20260928/receipt.json`.
See the [BFNT sprite note](reconstruction/product/TH04_NATIVE_SUPER_SPRITE_V870.md)
for its historical-library differential and fake-VRAM test.
See the [PI decoder note](reconstruction/product/TH04_NATIVE_PI_DECODE_V869.md)
for the current real-resource differential and binary-versus-readable-source
hazard. The [PAR archive/service note](reconstruction/product/TH04_NATIVE_PF_ARCHIVE_V867.md)
has the private PI/BFNT fixture hashes and the TH04-local file hook.
The [packed-row note](reconstruction/product/TH04_NATIVE_PACK_PUT_V866.md)
retains the fake-VRAM test and PI decoder handoff.
The [gaiji-storage note](reconstruction/product/TH04_NATIVE_GAIJI_STORAGE_V865.md)
retains the BFNT test and stdio memory-pressure counterexample. This work has
not reached MAINE at PC-98 runtime.
Start with `python3 scripts/probes/probe_th04_native_maine_link.py --check-manifest`;
the focused notes give the cold-link, comparator, MZ,
and call-ABI commands with fresh private output directories.

The TH04-only MZ was inserted into a disposable FAT12 diagnostic HDI through
`scripts/probes/prepare_th04_maine_diagnostic_hdi.py`, with original source HDI
identity checked before and after. A 20-second DOSBox-X `GAME.BAT` run reaches
the OP splash but writes only the `START` marker, so candidate MAINE entry is
unobserved (run receipt SHA-256
`bc168c158e083cb1e306915ccb9551fc548e6676ae9ee78478e07283a1990edc`).
This prepares the next runtime scenario;
new probes require fresh private output directories and only one writable
Borland/Wine build at a time.

The TH04-local `PFSTART`/`PFEND` INT 21h hook now passes nine pinned real
resource cases in a separate DOS test MZ, including all five declared-size
versus decoded-size one-byte cases. This is `runtime-observed` for the isolated
DOS service; it is not MAINE PC-98 runtime acceptance. The TH04-local
`GRAPH_PI_LOAD_PACK` decoder matches historical pixel hashes for two pinned
640×400 PI resources through both the PAR hook and loose DOS reads; its
paragraph indexing crosses 64 KiB and the isolated test MZ has 247 audited
relocations. The TH04-local BFNT sprite service matches the historical
small-model pattern and palette hashes for all 20 `SCNUM2.BFT` patterns. Its
fake-VRAM draw and lifecycle test passes with 256 audited test-MZ relocations.
The BGM DOS test covers 15 effects, 1496 frequency words, software IRQ entry,
and exit after `mem_unassign()`. It does not establish real PC-98 timer cadence
or MAINE game entry. The next bounded runtime batch should boot the normal
`GAME.BAT` route, record a checkpoint proving candidate MAINE entered, and
compare PC-98 input, video, sound, and save/config behavior.
Paragraph-based `hmem_alloc(unsigned)` can represent image buffers larger than
64 KiB; the byte-count `hmem_allocbyte(unsigned)` cannot. See the packed-row
note for the decoder and ownership details.

A prior calibration MZ linked despite far calls into near-return local ASM.
The [far-call ABI note](reconstruction/product/TH04_NATIVE_FAR_CALL_ABI_V856.md)
records the counterexample, product-only repair, and accepted-slice replay;
TLINK success and relocation counts alone are insufficient. After changing
shared headers, segment ownership, link order, or ABI, cold-replay every
affected accepted unit. Direct AUTOEXEC launch of even the original MAINE
faults at DOS interrupt 60H; the paired original/calibration `GAME.BAT`
diagnostic reaches OP only. The [runtime preparation note](reconstruction/product/TH04_NATIVE_BUILD_READINESS_V1.md)
has the private HDI commands. Continue the normal OP-to-MAIN-to-MAINE route
before making a runtime acceptance claim.

Whole-game product closure still needs MAIN's 73 distinct unresolved quoted
include paths and remaining data/BSS owners, its standalone source manifest and
link, and a combined candidate PC-98 scenario. OP, ZUN, and MAINE each have
source-only native build paths; the MAIN build is the controlling blocker.
The remaining two nonexact MAIN function slices are outside this build lane.
At this handoff, `python3 scripts/preflight.py`,
`python3 scripts/validate_tracking.py`, `python3 scripts/ci.py`, and
`git diff --check` pass. The last CI log is retained privately at
`.analysis/reconstruction/probes/native-build-ci-v897-20260928.log`.
Source ownership and runtime correctness are separate from the accepted
raw-zero function ledgers.

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
