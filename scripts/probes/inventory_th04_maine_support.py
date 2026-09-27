#!/usr/bin/env python3
"""Map native MAINE unresolved names to a pinned historical archive inventory.

This is an ownership queue. Archive membership does not establish a valid
large-model ABI or permit using that archive as the TH04 product dependency.
"""

from __future__ import annotations

import argparse
from collections import defaultdict
import hashlib
import json
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[2]
ARCHIVE = ROOT / "_reference/ReC98/bin/masters.lib"
ARCHIVE_SHA = "6be41dbcfcf4504977165ccc44443525a29a01f85a1580e6ad0c620bf802faf6"
LISTING = ROOT / ".analysis/reconstruction/probes/native-th04-maine-masterlib-inventory-20260927/masters.lst"
LISTING_SHA = "813dcfc86f7fe70559d3aa0efa7025c7e478ba8603ac02bb7a9b852ff39f55ed"
MODULE_RE = re.compile(r"^([^\s]+)\s+size = (\d+)")


def sha(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def archive_publics(text: str) -> dict[str, list[str]]:
    owners: dict[str, list[str]] = defaultdict(list)
    module = None
    for line in text.splitlines():
        match = MODULE_RE.match(line)
        if match:
            module = match.group(1)
        elif module and line.startswith("\t"):
            for symbol in line.split():
                owners[symbol].append(module)
    return dict(owners)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--link-receipt", type=Path, required=True)
    parser.add_argument("--output-dir", type=Path, required=True)
    args = parser.parse_args()
    output = args.output_dir.resolve()
    private = (ROOT / ".analysis/reconstruction/probes").resolve()
    if output.exists() or not output.is_relative_to(private):
        parser.error("output must be a new directory below .analysis/reconstruction/probes")
    receipt_path = args.link_receipt.resolve()
    if not receipt_path.is_relative_to(private) or receipt_path.name != "receipt.json":
        parser.error("link receipt must be a private native probe receipt")
    if sha(ARCHIVE) != ARCHIVE_SHA or sha(LISTING) != LISTING_SHA:
        raise ValueError("pinned historical archive or TLIB listing identity drift")
    link = json.loads(receipt_path.read_text(encoding="utf-8"))
    if link.get("support_lib_sha256") is not None or link.get("link_complete"):
        raise ValueError("expected a failing no-archive source link receipt")
    owners = archive_publics(LISTING.read_text(encoding="cp437"))
    rows = []
    for diagnostic in link["unresolved"]:
        symbol = diagnostic.split(" in module ", 1)[0]
        rows.append({"symbol": symbol, "archive_members": sorted(owners.get(symbol, [])),
                     "diagnostic": diagnostic})
    provided = [row for row in rows if row["archive_members"]]
    unprovided = [row for row in rows if not row["archive_members"]]
    members = sorted({member for row in provided for member in row["archive_members"]})
    report = {
        "schema_version": 1,
        "evidence_class": "compiler-library-analysis",
        "link_receipt_sha256": sha(receipt_path),
        "archive_sha256": ARCHIVE_SHA,
        "listing_sha256": LISTING_SHA,
        "unresolved": len(rows),
        "archive_provided": len(provided),
        "archive_members": len(members),
        "without_archive_provider": [row["symbol"] for row in unprovided],
        "rows": rows,
        "limit": "An archive public-name match is an ownership lead, not ABI or product-link acceptance.",
    }
    output.mkdir(parents=True)
    (output / "receipt.json").write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({key: report[key] for key in ("unresolved", "archive_provided",
                                                "archive_members", "without_archive_provider")},
                     sort_keys=True))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
