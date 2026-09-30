#!/usr/bin/env python3
"""Prepare a native MAIN candidate in a disposable PC-98 HDI.

The pinned Japanese image remains read-only.  The candidate is copied into a
private FAT12 image and the AUTOEXEC command is replaced for a diagnostic boot
only; this is not a product build, exactness claim, or runtime acceptance.
"""

from __future__ import annotations

import argparse
import json
from pathlib import Path
import tomllib

from prepare_th04_maine_diagnostic_hdi import (
    AUTOEXEC_PREFIX,
    Fat12,
    STARTUP_COMMANDS,
    ROOT,
    PRIVATE,
    sha,
    u16,
    u32,
)


MAIN_STARTUP_COMMANDS = {
    "game-bat": STARTUP_COMMANDS["game-bat"],
    "direct-main": b"MAIN.EXE\r\n",
}


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--link-receipt", type=Path, required=True)
    parser.add_argument("--startup", choices=MAIN_STARTUP_COMMANDS, default="game-bat")
    parser.add_argument("--output-dir", type=Path, required=True)
    args = parser.parse_args()

    output = args.output_dir.resolve()
    probes = (ROOT / ".analysis/reconstruction/probes").resolve()
    if output.exists() or not output.is_relative_to(PRIVATE):
        parser.error("use a new private output directory")
    link_path = args.link_receipt.resolve()
    if (not link_path.is_relative_to(probes)
            or link_path.name != "receipt.json"):
        parser.error("MAIN link receipt must be a private probe receipt")

    link = json.loads(link_path.read_text(encoding="utf-8"))
    link_meta = link.get("link", {})
    mz_meta = link_meta.get("mz", {})
    if (link.get("artifact") != "th04-main"
            or link_meta.get("exit") != 0
            or link_meta.get("errors")
            or link_meta.get("undefined_symbols")
            or link_meta.get("duplicate_errors")
            or link_meta.get("fixup_overflows")
            or link_meta.get("group_overflows")
            or not mz_meta.get("sha256")
            or not mz_meta.get("size")):
        raise ValueError("expected a complete native MAIN link receipt")

    candidate_path = link_path.parent / "source/bin/main-native.exe"
    candidate = candidate_path.read_bytes()
    candidate_sha = sha(candidate)
    if candidate_sha != mz_meta["sha256"] or len(candidate) != mz_meta["size"]:
        raise ValueError("native MAIN candidate does not match its link receipt")

    runtime = tomllib.loads((ROOT / "config/runtime.toml").read_text(encoding="utf-8"))
    image_path = ROOT / runtime["image"]["path"]
    original = image_path.read_bytes()
    if (len(original) != runtime["image"]["size"]
            or sha(original) != runtime["image"]["sha256"]):
        raise ValueError("pinned original HDI identity drift")
    targets = tomllib.loads((ROOT / "config/targets.toml").read_text(encoding="utf-8"))
    target = next(item for item in targets["artifacts"] if item["id"] == "th04-main")

    image = bytearray(original)
    fs = Fat12(image)
    genso = fs.find_entry([fs.root], b"GENSO      ")
    if not image[genso + 11] & 0x10:
        raise ValueError("GENSO is not a directory")
    genso_offsets = [fs.cluster_offset(k) for k in fs.chain(u16(image, genso + 26))]
    product_entry = fs.find_entry(genso_offsets, b"MAIN    EXE")
    old_first = u16(image, product_entry + 26)
    old_size = u32(image, product_entry + 28)
    old_chain = fs.chain(old_first)
    if (old_size != target["size"]
            or sha(fs.file_bytes(old_first, old_size)) != target["sha256"]):
        raise ValueError("HDI MAIN.EXE is not the pinned TH04 target")

    required = (len(candidate) + fs.cluster_bytes - 1) // fs.cluster_bytes
    extra = required - len(old_chain)
    if extra < 0:
        raise ValueError("candidate MAIN unexpectedly smaller than packed target")
    available = [k for k in range(2, fs.max_cluster + 1) if fs.fat(k) == 0]
    if len(available) < extra:
        raise ValueError("insufficient free FAT12 clusters")
    new_chain = old_chain + available[:extra]
    for current, following in zip(new_chain, new_chain[1:]):
        fs.set_fat(current, following)
    fs.set_fat(new_chain[-1], 0xFFF)
    for index, cluster in enumerate(new_chain):
        offset = fs.cluster_offset(cluster)
        block = candidate[index * fs.cluster_bytes:(index + 1) * fs.cluster_bytes]
        image[offset:offset + fs.cluster_bytes] = block.ljust(fs.cluster_bytes, b"\0")
    image[product_entry + 28:product_entry + 32] = len(candidate).to_bytes(4, "little")

    autoexec_bytes = (AUTOEXEC_PREFIX + MAIN_STARTUP_COMMANDS[args.startup]
                      + b"ECHO EXIT >> A:\\DIAG.TXT\r\n\x1a")
    autoexec = fs.find_entry([fs.root], b"AUTOEXECBAT")
    auto_chain = fs.chain(u16(image, autoexec + 26))
    if len(autoexec_bytes) > len(auto_chain) * fs.cluster_bytes:
        raise ValueError("AUTOEXEC diagnostic command exceeds allocated chain")
    for index, cluster in enumerate(auto_chain):
        offset = fs.cluster_offset(cluster)
        block = autoexec_bytes[index * fs.cluster_bytes:(index + 1) * fs.cluster_bytes]
        image[offset:offset + fs.cluster_bytes] = block.ljust(fs.cluster_bytes, b"\0")
    image[autoexec + 28:autoexec + 32] = len(autoexec_bytes).to_bytes(4, "little")

    verified = Fat12(image)
    if (sha(verified.file_bytes(old_first, len(candidate))) != candidate_sha
            or verified.file_bytes(u16(image, autoexec + 26), len(autoexec_bytes))
            != autoexec_bytes):
        raise ValueError("disposable HDI write did not read back")
    if sha(image_path.read_bytes()) != runtime["image"]["sha256"]:
        raise ValueError("source image changed during preparation")

    output.mkdir(parents=True)
    image_out = output / "diagnostic.hdi"
    image_out.write_bytes(image)
    receipt = {
        "schema_version": 1,
        "scope": "private diagnostic native MAIN boot image, not product acceptance",
        "artifact": "th04-main",
        "original_hdi_sha256": runtime["image"]["sha256"],
        "original_artifact_sha256": target["sha256"],
        "artifact_source": "TH04-only native MAIN diagnostic link",
        "startup": args.startup,
        "link_receipt_sha256": sha(link_path.read_bytes()),
        "candidate_artifact_sha256": candidate_sha,
        "candidate_artifact_size": len(candidate),
        "candidate_mz": mz_meta,
        "original_artifact_chain": old_chain,
        "candidate_artifact_chain": new_chain,
        "autoexec_sha256": sha(autoexec_bytes),
        "diagnostic_hdi_sha256": sha(image),
        "limit": "Disposable FAT12 image only; no PC-98 execution or behavioral evidence.",
    }
    (output / "receipt.json").write_text(
        json.dumps(receipt, indent=2) + "\n", encoding="utf-8"
    )
    print(json.dumps({
        "image": str(image_out),
        "candidate_size": len(candidate),
        "clusters": len(new_chain),
        "sha256": receipt["diagnostic_hdi_sha256"],
    }, sort_keys=True))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
