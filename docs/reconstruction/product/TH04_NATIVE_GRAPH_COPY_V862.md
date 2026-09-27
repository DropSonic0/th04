# TH04 native graphics page copy

2026-09-27 product-build investigation. `src/shared/hardware/graph_copy_page.cpp`
owns the far Pascal `GRAPH_COPY_PAGE(to_page)` entry. It allocates one plane's
temporary word buffer, turns GRCG off, and copies the blue, red, green, and
extra graphics planes from the opposite page to `to_page & 1`. For each
plane, port `0xA6` selects the source page before reading and the destination
page before writing. Success leaves the destination page selected and returns
1; an allocation failure changes neither GRCG mode nor access page and
returns 0. `graph_VramWords` supplies the live plane length.

The bounded historical routine uses `SMEM_WGET`/`SMEM_RELEASE`, while this
TH04 product candidate borrows a 32,000-byte block at the current 400-line
setting through `HMEM_ALLOCBYTE`/`HMEM_FREE`. Those two heap entries remain
unresolved in the no-archive build. The different buffer pool can change
allocation success under memory pressure; no MAINE runtime page-copy
checkpoint exists yet. The four-plane copy and page-selection behavior are
the supported semantic claim, not target bytes or timing.

TC4J 4.02J treats the loop variable from `for(unsigned i = ...)` as in scope
throughout the function. Repeating that declaration in the second copy loop
failed compilation (`compile-054.log` SHA-256
`f77acebfe6005226d7169b8ccae74c7c8b9b5d602d655704a62323ba33b8715b`).
Declaring `i` once before both loops compiled. This is a portable lesson for
TH05 code written for the same compiler.

Two cold no-archive MAINE builds compile 118 TH04 units (76 C++, 42 ASM),
leave 22 unresolved names and zero warnings. Their link-relevant and
timestamp-normalized OMF records agree; BGIMAGE alone has known raw timestamp
drift. Receipt SHA-256 values are
`c8a6165649f8f61400ee9d8c745ad631a76c892607a7df253477631017d5d4c4`
and `6eb1d284fbf8ae917ca0cd51f02ace8d10c6fcb7d68df5431f0c447acdc0241c`.
The explicit manifest SHA-256 is
`70cbceac771bc0f4a8105e43f9da0b446eb7b50cef6171c4529e169fa4bc8e89`.

The historical-library calibration TLINK link exits 0 (receipt
`b217c84e7268f81a7e7699fb69912e8539ce0ca8180826d03d721111f0ecd8b5`).
The MZ audit validates 599 relocation sites and two DOS load segments
(receipt `1fad510500cffd2c7ef87ad90cff95b4065da19090414d8986891c4ba047ac34`).
The call ABI audit checks 20 far entries, 85 relocated direct far calls and
one same-CS `push cs; call rel16` (receipt
`0c0cad8c484f15e60e8f35ff37136dcd6290ae5599d4c723eccf01f027c9ebc7`).
The calibration MAP locates `GRAPH_COPY_PAGE` at `098C:3105` with `RETF 2`;
this is a calibration address, not the original target address. The MZ still
contains historical-library support and has not reached MAINE in a runtime
scenario.
