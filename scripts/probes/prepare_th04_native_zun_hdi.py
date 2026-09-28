#!/usr/bin/env python3
"""Place source-only packed ZUN.COM in a disposable pinned TH04 PC-98 HDI."""

from __future__ import annotations

import argparse
import json
from pathlib import Path
import tomllib

from prepare_th04_maine_diagnostic_hdi import (
    AUTOEXEC_PREFIX, STARTUP_COMMANDS, Fat12, sha, u16, u32,
)


ROOT = Path(__file__).resolve().parents[2]
PROBES = (ROOT / ".analysis/reconstruction/probes").resolve()
PRIVATE = (ROOT / ".analysis/runtime/candidates").resolve()


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--composite-receipt", required=True, type=Path)
    parser.add_argument("--packed-receipt", required=True, type=Path)
    parser.add_argument("--output-dir", required=True, type=Path)
    args = parser.parse_args()
    composite_path = args.composite_receipt.resolve()
    packed_path = args.packed_receipt.resolve()
    output = args.output_dir.resolve()
    if (not composite_path.is_relative_to(PROBES)
            or not packed_path.is_relative_to(PROBES)
            or composite_path.name != "receipt.json"
            or packed_path.name != "receipt.json"
            or not output.is_relative_to(PRIVATE) or output.exists()):
        parser.error("use private probe receipts and a new runtime output directory")
    composite = json.loads(composite_path.read_text(encoding="utf-8"))
    packed = json.loads(packed_path.read_text(encoding="utf-8"))
    if (composite["artifact"] != "th04-zun" or not composite["cold_equal"]
            or packed["artifact"] != "th04-zun"
            or not packed["candidate_inputs_identical"]
            or not packed["packed_outputs_identical"]):
        raise ValueError("expected cold source-only ZUN composite and packed candidate")
    flat_sha = composite["rounds"]["a"]["flat_sha256"]
    if any(build["candidate_sha256"] != flat_sha for build in packed["builds"]):
        raise ValueError("DIET input does not match source-only flat payload")
    candidate_path = packed_path.parent / "a/ZUN.COM"
    candidate = candidate_path.read_bytes()
    if (any(build["packed_sha256"] != sha(candidate) for build in packed["builds"])
            or candidate[:2] != b"MZ"):
        raise ValueError("packed ZUN candidate identity drift")
    from sys import path as sys_path
    sys_path.insert(0, str(ROOT / "scripts"))
    from lib.pc98 import parse_mz  # noqa: E402
    mz = parse_mz(candidate)
    if not mz.valid or mz.header.number_of_relocations or mz.overlay:
        raise ValueError("packed ZUN MZ structure or relocation table invalid")

    runtime = tomllib.loads((ROOT / "config/runtime.toml").read_text())
    source_image = ROOT / runtime["image"]["path"]
    original = source_image.read_bytes()
    if len(original) != runtime["image"]["size"] or sha(original) != runtime["image"]["sha256"]:
        raise ValueError("pinned source HDI identity drift")
    targets = tomllib.loads((ROOT / "config/targets.toml").read_text())
    target = next(item for item in targets["artifacts"] if item["id"] == "th04-zun")
    image = bytearray(original)
    fs = Fat12(image)
    genso = fs.find_entry([fs.root], b"GENSO      ")
    if not image[genso + 11] & 0x10:
        raise ValueError("GENSO is not a directory")
    directory = [fs.cluster_offset(k) for k in fs.chain(u16(image, genso + 26))]
    entry = fs.find_entry(directory, b"ZUN     COM")
    first = u16(image, entry + 26)
    old_size = u32(image, entry + 28)
    if old_size != target["size"] or sha(fs.file_bytes(first, old_size)) != target["sha256"]:
        raise ValueError("HDI ZUN.COM is not the pinned target")
    chain = fs.chain(first)
    if len(candidate) > len(chain) * fs.cluster_bytes:
        raise ValueError("source-only ZUN candidate exceeds original FAT chain")
    for index, cluster in enumerate(chain):
        start = fs.cluster_offset(cluster)
        block = candidate[index * fs.cluster_bytes:(index + 1) * fs.cluster_bytes]
        image[start:start + fs.cluster_bytes] = block.ljust(fs.cluster_bytes, b"\0")
    image[entry + 28:entry + 32] = len(candidate).to_bytes(4, "little")

    autoexec = AUTOEXEC_PREFIX + STARTUP_COMMANDS["game-bat"] + b"ECHO EXIT >> A:\\DIAG.TXT\r\n\x1a"
    auto_entry = fs.find_entry([fs.root], b"AUTOEXECBAT")
    auto_chain = fs.chain(u16(image, auto_entry + 26))
    if len(autoexec) > len(auto_chain) * fs.cluster_bytes:
        raise ValueError("diagnostic AUTOEXEC exceeds original FAT chain")
    for index, cluster in enumerate(auto_chain):
        start = fs.cluster_offset(cluster)
        block = autoexec[index * fs.cluster_bytes:(index + 1) * fs.cluster_bytes]
        image[start:start + fs.cluster_bytes] = block.ljust(fs.cluster_bytes, b"\0")
    image[auto_entry + 28:auto_entry + 32] = len(autoexec).to_bytes(4, "little")
    verified = Fat12(image)
    if (verified.file_bytes(first, len(candidate)) != candidate
            or verified.file_bytes(u16(image, auto_entry + 26), len(autoexec)) != autoexec
            or sha(source_image.read_bytes()) != runtime["image"]["sha256"]):
        raise ValueError("disposable HDI readback or pinned-source identity failed")
    output.mkdir(parents=True)
    (output / "diagnostic.hdi").write_bytes(image)
    receipt = {
        "schema_version": 1,
        "scope": "private source-only ZUN packed-MZ diagnostic boot image",
        "artifact": "th04-zun",
        "artifact_source": "TH04-only native packed ZUN",
        "startup": "game-bat",
        "composite_receipt_sha256": sha(composite_path.read_bytes()),
        "packed_receipt_sha256": sha(packed_path.read_bytes()),
        "candidate_artifact_sha256": sha(candidate),
        "candidate_artifact_size": len(candidate),
        "original_hdi_sha256": runtime["image"]["sha256"],
        "original_artifact_sha256": target["sha256"],
        "diagnostic_hdi_sha256": sha(image),
        "mz_relocations": mz.header.number_of_relocations,
        "limit": "Disposable FAT12 image only; no PC-98 execution or runtime acceptance.",
    }
    (output / "receipt.json").write_text(json.dumps(receipt, indent=2) + "\n",
                                          encoding="utf-8")
    print(json.dumps({"candidate_size": len(candidate), "mz_valid": mz.valid,
                      "relocations": mz.header.number_of_relocations,
                      "hdi_sha256": sha(image)}, sort_keys=True))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
