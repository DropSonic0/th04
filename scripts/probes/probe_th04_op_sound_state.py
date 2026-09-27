#!/usr/bin/env python3
"""Attest OP's shared TH04 sound tables and initial SE state."""

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
from probe_th04_maine_sound_state import FRAMES, PRI, hits, sha  # noqa: E402

RESTORED = ROOT / ".analysis/reconstruction/diet-replay/v228-op-target-roundtrip/a/restored.bin"
RESTORED_SHA = "40a981a671657ea49c2f916058f27ab14ab53553f555f1be843b1c8e3e50695d"


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
        raise ValueError("pinned OP decoded target identity drift")
    mz = parse_mz(raw)
    if not mz.valid:
        raise ValueError("decoded OP target is not valid MZ")
    image = mz.program_image
    table = PRI + FRAMES
    if hits(image, table) != [0xFCFE]:
        raise ValueError(f"OP SE table occurrence drift: {hits(image, table)}")
    if image[0xFD80:0xFD82] != b"\xff\x00":
        raise ValueError("OP SE playing/frame initial bytes drift")
    ext = b"m26\0m86\0mmd\0"
    if hits(image, ext) != [0xFD34]:
        raise ValueError(f"OP sound extension strings drift: {hits(image, ext)}")
    output.mkdir(parents=True)
    receipt = {
        "schema_version": 1,
        "artifact": "th04-op",
        "evidence_class": "target-analysis",
        "restored_sha256": RESTORED_SHA,
        "priority_table_load_offset": "0xFCFE",
        "priority_table_size": len(table),
        "priority_table_sha256": sha(table),
        "se_initial_load_offset": "0xFD80",
        "se_initial_bytes": "ff00",
        "extensions_load_offset": "0xFD34",
        "extensions_sha256": sha(ext),
        "limit": "Only OP target bytes and unique occurrences are observed; product-link state ownership and runtime effects require separate checks.",
    }
    (output / "receipt.json").write_text(json.dumps(receipt, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"artifact": "th04-op", "table_offset": "0xFCFE",
                      "table_sha256": sha(table)}, sort_keys=True))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
