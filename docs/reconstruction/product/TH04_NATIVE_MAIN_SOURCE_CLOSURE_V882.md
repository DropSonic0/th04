# Native MAIN source closure frontier

MAIN is the remaining large TH04 product-build graph. This is a read-only
inventory of maintained `src/main` C/C++ and `.inl` files, not a standalone
build claim. It does not revisit the two deferred non-exact MAIN functions.
Replay the inventory with
`python3 scripts/probes/inventory_th04_native_main.py`.
The current v999 read-only result is retained at
`.analysis/reconstruction/probes/native-main-inventory-v999-pointnum-20260929/inventory.json`.

| Missing quoted include class | Unique paths | References |
| --- | ---: | ---: |
| `.h` / `.hpp` declarations | 12 | 18 |
| `.cpp` composite fragments | 35 | 35 |
| `.inl` composite fragments | 2 | 2 |
| **Total** | **49** | **55** |

The next remaining header edge is now `th04/gaiji/gaiji.h`, followed
by the remaining declaration and composite surfaces. These names denote needed
declarations, not approval to reuse cross-game product headers.

Eight maintained physical producers include `.cpp` fragments by historical
`th04/` paths: `boss_bg_main01.cpp`, `yuuka6_main034.cpp`,
`kurumi_update.cpp`, `elly_update.cpp`, `yuuka5.cpp`,
`mugetsu_main033.cpp`, `demo_prefix.cpp`, and `dialog/fused.cpp`. Most of
their included names are absent even from the pinned local ReC98 tree.
Related function bodies already exist as separate maintained `src/main`
translation units, but their composition and near-call segment ownership must
be reconciled before adding them to a native MAIN link manifest. Compiling
both a composite and its included child as objects would duplicate publics.
The local ReC98 reference has files at all 12 remaining missing header paths, but only
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

## MAIN null-header closure

The nineteenth declaration batch routes the historical
`th04/main/null.hpp` edge through the product wrapper
`src/main/include/th04/main/null.hpp` and the artifact-local
`src/main/null.hpp`. The local surface preserves the C-linked near and far
Pascal callback declarations `nullfunc_near` and `nullfunc_far`, reusing the
attested local callback typedef boundary without inventing callback storage.

The v957 TC4J reference/local probe produced identical link-semantic OMF
(`35fce2163d270dde89609cd884e6217648b40a4696423b8a26123561a26c531b`);
receipt SHA-256:
`2398b96c187f1e58cc557eedcd10edc2408b2ea25ada1027627800fc98789a1d`.
The focused `std_run` owner is raw/MAP/relocation exact at 0x6D bytes
(receipt SHA-256
`a252a2a8586767b4a2e7a38d3e996361f781c7f1029d55d43cea78ec15b63224`), with
candidate and target slice SHA-256
`123e8bfe546d80986cfaf6176f78704551dc50bf632210474f50c89c76199777`.
The v957 aggregate rewrites 11 staged occurrences and preserves all 275
accepted extents in two cold builds with identical diagnostic MAIN SHA-256
`d51db833654d139b6e79c059a70be2859d4f83a3d6c9777e7fbc547f1d3c0bdb`;
aggregate receipt SHA-256:
`d21177b8aa647c9bacbe22c508203987436bd1903b023e8e87ffddb631ecdcde`.
The v958 inventory is now 83 missing paths / 243 references: 46 headers (206
references), 35 `.cpp` fragments, and two `.inl` fragments; only
`th04/dialog.cpp` remains unmapped. Null callback implementation ownership,
standalone MAIN linking, and PC-98 startup remain open.

## MAIN phase-header closure

The twentieth declaration batch routes the historical
`th04/main/phase.hpp` edge through the product wrapper
`src/main/include/th04/main/phase.hpp` and the artifact-local
`src/main/phase.hpp`. The local surface preserves the GAME 4 phase constants
`PHASE_HP_FILL`, `PHASE_BOSS_ENTRANCE_BB`, `PHASE_EXPLODE_BIG`, and
`PHASE_NONE`, while retaining the historical GAME 5 conditional constants.

The v959 TC4J reference/local probe produced identical link-semantic OMF
(`212b0f77eb40972c801634cc27ea6f485ba8d0b74f21dd238aa8c608a315566c`);
receipt SHA-256:
`30c1cc7893fb226eafca34b996a96f1c29d4a6f7a8becc4b8a71a09d11aebc03`.
The focused `midboss4_render` owner is raw/MAP/relocation exact at 0x8D bytes
(receipt SHA-256
`fe5f79c1aa935cf1b0bacc51757f59e04c521ec202e67c61a7640243cbf89f75`), with
candidate and target slice SHA-256
`c877ff2878813ec50576de7bdb230983f318e6b220d741f422f1ef467ee03c97`.
The v959 aggregate rewrites 11 staged occurrences and preserves all 275
accepted extents in two cold builds with identical diagnostic MAIN SHA-256
`d51db833654d139b6e79c059a70be2859d4f83a3d6c9777e7fbc547f1d3c0bdb`;
aggregate receipt SHA-256:
`23d367d1263e27558c80c822db76f3b1b621531b9ebe052da72a2ac9748ef386`.
The v960 inventory is now 82 missing paths / 234 references: 45 headers (197
references), 35 `.cpp` fragments, and two `.inl` fragments; only
`th04/dialog.cpp` remains unmapped. Phase DATA/BSS ownership, standalone MAIN
linking, and PC-98 startup remain open.

## MAIN slowdown-state closure

The twenty-first declaration/data batch routes the historical
`th04/main/slowdown.hpp` edge through the product wrapper
`src/main/include/th04/main/slowdown.hpp` and the artifact-local
`src/main/slowdown.hpp`. The local surface preserves `turbo_mode`,
`slowdown_factor`, the GAME 5 conditional slowdown flag, and the existing near
`slowdown_frame_delay` declaration. `src/main/core/slowdown_state.asm` adds the
artifact-local `_turbo_mode` BSS owner; `src/main/core/frame_state.asm` remains
the owner of the frame totals and `_slowdown_factor`.

The v962 TC4J/TASM/TLINK/DOS probe passes the byte/word declaration and storage
ABI in two independent runs. Both runs emit the same slowdown-state and frame
OMF identities, a valid 217-relocation MZ, and linked SHA-256
`d9b0151e0f4beb22c8579320fce4c38d0777172e49ffa519aef620ab901be3f9`; receipt
SHA-256 for the second run:
`d062328a047cfe5c8dbaff7ab0658b532920a699c50079a852ef5dbec8ccc059`.
The focused `slowdown_frame_delay` owner is raw/MAP/relocation exact at 0x1A
bytes (receipt SHA-256
`0590ab8c18a96ec9838750fa0d6eca4b63f3f7fb5783444dfad46bea59ba9276`), with
candidate and target slice SHA-256
`c5a69d9d1869b865085b77c768747c2962bdbea165c7d1d21cc3d16320e59f54`.
The v962 aggregate rewrites 9 staged occurrences and preserves all 275 accepted
extents in two cold builds with identical diagnostic MAIN SHA-256
`d51db833654d139b6e79c059a70be2859d4f83a3d6c9777e7fbc547f1d3c0bdb`;
aggregate receipt SHA-256:
`67811ea3980c999dbfb14ff096476c343816d3be09f0f6d81ab12e8051d10aae`.
The v963 inventory is now 81 missing paths / 226 references: 44 headers (189
references), 35 `.cpp` fragments, and two `.inl` fragments; only
`th04/dialog.cpp` remains unmapped. Target BSS ordering/offsets, standalone MAIN
linking, and PC-98 startup remain open.

## MAIN quit-state closure

The twenty-second declaration/data batch routes the historical
`th04/main/quit.hpp` edge through the product wrapper
`src/main/include/th04/main/quit.hpp` and the artifact-local
`src/main/quit.hpp`. The local surface preserves the one-byte `quit_t` enum
(`Q_KEEP_RUNNING`, `Q_QUIT_TO_OP`, and `Q_NEXT_STAGE`) and
`src/main/core/quit_state.asm` owns the one-byte `_quit` BSS symbol.

