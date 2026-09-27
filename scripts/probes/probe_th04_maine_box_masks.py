#!/usr/bin/env python3
"""Attest MAINE's five semantic cutscene EGC masks in the decoded target."""

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
MAP = ROOT / ".analysis/gpt-web/v489-bgimage-hybrid-replay-003/a/maine/source/obj/th04/maine.map"
OFFSET = 0xEB5C
MASKS = (
    (0x8888, 0x0000, 0x2222, 0x0000),
    (0x8888, 0x4444, 0x2222, 0x1111),
    (0xAAAA, 0x4444, 0xAAAA, 0x1111),
    (0xAAAA, 0x4444, 0xAAAA, 0x5555),
    (0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF),
)


def sha(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


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
    words = [word for row in MASKS for word in row]
    pattern = struct.pack("<20H", *words)
    hits = []
    pos = 0
    while (pos := mz.program_image.find(pattern, pos)) != -1:
        hits.append(pos)
        pos += 1
    if hits != [OFFSET]:
        raise ValueError(f"mask occurrence drift: {hits}")
    map_text = MAP.read_text(encoding="cp437", errors="replace")
    if "0E53:062C       _BOX_MASKS" not in map_text:
        raise ValueError("candidate MAP corroboration drift")
    output.mkdir(parents=True)
    receipt = {
        "schema_version": 1,
        "artifact": "th04-maine",
        "evidence_class": "target-analysis",
        "restored_sha256": RESTORED_SHA,
        "load_module_offset": hex(OFFSET),
        "table_size": len(pattern),
        "table_sha256": sha(pattern),
        "unique_hits": [hex(hit) for hit in hits],
        "candidate_map_address": "0E53:062C",
        "candidate_map_sha256": sha(MAP.read_bytes()),
        "limit": "Only the bytes and unique occurrence are target observations; the MAP symbol name and segment binding are candidate corroboration. BSS state and product runtime require separate checks.",
    }
    (output / "receipt.json").write_text(json.dumps(receipt, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"offset": hex(OFFSET), "size": len(pattern),
                      "sha256": sha(pattern)}, sort_keys=True))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
