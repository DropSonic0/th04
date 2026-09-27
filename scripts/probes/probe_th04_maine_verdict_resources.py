#!/usr/bin/env python3
"""Attest the bounded MAINE verdict display data block in the decoded target."""

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
BLOCK_START = 0xEC4A
BLOCK_END = 0xED5C
BLOCK_SHA = "6d588142d2da59f6b695df4bf51f6e77ca1fc3ca36e206278ae2f5774324a8aa"


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
        raise ValueError("decoded MAINE MZ invalid")
    image = mz.program_image
    block = image[BLOCK_START:BLOCK_END]
    if hashlib.sha256(block).hexdigest() != BLOCK_SHA or image.count(block) != 1:
        raise ValueError("verdict display data block identity or uniqueness drift")
    labels = {
        0x0753: "　　　　　　　 腕前判定",
        0x076B: "難易度",
        0x0772: "最終得点",
        0x0784: "ボム使用回数",
        0x0791: "ゲーム達成率",
        0x07B8: "得点アイテム最高点率",
        0x07F5: "_ude.txt",
        0x080D: "処理落ちによる判定不可",
    }
    for offset, label in labels.items():
        expected = label.encode("cp932") + b"\0"
        actual = image[0xE530 + offset:0xE530 + offset + len(expected)]
        if actual != expected:
            raise ValueError(f"verdict label at 0E53:{offset:04X} drift")
    output.mkdir(parents=True)
    receipt = {
        "schema_version": 1,
        "artifact": "th04-maine",
        "evidence_class": "target-analysis",
        "restored_sha256": RESTORED_SHA,
        "load_offset": f"0x{BLOCK_START:X}",
        "block_size": len(block),
        "block_sha256": BLOCK_SHA,
        "semantic_label_checks": len(labels),
        "limit": "Target data observation only; individual data owners and product/runtime behavior need separate proof.",
    }
    (output / "receipt.json").write_text(json.dumps(receipt, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"load_offset": f"0x{BLOCK_START:X}", "block_size": len(block),
                      "labels": len(labels)}, sort_keys=True))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
