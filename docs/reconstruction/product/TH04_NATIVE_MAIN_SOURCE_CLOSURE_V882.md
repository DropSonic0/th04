# Native MAIN source closure frontier

MAIN is the remaining large TH04 product-build graph. This is a read-only
inventory of maintained `src/main` C/C++ and `.inl` files, not a standalone
build claim. It does not revisit the two deferred non-exact MAIN functions.
Replay the inventory with
`python3 scripts/probes/inventory_th04_native_main.py`.
The current v939 read-only result is retained at
`.analysis/reconstruction/probes/native-main-inventory-v939-20260929/inventory.json`.

| Missing quoted include class | Unique paths | References |
| --- | ---: | ---: |
| `.h` / `.hpp` declarations | 52 | 283 |
| `.cpp` composite fragments | 35 | 35 |
| `.inl` composite fragments | 2 | 2 |
| **Total** | **89** | **320** |

The heaviest remaining header edge is `th04/main/item/item.hpp` (16), followed
by `th04/main/midboss/midboss.hpp` (15), `th04/main/playfld.hpp` (15), and
`x86real.h` (13). Five
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
The local ReC98 reference has files at all 55 remaining missing header paths, but only
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

## MAIN score-header closure

The tenth declaration batch routes the historical `th04/main/score.hpp` edge
through the product wrapper `src/main/include/th04/main/score.hpp` and the
artifact-local `src/main/score.hpp`. The local surface keeps the established
`score_lebcd_t` representation from `src/shared/config/score.hpp`, adds the
TH04 high-score, graze, extend, and score-delta declarations, and preserves
both near/Pascal entry points. The v931 TC4J reference/local probe produced
identical link-semantic OMF
(`6e8d6bd4c9b06a5084fedfa5f86f5c211c19683573936ca128fcf6908bf79df0`); receipt
SHA-256: `3558ef52eaba8ce8c2dc3ddb52575ab9a75c7bdbfb0331f82d5724ab52b22568`.

The focused `ranking.cpp` owner is raw/MAP/relocation exact at 731 bytes
(receipt `0063bb63bdd6963e1ec3aeea3383807250a72dc804224332554709eecbb51054`).
The v931 aggregate rewrites 21 staged occurrences and preserves all 275
accepted extents in two cold builds with identical diagnostic MAIN SHA-256
`d51db833654d139b6e79c059a70be2859d4f83a3d6c9777e7fbc547f1d3c0bdb`; receipt
SHA-256: `b64b44c65a91ded2e9d99eabcad8f7642293ef3fc4e7ca382367163a9fcf5f97`.
The v932 inventory is now 92 missing paths / 373 references. Score DATA/BSS
ownership, standalone MAIN linking, and PC-98 startup remain open.

## MAIN custom-entity header closure

The eleventh declaration batch routes the historical `th04/main/custom.hpp`
edge through the product wrapper `src/main/include/th04/main/custom.hpp` and
the artifact-local `src/main/custom.hpp`. The local GAME 4 surface preserves
the target-observed 26-byte `custom_t`, 32 entities, all field widths and
subpixel/playfield members, plus `custom_assert_count`. The v933 TC4J
reference/local probe produced identical link-semantic OMF
(`20a755175823399bc8b40c0194a6d111ccd1eae6a9f7f907017145a0ea879a19`);
receipt SHA-256:
`f4bf39e083b24bdf808b8f0d024e05bea46148a5d08f5bf3553f809ba63e2b34`.

The focused `chasecrosses_add.cpp` owner is raw/MAP/relocation exact at 1206
bytes (receipt
`5ca8d7d418740ad6c076c12dd7aa640d6e3fb0710114695a06a082767aad18bc`), with
candidate slice SHA-256
`7a4b38f4b1d8ab22b35e074fd3784c2155c4c6a9eec099063449c3f3c7eea712`. The
v933 aggregate rewrites 23 staged occurrences and preserves all 275 accepted
extents in two cold builds with identical diagnostic MAIN SHA-256
`d51db833654d139b6e79c059a70be2859d4f83a3d6c9777e7fbc547f1d3c0bdb`;
aggregate receipt SHA-256:
`3f2df997eec400a6ce798cbfc705194bb863e5f921f9b427298e80a7d846c7b0`.
The v934 inventory is now 91 missing paths / 354 references. Custom BSS
ownership, standalone MAIN linking, and PC-98 startup remain open.

## MAIN spark-header closure