The v964 TC4J/TASM/TLINK/DOS probe passes `QUIT_PASS` in two independent runs.
Both runs validate the same one-byte state OMF and a valid 217-relocation MZ
with linked SHA-256
`caabf62cb0ba0bbfcde83381158f071663618f4765703945524291e936b6d28d`;
the second receipt SHA-256 is
`fa9577969537da71c251a559e813ce98443b27df2955b53b5f678f77ea7469c9`.
The focused `stage_state_init` owner is raw/MAP/relocation exact at 0xCB bytes
(receipt SHA-256
`368116f82ef660a266ce17375affe635778a09c5d3348f349cf40bee0c558297`), with
candidate and target slice SHA-256
`f41ff21e0254ae8050f35b00c514db046b74b4ae2b02538dd7856715a15dfed7`.
The v964 aggregate rewrites 8 staged occurrences and preserves all 275
accepted extents in two cold builds with identical diagnostic MAIN SHA-256
`d51db833654d139b6e79c059a70be2859d4f83a3d6c9777e7fbc547f1d3c0bdb`;
aggregate receipt SHA-256:
`f8008e85333d7f291a9c3e019cd0ed2a14dec0a609725e8bd44e5a9577e946d9`.
The v965 inventory is now 80 missing paths / 218 references: 43 headers (181
references), 35 `.cpp` fragments, and two `.inl` fragments; only
`th04/dialog.cpp` remains unmapped. Quit-state target BSS ordering/offsets,
standalone MAIN linking, and PC-98 startup remain open.

## MAIN play-character header closure

The twenty-third declaration batch routes the historical `th04/playchar.h`
edge through the product wrapper `src/main/include/th04/playchar.h` and the
artifact-local `src/main/playchar.hpp`. The GAME=4 surface preserves the
`playchar_t` and `shot_type_t` enums, `playchar_other()`, and the `playchar`
global declaration without importing the ReC98 header into product source.

The v966 TC4J reference/local probe passes with semantic OMF SHA-256
`dd123fa70e950c5c93493b3d0241434a705a2b3c07dca432eeafca9c6da7f0ec` (receipt
SHA-256
`6b931b76cb2f8eecd6a0a0c52ce9ffa881edaebb227770cf01246aaf2862b5ae`). The
focused `gameplay_session_init` owner is raw/MAP/relocation exact at 0x1CD
bytes (receipt SHA-256
`308b3095f422fa416f327df8c97435e8904c78fa7804a837be67650bbb2a771c`), with
candidate and target slice SHA-256
`2e37dff3ee0d9933fc207c7157cc5bdb3a8dd582128a07089f4cac2538cf833b`.
The v966 aggregate preserves all 275 accepted extents in two cold builds with
identical diagnostic MAIN SHA-256
`d51db833654d139b6e79c059a70be2859d4f83a3d6c9777e7fbc547f1d3c0bdb` while
rewriting 16 staged occurrences (receipt SHA-256
`1d94ab32501781db1796fbcb73565aca1a4da5244607f8388471df185cac0eda`). The
v967 inventory is now 79 missing paths / 210 references: 42 headers (173
references), 35 `.cpp` fragments, and two `.inl` fragments; only
`th04/dialog.cpp` remains unmapped. The play-character declaration is
compiler-observed only: target selector/storage ownership, standalone MAIN
linking, and PC-98 startup remain open.

## MAIN platform and x86 real-mode header closure

The twenty-fourth declaration batch routes the historical `platform.h` and
`x86real.h` edges through product wrappers
`src/main/include/platform.h` and `src/main/include/x86real.h`, backed by the
maintained shared declarations in `src/shared/platform/types.hpp` and
`src/shared/platform/x86.hpp`. The wrappers preserve the integer/callback
types, segmented register structures, port/interrupt declarations, and
`MK_FP` surface without importing the ReC98 headers into product source.

The v968 TC4J reference/local probe passes with semantic OMF SHA-256
`6a0b4ab3ddee2bee11b29d6adb14137aa4499d0ff243d6e4cc1c8dcf0f84e8d2` (receipt
SHA-256
`e4646fd2bdee379b160e27433acf02e2b28c3816ddda993c4dc142fe8a729bdf`). The
focused `main-entry` owner is raw/MAP/relocation exact at 0x7C bytes at
0xC30C (receipt SHA-256
`1d1b42461307a4f03b45b1432929bcb1c7e44cefc2472449b76f123b308f5eba`), and
the focused `bullets_render` owner is exact at 0x10B bytes at 0x144E5 (receipt
SHA-256
`f60d1ec9a2052dd0e8cc688454b408f5105f951fb5a88a182b6aa7b314b1310a`).
The v968 aggregate preserves all 275 accepted extents in two cold builds with
identical diagnostic MAIN SHA-256
`d51db833654d139b6e79c059a70be2859d4f83a3d6c9777e7fbc547f1d3c0bdb` while
rewriting 23 `platform.h` and 18 `x86real.h` occurrences (receipt SHA-256
`1faa03b0d8a80af58deaf976290eae187acc30211bf0360fab27c9c18bfebff1`). The
v969 inventory is now 77 missing paths / 186 references: 40 headers (149
references), 35 `.cpp` fragments, and two `.inl` fragments; only
`th04/dialog.cpp` remains unmapped. This is compiler-observed declaration
closure only: hardware implementation, target data/BSS placement, standalone
MAIN linking, and PC-98 startup remain open.

## MAIN common-macro header closure

The twenty-fifth declaration batch routes the historical `th04/common.h` edge
through the product wrapper `src/main/include/th04/common.h` and the
artifact-local `src/main/common.hpp`. The GAME=4 surface preserves the
`MAIN_STAGE_COUNT` and `STAGE_EXTRA` macros without importing the ReC98 header
into product source.

The v970 TC4J reference/local probe passes with semantic OMF SHA-256
`919b53d19d4ae0ae919c6957738ce61aaab87a4600007b55deb50acd73367d81` (receipt
SHA-256
`fc5ee84698717bc23a013d5a2e1073cd3841a0995908d06a23f7f4dc34b1f396`). The
focused EMS owner is raw/MAP/relocation exact at 0x1FA bytes at 0xCC88 (receipt
SHA-256
`0621b2512af5669039f24a1bcc1c4d5e20ed872247afc6df16921bf89e7b66b9`), with
candidate and target slice SHA-256
`038ac29ca0095a32ac3594b3bb40fc35605acd8226e5d8518005c721f02c842f`. The
v970 aggregate preserves all 275 accepted extents in two cold builds with
identical diagnostic MAIN SHA-256
`d51db833654d139b6e79c059a70be2859d4f83a3d6c9777e7fbc547f1d3c0bdb` while
rewriting 14 staged occurrences (receipt SHA-256
`1c8b4099622301a50d762cefd174ff4997183813987276b774dedebecee6af09`). The
v971 inventory is now 76 missing paths / 177 references: 39 headers (140
references), 35 `.cpp` fragments, and two `.inl` fragments; only
`th04/dialog.cpp` remains unmapped. This compiler-observed macro closure does
not establish stage-state/data-BSS ownership, standalone MAIN linking, or
PC-98 startup.

## MAIN combat, HUD, and CDG header closure

The twenty-sixth declaration batch routes the historical
`th04/main/bullet/clearzap.hpp`, `th04/main/hud/hud.hpp`, and
`th04/sprites/main_cdg.h` edges through product wrappers and the artifact-local
`src/main/bullet/clearzap.hpp`, `src/main/hud/hud.hpp`, and
`src/main/sprites/main_cdg.hpp`. The local headers preserve the clearzap union
and frame constants, the HUD HP/graze/number APIs, and the GAME=4/GAME=5 CDG
slot macros without importing ReC98 headers into product source.

