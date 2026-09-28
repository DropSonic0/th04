#!/usr/bin/env python3
"""Compare two cold TH04 native-link diagnostic receipts."""

from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
from lib.omf import normalize_dependency_timestamps  # noqa: E402


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("first", type=Path)
    parser.add_argument("second", type=Path)
    args = parser.parse_args()
    a = json.loads((args.first / "receipt.json").read_text(encoding="utf-8"))
    b = json.loads((args.second / "receipt.json").read_text(encoding="utf-8"))
    if a["scope"] != b["scope"] or a["runner_sha256"] != b["runner_sha256"]:
        raise ValueError("scope or toolchain identity drift")
    if a.get("artifact") != b.get("artifact") or a.get("source_manifest_sha256") != b.get("source_manifest_sha256"):
        raise ValueError("artifact or source manifest drift")
    if a["support_lib_sha256"] != b["support_lib_sha256"]:
        raise ValueError("support library identity drift")
    if a["compiler_flags"] != b["compiler_flags"]:
        raise ValueError("compiler flag drift")
    if a.get("assembler_flags") != b.get("assembler_flags"):
        raise ValueError("assembler flag drift")
    if len(a["objects"]) != len(b["objects"]):
        raise ValueError("object count drift")

    raw_drift = []
    for left, right in zip(a["objects"], b["objects"]):
        for key in ("source", "source_sha256", "object",
                    "link_relevant_sha256"):
            if left[key] != right[key]:
                raise ValueError(f"{key} drift at {left['source']}")
        if left["object_sha256"] != right["object_sha256"]:
            raw_drift.append(left["source"])
        left_bytes = (args.first / "source" / left["object"]).read_bytes()
        right_bytes = (args.second / "source" / right["object"]).read_bytes()
        if hashlib.sha256(left_bytes).hexdigest() != left["object_sha256"]:
            raise ValueError(f"first object identity drift: {left['source']}")
        if hashlib.sha256(right_bytes).hexdigest() != right["object_sha256"]:
            raise ValueError(f"second object identity drift: {right['source']}")
        if normalize_dependency_timestamps(left_bytes) != normalize_dependency_timestamps(right_bytes):
            raise ValueError(f"non-timestamp OMF drift: {left['source']}")
    for key in ("response_sha256", "link_exit", "unresolved", "warnings",
                "link_complete", "mz_header"):
        if a[key] != b[key]:
            raise ValueError(f"{key} drift")
    if a.get("link_errors", []) != b.get("link_errors", []):
        raise ValueError("link error drift")
    print(json.dumps({"objects": len(a["objects"]),
                      "link_relevant_equal": True,
                      "timestamp_normalized_equal": True,
                      "raw_object_drift": raw_drift,
                      "link_exit": a["link_exit"],
                      "unresolved": len(a["unresolved"]),
                      "mz_header_valid": a["mz_header"]["valid"] if a["mz_header"] else None,
                      "link_complete": a["link_complete"]}, sort_keys=True))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