The twelfth declaration batch routes the historical `th04/main/spark.hpp` edge
through the product wrapper `src/main/include/th04/main/spark.hpp` and the
artifact-local `src/main/spark/spark.hpp`. The local surface preserves the
GAME 4 `spark_t` layout, spark counts, ring-offset storage, add-entry ABIs, and
the target-specific C/C++ lifecycle linkage. The v935 TC4J reference/local
probe produced identical link-semantic OMF
(`22b2a2896d46d93082182d9020bb7d8f1e9cd654dd9270f9db81413a8d42353a`);
receipt SHA-256:
`2d2adfb01c2b29a52d1fdc70646cb9bb9671832b7df1e6d5c807f378b2bd7640`.

The first focused replay failed closed before compilation because the old
hash-bound `th04-entity-spark-header-local` rewrite saw the newly localized
spark header; that redundant rewrite was removed. The corrected focused
`sparks_init` owner is raw/MAP/relocation exact at 30 bytes (receipt
`692779c73e7595c5ca728f05ace652035bb2c533f0905bf061c76df7400a90a1`), with
candidate slice SHA-256
`16384305e570f94bd3fef1189d3d0e103e6e8ef32c7361a57b73db90c908201f`.
The v935 aggregate rewrites 19 staged occurrences and preserves all 275
accepted extents in two cold builds with identical diagnostic MAIN SHA-256
`d51db833654d139b6e79c059a70be2859d4f83a3d6c9777e7fbc547f1d3c0bdb`;
aggregate receipt SHA-256:
`5ba52fc6615f59609435aca5f5cf9327e489c6cefdb9510b195732a4b16c09a1`.
The v936 inventory is now 90 missing paths / 336 references. Spark BSS
ownership, standalone MAIN linking, and PC-98 startup remain open.

## MAIN tile-header closure

The thirteenth declaration batch routes the historical
`th04/main/tile/tile.hpp` edge through the product wrapper
`src/main/include/th04/main/tile/tile.hpp` and the artifact-local
`src/main/tile/tile.hpp`. The local surface preserves the TH04 tile-ring
dimensions, signed VRAM-offset representation, tile-image lookup arithmetic,
dirty-half flags, invalidation box, and mixed near/Pascal render entry points.
The v937 TC4J reference/local probe produced identical link-semantic OMF
(`d68b707cd1869aa667242f303960250779b7cca158a9b64b268f11d7ab57758b`);
receipt SHA-256:
`1b8a2c7999ac1f4c7e609ae85482e915768be6e735cebf7fa0236883696569ab`.

The first focused replay reached a TH05 consumer and failed on duplicate
`entity_flag_t` declarations after local TH04 entity headers mixed with
`th02/main/entity.hpp`; the artifact-local entity header now aliases the
canonical `TH02_MAIN_ENTITY_HPP` guard, preserving the semantic OMF while
preventing that cross-game redeclaration. The first aggregate then failed
closed because the redundant hash-bound spark entity rewrite saw a localized
spark overlay; that rewrite remains removed. The corrected focused
`tile_ring_set_vo` owner is raw/MAP/relocation exact at 79 bytes (receipt
`d52ba1315c6ba0f5e743245c635e44178a3740885c430ea48c5052c762040e0c`), with
candidate slice SHA-256
`d74af2b4ba105180caf2bf096a48b0c65cd71c8eb81d1086d99f5c628f333144`.
The v937 aggregate rewrites 22 staged occurrences and preserves all 275
accepted extents in two cold builds with identical diagnostic MAIN SHA-256
`d51db833654d139b6e79c059a70be2859d4f83a3d6c9777e7fbc547f1d3c0bdb`;
aggregate receipt SHA-256:
`327ec7be8460950a4c4832b285ff1f90e3b4fa1b65c75a23bce826578db24f30`.
The v939 inventory is now 89 missing paths / 320 references. Tile BSS
ownership, standalone MAIN linking, and PC-98 startup remain open.

## MAIN playfield-header closure

The fourteenth declaration batch routes the historical
`th04/main/playfld.hpp` edge through the product wrapper
`src/main/include/th04/main/playfld.hpp` and the short artifact-local
`src/main/playfld.hpp` path. The existing long
`src/main/playfield/motion.hpp` path remains a compatibility wrapper. The
short spelling is required by the PC-98 IDE's DOS 8.3 include lookup; the
long-directory projection failed before compilation, while the short path
passed the same integrated build.