The v977 TC4J reference/local probe passes with semantic OMF SHA-256
`f63c46cce7df656b34be719f1013a0edb0d936e79c64e40337449b64e5cfcb19` (receipt
SHA-256 `381fbc1ef9e626dedff8a09ad18ef56474c55edfea8d0d3f6b6201a2fd723228`).
The focused `bullets_render` owner is raw/MAP/relocation exact at 0x10B bytes
at 0x144E5 (receipt SHA-256
`0f45ecbc8676435c31c68b6168782dfa9307d7a1e6d24af3b045553e12338a34`),
`midboss_hud_defeat_tu` is exact at 0x1CB bytes at 0x642C (receipt SHA-256
`60f5edc32ae7ddbd73ee579513c9c7fd31f96eae41fc0bd6936bff8ad9ee3423`), and
`BOSS_BD_TEXT` is exact at 0x27 bytes at 0x7667 (receipt SHA-256
`a0f5af102af797e4152e7b0a8f200bcedebbb900ffca66da9ed0504ea800c141`). The
v977 aggregate preserves all 275 accepted extents in two cold builds with
identical diagnostic MAIN SHA-256
`d51db833654d139b6e79c059a70be2859d4f83a3d6c9777e7fbc547f1d3c0bdb` while
rewriting 8 clearzap, 10 HUD, and 11 MAIN CDG occurrences (receipt SHA-256
`375f07c9ece2969f4a45a5d1cf3ecedac6f64e838d3b34ae925ee3a522b32761`). The
v978 inventory is now 73 missing paths / 154 references: 36 headers (117
references), 35 `.cpp` fragments, and two `.inl` fragments; only
`th04/dialog.cpp` remains unmapped. This closes compiler-observed declaration
edges only: bullet/HUD/CDG backing storage, target DATA/BSS ownership,
standalone MAIN placement, and PC-98 startup remain open.

This include inventory is only the first frontier. The frame declaration and
storage batch demonstrates the required pairing, the scroll batch adds a
second declaration/storage split, and the sound, pattern, and vector batches
close declaration-only dependencies. The remaining 36 missing header
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

## MAIN player, shot, and super-sprite header closure

The twenty-seventh declaration batch routes the historical
`th04/main/player/bomb.hpp`, `th04/main/player/shot.hpp`, and
`th04/formats/super.h` edges through the product wrappers
`src/main/include/th04/main/player/bomb.hpp`,
`src/main/include/th04/main/player/shot.hpp`, and
`src/main/include/th04/formats/super.h`, backed by the artifact-local
`src/main/player/bomb.hpp`, `src/main/player/shot.hpp`, and
`src/main/formats/super.hpp`. The local surfaces preserve the bomb state and
callbacks, `Shot`/laser layouts and macros, and the three near Pascal
super-sprite entry wrappers. The GAME=5 conditional pattern include in the
shot header keeps shared TH05 calibration consumers on the same historical
constant surface.

The v979 TC4J reference/local probe passes with semantic OMF SHA-256
`5e0d628d73737cd418324e7cb1ae95dd157febf84740476b2fc721106959790a`
(receipt SHA-256
`04d56e457d97cf16296fc0d1e253fdc9bab36a6cfff648d263a2120acdd78f2c`).
The focused v979 bomb replay selects the downstream
`th04-main-bomb-stars-v178` trigger and proves the contiguous bomb core
(`0x3D9` at file `0x11734`, slice SHA-256
`354fe2369fbbb605d8b1e7deea259afd64470f1d15927e8a66c6c4c0b220f640`) and
bomb-star renderer (`0x11D` at `0x11B0D`, slice SHA-256
`7a97e3330f4d74739ba44c8e108f4dc2ef1b0d9187c595f370e68d79c1fc0664`) in two
cold builds. The focused shot replay selects
`th04-main-enemies-render-v177` and proves the shot producer (`0x2E9` at
`0x11C2A`, slice SHA-256
`47804105eef832fcc43f7fe4ad2ba28e0e395535c1f35f15722d17366614931e`) and
adjacent enemy renderer (`0xCF` at `0x11F13`, slice SHA-256
`1569689d4fb4c791a5f8e782282f01c6d4e5d2765a09e7651fe8fc2900d202f6`). These
passes include exact MAP placement and ordered MZ relocation checks; the bomb
core also validates the zero-code `m1rsuf` residual.

The v979 aggregate receipt
`gpt-5-6-sol-main-player-combat-aggregate-047-20260929/receipt.json` has SHA-256
`1f0d6e9e62467a837dcab3daeb6931a86357508423ed2d26196a36aab6324345` and
preserves all 275 accepted extents across two cold builds with identical
diagnostic MAIN SHA-256
`d51db833654d139b6e79c059a70be2859d4f83a3d6c9777e7fbc547f1d3c0bdb`; the new
rewrites stage 7 bomb, 8 shot, and 7 super-sprite occurrences. The v979
inventory is 70 missing paths / 135 references: 33 headers (98 references),
35 `.cpp` fragments, and two `.inl` fragments; only `th04/dialog.cpp` remains
unmapped. This is compiler-observed declaration closure and affected-unit
revalidation, not target DATA/BSS ownership, standalone MAIN placement, or
PC-98 startup acceptance.

## MAIN enemy API header closure

The twenty-eighth declaration batch routes the historical
`th04/main/enemy/enemy.hpp` edge through
`src/main/include/th04/main/enemy/enemy.hpp`, backed by the artifact-local
`src/main/enemy/enemy.hpp`. The local surface preserves the GAME 4 enemy flag
range, `enemy_t` field order, 32-entry storage declarations, random-position
constant, and near entry ABIs while taking its bullet, item, and pattern types
from product-owned headers.

The v980 TC4J reference/local probe passes with semantic OMF SHA-256
`386f9e1c92c7cd65bc1ccd05866fdb7eb344ec71b4f845a4a9d4da71cc430393`
(receipt SHA-256
`b9f0eae5e43169815f03b0619d94020892f7d0cbe45d96d7c25de391f3a28099`).
The focused replay selects downstream trigger
`th04-main-enemies-render-v177` and proves `enemies_add` (0xDE at file
0x194F3, slice SHA-256
`f4c44950296962e8e5956f772ad74131efd8712c680e242091508a4c517c42`),
`enemies_update` (0x1D6 at 0x19659, slice SHA-256
`0184a3ba5b510c76075380c3f4a6a5ba673fa0fc1badd5122264d731b17b3613`), and
`enemies_render` (0xCF at 0x11F13, slice SHA-256
`1569689d4fb4c791a5f8e782282f01c6d4e5d2765a09e7651fe8fc2900d202f6`) in two
cold builds, including exact MAP placement and ordered MZ relocation checks.

The v980 focused receipt
`gpt-5-6-sol-main-enemy-header-focused-048-20260929/receipt.json` has SHA-256
`d2576ba7a2eb28184c2ea7ccc96da423012b3d8f243380c8d72b51c8961b5871`.
The aggregate receipt
`gpt-5-6-sol-main-enemy-header-aggregate-049-20260929/receipt.json` has
SHA-256 `11ebcf7babe30d96c32a8a8aa293d5a96f034a86607262187a6891dca9f635ab`
and preserves all 275 accepted extents in two cold builds with identical
diagnostic MAIN SHA-256
`d51db833654d139b6e79c059a70be2859d4f83a3d6c9777e7fbc547f1d3c0bdb`; six
enemy-header occurrences are staged. Inventory v980 is 69 missing paths / 129
references: 32 headers (92 references), 35 `.cpp` fragments, and two `.inl`
fragments; only `th04/dialog.cpp` remains unmapped. This is compiler-observed
declaration closure and affected-unit revalidation, not enemy DATA/BSS
ownership, standalone MAIN placement, or PC-98 startup acceptance.

## MAIN planar API header closure

The thirtieth declaration batch routes the historical root `planar.h` edge
through `src/main/include/planar.h`, backed by the product-owned
`src/main/hardware/planar.hpp`. The local surface preserves the complete
historical planar guard, row-dot aliases, `Planar`/`DotRect` templates, VRAM
plane helpers, and GRCG/EGC macros; the current MAIN probe exercises the
row-width aliases, `dots_t()`, and paragraph-aligned `grcg_segment()` arithmetic.

