#!/usr/bin/env python3
"""Add reversible GAME.BAT phase markers to an attested private OP HDI.

Only the disposable boot image is changed. The five normal OP invocations
remain in place, with DOS ECHO markers immediately before and after each one.
"""

from __future__ import annotations

import argparse
import json
from pathlib import Path

from prepare_th04_maine_diagnostic_hdi import Fat12, sha, u16, u32


ROOT = Path(__file__).resolve().parents[2]
PRIVATE = (ROOT / ".analysis/runtime/candidates").resolve()


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--prepared-dir", required=True, type=Path)
    parser.add_argument("--output-dir", required=True, type=Path)
    args = parser.parse_args()
    source = args.prepared_dir.resolve()
    output = args.output_dir.resolve()
    if (not source.is_relative_to(PRIVATE) or not output.is_relative_to(PRIVATE)
            or output.exists() or source == output):
        parser.error("use separate private prepared and new output directories")
    prior = json.loads((source / "receipt.json").read_text(encoding="utf-8"))
    if prior["artifact"] != "th04-op":
        raise ValueError("phase markers require a prepared native OP image")
    input_image = source / "diagnostic.hdi"
    original = input_image.read_bytes()
    if sha(original) != prior["diagnostic_hdi_sha256"]:
        raise ValueError("prepared HDI identity drift")
    image = bytearray(original)
    fs = Fat12(image)
    genso = fs.find_entry([fs.root], b"GENSO      ")
    if not image[genso + 11] & 0x10:
        raise ValueError("GENSO is not a directory")
    directory = [fs.cluster_offset(k) for k in fs.chain(u16(image, genso + 26))]
    entry = fs.find_entry(directory, b"GAME    BAT")
    first = u16(image, entry + 26)
    old_size = u32(image, entry + 28)
    old = fs.file_bytes(first, old_size)
    pattern = b"\r\nop\r\n"
    if old.count(pattern) != 5:
        raise ValueError("expected exactly five normal OP batch invocations")
    replacement = (b"\r\nECHO OP_BEFORE >> A:\\DIAG.TXT\r\n"
                   b"op\r\nECHO OP_AFTER >> A:\\DIAG.TXT\r\n")
    updated = old.replace(pattern, replacement)
    chain = fs.chain(first)
    if len(updated) > len(chain) * fs.cluster_bytes:
        raise ValueError("instrumented GAME.BAT exceeds its allocated FAT chain")
    for index, cluster in enumerate(chain):
        start = fs.cluster_offset(cluster)
        block = updated[index * fs.cluster_bytes:(index + 1) * fs.cluster_bytes]
        image[start:start + fs.cluster_bytes] = block.ljust(fs.cluster_bytes, b"\0")
    image[entry + 28:entry + 32] = len(updated).to_bytes(4, "little")
    if Fat12(image).file_bytes(first, len(updated)) != updated:
        raise ValueError("instrumented GAME.BAT did not read back")
    if sha(input_image.read_bytes()) != prior["diagnostic_hdi_sha256"]:
        raise ValueError("prepared input image changed")
    output.mkdir(parents=True)
    (output / "diagnostic.hdi").write_bytes(image)
    receipt = dict(prior)
    receipt.update({
        "scope": "private diagnostic OP boot image with reversible GAME.BAT phase markers",
        "source_prepared_receipt_sha256": sha((source / "receipt.json").read_bytes()),
        "source_hdi_sha256": prior["diagnostic_hdi_sha256"],
        "original_game_bat_sha256": sha(old),
        "instrumented_game_bat_sha256": sha(updated),
        "game_bat_op_invocations": 5,
        "diagnostic_hdi_sha256": sha(image),
        "limit": "Disposable GAME.BAT instrumentation; runtime control only, not product acceptance.",
    })
    (output / "receipt.json").write_text(json.dumps(receipt, indent=2) + "\n",
                                          encoding="utf-8")
    print(json.dumps({"image_sha256": sha(image), "op_invocations": 5,
                      "game_bat_size": len(updated)}, sort_keys=True))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