The v944 TC4J reference/local probe produced identical link-semantic OMF
(`0c5117830fecd196fe5beebda0232068f823cce32206dc1f2943ccfae7750206`);
receipt SHA-256:
`63e14234445b64262621d2469b11e337b3ca2f6183c1eb986d76d378e5015753`.
The probe exercises the inherited VRAM/TRAM extents, clipping and enclosure
macros, point-to-screen and scroll conversion, motion ABI, and shake
declarations. The corrected focused `playfield_shake_update_and_render`
owner is raw/MAP/relocation exact at 208 bytes (receipt
`c89576fd5940dd34125e9bc3c7276d6a0ff1819a6010f8f5dc61346504dc5cf4`), with
candidate and target slice SHA-256
`37b91cc6809195f3b05b0303b571635f010d358d8fb4a5cbd02951f80091e591`.
The v944 aggregate rewrites 28 staged occurrences and preserves all 275
accepted extents in two cold builds with identical diagnostic MAIN SHA-256
`d51db833654d139b6e79c059a70be2859d4f83a3d6c9777e7fbc547f1d3c0bdb`;
aggregate receipt SHA-256:
`0eb9d94696a80cdfbc4e6ba42e93651a5f1f2d3aa965909b1801ad2a585caf6e`.
The v944 inventory is now 88 missing paths / 305 references: 51 headers (268
references), 35 `.cpp` fragments, and two `.inl` fragments; only
`th04/dialog.cpp` remains unmapped. Playfield DATA/BSS ownership, standalone
MAIN linking, and PC-98 startup remain open.

## MAIN item-header closure

The fifteenth declaration batch routes the historical
`th04/main/item/item.hpp` edge through the product wrapper
`src/main/include/th04/main/item/item.hpp` and the artifact-local
`src/main/item/item.hpp`. The local surface preserves the GAME 4 item enum,
`item_t` playfield-motion layout, item constants and storage declarations,
miss-velocity table, counters, and lifecycle entry points. Its `items_miss_add`
and `stage_point_items_collected` declarations retain the target-bound v154
far/`extern "C"` and byte-width corrections already required by the maintained
source.

The v946 TC4J reference/local probe produced identical link-semantic OMF
(`ec1897de6e3908a4a10c51def070e411a0cffa7c35088cb9f0de9c901cc89447`);
receipt SHA-256:
`262e66ff5751efb4348e9483843c7a1045ba10fe9742927bb22a1c10f1250aa4`.
The first focused replay failed closed on the pristine near/C++ declaration;
after applying the target-bound corrections, the focused
`items_update` owner is raw/MAP/relocation exact at 0x546 bytes (receipt
`e82eb933e6cccc8fcb9b3965d7713f1a3e21e8941b3acadd1dfbba551133582c`), with
candidate and target slice SHA-256
`a0949b541b517ba4be1cb7afd1094b60017235eefd777630b20c038a030f6531`.
The v946 aggregate rewrites 16 staged occurrences and preserves all 275
accepted extents in two cold builds with identical diagnostic MAIN SHA-256
`d51db833654d139b6e79c059a70be2859d4f83a3d6c9777e7fbc547f1d3c0bdb`;
aggregate receipt SHA-256:
`3741650ddd38f66cc1771b8b8b2368716e2dd80121625235c3aa5d7f57496e65`.
The v947 inventory is now 87 missing paths / 289 references: 50 headers (252
references), 35 `.cpp` fragments, and two `.inl` fragments; only
`th04/dialog.cpp` remains unmapped. Item DATA/BSS ownership, standalone MAIN
linking, and PC-98 startup remain open.

## MAIN midboss-header closure

The sixteenth declaration batch routes the historical
`th04/main/midboss/midboss.hpp` edge through the product wrapper
`src/main/include/th04/main/midboss/midboss.hpp` and the artifact-local
`src/main/midboss/midboss.hpp`. The local surface preserves the midboss
constants, `midboss_stuff_t` playfield-motion layout, callback pointer
distances, stage entry declarations, hit-test inline helpers, and the generic
flash/render macro while reusing the local midboss state and playfield APIs.

The v950 TC4J reference/local probe produced identical link-semantic OMF
(`dd141e4f49b86d2ff9573f73da202cefe9b5d9e0c36aa38cc37bf2c104c807a1`);
receipt SHA-256:
`45a666ce5359230df2b20faa648ae7f43a347a4c60d678da1d28918e9c9bde01`.
The focused `midboss4_render` owner is raw/MAP/relocation exact at 0x8D bytes
(receipt
`54734a2b85a9e8af4e4701724865f560c75a03f36485df1f7da304b6fd7193dd`), with
candidate and target slice SHA-256
`c877ff2878813ec50576de7bdb230983f318e6b220d741f422f1ef467ee03c97`.
The v950 aggregate rewrites 19 staged occurrences and preserves all 275
accepted extents in two cold builds with identical diagnostic MAIN SHA-256
`d51db833654d139b6e79c059a70be2859d4f83a3d6c9777e7fbc547f1d3c0bdb`;
aggregate receipt SHA-256:
`7a9f23674cfe9d2ec59d0e8ca1a4fe277f329e4a333adc0afc7b2fbb9c3ece07`.
The v951 inventory is now 86 missing paths / 274 references: 49 headers (237
references), 35 `.cpp` fragments, and two `.inl` fragments; only
`th04/dialog.cpp` remains unmapped. Midboss DATA/BSS ownership, standalone
MAIN linking, and PC-98 startup remain open.