The v982 TC4J reference/local probe passes with semantic OMF SHA-256
`113841b4368c98e426fe3456e601cff139cf4fac55cdaf1e35d527db8434695b`
(receipt SHA-256
`de44f2e42411c486de43a1fd44a96941700bb66c028785e05810058fdfdd0199`).
The focused `th04-main-elly-backdrop-v200` replay proves the 0x0E-byte owner at
file 0xD6CC, slice SHA-256
`6e1968ec26ed949a9659f9f56232985154f34111f607b4a0b8d51debb145688b`, with
exact MAP placement and zero ordered-relocation overlap in two cold builds.

The v982 focused receipt
`gpt-5-6-sol-main-planar-header-focused-054-20260929/receipt.json` has SHA-256
`ab4e6e70ab463837fa90a62284f89e4b19b65ffddc81837fc0d641887ca2d9fd`.
The aggregate receipt
`gpt-5-6-sol-main-planar-header-aggregate-055-20260929/receipt.json` has
SHA-256 `97987d4c472aae68ca7c8fe2b759954e6f4ad59a8c888a4202ca5892dce4d59c`
and preserves all 275 accepted extents in two cold builds with identical
diagnostic MAIN SHA-256
`d51db833654d139b6e79c059a70be2859d4f83a3d6c9777e7fbc547f1d3c0bdb`; eleven
planar-header occurrences are staged. Inventory v982 is 67 missing paths / 119
references: 30 headers (82 references), 35 `.cpp` fragments, and two `.inl`
fragments; only `th04/dialog.cpp` remains unmapped. The local full-surface
header keeps the historical `PLANAR_H` guard because the root-level rewrite
also reaches legacy scaffold consumers. This is compiler-observed declaration
closure and affected-unit revalidation, not target DATA/BSS ownership,
standalone MAIN placement, or PC-98 startup acceptance.

## MAIN playperf header closure

The thirty-sixth declaration batch routes the historical
`th04/main/playperf.hpp` edge through
`src/main/include/th04/main/playperf.hpp`, backed by the semantic MAIN-local
`src/main/playperf.hpp`. The local surface preserves the unsigned-byte
`playperf`/`playperf_max` state, signed-byte `playperf_min`, and the default
large-model Pascal `playperf_raise` / `playperf_lower` declarations. Existing
`src/main/playperf.asm` remains the code owner; this header does not allocate
the playperf DATA/BSS state.

The v988 TC4J reference/local probe passes with semantic OMF SHA-256
`2c8fb2d94fca2a62edb0b2c110fcb7cba9f82a13339c8e95a31d07087a2b0e7b`
(receipt SHA-256
`ce5e82c6d543f59da072390820230405b6d93d5383e665773a9c15b73ca25aa9`).
The focused `th04-main-enemy-script-dispatch-v328` replay proves the 0x690-byte
`th04/escript.cpp` owner at file 0x16DDD / B4M_UPDATE_TEXT:0x1B4D, slice
SHA-256 `b38e0b210ee7e7d2abb08056d1bbb97396466925acc988de42a06bd67a4ea100`,
with raw bytes, MAP placement, and relocations exact in two cold builds. Its
focused receipt SHA-256 is
`8e6826cb1a48af003292b285a4e97179dcea893185ee637752465e13eccd1c05`.
The aggregate receipt
`gpt-5-6-sol-main-playperf-header-aggregate-067-20260929/receipt.json` has
SHA-256 `219264ff348da1fcfebfb5ce3dd32252eef6584ae746be22b3d6f90bfde6b1a7`
and preserves all 275 accepted extents in two cold builds with identical
diagnostic MAIN SHA-256
`d51db833654d139b6e79c059a70be2859d4f83a3d6c9777e7fbc547f1d3c0bdb`;
four playperf rewrite occurrences are staged per build. Inventory v988 is 61
missing paths / 94 references: 24 headers (57 references), 35 `.cpp` fragments,
and two `.inl` fragments; only `th04/dialog.cpp` remains unmapped. This is
compiler-observed declaration closure and affected-unit revalidation, not
playperf DATA/BSS ownership, standalone MAIN placement, or PC-98 startup
acceptance.

## MAIN EMS/CDG header closure

The thirty-seventh declaration batch routes the historical
`th04/main/ems.hpp` edge through `src/main/include/th04/main/ems.hpp`, backed
by the product-owned `src/main/ems.hpp`. The local EMS surface preserves the
GAME-dependent cache layout constants, preload declarations, and inline
bomb-background load ABI without allocating EMS or CDG cache storage. Its CDG
dependency is product-owned through `src/main/include/th04/formats/cdg.h` and
`src/main/formats/cdg.hpp`; `src/shared/formats/cdg.hpp` supplies segment
accessors while reusing the pinned TH03 declaration when its historical guard is
already active. A global `th04/formats/cdg.h` rewrite is intentionally not used:
the OP artifact shares that TH03 surface.

The v989c TC4J reference/local probe passes with semantic OMF SHA-256
`031a8b81c73ee5e489d253ab108d7579943545c142ba4900c76d70ad868e3bf7`
(receipt SHA-256
`0a8ffc11dc81bed951493b05694e3a92673e79126d91164d54f6847cd9871284`).
The aggregate receipt
`gpt-5-6-sol-main-ems-header-aggregate-071-20260929/receipt.json` has SHA-256
`ccb6e8d02e02949b561ef96bcaa96bdc10a17d3bb0ec6d122ed242dd69123f6d` and
preserves all 275 accepted extents in two cold builds with identical diagnostic
MAIN SHA-256
`d51db833654d139b6e79c059a70be2859d4f83a3d6c9777e7fbc547f1d3c0bdb`.
The `th04/ems.cpp` owner is raw/MAP/relocation exact at file `0xCC88`, size
`0x1FA`, program/map start `0xB488` in `EMS_TEXT`, with slice SHA-256
`038ac29ca0095a32ac3594b3bb40fc35605acd8226e5d8518005c721f02c842f`.
Inventory v989 is 59 missing paths / 89 references: 22 headers (52
references), 35 `.cpp` fragments, and two `.inl` fragments; only
`th04/dialog.cpp` remains unmapped, and the next header edge is
`th04/main/hiscore.hpp`. This is compiler-observed declaration closure and
affected-unit revalidation, not EMS cache DATA/BSS ownership, standalone MAIN
placement, or PC-98 startup acceptance.

## MAIN hiscore header closure

The thirty-eighth declaration batch routes the historical
`th04/main/hiscore.hpp` edge through
`src/main/include/th04/main/hiscore.hpp`, backed by the product-owned
`src/main/hiscore.hpp`. The local surface preserves the two near high-score
entry declarations without allocating score-table DATA/BSS storage; existing
score-data ownership remains in the score implementation.

The v990 TC4J reference/local probe passes with semantic OMF SHA-256
`4be9978cbff7d6481af7732598dee1b17dc7b380e023bca2015f3c899eb16680`
(receipt SHA-256
`99b3e9ca2513ade6fb717d1ecc52ac08d381c2d8c846b26d6f522bf25ff84f59`).
The aggregate receipt
`gpt-5-6-sol-main-hiscore-header-aggregate-073-20260929/receipt.json` has
SHA-256 `eb5683763626575bec6f3c9532858a6a26dca38723ca40343fa9fb0d9345e648`
and preserves all 275 accepted extents in two cold builds with identical
diagnostic MAIN SHA-256
`d51db833654d139b6e79c059a70be2859d4f83a3d6c9777e7fbc547f1d3c0bdb`.
The `th04/score_rm.cpp` owner remains raw/MAP/relocation exact at file
`0x1420A`, size `0x2DB`, `SCORE_TEXT:0x7F1A`, slice SHA-256
`41706ba632de7dc75b50a9f55bb88c3111dab1716f9074ae983867d43fafa001`.
Inventory v990 is 58 missing paths / 85 references: 21 headers (48
references), 35 `.cpp` fragments, and two `.inl` fragments; only
`th04/dialog.cpp` remains unmapped, and the next header edge is
`th04/end/end.h`. This is compiler-observed declaration closure and
affected-unit revalidation, not score-table DATA/BSS ownership, standalone
MAIN placement, or PC-98 startup acceptance.

