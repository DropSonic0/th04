#!/usr/bin/env python3
"""Audit the ordered TH04 MAIN native-link frontier without linking it.

The historical Tupfile is a routing input only. This probe makes every
historical MAIN object explicit, verifies local owners and fused physical
producers, and reports the remaining owner set before a native compiler/linker
run. It intentionally returns success for an incomplete frontier; callers
must inspect ``ready_for_native_link`` and ``unmapped``.
"""

from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path
import re
import tomllib

ROOT = Path(__file__).resolve().parents[2]
MANIFEST = ROOT / "config/native_main_sources.toml"
TUPFILE = ROOT / "_reference/ReC98/Tupfile.lua"


def sha(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for chunk in iter(lambda: stream.read(1 << 20), b""):
            digest.update(chunk)
    return digest.hexdigest()


def reference_sources_from_tupfile() -> list[str]:
    text = TUPFILE.read_text(encoding="utf-8")
    start = text.index(
        'th04:branch(MODEL_LARGE, { cflags = "-DBINARY=\'M\'" }):link("main", {'
    )
    end = text.index(
        '})\nth04:branch(MODEL_LARGE, { cflags = "-DBINARY=\'E\'" }):link("maine", {',
        start,
    )
    return re.findall(r'^\s*"([^"]+\.(?:cpp|c|asm))",?$', text[start:end], re.MULTILINE)


def fail(message: str) -> None:
    raise RuntimeError(message)


def audit() -> dict[str, object]:
    data = tomllib.loads(MANIFEST.read_text(encoding="utf-8"))
    required = {
        "schema_version",
        "artifact",
        "reference_build_file",
        "reference_link_label",
        "scaffold_sources",
        "reference_sources",
        "direct_owners",
        "fused_owners",
        "unmapped",
        "scaffold_only",
    }
    if set(data) != required or data["schema_version"] != 1 or data["artifact"] != "th04-main":
        fail("native MAIN manifest schema/artifact mismatch")
    listed = data["reference_sources"]
    if not isinstance(listed, list) or not listed or not all(isinstance(item, str) for item in listed):
        fail("reference_sources must be a non-empty string list")
    if len(set(listed)) != len(listed):
        fail("reference_sources contains duplicates")
    observed = reference_sources_from_tupfile()
    if listed != observed:
        fail(f"Tupfile MAIN object order drift: manifest={len(listed)} observed={len(observed)}")
    direct = data["direct_owners"]
    fused = data["fused_owners"]
    unmapped = data["unmapped"]
    scaffold_only = data["scaffold_only"]
    if not isinstance(direct, dict) or not isinstance(fused, dict) or not isinstance(unmapped, list):
        fail("owner sections have invalid types")
    if not isinstance(scaffold_only, dict):
        fail("scaffold_only must be a table")
    known = set(direct) | set(fused) | set(unmapped)
    stale = sorted(known - set(listed))
    missing = sorted(set(listed) - known)
    if stale:
        fail(f"owner entry is not in reference_sources: {stale}")
    if missing:
        fail(f"reference source has no classification: {missing}")
    if len(unmapped) != len(set(unmapped)):
        fail("unmapped contains duplicates")
    if set(direct) & set(fused) or set(direct) & set(unmapped) or set(fused) & set(unmapped):
        fail("owner classifications overlap")

    owners: list[dict[str, object]] = []
    missing_local: list[str] = []
    physical: dict[str, list[str]] = {}
    for reference in listed:
        if reference in direct:
            local = direct[reference]
            if not isinstance(local, str) or not local:
                fail(f"direct owner is not a path: {reference}")
            local_path = ROOT / local
            present = local_path.is_file() and not local_path.is_symlink()
            if not present:
                missing_local.append(local)
            owners.append({"reference": reference, "mode": "direct", "local": local, "present": present})
            physical.setdefault(f"direct:{local}", []).append(reference)
        elif reference in fused:
            value = fused[reference]
            if not isinstance(value, dict) or set(value) != {"physical_id", "local"}:
                fail(f"fused owner shape mismatch: {reference}")
            physical_id = value["physical_id"]
            local = value["local"]
            if not isinstance(physical_id, str) or not physical_id or not isinstance(local, str) or not local:
                fail(f"fused owner fields invalid: {reference}")
            local_path = ROOT / local
            present = local_path.is_file() and not local_path.is_symlink()
            if not present:
                missing_local.append(local)
            owners.append({"reference": reference, "mode": "fused", "physical_id": physical_id,
                           "local": local, "present": present})
            physical.setdefault(f"fused:{physical_id}", []).append(reference)
        else:
            owners.append({"reference": reference, "mode": "unmapped", "present": False})

    invalid_scaffold = []
    for label, path in scaffold_only.items():
        if not isinstance(label, str) or not isinstance(path, str) or not path:
            fail("scaffold_only has invalid entry")
        if not (ROOT / path).is_file():
            invalid_scaffold.append(path)

    mapped = [item for item in owners if item["mode"] != "unmapped"]
    ready = not missing_local and not invalid_scaffold and not unmapped
    return {
        "schema_version": 1,
        "artifact": "th04-main",
        "manifest_sha256": sha(MANIFEST),
        "reference_build_file_sha256": sha(TUPFILE),
        "reference_sources": len(listed),
        "mapped_reference_sources": len(mapped),
        "direct_reference_sources": sum(item["mode"] == "direct" for item in owners),
        "fused_reference_sources": sum(item["mode"] == "fused" for item in owners),
        "unmapped": list(unmapped),
        "missing_local": sorted(set(missing_local)),
        "invalid_scaffold": invalid_scaffold,
        "physical_producers": {key: value for key, value in physical.items()},
        "ready_for_native_link": ready,
        "owners": owners,
    }


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output", type=Path)
    parser.add_argument("--check", action="store_true", help="check manifest drift without requiring link readiness")
    args = parser.parse_args()
    result = audit()
    if args.output:
        output = args.output.resolve()
        private = (ROOT / ".analysis/reconstruction/probes").resolve()
        if not output.is_relative_to(private):
            parser.error("--output must stay below .analysis/reconstruction/probes")
        output.parent.mkdir(parents=True, exist_ok=True)
        output.write_text(json.dumps(result, indent=2, sort_keys=True) + "\n", encoding="utf-8")
    print(json.dumps({
        "artifact": result["artifact"],
        "reference_sources": result["reference_sources"],
        "mapped_reference_sources": result["mapped_reference_sources"],
        "unmapped": len(result["unmapped"]),
        "missing_local": len(result["missing_local"]),
        "ready_for_native_link": result["ready_for_native_link"],
    }, sort_keys=True))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
