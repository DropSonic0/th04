# Native MAIN source closure frontier

MAIN is the remaining large TH04 product-build graph. This is a read-only
inventory of maintained `src/main` C/C++ and `.inl` files, not a standalone
build claim. It does not revisit the two deferred non-exact MAIN functions.
Replay the inventory with
`python3 scripts/probes/inventory_th04_native_main.py`.
The current v930 read-only result is retained at
`.analysis/reconstruction/probes/native-main-inventory-v930-20260928/inventory.json`.

| Missing quoted include class | Unique paths | References |
| --- | ---: | ---: |
| `.h` / `.hpp` declarations | 56 | 355 |
| `.cpp` composite fragments | 35 | 35 |
| `.inl` composite fragments | 2 | 2 |
| **Total** | **93** | **392** |

The heaviest remaining header edges are the score and custom headers (19 each),
followed by `th04/main/spark.hpp` (18) and `th04/main/tile/tile.hpp` (16). Five
other missing platform/header names
have no `th04/` prefix: `platform.h`, `x86real.h`, `planar.h`, `decomp.hpp`,
and `shiftjis.hpp`. These names denote needed declarations, not approval to
reuse cross-game product headers.

Eight maintained physical producers include `.cpp` fragments by historical
`th04/` paths: `boss_bg_main01.cpp`, `yuuka6_main034.cpp`,
`kurumi_update.cpp`, `elly_update.cpp`, `yuuka5.cpp`,
`mugetsu_main033.cpp`, `demo_prefix.cpp`, and `dialog/fused.cpp`. Most of
their included names are absent even from the pinned local ReC98 tree.
Related function bodies already exist as separate maintained `src/main`
translation units, but their composition and near-call segment ownership must
be reconciled before adding them to a native MAIN link manifest. Compiling
both a composite and its included child as objects would duplicate publics.
The local ReC98 reference has files at all 56 remaining missing header paths, but only
two of the 35 missing `.cpp` paths and neither missing `.inl` path. This is a
source-location observation, not evidence that those headers are ready for a
TH04-owned product build. The existing maintained function bodies must be
composed into their physical translation units and their state owners found.
The inventory now joins historical include paths to all three exact-replay
composition surfaces: ordinary unit overlays, build inserts, and source
splits. Maintained source maps 36 of 37 missing body fragment paths. The four
v898 false negatives are `th04/gsinit.cpp`, `th04/m4tail.inl`,
`th04/main/dialog/init_exit.inl`, and `th04/y5p2.cpp`; their checked-in replay
rules already bind them to maintained source. Only `th04/dialog.cpp` remains
without a complete local physical composition. It is the historical root
wrapper for the dialog producer, whose accepted bodies currently enter
through several fragment and scaffold patch surfaces.

## MAIN frame-state closure

The first declaration/data batch localizes the former
`th04/main/frames.h` dependency at
`src/main/include/th04/main/frames.h`. This preserves the historical include
spelling through the product-owned `-Isrc/main/include` root and closes 62
quoted-include references without changing accepted TU source composition.
`src/main/core/frame_state.asm` owns the ten public frame/slowdown symbols in
`_DATA` and `_BSS`; initialized totals begin at zero while stage-relative and
slowdown state remains uninitialized until normal game setup.

`scripts/probes/probe_th04_native_main_frames.py` attests the pinned toolchain,
compiles a large-model TC4J consumer, assembles the owner with TASM32, validates
the single-module OMF producer and ten PUBDEF records, links a valid MZ, and
runs it under the pinned MS-DOS Player. Two fresh directories both print
`FRAMES_PASS`; the complete MZ is identical in both runs at SHA-256
`cbdedb41ae7b7073c8483f21b6cb572ef5b6efd4902642ddaea3baf1352d1047`
with 217 relocations, and the frame-state OMF is raw-identical at SHA-256
`9626d47ea08919bf316d21bf2214f16ffb43e829f0fa29522a72bc963a556fe7`.
This is runtime-observed declaration/storage behavior for an isolated DOS
probe. Target DATA/BSS offsets, full native MAIN linking, and PC-98 execution
remain open.

