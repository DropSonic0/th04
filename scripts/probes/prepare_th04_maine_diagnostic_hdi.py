#!/usr/bin/env python3
"""Prepare paired original/calibration MAINE probes in disposable PC-98 HDIs.

The source image and original target remain read-only. The output is a private
diagnostic boot image, never a TH04-owned product or runtime acceptance.
"""

from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path
import tomllib

ROOT = Path(__file__).resolve().parents[2]
PRIVATE = (ROOT / ".analysis/runtime/candidates").resolve()
PARTITION_OFFSET = 0x9800
AUTOEXEC_PREFIX = (
    b"@ECHO OFF\r\nPATH A:\\DOS;A:\\\r\nSET TEMP=A:\\DOS\r\n"
    b"SET DOSDIR=A:\\DOS\r\nCD \\GENSO\r\n"
    b"ECHO START > A:\\DIAG.TXT\r\n"
)
STARTUP_COMMANDS = {
    "game-bat": b"CALL GAME.BAT\r\n",
    # Negative control: the original MAINE faults when invoked without OP.
    "direct-maine": b"MAINE.EXE\r\n",
}


def sha(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def u16(data: bytes, offset: int) -> int:
    return int.from_bytes(data[offset:offset + 2], "little")


def u32(data: bytes, offset: int) -> int:
    return int.from_bytes(data[offset:offset + 4], "little")


class Fat12:
    def __init__(self, image: bytearray):
        self.image = image
        p = PARTITION_OFFSET
        self.sector = u16(image, p + 11)
        self.cluster_sectors = image[p + 13]
        self.reserved = u16(image, p + 14)
        self.fat_count = image[p + 16]
        self.root_entries = u16(image, p + 17)
        self.total_sectors = u16(image, p + 19)
        self.fat_sectors = u16(image, p + 22)
        if ((self.sector, self.cluster_sectors, self.reserved, self.fat_count,
             self.root_entries, self.total_sectors, self.fat_sectors)
                != (1024, 8, 1, 2, 1536, 20706, 4)):
            raise ValueError("unexpected pinned PC-98 FAT12 geometry")
        self.cluster_bytes = self.sector * self.cluster_sectors
        self.fat_offsets = [p + (self.reserved + i * self.fat_sectors) * self.sector
                            for i in range(self.fat_count)]
        self.root = p + (self.reserved + self.fat_count * self.fat_sectors) * self.sector
        root_sectors = (self.root_entries * 32 + self.sector - 1) // self.sector
        self.data = self.root + root_sectors * self.sector
        data_sectors = self.total_sectors - self.reserved - self.fat_count * self.fat_sectors - root_sectors
        self.max_cluster = 1 + data_sectors // self.cluster_sectors
        if p + self.total_sectors * self.sector > len(image):
            raise ValueError("partition extends past pinned HDI")
        size = self.fat_sectors * self.sector
        if image[self.fat_offsets[0]:self.fat_offsets[0] + size] != image[self.fat_offsets[1]:self.fat_offsets[1] + size]:
            raise ValueError("FAT mirrors disagree")

    def fat(self, cluster: int) -> int:
        if not 2 <= cluster <= self.max_cluster:
            raise ValueError(f"cluster out of range: {cluster}")
        offset = self.fat_offsets[0] + cluster + cluster // 2
        word = u16(self.image, offset)
        return ((word >> 4) if cluster & 1 else word) & 0xFFF

    def set_fat(self, cluster: int, value: int) -> None:
        if not 2 <= cluster <= self.max_cluster or not 0 <= value <= 0xFFF:
            raise ValueError("invalid FAT12 assignment")
        for base in self.fat_offsets:
            offset = base + cluster + cluster // 2
            word = u16(self.image, offset)
            word = ((word & 0x000F) | (value << 4)) if cluster & 1 else ((word & 0xF000) | value)
            self.image[offset:offset + 2] = word.to_bytes(2, "little")

    def chain(self, first: int) -> list[int]:
        result = []
        cluster = first
        while 2 <= cluster < 0xFF8:
            if cluster in result or cluster > self.max_cluster:
                raise ValueError("cyclic or out-of-range FAT12 chain")
            result.append(cluster)
            cluster = self.fat(cluster)
        if not 0xFF8 <= cluster <= 0xFFF:
            raise ValueError("unterminated FAT12 chain")
        return result

    def cluster_offset(self, cluster: int) -> int:
        if not 2 <= cluster <= self.max_cluster:
            raise ValueError("cluster out of range")
        offset = self.data + (cluster - 2) * self.cluster_bytes
        if offset + self.cluster_bytes > len(self.image):
            raise ValueError("cluster extends past image")
        return offset

    def file_bytes(self, first: int, size: int) -> bytes:
        chain = self.chain(first)
        if size > len(chain) * self.cluster_bytes:
            raise ValueError("directory file size exceeds FAT chain")
        return b"".join(self.image[self.cluster_offset(k):self.cluster_offset(k) + self.cluster_bytes]
                        for k in chain)[:size]

    def find_entry(self, directory_offsets: list[int], short_name: bytes) -> int:
        if len(short_name) != 11:
            raise ValueError("expected FAT 8.3 name")
        for start in directory_offsets:
            length = self.root_entries * 32 if start == self.root else self.cluster_bytes
            for offset in range(start, start + length, 32):
                entry = self.image[offset:offset + 32]
                if entry[0] == 0:
                    break
                if entry[0] != 0xE5 and entry[11] != 0x0F and entry[:11] == short_name:
                    return offset
        raise ValueError(f"FAT directory entry missing: {short_name!r}")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--link-receipt", type=Path)
    parser.add_argument("--mz-audit-receipt", type=Path)
    parser.add_argument("--original-maine", action="store_true")
    parser.add_argument("--startup", choices=STARTUP_COMMANDS, default="game-bat")
    parser.add_argument("--output-dir", type=Path, required=True)
    args = parser.parse_args()
    output = args.output_dir.resolve()
    probes = (ROOT / ".analysis/reconstruction/probes").resolve()
    if output.exists() or not output.is_relative_to(PRIVATE):
        parser.error("use a new private output directory")
    if args.original_maine:
        if args.link_receipt or args.mz_audit_receipt:
            parser.error("original MAINE must not take candidate receipts")
        link_path = audit_path = None
        candidate = None
    else:
        if not args.link_receipt or not args.mz_audit_receipt:
            parser.error("candidate MAINE requires both probe receipts")
        link_path = args.link_receipt.resolve()
        audit_path = args.mz_audit_receipt.resolve()
        if (not link_path.is_relative_to(probes) or link_path.name != "receipt.json"
                or not audit_path.is_relative_to(probes) or audit_path.name != "receipt.json"):
            parser.error("candidate receipts must be private probe receipts")
        link = json.loads(link_path.read_text(encoding="utf-8"))
        audit = json.loads(audit_path.read_text(encoding="utf-8"))
        if (not link["link_complete"] or not link["support_lib_sha256"]
                or audit["link_receipt_sha256"] != sha(link_path.read_bytes())
                or audit["mz_sha256"] != link["mz_header"]["sha256"]):
            raise ValueError("expected statically audited historical-library calibration MZ")
        exe = link_path.parent / "source/bin/maine-native.exe"
        candidate = exe.read_bytes()
        if sha(candidate) != audit["mz_sha256"]:
            raise ValueError("candidate MZ identity drift")

    runtime = tomllib.loads((ROOT / "config/runtime.toml").read_text(encoding="utf-8"))
    image_path = ROOT / runtime["image"]["path"]
    original = image_path.read_bytes()
    if len(original) != runtime["image"]["size"] or sha(original) != runtime["image"]["sha256"]:
        raise ValueError("pinned original HDI identity drift")
    targets = tomllib.loads((ROOT / "config/targets.toml").read_text(encoding="utf-8"))
    target = next(item for item in targets["artifacts"] if item["id"] == "th04-maine")

    image = bytearray(original)
    fs = Fat12(image)
    genso = fs.find_entry([fs.root], b"GENSO      ")
    if not image[genso + 11] & 0x10:
        raise ValueError("GENSO is not a directory")
    genso_offsets = [fs.cluster_offset(k) for k in fs.chain(u16(image, genso + 26))]
    maine = fs.find_entry(genso_offsets, b"MAINE   EXE")
    old_first = u16(image, maine + 26)
    old_size = u32(image, maine + 28)
    old_chain = fs.chain(old_first)
    if old_size != target["size"] or sha(fs.file_bytes(old_first, old_size)) != target["sha256"]:
        raise ValueError("HDI MAINE.EXE is not the pinned TH04 target")

    new_chain = old_chain
    if candidate is not None:
        required = (len(candidate) + fs.cluster_bytes - 1) // fs.cluster_bytes
        extra = required - len(old_chain)
        if extra < 0:
            raise ValueError("calibration MZ unexpectedly smaller than packed target")
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
            image[offset:offset + fs.cluster_bytes] = block.ljust(fs.cluster_bytes, b"\x00")
        image[maine + 28:maine + 32] = len(candidate).to_bytes(4, "little")

    autoexec_bytes = (AUTOEXEC_PREFIX + STARTUP_COMMANDS[args.startup]
                      + b"ECHO EXIT >> A:\\DIAG.TXT\r\n\x1a")

    autoexec = fs.find_entry([fs.root], b"AUTOEXECBAT")
    auto_chain = fs.chain(u16(image, autoexec + 26))
    if len(autoexec_bytes) > len(auto_chain) * fs.cluster_bytes:
        raise ValueError("AUTOEXEC diagnostic command exceeds allocated chain")
    for index, cluster in enumerate(auto_chain):
        offset = fs.cluster_offset(cluster)
        block = autoexec_bytes[index * fs.cluster_bytes:(index + 1) * fs.cluster_bytes]
        image[offset:offset + fs.cluster_bytes] = block.ljust(fs.cluster_bytes, b"\x00")
    image[autoexec + 28:autoexec + 32] = len(autoexec_bytes).to_bytes(4, "little")

    verified = Fat12(image)
    if ((candidate is not None and sha(verified.file_bytes(old_first, len(candidate))) != sha(candidate))
            or verified.file_bytes(u16(image, autoexec + 26), len(autoexec_bytes)) != autoexec_bytes):
        raise ValueError("disposable HDI write did not read back")
    if sha(image_path.read_bytes()) != runtime["image"]["sha256"]:
        raise ValueError("source image changed during preparation")

    output.mkdir(parents=True)
    image_out = output / "diagnostic.hdi"
    image_out.write_bytes(image)
    receipt = {
        "schema_version": 1,
        "scope": "private diagnostic MAINE boot image, not product acceptance",
        "original_hdi_sha256": runtime["image"]["sha256"],
        "original_maine_sha256": target["sha256"],
        "maine_source": "original" if candidate is None else "historical-library calibration",
        "startup": args.startup,
        "link_receipt_sha256": sha(link_path.read_bytes()) if link_path else None,
        "mz_audit_receipt_sha256": sha(audit_path.read_bytes()) if audit_path else None,
        "candidate_maine_sha256": sha(candidate) if candidate is not None else None,
        "candidate_maine_size": len(candidate) if candidate is not None else None,
        "original_maine_chain": old_chain,
        "candidate_maine_chain": new_chain,
        "autoexec_sha256": sha(autoexec_bytes),
        "diagnostic_hdi_sha256": sha(image),
        "limit": "Disposable FAT12 image only; no PC-98 execution or behavioral evidence.",
    }
    (output / "receipt.json").write_text(json.dumps(receipt, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"image": str(image_out), "candidate_size": len(candidate) if candidate is not None else None,
                      "clusters": len(new_chain), "sha256": receipt["diagnostic_hdi_sha256"]}, sort_keys=True))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
