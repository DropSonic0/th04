#!/usr/bin/env python3
"""Validate TH04's pinned PAR archives and extract private decoder fixtures."""

from __future__ import annotations

import argparse
from collections import Counter
from datetime import datetime, timezone
import hashlib
import json
from pathlib import Path
import sys
import tomllib


ROOT = Path(__file__).resolve().parents[2]
PRIVATE = (ROOT / ".analysis/reconstruction/probes").resolve()
sys.path.insert(0, str(ROOT))
from scripts.probes.prepare_th04_maine_diagnostic_hdi import Fat12  # noqa: E402


ARCHIVES = {
    "op_end": {
        "fat_name": bytes.fromhex("8cb6917a8bbd4544444154"),
        "size": 1120284,
        "sha256": "ca787b8ff66f7b3f10c97b3ecc77cd466772767e3e9e8cf5a0c71dd612b1c8d7",
        "entries": 132,
        "expected_overruns": {"SCNUM2.BFT"},
    },
    "main": {
        "fat_name": bytes.fromhex("938c95fb8cb6917a8bbd20"),
        "size": 1053177,
        "sha256": "4d975850a66b4ec2ca5fd4fba7f3166dd9293d220ba8ab6c1be87ed98b1399be",
        "entries": 158,
        "expected_overruns": {"ST00.BFT", "EYE1.CDG", "EYE4.CDG", "MIKO.EFC"},
    },
}
DEFAULT_FIXTURES = (("op_end", "GAMEFT.BFT"),
                    ("op_end", "CONG10.PI"),
                    ("op_end", "CONG14.PI"),
                    ("main", "ST00.BFT"))