## MAIN end-sequence header closure

The thirty-ninth declaration batch routes the historical
`th04/end/end.h` edge through `src/main/include/th04/end/end.h`, backed by
the product-owned `src/main/end/end.hpp`. The local surface preserves
`end_sequence_t` and the five pinned enum values only; it does not allocate
resident or ending-state DATA/BSS.

The v991 TC4J reference/local probe passes with semantic OMF SHA-256
`4933f81c01f4248c19809488c4fd9ca89cfcd58a6f2b646ac7b152ab0c046693`
(receipt SHA-256
`0806879008b6fa40621755fca2cb01d752c9f8cc16f748baf5dab3655fb8684c`).
The aggregate receipt
`gpt-5-6-sol-main-end-header-aggregate-075-20260929/receipt.json` has
SHA-256 `186171fe1bbd712df330e4fee53c3467bfe6112520de3140cf4468c75190a21d`
and preserves all 275 accepted extents in two cold builds with identical
diagnostic MAIN SHA-256
`d51db833654d139b6e79c059a70be2859d4f83a3d6c9777e7fbc547f1d3c0bdb`.
The `th04/endmain.cpp` owner remains raw/MAP/relocation exact at file
`0xCFB9`, size `0x7C`, `END_TEXT:0x0CC9`, slice SHA-256
`0cc7e7f846ffba45efc6d49668da80370b4b12acc1752b20daa14f3d045afcbb`.
Inventory v991 is 57 missing paths / 81 references: 20 headers (44
references), 35 `.cpp` fragments, and two `.inl` fragments; only
`th04/dialog.cpp` remains unmapped, and the next header edge is
`th04/formats/std.hpp`. A focused replay attempt stops before compilation
because the existing tile-BB rewrite has no occurrence in the minimal unit
tree; it receives no exact credit. This is compiler-observed declaration
closure and affected-unit revalidation, not ending control-flow or
resident DATA/BSS ownership, standalone MAIN placement, or PC-98 startup
acceptance.

## MAIN stage-format header closure

The fortieth declaration batch routes the historical
`th04/formats/std.hpp` edge through
`src/main/include/th04/formats/std.hpp`, backed by the product-owned
`src/main/formats/std.hpp`. The local surface preserves `std_seg`, the GAME=4
map-section ID, scroll-speed pointer, 32 enemy-script slots, far `std_ip`,
`stage_vm`, and the near/Pascal loader, free, and runner declarations. It
declares the stage interface only; it does not allocate stage DATA/BSS state.

The v992 TC4J reference/local probe passes with semantic OMF SHA-256
`ed832abf907f0b0c7cab6e996e51710198deeab9da985b7f5e78324f24c25b06`
(receipt SHA-256
`32347aa6d6a156db44cea390716341fd5df5d4c7377d8dc7db56690a6ae47bd8`).
The aggregate receipt
`gpt-5-6-sol-main-std-header-aggregate-076-20260929/receipt.json` has
SHA-256 `b30659e710644a95bb224e5f282120f483161fd5c8008731b9239990b1a07177`
and preserves all 275 accepted extents in two cold builds with identical
diagnostic MAIN SHA-256
`d51db833654d139b6e79c059a70be2859d4f83a3d6c9777e7fbc547f1d3c0bdb`.
The `th04/std.cpp` owner remains raw/MAP/relocation exact at file `0xCED1`,
size `0xE8`, `STD_TEXT:0x0BE1`, slice SHA-256
`6ab7989e03ddb80fdc8d288dc0ec7a18d964f5b74b705d0c5c82a16ac8000153`;
the dependent `std_run`, enemy-script dispatch, and `enemies_add` owners also
remain exact. Inventory v992 is 56 missing paths / 77 references: 19 headers
(40 references), 35 `.cpp` fragments, and two `.inl` fragments; only
`th04/dialog.cpp` remains unmapped, and the next header edge is
`th04/main/item/splash.hpp`. Existing independent legacy declarations in
`tile_ring_update.cpp` still require a separate ownership/type reconciliation.
This is compiler-observed declaration closure and affected-unit revalidation,
not stage DATA/BSS ownership, loader contents, standalone MAIN placement, or
PC-98 startup acceptance.

## MAIN item-splash header closure

The forty-first declaration batch routes the historical
`th04/main/item/splash.hpp` edge through
`src/main/include/th04/main/item/splash.hpp`, backed by the existing
product-owned `src/main/item/splash.hpp`. The local surface preserves
`item_splash_t`, the eight-entry splash array, the GAME=4 dot count, and the
fastcall/near/Pascal entry declarations. It declares the item-splash interface
only; it does not allocate the splash DATA/BSS state.

The v993 TC4J reference/local probe passes with semantic OMF SHA-256
`1acf41339bc13ce5ca328ecd620bcc90cd7600bee4bb3478c65e83c1dc3a3d5a`
(receipt SHA-256
`64a55d63ac0726d35ac1c30cadf4785cd97cc7587f75afb79aa6658de8ac2b8c`).
The aggregate receipt
`gpt-5-6-sol-main-splash-header-aggregate-077-20260929/receipt.json` has
SHA-256 `c31cee89b1b5756e34ac2e702b6bb08030113857c7009dfc65858bd02323060b`
and preserves all 275 accepted extents in two cold builds with identical
diagnostic MAIN SHA-256
`d51db833654d139b6e79c059a70be2859d4f83a3d6c9777e7fbc547f1d3c0bdb`.
The `items_invalidate` owner remains raw/MAP/relocation exact at file `0xD8FC`,
size `0x4B`, `CIRCLE_TEXT:0x160C`, slice SHA-256
`4c138c62ef87c53e2da2b73c6c8eaeb79c6eac59f41559982c578a9d191dfee7`; the
`item_splashes_render`, `item_splashes_init`, and `items_update` owners also
remain exact. Inventory v993 is 55 missing paths / 73 references: 18 headers
(36 references), 35 `.cpp` fragments, and two `.inl` fragments; only
`th04/dialog.cpp` remains unmapped, and the next header edge is `decomp.hpp`.
The reference BSS slice still owns `_item_splashes_unused`, `_item_splashes`,
and `_item_splash_last_id`; their target placement, standalone MAIN placement,
and PC-98 startup acceptance remain open. This is compiler-observed declaration
closure and affected-unit revalidation, not splash DATA/BSS ownership or
runtime exactness.

## MAIN decomp helper closure

The forty-second declaration/helper batch routes the historical decomp.hpp
edge through src/main/include/decomp.hpp. The local surface preserves the
bytewise wrapper template, REP MOVSW register-order helpers, compiler-layout
barriers, port/emit helpers, and GAME=4 copy ordering used by
boss.cpp, boss_prefix.inl, and gather.cpp. These are source-level compiler
helpers; the header does not allocate resident or gameplay DATA/BSS.

The v994 TC4J reference/local probe passes with semantic OMF SHA-256
693393aab5fde7545dc19a53392082310bd3f0c70c132b77ff471d0c2526a2eb
(receipt SHA-256
dfe3f72c838c72f67ac7edf76c957e1ae9d64a689a02b0caad3becd3f49eefdd).
The aggregate receipt
gpt-5-6-sol-main-decomp-header-aggregate-078-20260929/receipt.json has
SHA-256 626e225f075df6f9bf130abe4088cc414a1c9bf1f582f60fcee89daf74a13bbd
and preserves all 275 accepted extents in two cold builds with identical
diagnostic MAIN SHA-256
d51db833654d139b6e79c059a70be2859d4f83a3d6c9777e7fbc547f1d3c0bdb.
The gather.cpp owner remains raw/MAP/relocation exact at file 0x1529C,
size 0x24B, GATHER_TEXT:0x000C, slice SHA-256
761972f8f1e1b92d48a447515f17b904ace1f184485174ac07063f31c6646c1c; the
boss.cpp owner remains exact at file 0x1F761, size 0x9B6,
MAIN_035_TEXT:0xA4D1, slice SHA-256
14ac3390b67ec8ee06b25669e9f8a62b9da07a9a82d96e100864a9fbe24cad1f.
The tree rewrite stages seven decomp.hpp occurrences. Inventory v994 is
54 missing paths / 70 references: 17 headers (33 references), 35 .cpp
fragments, and two .inl fragments; only th04/dialog.cpp remains unmapped,
and the next header edge is th04/main/bg.hpp. This is compiler-observed
helper-surface closure and affected-unit revalidation, not target low-level
ownership, standalone MAIN placement, or PC-98 startup acceptance.

