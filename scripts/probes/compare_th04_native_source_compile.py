#!/usr/bin/env python3
"""Compare two TH04-only compile receipts and all their OMF objects."""

from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
from lib.omf import normalize_dependency_timestamps, parse_omf  # noqa: E402


def sha(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def linking_digest(data: bytes) -> str:
    digest = hashlib.sha256()
    for record in parse_omf(data):
        if record.record_type == 0x88:  # Full-object comparison checks comments separately.
            continue
        digest.update(bytes([record.record_type]))
        digest.update(len(record.data).to_bytes(2, "little"))
        digest.update(record.data)
    return digest.hexdigest()


def load(directory: Path) -> dict:
    receipt = json.loads((directory / "receipt.json").read_text(encoding="utf-8"))
    if receipt["schema_version"] != 1 or not receipt["sources"]:
        raise ValueError(f"unexpected TH04 source manifest: {directory}")
    for item in receipt["sources"]:
        obj = directory / "source" / item["object"]
        if sha(obj.read_bytes()) != item["object_sha256"]:
            raise ValueError(f"object identity drift: {obj}")
    return receipt


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("first", type=Path)
    parser.add_argument("second", type=Path)
    args = parser.parse_args()
    first, second = (args.first.resolve(), args.second.resolve())
    a, b = load(first), load(second)
    for key in ("scope", "runner_sha256", "flags"):
        if a[key] != b[key]:
            raise ValueError(f"compile profile differs: {key}")
    if [(x["source"], x["source_sha256"]) for x in a["sources"]] != [
        (x["source"], x["source_sha256"]) for x in b["sources"]
    ]:
        raise ValueError("source manifest differs")

    raw_differences = []
    for x, y in zip(a["sources"], b["sources"]):
        left = (first / "source" / x["object"]).read_bytes()
        right = (second / "source" / y["object"]).read_bytes()
        if left != right:
            raw_differences.append(x["source"])
        if normalize_dependency_timestamps(left) != normalize_dependency_timestamps(right):
            raise ValueError(f"timestamp-normalized OMF differs: {x['source']}")
        if linking_digest(left) != linking_digest(right):
            raise ValueError(f"link-relevant OMF differs: {x['source']}")
    print(json.dumps({"sources": len(a["sources"]),
                      "raw_object_differences": raw_differences,
                      "timestamp_normalized_equal": True,
                      "link_relevant_equal": True}, sort_keys=True))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
