#!/usr/bin/env python3
"""Attest MAINE staffroll and verdict filenames in the decoded TH04 target."""

from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
sys.path[0:0] = [str(ROOT / "scripts"), str(ROOT / "scripts/probes")]
from lib.pc98 import parse_mz  # noqa: E402
from probe_th04_maine_sound_state import hits, sha  # noqa: E402

RESTORED = ROOT / ".analysis/reconstruction/diet-replay/v228-maine-target-roundtrip/a/restored.bin"
RESTORED_SHA = "6b4547182b9d53d069c0e4efc33bdabb69065cb544bb187ced7b0f51918aa533"
STAFF_NAMES = (
    "sff1.pi", "staff", "sff1.cdg", "sff1b.cdg", "sff2.cdg",
    "sff2b.cdg", "sff3.cdg", "sff3b.cdg", "sff2.pi", "sff4.cdg",
    "sff4b.cdg", "sff5.cdg", "sff5b.cdg", "sff8.cdg",
    "sff8b.cdg", "sff9.cdg", "sff9b.cdg", "sff6.cdg",
    "sff6b.cdg", "sff7.cdg", "sff7b.cdg",
)


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
    if hashlib.sha256(raw).hexdigest() != RESTORED_SHA:
        raise ValueError("pinned decoded MAINE target identity drift")
    mz = parse_mz(raw)
    if not mz.valid:
        raise ValueError("decoded MAINE target is not valid MZ")
    image = mz.program_image
    block = b"".join(name.encode("ascii") + b"\0" for name in STAFF_NAMES)
    verdict = b"ude.pi\0"
    if hits(image, block) != [0xEB88]:
        raise ValueError(f"staff filenames drift: {hits(image, block)}")
    if hits(image, verdict) != [0xED54]:
        raise ValueError(f"verdict filename drift: {hits(image, verdict)}")
    output.mkdir(parents=True)
    receipt = {
        "schema_version": 1,
        "artifact": "th04-maine",
        "evidence_class": "target-analysis",
        "restored_sha256": RESTORED_SHA,
        "staff_names_load_offset": "0xEB88",
        "staff_names_count": len(STAFF_NAMES),
        "staff_names_size": len(block),
        "staff_names_sha256": sha(block),
        "verdict_name_load_offset": "0xED54",
        "verdict_name_sha256": sha(verdict),
        "limit": "Only filename bytes and unique occurrences are target observations; a new product's data owners and runtime asset loading need separate checks.",
    }
    (output / "receipt.json").write_text(json.dumps(receipt, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"staff_names": len(STAFF_NAMES), "staff_offset": "0xEB88",
                      "verdict_offset": "0xED54"}, sort_keys=True))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