## MAIN background callback header closure

The forty-third declaration batch routes the historical th04/main/bg.hpp edge
through src/main/include/th04/main/bg.hpp, backed by src/main/bg.hpp. The local
surface preserves the three nearfunc_t_near callback declarations
(bg_render_not_bombing, bg_render_bombing, and bg_render_bombing_func) and
remains declaration-only; it allocates no background or tile state.

The v995 TC4J reference/local probe passes with semantic OMF SHA-256
5ae107e4e7593f62c024f04c52740f6fa0858711718451334c22355692ab34ee
(receipt SHA-256
f749fc960491c0ae6782d79d4a37830579fd65727c49b729190f3b99d01ad93a).
The aggregate receipt
gpt-5-6-sol-main-bg-header-aggregate-079-20260929/receipt.json has
SHA-256 1e86bfc8aa06ef636f8272eaa4134937e3efa09dc262391f4d2ea78bf0d19bb3
and preserves all 275 accepted extents in two cold builds with identical
diagnostic MAIN SHA-256
d51db833654d139b6e79c059a70be2859d4f83a3d6c9777e7fbc547f1d3c0bdb.
The boss.cpp owner remains raw/MAP/relocation exact at file 0x1F761, size
0x9B6, MAIN_035_TEXT:0xA4D1, slice SHA-256
14ac3390b67ec8ee06b25669e9f8a62b9da07a9a82d96e100864a9fbe24cad1f; the
bomb.cpp owner remains exact at file 0x11734, size 0x3D9, MAIN__TEXT:0x5444,
slice SHA-256
354fe2369fbbb605d8b1e7deea259afd64470f1d15927e8a66c6c4c0b220f640.
The v995 inventory is 53 missing paths / 67 references: 16 headers
(30 references), 35 .cpp fragments, and two .inl fragments; only
th04/dialog.cpp remains unmapped, and the next header edge is
th04/main/dialog/dialog.hpp. The reference th04/main/tile/inv[bss].asm still
owns the callback words beside tile invalidation state. This is
compiler-observed callback declaration closure, not BSS ownership, standalone
MAIN placement, or PC-98 startup acceptance.

## MAIN point-number header closure

The forty-seventh declaration/layout batch routes the historical
`th04/main/pointnum/pointnum.hpp` edge through
`src/main/include/th04/main/pointnum/pointnum.hpp`, backed by the
artifact-local `src/main/pointnum/pointnum.hpp`. The local surface preserves
the GAME=4 pointnum_t field order and 16-byte layout, popup/frame/digit/ring
constants, near/pascal entry declarations, alive-pointer state, and the
`pointnum_put` macro. It declares the state owned by the existing point-number
ASM producers but allocates no DATA/BSS.

The v999 TC4J GAME=4 reference/local probe passes with semantic OMF SHA-256
`2a8607c70cf5c872e46b5642d0f1afabae678ffbdbd297fe135fb8191099ad0c`
(receipt SHA-256
`5172b3a9d64591ebc078e71c1288e00a03b6bdfb6817ae53b5dd74eee2e33db1`).
The v999 aggregate receipt
`gpt-5-6-sol-main-pointnum-header-aggregate-083-20260929/receipt.json` has
SHA-256 `e4cd34626dfbdd6b0e3c5213e9998dd3307f848ac3ba717996d4d1e69c3a12ce`
and preserves all 275 accepted extents in two cold builds with identical
diagnostic MAIN SHA-256
`d51db833654d139b6e79c059a70be2859d4f83a3d6c9777e7fbc547f1d3c0bdb`.
Inventory v999 is now 49 missing paths / 55 references: 12 headers (18
references), 35 `.cpp` fragments, and two `.inl` fragments; only
`th04/dialog.cpp` remains unmapped, and the next header edge is
`th04/gaiji/gaiji.h`. This is compiler-observed declaration/layout closure,
not point-number DATA/BSS ownership, standalone MAIN placement, or PC-98
startup acceptance.

## MAIN overlay header closure

The forty-sixth declaration/helper batch routes the historical
`th04/main/hud/overlay.hpp` edge through
`src/main/include/th04/main/hud/overlay.hpp`, backed by the artifact-local
`src/main/hud/overlay.hpp`. The local surface preserves the near overlay
callbacks, fade constants, popup enum/entry points, title callbacks, and the
inherited text-RAM fill macros. It also carries the gaiji enum needed by the
existing overlay producer, but allocates no overlay or gaiji DATA/BSS.

The v998 TC4J GAME=4 reference/local probe passes with semantic OMF SHA-256
`a26b77bd8663d61c764d0288110cd97b82c7a46614a0fdf3722ce00456dd793c`
(receipt SHA-256
`1fc2aade4882af7e717d954aca8ca6bcbd5bfd620eb6b09e817f72b007f2f5ab`).
The v998 aggregate receipt
`gpt-5-6-sol-main-hud-overlay-aggregate-082-20260929/receipt.json` has
SHA-256 `a41dcc3ec69751b2676f2e40047c51362ccafd2502517f42ed4cccb06cfc2e1f`
and preserves all 275 accepted extents in two cold builds with identical
diagnostic MAIN SHA-256
`d51db833654d139b6e79c059a70be2859d4f83a3d6c9777e7fbc547f1d3c0bdb`.
The maintained `overlay.cpp` owner remains raw/MAP/relocation exact at its
accepted slice (source SHA-256
`26254650e84d3e8c28d27b1e17f73623fa620a0844f93221684ecee59dfec850`).
Inventory v998 is now 50 missing paths / 58 references: 13 headers
(21 references), 35 `.cpp` fragments, and two `.inl` fragments; only
`th04/dialog.cpp` remains unmapped, and the next header edge is
`th04/main/pointnum/pointnum.hpp`. This is compiler-observed declaration and
helper closure, not overlay/gaiji storage ownership, standalone MAIN
placement, or PC-98 startup acceptance.

## MAIN Shift-JIS filename macro closure

The forty-fifth declaration batch routes the historical
`th04/shiftjis/fns.hpp` edge through
`src/main/include/th04/shiftjis/fns.hpp`, backed by the artifact-local
`src/main/shiftjis/fns.hpp`. The local surface preserves the TH04 packfile,
eyecatch, faceset, boss BB/background, and redefined faceset-loader macros.
The Shift-JIS packfile bytes are represented with escaped source bytes so the
product header remains UTF-8-safe; no DATA/BSS storage is introduced.

The v997 TC4J GAME=4 reference/local probe passes with semantic OMF SHA-256
`91ff184b745fe3f5b493846d93ffca9628c272b5d7b6e2c49bdfd9720dd5c93e`
(receipt SHA-256
`15bbe3d7bd7dca8c5ae60004659157634761c233b456ca11c1927a0f4c479159`).
The v997 aggregate receipt
`gpt-5-6-sol-main-shiftjis-fns-aggregate-081-20260929/receipt.json` has
SHA-256 `41f8d26cd52c1d94f92cb00e6f2781e6fe85e9f9e3ea7cbdd11dbf8c823331dc`
and preserves all 275 accepted extents in two cold builds with identical
diagnostic MAIN SHA-256
`d51db833654d139b6e79c059a70be2859d4f83a3d6c9777e7fbc547f1d3c0bdb`;
`boss.cpp` remains raw/MAP/relocation exact at file `0x1F761`, size `0x9B6`,
`MAIN_035_TEXT:0xA4D1`, slice SHA-256
`14ac3390b67ec8ee06b25669e9f8a62b9da07a9a82d96e100864a9fbe24cad1f`.
Inventory v997 is now 51 missing paths / 61 references: 14 headers (24
references), 35 `.cpp` fragments, and two `.inl` fragments; only
`th04/dialog.cpp` remains unmapped, and the next header edge is
`th04/main/hud/overlay.hpp`. This is compiler-observed macro closure and
affected-unit revalidation, not target string-data ownership, standalone MAIN
placement, or PC-98 startup acceptance.

