#!/usr/bin/env python3
"""Attest the TH04 MAINE SE tables and initial sound state in the decoded target."""

from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
from lib.pc98 import parse_mz  # noqa: E402

RESTORED = ROOT / ".analysis/reconstruction/diet-replay/v228-maine-target-roundtrip/a/restored.bin"
RESTORED_SHA = "6b4547182b9d53d069c0e4efc33bdabb69065cb544bb187ced7b0f51918aa533"
PRI = bytes((0, 0, 32, 16, 2, 18, 18, 64, 16, 17, 2, 18, 32, 32, 32, 32, 0))
FRAMES = bytes((0, 0, 36, 16, 4, 16, 8, 48, 80, 17, 4, 11, 80, 80, 80, 32, 0))


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
    table = PRI + FRAMES
    if hits(image, table) != [0xEA8E]:
        raise ValueError(f"SE priority table occurrence drift: {hits(image, table)}")
    if image[0xEB30:0xEB32] != b"\xff\x00":
        raise ValueError("SE playing/frame initial bytes drift")
    ext = b"m26\0m86\0mmd\0"
    if hits(image, ext) != [0xEAE4]:
        raise ValueError(f"sound extension strings drift: {hits(image, ext)}")
    output.mkdir(parents=True)
    receipt = {
        "schema_version": 1,
        "artifact": "th04-maine",
        "evidence_class": "target-analysis",
        "restored_sha256": RESTORED_SHA,
        "priority_table_load_offset": "0xEA8E",
        "priority_table_size": len(table),
        "priority_table_sha256": sha(table),
        "se_initial_load_offset": "0xEB30",
        "se_initial_bytes": "ff00",
        "extensions_load_offset": "0xEAE4",
        "extensions_sha256": sha(ext),
        "limit": "Only target bytes and unique occurrences are observed. Variable names, runtime effects, and layout in a new product link require separate checks.",
    }
    (output / "receipt.json").write_text(json.dumps(receipt, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"table_offset": "0xEA8E", "table_size": len(table),
                      "table_sha256": sha(table)}, sort_keys=True))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