## MAIN rank-header closure

The seventeenth declaration batch routes the historical
`th04/main/rank.hpp` edge through the product wrapper
`src/main/include/th04/main/rank.hpp` and the artifact-local
`src/main/rank.hpp`. The local surface preserves the GAME 4 rank enum,
display-string macros, `rank` global, and `select_for_rank` Pascal ABI. Its
type block also honors the historical `TH01_RANK_H` boundary so a mixed
translation unit that has already included the inherited rank header does not
redeclare `rank_t`.

The v953 TC4J reference/local probe produced identical link-semantic OMF
(`83f8091d847f4b7babb72502cb70f94ef66a80deaf6ce8fcf93314bf9bfddc3f`);
receipt SHA-256:
`8f1493886f31cabdf193a2e26e517450e77befaf5bac42e0ab7d4aac51b916d5`.
The accepted full cohort is the 275-owner aggregate: both cold builds retain
diagnostic MAIN SHA-256
`d51db833654d139b6e79c059a70be2859d4f83a3d6c9777e7fbc547f1d3c0bdb`, and the
rank rewrite records 14 staged occurrences. The `stage_bonus` owner at
0x1EC8E/0x58D is raw/MAP/relocation exact with candidate and target slice
SHA-256
`9d34b803f8675921e44088a82add869aa55c3fc4f337ad0d31c3e2289b24adda`;
aggregate receipt SHA-256:
`6a6eb52ffaa032280d65b8800b70f15947edd0228664421cc463122ce5a734be`.
The v954 inventory is now 85 missing paths / 262 references: 48 headers (225
references), 35 `.cpp` fragments, and two `.inl` fragments; only
`th04/dialog.cpp` remains unmapped. The first focused controls exposed the
mixed rank-guard and HUD dependency/cohort limits, so no minimal focused exact
claim is made. Rank DATA/BSS ownership, standalone MAIN linking, and PC-98
startup remain open.

## MAIN stage-header closure

The eighteenth declaration batch routes the historical
`th04/main/stage/stage.hpp` edge through the product wrapper
`src/main/include/th04/main/stage/stage.hpp` and the artifact-local
`src/main/stage/stage.hpp`. The local surface preserves the byte-sized
`stage_id` global and the two near Pascal callback pointers
`stage_invalidate` and `stage_render`, reusing the attested local callback
typedefs in `src/shared/platform/types.hpp`.

The v955 TC4J reference/local probe produced identical link-semantic OMF
(`b2d6a74eae9046ad2ab910bd0674eddcad26a70d5af860f877e1c77137a86d75`);
receipt SHA-256:
`e6a322428d760143f5bf0418a015be0d06e5a38ccabfd9d663a6b41aea263215`.
The focused `demo-session` owner is raw/MAP/relocation exact at 0x51E bytes
(receipt SHA-256
`5e6865be7aefa18aeb4ab93f1343fc150795377fb586f76131bcdea281cbf1f1`), with
candidate and target slice SHA-256
`1c409149015a161d36078c7297e9e5fee04bb2dc4ef1b088c72ec81ed1f71e34`.
The v955 aggregate rewrites 17 staged occurrences and preserves all 275
accepted extents in two cold builds with identical diagnostic MAIN SHA-256
`d51db833654d139b6e79c059a70be2859d4f83a3d6c9777e7fbc547f1d3c0bdb`;
aggregate receipt SHA-256:
`24f33dc5b3e3eb9b0f06e65bdc86cced2c5e85dd970cf9ee758d389f2a9bc6e`.
The v956 inventory is now 84 missing paths / 251 references: 47 headers (214
references), 35 `.cpp` fragments, and two `.inl` fragments; only
`th04/dialog.cpp` remains unmapped. Stage DATA/BSS ownership, standalone MAIN
linking, and PC-98 startup remain open.

This include inventory is only the first frontier. The frame declaration and
storage batch demonstrates the required pairing, the scroll batch adds a
second declaration/storage split, and the sound, pattern, and vector batches
close declaration-only dependencies. The remaining 47 missing header
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