## MAIN dialog header closure

The forty-fourth declaration batch routes the historical th04/main/dialog/dialog.hpp
edge through src/main/include/th04/main/dialog/dialog.hpp, backed by
src/main/dialog/dialog.hpp. The local surface preserves the stage-transition
callback pointer, dialog_animate(), and the GAME=4 dialog_init()/dialog_exit()
entry declarations; it allocates no dialog state.

The v996 TC4J reference/local probe passes with semantic OMF SHA-256
a426841032133841315a534943fcb80065df91e6700d02d460b0f0661f1bacdf
(receipt SHA-256
8dda346783b8f5e2f27447712d8c33c72dd6b226edd01825993e0d3091c770a1).
The aggregate receipt
gpt-5-6-sol-main-dialog-header-aggregate-080-20260929/receipt.json has
SHA-256 665606c3c0ef15d73334afd6f3addfd59fc655ca95a5609a67dec941c14680b2
and preserves all 275 accepted extents in two cold builds with identical
diagnostic MAIN SHA-256
d51db833654d139b6e79c059a70be2859d4f83a3d6c9777e7fbc547f1d3c0bdb.
The boss.cpp owner remains raw/MAP/relocation exact at file 0x1F761, size
0x9B6, MAIN_035_TEXT:0xA4D1, slice SHA-256
14ac3390b67ec8ee06b25669e9f8a62b9da07a9a82d96e100864a9fbe24cad1f; the
session_init.cpp owner remains exact at file 0xC503, size 0x1CD,
DEMO_TEXT:0x08FC, slice SHA-256
2e37dff3ee0d9933fc207c7157cc5bdb3a8dd582128a07089f4cac2538cf833b.
The v996 inventory is 52 missing paths / 64 references: 15 headers
(27 references), 35 .cpp fragments, and two .inl fragments; only
th04/dialog.cpp remains unmapped, and the next header edge is
th04/shiftjis/fns.hpp. The reference th04/main/dialog/dialog[bss].asm still
owns _std_update, _dialog_p, _dialog_cursor, and _dialog_side. This is
compiler-observed dialog declaration closure, not dialog BSS ownership,
standalone MAIN placement, or PC-98 startup acceptance.

## MAIN tile-BB header closure

The thirty-fifth declaration batch routes the historical
`th04/main/tile/bb.hpp` edge through
`src/main/include/th04/main/tile/bb.hpp`, backed by the semantic MAIN-local
`src/main/tile/bb.hpp`. Unlike the existing `.BB` file-format interface, the
local surface preserves the 512x512 animation-cell metrics, segmented animation
storage, GRCG tile entry point, and `tiles_bb_put` / `tiles_bb_invalidate`
macros. It declares the animation interface only; it does not allocate the BB
storage or other DATA/BSS state.

The v987 TC4J reference/local probe passes with semantic OMF SHA-256
`2af6fc5d7b45c1d0d358db712537bab9d249b74ccece104026ab14eeccb05490`
(receipt SHA-256
`145f83e3369332182f8d33a73b459b774ac05113098b6892a8c760ecbc385f18`).
The focused `th04-main-mugetsu-gengetsu-bg-v102` replay again proves the
0x91-byte `th04/mgbg.cpp` owner at file 0x14179 / BOSS_BG_TEXT:0x7E89, slice
SHA-256 `92adca5d8d07674114f2b5a9c65a89a8a54cbef5f929eeb5c583e28c765cb683`,
with raw bytes, MAP placement, and relocations exact in two cold builds. Its
focused receipt SHA-256 is
`3be3bfe18673cbb509b1bc40d30698b2d61ce04f78acaa460756df6d83599f82`.
The aggregate receipt
`gpt-5-6-sol-main-bb-header-aggregate-065-20260929/receipt.json` has SHA-256
`58f7118b54ad73888ba16d68f6fab83da2e2bcad143c5005641608f744b3c542` and
preserves all 275 accepted extents in two cold builds with identical diagnostic
MAIN SHA-256
`d51db833654d139b6e79c059a70be2859d4f83a3d6c9777e7fbc547f1d3c0bdb`;
four tile-BB rewrite occurrences are staged per build. Inventory v987 is 62
missing paths / 98 references: 25 headers (61 references), 35 `.cpp` fragments,
and two `.inl` fragments; only `th04/dialog.cpp` remains unmapped. This is
compiler-observed declaration closure and affected-unit revalidation, not BB
storage ownership, standalone MAIN placement, or PC-98 startup acceptance.

## MAIN dialog-format header closure

The thirty-fourth declaration batch routes the historical
`th04/formats/dialog.hpp` edge through
`src/main/include/th04/formats/dialog.hpp`, backed by the semantic MAIN-local
`src/main/formats/dialog.hpp`. The local surface preserves the far dialog
script-buffer pointer, the near `dialog_load(const char *)` and
`dialog_load()` overloads, the default-distance Yuuka-5 loader, and the near
`dialog_free()` declaration. It declares the dialog interface only; it does not
allocate the dialog buffer or other DATA/BSS storage.

The v986 TC4J reference/local probe passes with semantic OMF SHA-256
`f4da531e53b2dd33dd023cc82d237d1011f15714504ed69aeff8678cfd2ad45c`
(receipt SHA-256
`7e7ac4c1d9bea54ac3f07bbcb48d6c49a799572f1e402371d3cabc4d964158f3`).
The focused `th04-main-module-th04-f-dialog-cpp-ce93` replay proves the 0xAA-byte
`th04/f_dialog.cpp` owner at file 0xE693, slice SHA-256
`33389d17fee16f4d25b700efdfae8455c865d143fac26bfa2a33753b9171afe3`, with
raw bytes, MAP placement, and relocations exact in two cold builds. Its receipt
SHA-256 is
`c70dc189f201caaa498bd914c2daadd8242bdb25c80a389081bde07943afeea2`.
The aggregate receipt
`gpt-5-6-sol-main-dialog-header-aggregate-063-20260929/receipt.json` has SHA-256
`280b80de269e831f23ae8e9af8c49853c60fd090dfa87d29cef5abbf1c818abc` and
preserves all 275 accepted extents in two cold builds with identical diagnostic
MAIN SHA-256
`d51db833654d139b6e79c059a70be2859d4f83a3d6c9777e7fbc547f1d3c0bdb`;
six dialog-format rewrites are staged per build. Inventory v986 is 63 missing
paths / 102 references: 26 headers (65 references), 35 `.cpp` fragments, and
two `.inl` fragments; only `th04/dialog.cpp` remains unmapped. This is
compiler-observed declaration closure and affected-unit revalidation, not dialog
DATA/BSS ownership, standalone MAIN placement, or PC-98 startup acceptance.

## MAIN boss-declarations header closure

The thirty-third declaration batch routes the historical
`th04/main/boss/bosses.hpp` edge through
`src/main/include/th04/main/boss/bosses.hpp`, backed by the semantic
MAIN-local `src/main/boss/bosses.hpp`. The local surface preserves the six
Pascal near backdrop callbacks, the near/far Mugetsu and Gengetsu entry
declarations, and the `GENGETSU_W`/`GENGETSU_H` constants. It declares these
interfaces only; it does not allocate boss callback state or DATA/BSS storage.