## MAIN sound-header closure

The second declaration batch preserves the historical `th04/snd/snd.h`
spelling at `src/main/include/th04/snd/snd.h`, but forwards only to the already
maintained `src/shared/sound/api.hpp`. It therefore closes 68 quoted-include
references without copying the TH03/TH02 candidate header chain into product
source. A pinned TC4J dual compile exercises enum widths, constants, calling
conventions, inline helpers, and public references against both the pinned
reference header and the local shared API. Their link-semantic OMF records are
identical; raw objects differ only in dependency comments and equivalent
PUBDEF order.

The first full cold aggregate attempt exposed a declaration omitted by that
initial probe: `dialog_op()` still needed the transitively supplied
`PF_FN_LEN`. After adding the 8.3-plus-null value (`13`) to the shared API and
the probe, run v905 rewrote 77 includes across 76 staged TH04 files to the
local API. Both cold builds completed, selected all 275 units, and preserved
raw bytes, MAP extents, and relocations for every accepted extent. The two
diagnostic MAIN candidates are identical at SHA-256
`d51db833654d139b6e79c059a70be2859d4f83a3d6c9777e7fbc547f1d3c0bdb`.
Receipt SHA-256:
`c5ed61a81376b3cf3873f688d78106f8adc6b17522cd194d8879539a2ac60124`.
This aggregate still depends on the pinned scaffold and is not a standalone
MAIN build or runtime result.

## MAIN pattern-header closure

The third declaration batch preserves the historical
`th04/sprites/main_pat.h` spelling through a product-owned include wrapper.
`src/main/sprites/main_pat.hpp` now owns the complete TH04 pattern-number enum,
and `src/main/sprites/cels.hpp` owns the animation counts used to derive its
ranges. This closes 64 maintained-source references. A pinned TC4J dual
compile materializes every TH04 pattern and cel constant from the pinned
reference and local headers; the link-semantic OMF SHA-256 is identical at
`c2cf7e0a22986ad13a661ec01bfa5e49c27c83be8a0e546a78541a28736c544d`.
Probe receipt SHA-256:
`8f315d647de255c82f422fde9d86d46337d7a2b8822e26d1ce2e7f1332583bcd`.

The first full aggregate exposed a cross-game closure edge: TH05
`mb_dft.cpp` includes the shared TH04 midboss-defeat fragment, which uses
`PAT_ENEMY_KILL_last`. The original local `GAME == 5` calibration subset did
not declare that value, so v906 stopped at compile time. Adding the actual
TH05 stage-independent range (`PAT_ENEMY_KILL = 4`, last `= 11`) closes that
shared consumer without importing TH05 stage-specific patterns. In final run
v909, 68 occurrences across 67 staged TH04 files use the local pattern table;
both cold builds preserve raw bytes, MAP extents, and relocations for all 275
accepted units. The diagnostic MAIN candidates remain identical at SHA-256
`d51db833654d139b6e79c059a70be2859d4f83a3d6c9777e7fbc547f1d3c0bdb`.
Aggregate receipt SHA-256:
`39830c44554d42b25933c113c5de32596e7e483cc5204a6e0475f94e32f22c4c`.
This is compiler and regression evidence only, not a standalone MAIN build or
runtime result.

## MAIN vector-header closure

The fourth declaration batch preserves `th04/math/vector.hpp` through a thin
product include wrapper and the complete artifact-local
`src/main/math/vector.hpp`. The latter declares `vector2()`,
`vector2_between_plus()`, near `vector2_near()`, and `vector2_at()` against
the already maintained `SPPoint`/subpixel ABI. It deliberately does not widen
the proved OP/MAINE-facing `src/shared/math/vector.hpp`: an initial v910 probe
did so and immediately failed because a shared source snapshot does not own
the MAIN-only subpixel header.

