# TH04 native text VRAM gaiji writers

2026-09-27 product-build investigation. The TH04-local
`src/shared/hardware/gaiji_text.cpp` owns `GAIJI_PUTCA` and `GAIJI_PUTSA`.
Each input byte selects two adjacent PC-98 text cells. The first glyph word
is `((c & 0x7F) << 8) | (0x56 + (c >> 7))`; the second adds bit 15.
The words go to text VRAM at segment `0xA000 + y * 10`, offset `x * 2`,
with their attributes in the plane at byte offset `0x2000`. An exhaustive
256-byte local calculation reproduced the bounded historical reference's
`ROL`/`SHR`/`ADC` glyph transform. This is a semantic candidate; target bytes,
display timing, and runtime output have not been observed. The C++ writer
interleaves glyph and attribute stores for each character, whereas the
historical string writer stores all glyphs before filling attributes; only
the final VRAM contents for nonaliasing inputs have been corroborated from
the reference.

Two cold no-archive MAINE builds compile 116 TH04 units (74 C++, 42 ASM).
They leave 25 unresolved names and zero warnings, two fewer than the PI
cleanup checkpoint. Their link-relevant and timestamp-normalized OMF records
agree; BGIMAGE alone has the known raw timestamp drift. The private receipt
SHA-256 values are
`f372c6313e63c55366c9923ff73f9172ddf6ca505be3b0423c6218cb8d8c2fe8`
and `a006d8a2a6d00e64eaca4bb19f1d883532718780931705fcbcc55ae3de657cea`.
The explicit manifest SHA-256 is
`5c03df64a0474917a59a480cc2523059340a53e7a9a318478f0640907c9b63a6`.

The historical-library calibration TLINK link exits 0 (receipt
`be2081c9d2c3942947156ca99fb1bed392120095541106262dbad163a7413958`).
The MZ audit validates 597 relocation sites and two DOS load segments
(receipt `17704bf2c67a98680efaa50c2770d77ca8a282979b69419aa424792529a934c0`).
The ABI audit checks 17 far entries and 66 relocated direct far calls, plus
one same-CS `push cs; call rel16` (receipt
`df2a4bfced906ce64a1be0a5a3515c23a4bae33f75a1efa1e809fa94b3155789`).
The linked calibration MAP locates `GAIJI_PUTCA` at `09A6:3007` with `RETF 8`
and `GAIJI_PUTSA` at `09A6:304A` with `RETF 10`; these are calibration
addresses, not original MAINE target addresses. The calibration MZ still
contains historical support and has not reached MAINE under a runtime route.

For TH05, keep byte-to-glyph mapping, the two text VRAM planes, far Pascal
argument cleanup, and hardware write timing as separate verification points.
