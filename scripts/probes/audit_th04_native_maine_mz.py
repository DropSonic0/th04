#!/usr/bin/env python3
"""Audit a successful native MAINE MZ at two DOS load segments."""

from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
import sys
sys.path.insert(0, str(ROOT / "scripts"))
from lib.pc98 import parse_mz  # noqa: E402


def sha(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--link-receipt", type=Path, required=True)
    parser.add_argument("--output-dir", type=Path, required=True)
    args = parser.parse_args()
    private = (ROOT / ".analysis/reconstruction/probes").resolve()
    source_receipt = args.link_receipt.resolve()
    output = args.output_dir.resolve()
    if (not source_receipt.is_relative_to(private) or source_receipt.name != "receipt.json"
            or output.exists() or not output.is_relative_to(private)):
        parser.error("receipt and new output must be below .analysis/reconstruction/probes")
    link = json.loads(source_receipt.read_text(encoding="utf-8"))
    if not link["link_complete"]:
        raise ValueError("expected a successful native MAINE link")
    exe = source_receipt.parent / "source/bin/maine-native.exe"
    raw = exe.read_bytes()
    if sha(raw) != link["mz_header"]["sha256"]:
        raise ValueError("native MZ identity drift")
    mz = parse_mz(raw)
    if not mz.valid or len(mz.relocations) != link["mz_header"]["relocations"]:
        raise ValueError("MZ format or relocation count drift")
    header = mz.header
    image = mz.program_image
    image_paragraphs = ((len(image) + 15) // 16)
    sites = sorted((rel.segment * 16 + rel.offset) for rel in mz.relocations)
    if (len(set(sites)) != len(sites) or any(site + 2 > len(image) for site in sites)
            or any(right < left + 2 for left, right in zip(sites, sites[1:]))):
        raise ValueError("duplicate, overlapping, or out-of-image relocation site")
    values = [int.from_bytes(image[site:site + 2], "little") for site in sites]
    if any(value >= image_paragraphs for value in values):
        raise ValueError("relocation target segment falls outside load image")
    if mz.relocation_table_end > (header.header_paragraphs * 16):
        raise ValueError("relocation table extends beyond MZ header")
    if (header.initial_relative_cs * 16 + header.initial_ip) >= len(image):
        raise ValueError("entry CS:IP falls outside load image")
    stack_top = header.initial_relative_ss * 16 + header.initial_sp
    minimum_allocated = (image_paragraphs + header.minimum_extra_allocation) * 16
    if stack_top > minimum_allocated:
        raise ValueError("initial stack exceeds minimum allocated image")
    load_segments = (0x2000, 0x6000)
    for load_segment in load_segments:
        if any(load_segment + value > 0xFFFF for value in values):
            raise ValueError("relocation would wrap a DOS segment word")
        if (load_segment + header.initial_relative_cs > 0xFFFF
                or load_segment + header.initial_relative_ss > 0xFFFF):
            raise ValueError("entry or stack segment would wrap")

    output.mkdir(parents=True)
    receipt = {
        "schema_version": 1,
        "artifact": "th04-maine",
        "evidence_class": "static-relocation",
        "historical_support_library": bool(link["support_lib_sha256"]),
        "link_receipt_sha256": sha(source_receipt.read_bytes()),
        "mz_sha256": sha(raw),
        "image_bytes": len(image),
        "image_paragraphs": image_paragraphs,
        "relocation_count": len(sites),
        "relocation_sites_unique_nonoverlapping": True,
        "relocation_target_segments": sorted(set(values)),
        "entry_cs_ip": [header.initial_relative_cs, header.initial_ip],
        "stack_ss_sp": [header.initial_relative_ss, header.initial_sp],
        "stack_top_bytes": stack_top,
        "minimum_allocated_bytes": minimum_allocated,
        "load_segments_checked": list(load_segments),
        "limit": "Static MZ structure only; PC-98 emulator execution and behavioral acceptance remain separate.",
    }
    (output / "receipt.json").write_text(json.dumps(receipt, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"relocations": len(sites), "target_segments": len(set(values)),
                      "load_segments": list(load_segments)}, sort_keys=True))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