The v985 TC4J reference/local probe passes with semantic OMF SHA-256
`2813a2905698aa0f6fb94d9aab80d536e96082ed654e5b67d1cd13b558ee800a`
(receipt SHA-256
`fb9cb29d5c3ac13fd13f1d1823eab462f64845a209f8937cab0a72107dc0f4c7`).
The focused `th04-main-main035-boss-tu` replay proves the 0x9B6-byte
`th04/boss.cpp` owner at file 0x1F761, slice SHA-256
`14ac3390b67ec8ee06b25669e9f8a62b9da07a9a82d96e100864a9fbe24cad1f`, with
raw bytes, MAP placement, and relocations exact in two cold builds. Its receipt
SHA-256 is
`c4604bc80339715d68c82e013e1f14fdbf84cefbbe5fd367170d7881a6651329`.
The aggregate receipt
`gpt-5-6-sol-main-bosses-header-aggregate-061-20260929/receipt.json` has
SHA-256 `180b6f77c6ab3225ae73f12d83861f114a084feab1135d375181d160885cc40b`
and preserves all 275 accepted extents in two cold builds with identical
diagnostic MAIN SHA-256
`d51db833654d139b6e79c059a70be2859d4f83a3d6c9777e7fbc547f1d3c0bdb`;
four boss-declaration rewrites are staged per build. Inventory v985 is 64
missing paths / 106 references: 27 headers (69 references), 35 `.cpp` fragments,
and two `.inl` fragments; only `th04/dialog.cpp` remains unmapped. This is
compiler-observed declaration closure and affected-unit revalidation, not boss
callback DATA/BSS ownership, standalone MAIN placement, or PC-98 startup
acceptance.

## MAIN boss-backdrop header closure

The thirty-second declaration batch routes the historical
`th04/main/boss/backdrop.hpp` edge through
`src/main/include/th04/main/boss/backdrop.hpp`, backed by the semantic
MAIN-local `src/main/boss/backdrop.hpp`. The local surface preserves the
`nearfunc_t_near boss_backdrop_colorfill` callback and the Pascal near
`boss_backdrop_render(screen_x_t, vram_y_t, vc_t)` declaration. It does not
allocate the callback's target DATA/BSS slot.

The v984 TC4J reference/local probe passes with semantic OMF SHA-256
`77301002e906ff337223b81062afe57f3e5180237758d5f2cd36c04edbec1fa3`
(receipt SHA-256
`14ac78cf2bd9e691bd6bb2c2ad0e35f57788870ef902d86e451a279dc5261dec`).
The focused `th04-main-mugetsu-gengetsu-bg-v102` replay proves the 0x91-byte
`th04/mgbg.cpp` owner at file 0x14179 / BOSS_BG_TEXT:0x7E89, slice SHA-256
`92adca5d8d07674114f2b5a9c65a89a8a54cbef5f929eeb5c583e28c765cb683`, with
raw bytes, MAP placement, and the target's ordered relocation overlap exact in
two cold builds. Its focused receipt
`gpt-5-6-sol-main-backdrop-header-focused-058-20260929/receipt.json` has
SHA-256 `e9cd19592cb78cf1d5098904cf404465bb874d89991e009a7b4ba97d6b1e47fd`.

The aggregate receipt
`gpt-5-6-sol-main-backdrop-header-aggregate-059-20260929/receipt.json` has
SHA-256 `b17a0c6b40a6cb92b4605d5df5f59ef12b0c3c8c626dbf3206a7cf9e55cb9f42`
and preserves all 275 accepted extents in two cold builds with identical
diagnostic MAIN SHA-256
`d51db833654d139b6e79c059a70be2859d4f83a3d6c9777e7fbc547f1d3c0bdb`; five
backdrop-header occurrences are staged. Inventory v984 is 65 missing paths /
110 references: 28 headers (73 references), 35 `.cpp` fragments, and two
`.inl` fragments. This is compiler-observed declaration closure and affected
unit revalidation, not boss backdrop DATA/BSS placement, standalone MAIN
placement, or PC-98 startup acceptance.

## MAIN drawpoint-header closure

The thirty-first declaration batch routes the historical
`th04/main/drawp.hpp` edge through
`src/main/include/th04/main/drawp.hpp`, backed by the semantic player-owned
`src/main/player/drawp.hpp`. The local surface preserves the historical
`PlayfieldPoint` dependency and the single `extern PlayfieldPoint drawpoint`
declaration. The existing v176 replay transform remains the owner of the
drawpoint BSS field split; this header does not allocate a second storage
definition.

The v983 TC4J reference/local probe passes with semantic OMF SHA-256
`be3864707b1601a673d64373aaf28b67ebd017b8aab918953e64f04d687c04c0`
(receipt SHA-256
`d8f34d7bb6523b1d3dcaa57e6fc646bda9e14842972f2dbe35be12dddd557f85`).
The focused `th04-main-player-invalidate-v176` replay proves the 0xB6-byte
owner at file 0x11FE2, slice SHA-256
`13b2ea1657c35eefbd27fb1eaa66c637f48815877175dfbe541e162d0e7a218b`, with
exact MAP placement and the target's ordered relocation overlap in two cold
builds.

The v983 focused receipt
`gpt-5-6-sol-main-drawpoint-header-focused-056-20260929/receipt.json` has
SHA-256 `b647066a687e56251eb7fda4a5092bc79e111731d0c64960352bd4f2a14b3d0e`.
The aggregate receipt
`gpt-5-6-sol-main-drawpoint-header-aggregate-057-20260929/receipt.json` has
SHA-256 `6db0643197e6aa51138701e26b32fb7966125024999685c04430b346c755a15c`
and preserves all 275 accepted extents in two cold builds with identical
diagnostic MAIN SHA-256
`d51db833654d139b6e79c059a70be2859d4f83a3d6c9777e7fbc547f1d3c0bdb`; six
drawpoint-header occurrences are staged. Inventory v983 is 66 missing paths /
114 references: 29 headers (77 references), 35 `.cpp` fragments, and two
`.inl` fragments; only `th04/dialog.cpp` remains unmapped. This is
compiler-observed declaration closure and affected-unit revalidation, not
drawpoint DATA/BSS placement, standalone MAIN placement, or PC-98 startup
acceptance.

## MAIN input API header closure

The twenty-ninth declaration batch routes the historical
`th04/hardware/input.h` edge through
`src/main/include/th04/hardware/input.h`, backed by the MAIN-local
`src/main/hardware/input.hpp`. The local surface preserves the historical
inputvar/input declaration order, 16-bit input flags, replay byte type,
diagonal and Q flags, `shiftkey`, and the interface alias. It intentionally
does not widen `src/shared/hardware/input.hpp`, which remains the shared
OP/MAINE owner.

The v981 TC4J reference/local probe passes with semantic OMF SHA-256
`4ad85e3888b72d31bb17af992df891b9858480c2742fa223a54222af25d0212f`
(receipt SHA-256
`9681b0e36bff4eb8edd06d992fdd5f34ef11d4fcfef1dc8388e576d37fe1291c`).
The focused `th04-main-input-wait-for-change` replay proves the 0x56-byte
owner at file 0x14A13, slice SHA-256
`1b01e6a95f7ce32132d8760ec73ce22bd17b299758d63ebb554e8abc12976518`,
with exact MAP placement and empty ordered-relocation overlap in two cold
builds.

The v981 focused receipt
`gpt-5-6-sol-main-input-header-focused-050-20260929/receipt.json` has SHA-256
`5bba457aa4810b6c7db7459a8a4de96b541cc372b2f683d268304dfbabbacd84`.
The aggregate receipt
`gpt-5-6-sol-main-input-header-aggregate-051-20260929/receipt.json` has
SHA-256 `d10911be70dba1e7b9bb400dfac7815bcc22cbf0bdee60188d1764fac72fb5fe`
and preserves all 275 accepted extents in two cold builds with identical
diagnostic MAIN SHA-256
`d51db833654d139b6e79c059a70be2859d4f83a3d6c9777e7fbc547f1d3c0bdb`; five
input-header occurrences are staged. Inventory v981 is 68 missing paths / 124
references: 31 headers (87 references), 35 `.cpp` fragments, and two `.inl`
fragments; only `th04/dialog.cpp` remains unmapped. This is compiler-observed
declaration closure and affected-unit revalidation, not key-state DATA/BSS
ownership, standalone MAIN placement, or PC-98 startup acceptance.
