# TH04 native PC-98 scroll and GRCG rectangle owners

2026-09-27 product-build investigation. The local historical library supplies
bounded semantic reference behavior; it is not evidence that these TH04
implementations reproduce target bytes or real hardware timing.

`src/shared/hardware/graph_scrollup.asm` owns the far Pascal
`GRAPH_SCROLLUP(line)` entry. It clamps the requested line count to the
current `graph_VramLines`, applies `graph_VramZoom`, waits for the graphics
GDC parameter FIFO, and sends the two start-address/line-count runs through
PC-98 ports `0xA2` and `0xA0`. Its near helper writes a 16-bit parameter
with the inter-byte delay used by the bounded reference.

`src/shared/hardware/grcg_byteboxfill_x.asm` owns the far Pascal
`GRCG_BYTEBOXFILL_X(left, top, right, bottom)` entry. It clips vertical
coordinates against the TH04-owned `ClipYT` and `ClipYH`, rejects reversed
X or Y ranges, then writes `0xFF` to each byte in the inclusive rectangle.
It advances ES by five paragraphs per row, equal to 80 VRAM bytes. The
caller owns GRCG mode and tile state; this routine does not silently change
it. Horizontal clipping is absent, as in the bounded reference.

Both additions are semantic product candidates. TC4J/TASM32 emitted valid
OMF and two cold 114-TU no-archive MAINE links each leave 28 unresolved
names with zero warnings, down exactly two from the prior graphics-start
checkpoint. Their link-relevant and timestamp-normalized objects agree;
BGIMAGE has only the known raw dependency timestamp drift. Cold receipt
SHA-256: `5f4fceb14aa7e50ce465408c6725fb4bd92dbb11901c7ad6369714cb123097da`
and `7b44e3af3fb3e7d7b218e183e63a33a4d8b5db107b5e8e3b79ad7b7c17bf9f15`.

The historical-library calibration link exits 0 with its existing archive
dictionary warning (receipt
`888c8234cd377d7baa2ca87bd9d1a5f9fd10540ec2c3ea4bb684bd1d16eab746`).
The resulting MZ has 594 unique in-image relocation sites; its entry, stack,
and two DOS load checks pass (receipt
`8985974013985b08718dff408cd9ff93bfb0b22719545886cec91b52eabba63d`).
The call ABI audit checks 14 local entry returns, 51 relocated direct far
calls, and one same-CS `push cs; call rel16` (receipt
`63e84693b5be4e38a9d989ab269b9b8742b6ba39bbca628246bff3f7a9aa4bf5`).

These static checks cannot establish GDC timing, GRCG write effects, or a
runnable TH04-only product. The original MAINE entry still needs a valid
OP→MAIN→MAINE scenario; the no-archive link still needs 28 local support
names. For TH05, keep graphics state ownership, far ABI, GDC FIFO timing,
and VRAM write semantics as separately testable surfaces.
