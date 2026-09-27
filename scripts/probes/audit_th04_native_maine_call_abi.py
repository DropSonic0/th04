#!/usr/bin/env python3
"""Check MAINE far calls, returns, and relocation sites for local ASM owners.

TLINK can resolve a far call to a symbol whose implementation uses a near
return. This gate catches that invalid ABI even when the MZ structure passes.
"""

from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path
import re
import shutil
import subprocess
import tomllib


ROOT = Path(__file__).resolve().parents[2]
PROBES = (ROOT / ".analysis/reconstruction/probes").resolve()
RET_POP = {
    "FILE_APPEND": 4,
    "FILE_CLOSE": 0,
    "FILE_CREATE": 4,
    "FILE_READ": 6,
    "FILE_ROPEN": 4,
    "FILE_SEEK": 6,
    "FILE_SIZE": 0,
    "FILE_WRITE": 6,
    "GRAPH_400LINE": 0,
    "GRAPH_CLEAR": 0,
    "GRAPH_PI_FREE": 8,
    "GRAPH_START": 0,
    "GRAPH_SCROLLUP": 2,
    "GRCG_BYTEBOXFILL_X": 8,
    "PALETTE_INIT": 0,
}


def sha(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def u16(data: bytes, offset: int) -> int:
    return int.from_bytes(data[offset:offset + 2], "little")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--link-receipt", required=True, type=Path)
    parser.add_argument("--output-dir", required=True, type=Path)
    args = parser.parse_args()
    link_path = args.link_receipt.resolve()
    output = args.output_dir.resolve()
    if (not link_path.is_relative_to(PROBES) or link_path.name != "receipt.json"
            or not output.is_relative_to(PROBES) or output.exists()):
        parser.error("use a private link receipt and a new private output directory")
    link = json.loads(link_path.read_text(encoding="utf-8"))
    if not link["link_complete"] or not link["support_lib_sha256"]:
        raise ValueError("requires a complete historical-library calibration MZ")
    base = link_path.parent / "source"
    exe = (base / "bin/maine-native.exe").read_bytes()
    map_bytes = (base / "obj/product/maine-native.map").read_bytes()
    if sha(exe) != link["mz_header"]["sha256"] or exe[:2] != b"MZ":
        raise ValueError("calibration MZ identity drift")
    header_size = u16(exe, 8) * 16
    body = exe[header_size:]
    reloc_count = u16(exe, 6)
    reloc_offset = u16(exe, 0x18)
    if reloc_offset + reloc_count * 4 > header_size:
        raise ValueError("MZ relocation table exceeds header")
    reloc_sites = {
        u16(exe, reloc_offset + i * 4) + 16 * u16(exe, reloc_offset + i * 4 + 2)
        for i in range(reloc_count)
    }
    publics = {}
    code_ranges: dict[int, list[tuple[int, int]]] = {}
    for line in map_bytes.decode("ascii", errors="replace").splitlines():
        contribution = re.match(r"^\s*([0-9A-F]{4}):([0-9A-F]{4})\s+([0-9A-F]{4})\s+C=CODE\b", line)
        if contribution:
            segment = int(contribution[1], 16)
            start = int(contribution[2], 16)
            length = int(contribution[3], 16)
            code_ranges.setdefault(segment, []).append((start, start + length))
        match = re.match(r"^\s*([0-9A-F]{4}):([0-9A-F]{4})\s+(?:idle\s+)?([A-Z][A-Z0-9_]+)\s*$", line)
        if match:
            address = (int(match[1], 16), int(match[2], 16))
            prior = publics.setdefault(match[3], address)
            if prior != address:
                raise ValueError(f"MAP public has multiple addresses: {match[3]}")
    ndisasm = shutil.which("ndisasm")
    if ndisasm is None:
        raise ValueError("ndisasm is required for instruction boundary checks")
    manifest = tomllib.loads((ROOT / "config/native_maine_sources.toml").read_text(encoding="utf-8"))
    for relative in manifest["asm_sources"]:
        source = (ROOT / relative).read_text(encoding="utf-8")
        for found in re.finditer(r"^\s*([A-Za-z_][\w@$?]*)\s+proc\s+near\b", source,
                                 flags=re.MULTILINE | re.IGNORECASE):
            name = found[1].upper()
            if name not in publics:
                continue
            segment, offset = publics[name]
            call_bytes = b"\x9a" + offset.to_bytes(2, "little") + segment.to_bytes(2, "little")
            if call_bytes in body and name not in RET_POP:
                raise ValueError(f"far call to unaudited near-source ASM public: {name}")
    rows = []
    for name, expected_pop in RET_POP.items():
        if name not in publics:
            raise ValueError(f"missing MAINE ASM public: {name}")
        segment, offset = publics[name]
        start = segment * 16 + offset
        if start >= len(body):
            raise ValueError(f"public outside load image: {name}")
        proc = subprocess.run([ndisasm, "-b16", "-o", hex(offset), "-"],
                              input=body[start:start + 256], capture_output=True,
                              check=True, timeout=10).stdout.decode("ascii")
        first_return = None
        for line in proc.splitlines():
            instruction = re.match(r"^[0-9A-F]+\s+[0-9A-F]+\s+(.+)$", line)
            if instruction and re.match(r"^retf?(?:\s|$)", instruction[1]):
                first_return = instruction[1].strip()
                break
        expected = "retf" if expected_pop == 0 else f"retf 0x{expected_pop:x}"
        if first_return != expected:
            raise ValueError(f"{name} first return is {first_return!r}, expected {expected!r}")
        call_bytes = b"\x9a" + offset.to_bytes(2, "little") + segment.to_bytes(2, "little")
        calls = []
        cursor = 0
        while (site := body.find(call_bytes, cursor)) != -1:
            calls.append(site)
            cursor = site + 1
        if any(site + 3 not in reloc_sites for site in calls):
            raise ValueError(f"{name} has an unrelocated far-call segment")
        # TC4J can implement a far call within CS as PUSH CS; CALL rel16.
        # Its far callee still needs RETF, but the near displacement needs no
        # MZ relocation because CS supplies the segment at run time.
        cs_push_calls = []
        base = segment * 16
        for lower, upper in code_ranges.get(segment, []):
            for local in range(lower, upper - 3):
                site = base + local
                if body[site:site + 2] != b"\x0e\xe8":
                    continue
                displacement = int.from_bytes(body[site + 2:site + 4], "little", signed=True)
                if (local + 4 + displacement) & 0xFFFF == offset:
                    cs_push_calls.append(site)
        if not calls and not cs_push_calls:
            raise ValueError(f"{name} has no far or CS-pushed same-segment calls")
        rows.append({"public": name, "segment": segment, "offset": offset,
                     "first_return": first_return, "far_call_sites": calls,
                     "cs_push_near_call_sites": cs_push_calls})
    output.mkdir(parents=True)
    receipt = {
        "schema_version": 1,
        "scope": "MAINE calibration ASM call distance and MZ relocation sites",
        "link_receipt_sha256": sha(link_path.read_bytes()),
        "mz_sha256": sha(exe),
        "map_sha256": sha(map_bytes),
        "ndisasm_sha256": sha(Path(ndisasm).read_bytes()),
        "total_far_calls": sum(len(row["far_call_sites"]) for row in rows),
        "total_cs_push_near_calls": sum(len(row["cs_push_near_call_sites"]) for row in rows),
        "functions": rows,
        "limit": "Checks call/return distance, stack cleanup, and relocation presence; runtime behavior and argument values remain unobserved.",
    }
    (output / "receipt.json").write_text(json.dumps(receipt, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"functions": len(rows), "far_calls": receipt["total_far_calls"],
                      "cs_push_near_calls": receipt["total_cs_push_near_calls"],
                      "relocations": reloc_count}, sort_keys=True))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