A pinned TC4J dual compile exercises all four calls, near/far distance,
Pascal argument order, references, and point layout. Reference and local
headers produce identical link-semantic OMF SHA-256
`ea2d4cafa2585ebb907486e26bcff121f2fc0827cb954999aa1b8244eff08406`.
Probe receipt SHA-256:
`70282b54e1331d5338a61b49d97df4e48ea011fdc1ab9f096a07ae0a31f6e000`.

The first aggregate v911 exposed a replay-control false positive rather than
a compiler mismatch. Its manifest named the new tree rewrite, but the cold
snapshot collected only headers already reachable from selected maintained
sources. Because the new vector header was introduced solely by the rewrite,
the materializer silently omitted the rewrite and still reported 275 passing
units. The replay input collector now freezes every configured tree-rewrite
header and its transitive `src/` closure; a unit test prevents recurrence.
Final v913 records the vector header in `repo_input_snapshot` and rewrites 27
occurrences across 27 staged files. Both cold builds preserve raw bytes, MAP
extents, and relocations for all 275 accepted units, with identical diagnostic
MAIN SHA-256
`d51db833654d139b6e79c059a70be2859d4f83a3d6c9777e7fbc547f1d3c0bdb`.
Aggregate receipt SHA-256:
`075ee33b00435a57dd2a0b290c17b8b749f22df13f4ac7914418f919f3d1e1dd`.
This closes 25 maintained-source references but remains declaration and replay
evidence, not standalone linking or runtime acceptance.

## MAIN scroll-header and state closure

The fifth declaration batch preserves `th04/main/scroll.hpp` through a thin
product include wrapper and `src/main/scroll/scroll.hpp`. The artifact-local
header retains the inherited `scroll_line` declaration, the five TH04 scroll
state declarations, both per-page lines, and all three near Pascal coordinate
conversion entries. It depends only on the already localized subpixel and
PC-98 type surfaces.

The historical storage is not one contiguous ownership block. Five scroll
state symbols occur in the playfield BSS fragment, while
`scroll_line_on_page` occurs beside tile-invalidation state. The product tree
therefore keeps `src/main/scroll/state.asm` and
`src/main/scroll/page_state.asm` separate so a future standalone link manifest
can preserve both physical positions. Both use uninitialized BSS, including
TASM `evendata` rather than an initialized alignment byte.

Two fresh v914 probes compile the reference and local APIs for both GAME 4 and
GAME 5. Their non-COMENT OMF SHA-256 values agree at
`dfc5f4bee6ddaf3d8e209db275c14ad95852dcc36a449920c458b8e60a9048d6`
and
`7d66e5bdb4310416079a96ed685d5fe50212d05f113f5949e5e7696b9d6c920a`,
respectively. TASM produces stable five-public and one-public BSS objects, and
the linked DOS storage test prints `SCROLL_PASS`. Both complete 217-relocation
MZ files have SHA-256
`d3d5f8c105b35a60e65f37f522e9f616c66133c662bdf6e6c64cef1ce7d80541`.
Primary probe receipt SHA-256:
`fad633d68a3d088f25b5da6195ea831b47c46a259d07981249dfffc8069e944f`.

The v915 strict aggregate freezes the new scroll header and rewrites 29
occurrences across 29 staged files. Both cold builds preserve raw bytes, MAP
extents, and relocations for all 275 accepted units and produce identical
diagnostic MAIN SHA-256
`d51db833654d139b6e79c059a70be2859d4f83a3d6c9777e7fbc547f1d3c0bdb`.
Aggregate receipt SHA-256:
`cf41672289dee812d46bb6fa15ad94201b506489d40ae71150fa9cb55766a532`.
This closes 22 maintained-source references and supplies six split BSS owners.
Their target offsets and integration into a standalone MAIN link remain open.

## MAIN circle-header closure

The sixth declaration batch preserves `th04/main/circle.hpp` through a thin
product include wrapper and the artifact-local `src/main/circle.hpp`. The
header declares the two Pascal circle constructors, the two near update/render
entries, and the `vc_t circles_color` state without importing the candidate
TH01 header chain. The local declaration surface is 560 bytes and the pinned
candidate header is bound to SHA-256
`f2804f8a632495bbf5e796e8bdfdb4b04f23eaac79f47186c2212679f0abc8f9`.