def digest(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def u16(data: bytes, offset: int) -> int:
    return int.from_bytes(data[offset:offset + 2], "little")


def u32(data: bytes, offset: int) -> int:
    return int.from_bytes(data[offset:offset + 4], "little")


def decode_member(blob: bytes, compression: int, aux: int) -> bytes:
    # The TH04 payload cipher XORs each byte with its entry's fixed aux key.
    physical = bytes(value ^ aux for value in blob)
    if compression == 0xF388:
        return physical
    if compression != 0x9595:
        raise ValueError(f"unknown PAR compression 0x{compression:04X}")

    # Two equal literal bytes introduce a third byte giving the number of
    # additional repeats. The second literal itself is part of the output.
    output = bytearray()
    previous = -1
    cursor = 0
    while cursor < len(physical):
        value = physical[cursor]
        cursor += 1
        output.append(value)
        if value == previous:
            if cursor >= len(physical):
                raise ValueError("PAR repeat marker lacks a count")
            count = physical[cursor]
            cursor += 1
            output.extend(bytes((value,)) * count)
        previous = value
    return bytes(output)


def parse_archive(blob: bytes, archive_id: str, expected: dict) -> tuple[dict, dict[str, bytes]]:
    if len(blob) < 16:
        raise ValueError("short PAR header")
    entries_size = u16(blob, 0)
    unknown = u16(blob, 2)
    entries_count = u16(blob, 4)
    initial_key = u16(blob, 6)
    if (entries_count != expected["entries"] or entries_size != (entries_count + 1) * 32
            or 16 + entries_size > len(blob) or blob[8:16] != bytes(8)
            or initial_key > 0xFF):
        raise ValueError(f"{archive_id}: unexpected PAR header")

    directory = bytearray(blob[16:16 + entries_size])
    key = initial_key
    for index, encoded in enumerate(directory):
        value = encoded ^ key
        directory[index] = value
        key = (key - value) & 0xFF
    if directory[entries_count * 32:] != bytes(32):
        raise ValueError(f"{archive_id}: PAR terminal entry changed")

    records = []
    files = {}
    types = Counter()
    extensions = Counter()
    overruns = set()
    for index in range(entries_count):
        entry = directory[index * 32:(index + 1) * 32]
        compression = u16(entry, 0)
        aux = entry[2]
        raw_name = bytes(entry[3:16]).split(b"\x00", 1)[0]
        name = raw_name.decode("cp932")
        packed_size = u16(entry, 16)
        original_size = u16(entry, 18)
        offset = u32(entry, 20)
        if (not name or name in files or "/" in name or "\\" in name or ":" in name
                or offset < 16 + entries_size or offset + packed_size > len(blob)):
            raise ValueError(f"{archive_id}: invalid member {index}: {name!r}")
        decoded = decode_member(blob[offset:offset + packed_size], compression, aux)
        overrun = len(decoded) - original_size
        if overrun not in (0, 1):
            raise ValueError(f"{archive_id}/{name}: decoded length delta {overrun}")
        if overrun:
            overruns.add(name)
        logical = decoded[:original_size]
        extension = name.rsplit(".", 1)[-1].upper()
        if extension == "PI" and not logical.startswith(b"Pi"):
            raise ValueError(f"{archive_id}/{name}: PI magic mismatch")
        if extension == "BFT" and not logical.startswith(b"BFNT\x1A"):
            raise ValueError(f"{archive_id}/{name}: BFNT magic mismatch")
        types[f"0x{compression:04X}"] += 1
        extensions[extension] += 1
        files[name] = logical
        records.append({
            "name": name,
            "type": f"0x{compression:04X}",
            "aux": aux,
            "packed_size": packed_size,
            "declared_size": original_size,
            "expanded_size": len(decoded),
            "offset": offset,
            "logical_sha256": digest(logical),
        })
    if overruns != expected["expected_overruns"]:
        raise ValueError(f"{archive_id}: PAR overrun set changed: {sorted(overruns)}")
    return ({
        "id": archive_id,
        "fat_name_hex": expected["fat_name"].hex(),
        "archive_sha256": digest(blob),
        "archive_size": len(blob),
        "header_unknown": unknown,
        "directory_key": initial_key,
        "directory_sha256": digest(directory),
        "entry_count": entries_count,
        "type_counts": dict(sorted(types.items())),
        "extension_counts": dict(sorted(extensions.items())),
        "one_byte_overruns": sorted(overruns),
        "members": records,
    }, files)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output-dir", type=Path, required=True)
    parser.add_argument("--extract", action="append", default=[], metavar="ARCHIVE:MEMBER",
                        help="extract one named member; repeat for multiple members")
    args = parser.parse_args()
    output = args.output_dir.resolve()
    if output.exists() or not output.is_relative_to(PRIVATE):
        parser.error("use a new private .analysis/reconstruction/probes output directory")

    runtime = tomllib.loads((ROOT / "config/runtime.toml").read_text(encoding="utf-8"))
    image_path = ROOT / runtime["image"]["path"]
    image = image_path.read_bytes()
    image_sha256 = digest(image)
    if (len(image) != runtime["image"]["size"]
            or image_sha256 != runtime["image"]["sha256"]):
        raise ValueError("pinned HDI identity drift")
    fat = Fat12(bytearray(image))  # Also checks geometry and both FAT mirrors.
    genso = fat.find_entry([fat.root], b"GENSO      ")
    first = u16(fat.image, genso + 26)
    directory_offsets = [fat.cluster_offset(cluster) for cluster in fat.chain(first)]

    archives = []
    decoded_by_archive = {}
    for archive_id, expected in ARCHIVES.items():
        offset = fat.find_entry(directory_offsets, expected["fat_name"])
        first = u16(fat.image, offset + 26)
        size = u32(fat.image, offset + 28)
        blob = fat.file_bytes(first, size)
        if size != expected["size"] or digest(blob) != expected["sha256"]:
            raise ValueError(f"{archive_id}: pinned archive identity drift")
        archive_receipt, decoded = parse_archive(blob, archive_id, expected)
        archives.append(archive_receipt)
        decoded_by_archive[archive_id] = decoded

    requested = []
    for value in args.extract:
        archive_id, separator, name = value.partition(":")
        if not separator or archive_id not in decoded_by_archive or name not in decoded_by_archive[archive_id]:
            parser.error(f"unknown archive member: {value}")
        requested.append((archive_id, name))
    if not requested:
        requested = list(DEFAULT_FIXTURES)
    output.mkdir(parents=True)
    fixtures = output / "fixtures"
    fixtures.mkdir()
    extracted = []
    for archive_id, name in dict.fromkeys(requested):
        data = decoded_by_archive[archive_id][name]
        path = fixtures / f"{archive_id}-{name}"
        path.write_bytes(data)
        extracted.append({"archive": archive_id, "name": name,
                          "relative_path": str(path.relative_to(output)),
                          "size": len(data), "sha256": digest(data)})

    receipt = {
        "schema_version": 1,
        "observed_utc": datetime.now(timezone.utc).isoformat(timespec="seconds"),
        "scope": "pinned TH04 GENSO FAT12 archive format and private fixtures",
        "image_sha256": image_sha256,
        "probe_sha256": digest(Path(__file__).read_bytes()),
        "fat_reader_sha256": digest((ROOT / "scripts/probes/prepare_th04_maine_diagnostic_hdi.py").read_bytes()),
        "archives": archives,
        "extracted": extracted,
        "limit": "Host decoder is an archive-format oracle, not a native PFSTART/PFEND or DOS INT 21h runtime acceptance.",
    }
    (output / "receipt.json").write_text(json.dumps(receipt, indent=2, sort_keys=True) + "\n",
                                         encoding="utf-8")
    print(json.dumps({"archives": {archive["id"]: archive["entry_count"] for archive in archives},
                      "one_byte_overruns": {archive["id"]: archive["one_byte_overruns"]
                                            for archive in archives},
                      "fixtures": [entry["relative_path"] for entry in extracted]}, sort_keys=True))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
