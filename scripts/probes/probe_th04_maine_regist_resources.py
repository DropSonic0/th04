#!/usr/bin/env python3
"""Attest MAINE score-registration filenames and warning text in the target."""

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
WARNING = "スローモードでのプレイでは、スコアは記録されません"
NAMES = ("hi01.pi", "scnum2.bft")


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
        raise ValueError("pinned decoded MAINE identity drift")
    mz = parse_mz(raw)
    if not mz.valid:
        raise ValueError("decoded MAINE MZ structure invalid")
    text = WARNING.encode("cp932") + b"\0"
    block = b"".join(name.encode("ascii") + b"\0" for name in NAMES)
    block += text + text + b"name\0"
    image = mz.program_image
    hits = [i for i in range(len(image) - len(block) + 1)
            if image.startswith(block, i)]
    if hits != [0xEDBB] or len(text) != 51 or len(block) != 126:
        raise ValueError(f"registration data block drift: {hits}")
    output.mkdir(parents=True)
    receipt = {
        "schema_version": 1,
        "artifact": "th04-maine",
        "evidence_class": "target-analysis",
        "restored_sha256": RESTORED_SHA,
        "load_offset": "0xEDBB",
        "block_size": len(block),
        "block_sha256": hashlib.sha256(block).hexdigest(),
        "warning_cp932_sha256": hashlib.sha256(text).hexdigest(),
        "warning_occurrences": 2,
        "limit": "Filename and warning bytes only; product data placement and runtime rendering require separate checks.",
    }
    (output / "receipt.json").write_text(json.dumps(receipt, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"load_offset": "0xEDBB", "block_size": len(block),
                      "warning_occurrences": 2}, sort_keys=True))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