The independent v917 TC4J probe exercises all four entry points, the color
state, subpixel parameter width, and the palette-index type. Reference and
local builds produce the same link-semantic OMF SHA-256
`51e9e794c0d7a2f03d1c27517080b335e2fec704c75b3ad3a359d6ba7e3b066c`.
Probe receipt SHA-256:
`cef1c43cec929882d7daf05e116d46c6651d830e6348228c39b10ad93e7c658e`.

Focused v005 replay records the accepted `circle.cpp` extent as raw-zero and
preserves its MAP and relocation ownership; receipt SHA-256
`d5053194d1cd8b011b33a5cdde2ef402f39248ef0fe6e04e3de434fbe48414de`.
The strict v916 aggregate freezes `src/main/circle.hpp`, rewrites all 20
historical circle-header occurrences across 20 staged files, and records the
header rewrite from the attested scaffold candidate. Both cold builds preserve
raw bytes, MAP extents, and relocations for all 275 accepted units; their
diagnostic MAIN candidates remain identical at SHA-256
`d51db833654d139b6e79c059a70be2859d4f83a3d6c9777e7fbc547f1d3c0bdb`.
Aggregate receipt SHA-256:
`b28f33fe653d24342614435e685f63bfd664c8b3f8f78eee98a8648505b516cf`.
This is declaration, compiler, and scaffold-regression evidence; circle BSS
ownership and a standalone MAIN link remain open.

## MAIN player-header closure

The seventh declaration batch routes the historical
`th04/main/player/player.hpp` edge through the product wrapper
`src/main/include/th04/main/player/player.hpp` and the artifact-local
`src/main/player/player.hpp`. The local surface preserves the candidate's
player dimensions, option spacing, power/shot constants, player state
declarations, near entry points, and the transitive `SHOT_W/H` and playfield
shake declarations needed by the maintained consumers. The candidate header
is pinned at SHA-256
`2c5b69b76f3de1be2d2bbd900b60ebde9d4eebc250671ca3fa94b29a7a3e3ded`; the
current local header is 885 bytes at SHA-256
`1b40824466d91aa295da6361ed2a295c0dd1867610050f2b3e41a183999f5847`.

The independent v922 TC4J probe exercises the player position/clamp and
invalidate near calls, global state widths, constants, and `PlayfieldMotion`
layout. Reference and local builds produce the same link-semantic OMF
SHA-256 `f5ee3b8c79c907cdfb8f71469b0f224f80ca3051fd10e0726e5cefed855b7212`.
Probe receipt SHA-256:
`4aa45e68aea469b068ff144837d5a52df9ca3954e4f87da80dca1bd50c45ecd8`.

The first aggregate compile intentionally exposed two missing transitive
declarations (`playfield_shake_{x,y,anim_time}` and `SHOT_W/H`); no promotion
was made from that failed run. After recovering those observed interfaces in
the local playfield/player headers, the focused player owner replay is
raw/MAP/relocation exact (56 bytes; receipt SHA-256
`9362c889c119e40570084e0c678a912421964d4c8efd17695f6e98a052f6b4d2`). The
strict v922 aggregate freezes the local player header and rewrites 43 staged
occurrences. Both cold builds preserve all 275 accepted extents and produce
identical diagnostic MAIN SHA-256
`d51db833654d139b6e79c059a70be2859d4f83a3d6c9777e7fbc547f1d3c0bdb`;
aggregate receipt SHA-256:
`858c06dbd396e2f6f7d90414bc292bfa319ba00854715d72697ae879f0dc640c`.
The inventory falls to 93 missing paths / 392 references (56 declaration
paths, 35 `.cpp` fragments, and two `.inl` fragments); `th04/dialog.cpp`
remains the only unmapped physical fragment. This closes a declaration and
replay-control edge, not the player/playfield BSS ownership or standalone
MAIN link/runtime gates.

