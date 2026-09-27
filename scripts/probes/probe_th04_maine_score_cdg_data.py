#!/usr/bin/env python3
"""Attest TH04 MAINE's score keyboard, filename, and CDG bit-mask data."""

from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path
import struct
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
from lib.pc98 import parse_mz  # noqa: E402

RESTORED = ROOT / ".analysis/reconstruction/diet-replay/v228-maine-target-roundtrip/a/restored.bin"
RESTORED_SHA = "6b4547182b9d53d069c0e4efc33bdabb69065cb544bb187ced7b0f51918aa533"
ALPHABET = bytes((*range(0xAA, 0xC6), 3, 6, 7, 8, 12, 15,
                  *range(0xA0, 0xAA), 0xE6, 0xE7, 0xE8, 0xCE, 0xCF, 0xCD, 0xD5))
SCOREFN = b"GENSOU.SCR\0" * 4
CDG_MASK = struct.pack(
    "<16H", *([0xFF00 | (0xFF >> j) for j in range(8)] +
               [(0xFF >> (j - 8)) << 8 for j in range(8, 16)])
)


def sha(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def hits(haystack: bytes, needle: bytes) -> list[int]:
    found = []
    pos = 0
    while (pos := haystack.find(needle, pos)) != -1:
        found.append(pos)
        pos += 1
    return found


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output-dir", type=Path, required=True)
    args = parser.parse_args()
    output = args.output_dir.resolve()
    private = (ROOT / ".analysis/reconstruction/probes").resolve()
    if output.exists() or not output.is_relative_to(private):
        parser.error("output must be a new directory below .analysis/reconstruction/probes")
    subprocess.run([sys.executable, "scripts/preflight.py"], cwd=ROOT, check=True,
                   stdout=subprocess.DEVNULL)
    raw = RESTORED.read_bytes()
    if sha(raw) != RESTORED_SHA:
        raise ValueError("pinned decoded target identity drift")
    mz = parse_mz(raw)
    if not mz.valid:
        raise ValueError("decoded target is not valid MZ")
    image = mz.program_image
    expected = (("alphabet", ALPHABET, 0xED5C),
                ("score_filename_x4", SCOREFN, 0xED8F),
                ("cdg_first_word_masks", CDG_MASK, 0xEAB0))
    for name, pattern, offset in expected:
        actual = hits(image, pattern)
        if actual != [offset]:
            raise ValueError(f"{name} occurrence drift: {actual}")
    output.mkdir(parents=True)
    receipt = {
        "schema_version": 1,
        "artifact": "th04-maine",
        "evidence_class": "target-analysis",
        "restored_sha256": RESTORED_SHA,
        "patterns": [
            {"name": name, "load_offset": hex(offset),
             "size": len(pattern), "sha256": sha(pattern),
             "unique_hits": [hex(offset)]}
            for name, pattern, offset in expected
        ],
        "limit": "Only target bytes and unique occurrences are observed. Source symbol names, BSS ownership, and new-link runtime behavior require independent checks.",
    }
    (output / "receipt.json").write_text(json.dumps(receipt, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({item[0]: hex(item[2]) for item in expected}, sort_keys=True))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
