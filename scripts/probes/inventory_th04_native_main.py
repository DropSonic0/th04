#!/usr/bin/env python3
"""Report missing quoted includes in maintained MAIN source and inlines."""

from __future__ import annotations

import argparse
from collections import Counter, defaultdict
import json
from pathlib import Path
import re
import tomllib


ROOT = Path(__file__).resolve().parents[2]
INCLUDE = re.compile(r'^\s*#\s*include\s*"([^"]+)"', re.MULTILINE)
LOCAL_INCLUDE_ROOTS = (ROOT, ROOT / "src/main/include", ROOT / "src/shared/include")


def add_replay_source(
    replay_sources: dict[str, set[str]], row: dict[str, object],
    overlay_key: str, source_key: str,
) -> None:
    overlay = row.get(overlay_key)
    source = row.get(source_key)
    if isinstance(overlay, str) and isinstance(source, str):
        replay_sources[overlay].add(source)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output", type=Path)
    args = parser.parse_args()
    exact_units = tomllib.loads(
        (ROOT / "config/th04_main_exact_units.toml").read_text(encoding="utf-8")
    )
    replay_sources: dict[str, set[str]] = defaultdict(set)
    for unit in exact_units["units"]:
        add_replay_source(replay_sources, unit, "overlay_path", "repo_source")
    for insert in exact_units.get("build_inserts", []):
        add_replay_source(replay_sources, insert, "overlay_path", "repo_source")
    for split in exact_units.get("splits", []):
        add_replay_source(
            replay_sources, split, "fragment_overlay_path", "repo_fragment"
        )
        add_replay_source(
            replay_sources, split, "wrapper_overlay_path", "repo_wrapper"
        )
    missing: Counter[str] = Counter()
    owners: dict[str, list[str]] = defaultdict(list)
    for source in sorted((ROOT / "src/main").rglob("*")):
        if source.suffix not in {".c", ".cpp", ".inl"}:
            continue
        for include in INCLUDE.findall(source.read_text(encoding="utf-8")):
            if (not any((root / include).is_file() for root in LOCAL_INCLUDE_ROOTS)
                    and not (source.parent / include).is_file()):
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
    fragments = [name for name in missing if Path(name).suffix in {".cpp", ".inl"}]
    reference = ROOT / "_reference/ReC98"
    report = {
        "schema_version": 1,
        "scope": "read-only MAIN quoted-include source closure",
        "unique_missing": len(missing),
        "references": sum(missing.values()),
        "by_suffix": by_suffix,
        "composite_producers": composite,
        "fragment_replay_mapped": sum(bool(replay_sources[name]) for name in fragments),
        "fragment_replay_unmapped": sorted(name for name in fragments
                                           if not replay_sources[name]),
        "reference_header_paths_present": sum((reference / name).is_file()
                                              for name in missing
                                              if Path(name).suffix in {".h", ".hpp"}),
        "missing": [{"include": name, "references": count,
                     "owners": sorted(set(owners[name])),
                     "replay_sources": sorted(replay_sources[name])}
                    for name, count in missing.most_common()],
    }
    rendered = json.dumps(report, indent=2, ensure_ascii=False) + "\n"
    if args.output:
        output = args.output.resolve()
        private = (ROOT / ".analysis/reconstruction/probes").resolve()
        if output.exists() or not output.is_relative_to(private):
            parser.error("output must be a new private file")
        output.parent.mkdir(parents=True, exist_ok=True)
        output.write_text(rendered, encoding="utf-8")
    else:
        print(rendered, end="")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