## MAIN bullet-header closure

The eighth declaration batch routes the historical
`th04/main/bullet/bullet.hpp` edge through the product wrapper
`src/main/include/th04/main/bullet/bullet.hpp` and the artifact-local
`src/main/bullet/bullet.hpp` plus `types.hpp`. It preserves the GAME 4/GAME 5
group and spawn values, bullet state unions, template layout, and add-entry
point declarations. The v928 TC4J reference/local probe produced identical
link-semantic OMF (`9ed2b36bcdd5ce19767e7f90c6230a81e03fc3f295d4ceeb3e384f3c3c1ca9d8`); receipt SHA-256:
`80fc64e84bc17731ebb072a8d3dc9570ce74604e8b03fbb49447aefa21f9e1b9`.

The first strict aggregate exposed PC-98 IDE path resolution and a duplicate
rank declaration in cross-game consumers. Keeping the attested TH04 playfield
and rank declarations on this bounded API resolves those compiler edges. The
focused `bullet_a.cpp` owner is raw/MAP/relocation exact at 2139 bytes
(receipt `9ebb25f673e53eeacf64a94f4116f1ca2ac0081a9eaa94901a944706e36397b6`).
The final v928 aggregate rewrites 44 staged occurrences and preserves all 275
accepted extents in two cold builds with identical diagnostic MAIN SHA-256
`d51db833654d139b6e79c059a70be2859d4f83a3d6c9777e7fbc547f1d3c0bdb`;
receipt SHA-256:
`6f1fa257a62eb9ff356c4ac00cf0f42a36fc5e56920fcc65ced49bf2237a233f`.
The v929 inventory is now 94 missing paths / 416 references. Bullet BSS
ownership, standalone MAIN linking, and PC-98 startup remain open.

## MAIN gather-header closure

The ninth declaration batch routes the historical `th04/main/gather.hpp` edge
through the product wrapper `src/main/include/th04/main/gather.hpp` and the
artifact-local `src/main/gather.hpp`. The local surface preserves the
GAME 4/GAME 5 gather-circle and gather-template layouts, constants, initializer,
and all six near/far entry declarations. The v930 TC4J reference/local probe
produced identical link-semantic OMF
(`25297b2c77b7c2e0ab1977300b4b964a7fca3e1b8cada00cd52eb4c35286602`); receipt
SHA-256: `7c1526b9a8c42e8558e8be6176bd46aa844f03ab551914d1be1ccdca8daa9a02`.

The focused `gather.cpp` owner is raw/MAP/relocation exact at 587 bytes
(receipt `81354150a5bd1b7dfbad75079ed7b2f6a99b0330fc754a9404dd19264046c126`).
The v930 aggregate rewrites 24 staged occurrences and preserves all 275
accepted extents in two cold builds with identical diagnostic MAIN SHA-256
`d51db833654d139b6e79c059a70be2859d4f83a3d6c9777e7fbc547f1d3c0bdb`; receipt
SHA-256: `210fde040ef429dabdff6f730b8126c3e94eae663758f331b9505ebfc40fb3fc`.
The v930 inventory is now 93 missing paths / 392 references. Gather BSS
ownership, standalone MAIN linking, and PC-98 startup remain open.

This include inventory is only the first frontier. The frame declaration and
storage batch demonstrates the required pairing, the scroll batch adds a
second declaration/storage split, and the sound, pattern, and vector batches
close declaration-only dependencies. The other 56 missing header
paths still need
product-owned declarations and, where applicable, their data/BSS owners.
A native link manifest must account for those owners and not infer completeness
from closing the include list alone.

Recover TH04-specific declarations under `src/main` or proved `src/shared`
ownership, with a build-time include projection if an accepted historical
include path must remain for strict replay. The ReC98 headers are candidate
material only; a forwarding layer is a temporary migration boundary, not
standalone product closure. A first native MAIN compile frontier should use
a strict ordered source manifest and report its exact unresolved/include
set before changing shared ABI or segment groups. Rebuild every affected
accepted unit after common declaration or layout changes.
