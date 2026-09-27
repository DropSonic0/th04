# TH04 native graphics gaiji writers

2026-09-27 product-build investigation. The TH04-local
`src/shared/hardware/graph_gaiji.cpp` owns far Pascal `GRAPH_GAIJI_PUTC` and
`GRAPH_GAIJI_PUTS`. The code selects GRCG RMW mode and four color tiles,
switches the character generator to dot access, reads both halves of each
of 16 glyph rows from ports `0xA5`/`0xA9`, and writes three shifted bytes per
row into the current `graph_VramSeg` graphics plane. It restores CG code
access and turns GRCG off. The interrupt flag is preserved around the GRCG
mode port write. Drawing has no clipping, as in the bounded reference.

The glyph code uses `(c + 0x5680) & 0xFF7F`. This ordinary addition is also
observed at the local opcode sites of the four independently restored
TH04/TH05 OP/MAINE targets in the
[v493 MASTER version note](../packed/TH04_ZUN_MASTER_VERSION_V493.md);
the generic historical library still has the older carry-dependent variant.
All 524,288 combinations of two CG bytes and eight bit shifts agreed with
the bounded reference's three output bytes in a local algebra check.
MAINE's six maintained `GRAPH_GAIJI_PUTS` calls use `GAIJI_W == 16` as their
step. Other steps and interrupt/display timing remain unobserved; the
historical string routine has an unusual 16-bit byte-offset recurrence for
negative steps that this C++ product candidate does not reproduce.

Two cold no-archive MAINE builds compile 117 TH04 units (75 C++, 42 ASM).
They leave 23 unresolved names and zero warnings, two fewer than the text
gaiji checkpoint. Link-relevant and timestamp-normalized OMF records agree;
BGIMAGE alone has its known raw timestamp drift. The two private receipt
SHA-256 values are
`c9caa0e92b3829d5c7349c718b2bbd7ba9919bbae30dec984c13afd80a3aad7d`
and `e59a67d5302e5a4552ba6b89959020d3f9e5132076270a3fb9824f15d0ec9f6e`.
The manifest SHA-256 is
`c9b5552e1d6547aac0a3c5f9b55a8008850123c3f8f094c7c18c7e9e44dcdf56`.

The historical-library calibration TLINK link exits 0 (receipt
`b3006658a1349ec1402e8c663efff528f6d0c3d1dc25828fc7041bc7515af098`).
The MZ audit validates 597 relocation sites and two DOS load segments
(receipt `cc76f5c7e9cd97ed408207c55a139f1361911448db401f37b00b61ee034e03a7`).
The call ABI audit checks 19 far entries, 75 relocated direct far calls and
one same-CS `push cs; call rel16` (receipt
`02e2a1aa2fa2066151d562f16b093bd00d82b4e2d390efcf13a49912bffcf65c`).
The calibration MAP locates `GRAPH_GAIJI_PUTC` at `0993:3191` with `RETF 8`
and `GRAPH_GAIJI_PUTS` at `0993:31B3` with `RETF 12`; these are linked
calibration addresses, not original target addresses. This MZ still contains
historical-library support, and no MAINE runtime scenario has exercised the
PC-98 display path.

For TH05, keep target glyph-code opcodes, far Pascal cleanup, CG/GRCG port
order, VRAM output bytes, and actual screen timing as separate gates.
