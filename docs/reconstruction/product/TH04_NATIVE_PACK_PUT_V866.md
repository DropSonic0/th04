# TH04 native packed-row renderer

2026-09-27 product-build investigation. `src/shared/hardware/graph_pack_put_8.cpp`
owns the far Pascal `GRAPH_PACK_PUT_8` entry. It applies the shared vertical
clip rectangle, then calls the existing TH04-local
`graph_pack_put_8_noclip.cpp` for 4-bit packed pixels, horizontal clipping,
and writes to the four PC-98 graphics planes. `pi_put_8` and
`pi_put_quarter_8` use this entry. The source is a semantic implementation;
neither this renderer nor its historical master-library counterpart is a
target-byte acceptance claim. The original routine selects `ClipYT_seg` as
the graphics base, whereas the current local unpacker uses the shared VRAM
plane pointers. The default 640×400 base is compatible, but nondefault clip
segment selection still needs a real PC-98 scenario.

The checked-in `scripts/probes/probe_th04_native_pack_put_runtime.py` compiles
both TH04 product units with pinned TC4J, links a DOS test MZ, and runs it
under pinned MS-DOS Player. Four fake VRAM planes verify nibble-to-plane
mapping, inclusive vertical clipping, negative-X source advancement using
different pixel groups, and right-edge clipping. The v2 receipt SHA-256 is
`fe0663661132235e27807f248924803ccc2be1ee1800e09f8e14749ddf6de1de`;
the process exits 0. This verifies the software byte writes, not hardware
page selection or displayed pixels.

Two cold no-archive MAINE builds compile 122 TH04 units (79 C++, 43 ASM) and
leave 11 unresolved names with zero warnings. The comparator reports equal
link-relevant and timestamp-normalized OMF records; only BGIMAGE has its known
raw timestamp drift. The A/B receipt SHA-256 values are
`8839f67316ae1f1c8e8694191ffd854ae4908ae343e1733aad0d56b7d0dfbf72`
and `7a3a91a0180f8c610aee0ae0888f2ec9fdef5828828014cb767281448da31ea7`.
The source manifest digest is
`59bbf4a1e85fce3c5ad5d3e9a021f188393e139bfbc5408097f34e514f28f4b8`.
Run `python3 scripts/probes/compare_th04_native_maine_link.py` on the two
output directories, not on their receipt files.

The historical-library calibration TLINK exits 0 with the known archive
dictionary warning (receipt
`87a170fe38a7e73f62b5252f3df978e35b7987bd255e1ccfe66f4cb7adaa384f`).
The MZ audit checks 614 relocation sites at two DOS load segments (receipt
`45d8153ad3a3e4d07a40e511ad2faabc32b1a3fa1df02ec5bb282b2a3c98f3cd`).
The call audit checks 33 far returns, 124 relocated direct far calls, and one
same-CS `push cs; call` (receipt
`3b7bad60df4d2f7c56aec1a090b1faa878e2252a3e9ba72a08dbe98e42bd57ce`).
`GRAPH_PACK_PUT_8` at calibration MAP `08EE:351F` returns with `RETF 10` and
has two relocated far call sites. This address belongs to the calibration
MZ, not the target. These checks validate the static structure of a
diagnostic mixed-support MZ; they do not establish a TH04-only product or
MAINE PC-98 runtime success.

## PI decoder handoff

This v866 handoff was completed by the
[v869 PI decoder batch](TH04_NATIVE_PI_DECODE_V869.md); its seven-name
no-support frontier and real-resource differential are the current state.
At v866, `GRAPH_PI_LOAD_PACK` was a separate 11-name frontier item. The bounded
historical decoder source is `libs/master.lib/graph_pi_load_pack.asm`; it
parses `Pi` plus a 0x1A-terminated comment, mode/aspect/plane fields,
machine extension, big-endian dimensions, optional 48-byte palette, then
adaptive packed-pixel data. It reserves two previous rows ahead of the
returned image pointer. The TH04 heap has `hmem_alloc(unsigned)` in 16-byte
paragraphs, so allocations over 64 KiB can be represented; its
`hmem_allocbyte(unsigned)` entry alone cannot represent such a byte count.
Before implementing, establish the decoder's buffer ownership and cross-
segment pointer arithmetic with a synthetic PI oracle. `graph_pi_free` frees
the image by its base segment. The pinned HDI
(`0d5ea773a9e4f3e28f473b6deeedb6a7cdaccbb5b940a97983c4e3597dd4ebfd`)
has no standalone `.PI` directory entry; TH04 data resides in game container
files under `GENSO/`. The checked-in FAT12 reader in
`scripts/probes/prepare_th04_maine_diagnostic_hdi.py` can enumerate or extract
files from a private copy. A reference decoder or independently constructed
PI fixture is needed before claiming decoded image behavior. Do not close the
name with an inert return or use the historical archive in the product link.
