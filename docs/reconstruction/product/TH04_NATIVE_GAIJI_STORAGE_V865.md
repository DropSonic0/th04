# TH04 native gaiji storage and BFNT loader

2026-09-27 product-build investigation. `src/shared/hardware/gaiji_storage.cpp`
owns far Pascal `GAIJI_BACKUP`, `GAIJI_RESTORE`, and `GAIJI_ENTRY_BFNT`.
Backup allocates 512 paragraphs in the TH04 heap for 256 two-byte-wide,
16-row CG glyphs. Restore writes that image to CG RAM and frees the handle.
The CG code is `(glyph + 0x5680) & 0xFF7F`; each row stores its left byte
before its right byte. Port `0x68` selects dot access, ports `0xA1/0xA3`
select the code, `0xA5` selects the half and row, and `0xA9` transfers data.
The bounded historical gaiji routines corroborate this PC-98 mechanism, but
the current owner is TH04-local source and is not a target byte claim.

The BFNT loader uses TH04's file API to read a 32-byte header and requires
`BFNT\x1A`, monochrome, 16×16 pixels, and glyph range 0–255. It skips the
little-endian extension length at header offset 28, requires a complete
8,192-byte payload, then writes it to CG RAM. The header layout is
reference-corroborated; the actual `GAMEFT.bft` asset has not yet been
checked against this parser in a pinned PC-98 game run. The historical BFNT
loader borrowed an 8 KB short-memory buffer, whereas this TH04 candidate
uses `HMEM_ALLOCBYTE`; success under memory pressure can therefore differ.

The checked-in `scripts/probes/probe_th04_native_gaiji_storage_runtime.py`
links this owner and the TH04 heap under pinned TC4J and runs synthetic BFNT
fixtures under pinned MS-DOS Player. Test-only DOS adapters supply the four
file API entries. The v8 receipt SHA-256 is
`c1d08f7d3cc712b037af46438c2190ffd7cc056466dd0f38757b096fd220aa0d`:
`GAIJI_PASS` exits 0 after valid, invalid-color, truncated, and absent-file
cases plus repeated backup/restore cycles. This tests return paths, ownership,
header checks, extension skip, and payload length. It does not verify CG port
pixel values or the product's file hook.

An earlier test-only stdio adapter failed at the valid BFNT case. Before
segmented-heap assignment it could open the fixture; after assignment and
backup, `fopen` returned DOS error 8. The v7 failed receipt is
`d053ec5310ab84636ead0329a821d95a8e5d7c623fba6a9396441909cdae8049`
and its runtime log SHA-256 is
`17b6c86a12ac6a7d47b0381cd42aa696b063e5e6b4b6b654f00e4c5389cbdca9`.
Replacing only that test adapter with direct DOS open/read/seek/close calls
passes without changing product source. For TH05, use a file adapter whose
own heap demands do not mask the code under test; preserve failed logs when
diagnosing memory pressure.

Two cold no-archive MAINE builds compile 121 TH04 units (78 C++, 43 ASM),
leave 12 unresolved names, and report zero warnings. Link-relevant and
timestamp-normalized OMF records agree; BGIMAGE alone has known raw timestamp
drift. Receipt SHA-256 values are
`14fdd6e86c417924468f2afc2f5dd3d8a5b285690bebe528950a0c5f3683c325`
and `b2215cfaf583a3a7449767613bdb0907f18bf8f0e40429b29732b6d1d7ee9ef5`.
The manifest digest is
`273015e98e1354650e0cf29fdda3665409d5ac6a6f1e72ac43013e828b4e89d1`.

The historical-library calibration TLINK exits 0 (receipt
`dcda57b435a956f7034018203a5f6cab4c71eb7294af61c7bd0a4f31cd2787f6`)
with the known archive-dictionary warning. Its MZ validates 613 relocation
sites at two DOS load segments (receipt
`ce823d4169977e41ef988ecec293ca61f5a6a0b19a24a7e8bfaa50805332af84`).
The call audit checks 32 far returns, 122 relocated direct far calls, and one
same-CS `push cs; call` (receipt
`d938ffbfd3a331be65a4cf8babe8043279815b1053c24b307479459edb245fcf`).
At calibration MAP `093B:315A`, `093B:3190`, and `093B:31C1`, the three
gaiji entries return with `RETF`, `RETF`, and `RETF 4`; these are calibration
addresses, not target addresses. The calibration MZ still contains historical
support and has not reached MAINE under PC-98 runtime.

The remaining no-archive symbols are sprite storage/rendering (3), sound (4),
packed-file service (3), and packed graphics (2). Their closure, a standalone
MAINE MZ, and a PC-98 scenario remain separate gates.
