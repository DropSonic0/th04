# TH04 PAR archive fixtures and native state owner

2026-09-27 product-build investigation. The pinned HDI has two game archive
files under `GENSO/`: the OP/ending container is 1,120,284 bytes with 132
members, and the main-game container is 1,053,177 bytes with 158 members.
Their SHA-256 values and raw FAT short names are pinned in
`scripts/probes/probe_th04_pf_archive.py`. This is a local HDI observation;
the HDI still has `candidate-local-attested` provenance.

The 16-byte PAR header contains a directory byte length, member count, and
initial directory key. Its encrypted directory has one 32-byte terminal
record after the declared member count. For each directory byte, the decoder
XORs the current key and subtracts the resulting plain byte from the key.
Each member record stores type, auxiliary XOR key, 13-byte name, packed and
declared original sizes, and 32-bit payload offset. Observed types are
`0xF388` (stored) and `0x9595` (repeated-byte encoding). The historical
PFGETC source corroborates that two equal literal bytes are followed by a
count of *additional* copies. Its optional nibble rotation is commented out
for this game; the auxiliary key applies only an XOR to payload bytes.

The checked-in host probe verifies the pinned HDI hash, FAT geometry and both
mirrors, both archive hashes, directory terminators, all 290 member offsets,
every decoded length, and PI/BFNT magic. It writes four selected fixtures only
under a fresh private `.analysis/reconstruction/probes/` directory. The v3
receipt SHA-256 is
`a3200ed18a538f52a1453f33dd1c5e8d69536ce57ce6f392b20f30fa740c8039`.
The ending archive has 42 PI and 9 BFNT members; the main archive has 13
BFNT members. Private fixtures include `GAMEFT.BFT`, an uncompressed
`CONG10.PI`, a packed `CONG14.PI`, and `ST00.BFT`. Use the receipt's content
hashes to bind a future DOS/PC-98 differential test without checking game
assets into Git. Re-run with a fresh `--output-dir` and repeatable
`--extract op_end:NAME` or `--extract main:NAME` options for other fixtures.

Five complete repeated-byte expansions exceed their directory's declared
logical size by exactly one byte: ending `SCNUM2.BFT`, and main `ST00.BFT`,
`EYE1.CDG`, `EYE4.CDG`, `MIKO.EFC`. The probe reports and pins those cases;
it writes only the declared logical extent for fixtures. A future native
file-hook test must establish how TH04's DOS reads and seeks handle these
members. Do not silently change the declared sizes or treat the extra byte
as proof of corrupt media.

`src/shared/formats/pf_state.asm` owns the TH04-local `bbufsiz`, `pferrno`,
and `pfkey` storage and C/Pascal public aliases. The default buffer size is
512 bytes; `game_init_main()` sets it to 4096 and `game_init_op()` to 8192
before calling `pfstart()`. This data owner does not implement PAR opening or
the DOS INT 21h hook. Two cold no-archive MAINE builds compile 123 TH04 units
(79 C++, 44 ASM), leave 10 unresolved names, and report zero warnings.
The comparator finds equal link-relevant and timestamp-normalized OMF
records; only BGIMAGE has its known raw timestamp drift. The A/B receipt
SHA-256 values are
`95bb7a7aba3f992c0bfa8eab9fda6a0ca0e68713a133ca56cf35b228a7c441bf`
and `5fdd6401e51d9c260208437eaff18828e986bdf353551b6d072ba39d93c36b8b`.
The source manifest digest is
`e69c011ea3bd6748c09df3e264951e449212bcb60aa02b8d6be3f4421c77a68d`.

The historical-library calibration TLINK exits 0 with the known archive
dictionary warning (receipt
`22f204c4be6da2c9e7fc111d208d4e2236b1e7038aadf8b97470fdfafadf85de`).
Its MZ passes 614 relocation sites at two DOS load segments (receipt
`a5388877a411080123247417311b12b84479d43edaead30f56b504f1fea112ca`),
and the far-call audit checks 33 returns, 124 relocated direct calls, and
one same-CS call (receipt
`1fababaf022f9af428ba1c2004b85ed49d973502ad7699c5e7fe281b2b892de8`).
Calibration MAP `0E57:0424` contains both `bbufsiz` and `_bbufsiz`; the
corresponding load-module word is `0x0200`. `_pferrno` and `_pfkey` follow at
`0E57:0426` and `0E57:0428`, initialized to zero. These are calibration
addresses, not target addresses. No independent TH04 product or PC-98
runtime claim follows from either the host archive probe or this data-only
owner.
