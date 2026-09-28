#!/usr/bin/env python3
"""Report missing quoted includes in maintained MAIN source and inlines."""

from __future__ import annotations

from collections import Counter, defaultdict
import json
from pathlib import Path
import re


ROOT = Path(__file__).resolve().parents[2]
INCLUDE = re.compile(r'^\s*#\s*include\s*"([^"]+)"', re.MULTILINE)


def main() -> int:
    missing: Counter[str] = Counter()
    owners: dict[str, list[str]] = defaultdict(list)
    for source in sorted((ROOT / "src/main").rglob("*")):
        if source.suffix not in {".c", ".cpp", ".inl"}:
            continue
        for include in INCLUDE.findall(source.read_text(encoding="utf-8")):
            if not (ROOT / include).is_file() and not (source.parent / include).is_file():
                missing[include] += 1
                owners[include].append(source.relative_to(ROOT).as_posix())
    by_suffix = {}
    for group, suffixes in (("headers", {".h", ".hpp"}),
                            ("cpp", {".cpp"}), ("inl", {".inl"})):
        matches = {name: count for name, count in missing.items()
                   if Path(name).suffix in suffixes}
        by_suffix[group] = {"paths": len(matches), "references": sum(matches.values())}
    composite = sorted({owner for name, paths in owners.items()
                        if Path(name).suffix == ".cpp" for owner in paths})
    report = {
        "schema_version": 1,
        "scope": "read-only MAIN quoted-include source closure",
        "unique_missing": len(missing),
        "references": sum(missing.values()),
        "by_suffix": by_suffix,
        "composite_producers": composite,
        "missing": [{"include": name, "references": count,
                     "owners": sorted(set(owners[name]))}
                    for name, count in missing.most_common()],
    }
    print(json.dumps(report, indent=2, ensure_ascii=False))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
