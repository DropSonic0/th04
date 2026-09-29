#!/usr/bin/env python3
"""Link the maintained TH04 MAIN physical roots and report the frontier.

This is a native-link diagnostic, not an exact MAIN build.  It preserves the
historical basename for manifest-routed owners (Borland derives default
segment names from that basename), compiles one object per physical C/C++
root, assembles the eight maintained DATA/BSS owners, and records TLINK's
unresolved/duplicate/group/fixup frontier.  The external ``masters.lib`` is
calibration only; ``--without-support`` is the TH04-source-only control.
"""

from __future__ import annotations

import argparse
from collections import Counter
from datetime import datetime, timezone
import hashlib
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys
import tomllib

ROOT = Path(__file__).resolve().parents[2]
PRIVATE = (ROOT / ".analysis/reconstruction/probes").resolve()
RUNNER = ROOT / "_reference/ReC98/bin/msdos.exe"
RUNNER_SHA256 = "f7f6cb0a3e816c5edb13112d327c1bddbf7463fe7bf9a005ca1eb5317751bd02"
SUPPORT_LIB = ROOT / "_reference/ReC98/bin/masters.lib"
SUPPORT_SHA256 = "6be41dbcfcf4504977165ccc44443525a29a01f85a1580e6ad0c620bf802faf6"
MANIFEST = ROOT / "config/native_main_sources.toml"
STATE_SOURCES = (
    Path("src/main/core/frame_state.asm"),
    Path("src/main/core/quit_state.asm"),
    Path("src/main/core/slowdown_state.asm"),
    Path("src/main/dialog/data.asm"),
    Path("src/main/dialog/script_state.asm"),
    Path("src/main/dialog/state.asm"),
    Path("src/main/scroll/page_state.asm"),
    Path("src/main/scroll/state.asm"),
)
SOURCE_SUFFIXES = {".c", ".cpp"}
SOURCE_ROOTS = (ROOT / "src/main", ROOT / "src/shared")
LAYOUT_ANCHOR = Path("src/main/layout/main_code_order_anchor.asm")

# These are physical alternatives with duplicate publics.  The manifest's
# product owner is the left-hand side; the other producer stays available for
# its own focused replay but is not a MAIN link input.
SOURCE_EXCLUSIONS = {
    "src/main/item/splashes_init.cpp",
    "src/main/player/reimu_shot_b.cpp",
    "src/shared/core/game_exit.cpp",
    "src/shared/core/game_init_main.cpp",
    "src/shared/hardware/input_wait.cpp",
    "src/shared/math/vector.cpp",
}
# MAIN's large-model support library already owns the near `_TEXT` GRCG
# entry points consumed by its `superzom`/`supercln` modules.  The maintained
# far display-control owner is still used by OP/MAINE, but linking it here
# shadows masters.lib and creates unavoidable near-call fixup overflows.
ASM_EXCLUSIONS = {
    "src/shared/hardware/display_control.asm",
}
FLAGS = (
    "-c", "-I.", "-Isrc/main/include", "-O", "-b-", "-3", "-Z", "-d",
    "-DGAME=4", "-DTH04P", "-ml", "-DBINARY='M'",
)

sys.path.insert(0, str(ROOT / "scripts"))
from lib.omf import describe_omf, parse_omf  # noqa: E402
from lib.pc98 import parse_mz  # noqa: E402
from probe_th04_native_main_manifest import audit  # noqa: E402
from lib.th04_sprites import generate_sprite_sources  # noqa: E402


def sha256_bytes(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def sha256(path: Path) -> str:
    return sha256_bytes(path.read_bytes())


def run(command: list[str], cwd: Path, env: dict[str, str], log: Path) -> subprocess.CompletedProcess[str]:
    result = subprocess.run(command, cwd=cwd, env=env, capture_output=True,
                            text=True, timeout=300)
    log.write_text(json.dumps(command) + f"\nexit={result.returncode}\n"
                   + result.stdout + result.stderr, encoding="utf-8")
    return result


def source_tree_digest(sources: list[Path]) -> str:
    digest = hashlib.sha256()
    for source in sources:
        digest.update(source.relative_to(ROOT).as_posix().encode("utf-8"))
        digest.update(b"\0")
        digest.update(bytes.fromhex(sha256(source)))
    return digest.hexdigest()


def index_value(data: bytes, cursor: int) -> tuple[int, int]:
    first = data[cursor]
    if first & 0x80:
        return ((first & 0x7F) << 8) | data[cursor + 1], cursor + 2
    return first, cursor + 1


def code_group(obj: Path) -> tuple[str, bool, tuple[str, ...]]:
    """Return primary code group, nonzero-code flag, and all code groups."""
    names = [""]
    segments: list[tuple[str, str, int]] = []
    groups: list[tuple[str, list[int]]] = []
    for record in parse_omf(obj.read_bytes()):
        data = record.data
        if record.record_type == 0x96:  # LNAMES
            cursor = 0
            while cursor < len(data):
                size = data[cursor]
                cursor += 1
                names.append(data[cursor:cursor + size].decode("ascii", "replace"))
                cursor += size
        elif record.record_type in (0x98, 0x99):  # SEGDEF16/32
            cursor = 1
            acbp = data[0]
            if ((acbp >> 5) & 0x7) == 0:
                cursor += 3
            length = int.from_bytes(data[cursor:cursor + 2], "little")
            cursor += 2
            segment_name, cursor = index_value(data, cursor)
            class_name, cursor = index_value(data, cursor)
            _, cursor = index_value(data, cursor)
            segments.append((names[segment_name], names[class_name], length))
        elif record.record_type == 0x9A:  # GRPDEF
            cursor = 0
            group_name, cursor = index_value(data, cursor)
            members: list[int] = []
            while cursor < len(data):
                if data[cursor] != 0xFF:
                    raise ValueError(f"malformed GRPDEF in {obj}")
                cursor += 1
                member, cursor = index_value(data, cursor)
                members.append(member)
            groups.append((names[group_name], members))

    group_by_segment: dict[int, str] = {}
    for group_name, members in groups:
        for member in members:
            group_by_segment[member] = group_name
    code = [
        (name, length, group_by_segment.get(index + 1))
        for index, (name, cls, length) in enumerate(segments)
        if cls.upper() == "CODE"
    ]
    all_groups = tuple(sorted({group for _, _, group in code if group}))
    # Zero-length code SEGDEFs still establish the producer's group.  Prefer
    # MAIN_03 when a TU also carries a zero MAIN_01 contribution.
    if "MAIN_03" in all_groups:
        primary = "MAIN_03"
    elif all_groups:
        primary = all_groups[0]
    else:
        primary = "none"
    return primary, any(length for _, length, _ in code), all_groups


def cpp_roots() -> tuple[list[Path], set[str]]:
    sources = sorted(
        source for root in SOURCE_ROOTS for source in root.rglob("*")
        if source.is_file() and source.suffix.lower() in SOURCE_SUFFIXES
    )
    included: set[str] = set()
    include_pattern = re.compile(r"#include\s+[\"<](src/[^\">]+\.cpp)[\">]")
    for source in sources:
        included.update(include_pattern.findall(source.read_text(encoding="utf-8", errors="replace")))
    roots = [source for source in sources
             if source.relative_to(ROOT).as_posix() not in included
             and source.relative_to(ROOT).as_posix() not in SOURCE_EXCLUSIONS]
    return roots, included


def asm_sources() -> list[Path]:
    return sorted(
        source for root in SOURCE_ROOTS for source in root.rglob("*.asm")
        if source.is_file()
        and source.relative_to(ROOT).as_posix() not in ASM_EXCLUSIONS
    )


def manifest_aliases() -> dict[str, str]:
    data = tomllib.loads(MANIFEST.read_text(encoding="utf-8"))
    result: dict[str, str] = {}
    for reference in data["reference_sources"]:
        if reference in data["direct_owners"]:
            local = data["direct_owners"][reference]
        elif reference in data["fused_owners"]:
            local = data["fused_owners"][reference]["local"]
        else:
            continue
        if local.endswith((".c", ".cpp")):
            # First historical route owns the physical producer's basename.
            result.setdefault(local, data["compile_aliases"].get(reference, reference))
    return result


EXTRA_ALIASES = {
    "src/main/core/demo_prefix.cpp": "th04/demo_main.cpp",
    "src/main/boss/boss_bg_main01.cpp": "th04/boss_bg.cpp",
    "src/main/boss/elly_update.cpp": "th04/elly.cpp",
    "src/main/boss/kurumi_update.cpp": "th04/kurumi.cpp",
    "src/main/boss/marisa4_main033.cpp": "th04/marisa4.cpp",
    "src/main/boss/mugetsu_main033.cpp": "th04/mugetsu.cpp",
    "src/main/boss/yuuka5.cpp": "th04/yuuka5.cpp",
    "src/main/boss/yuuka6_main034.cpp": "th04/yuuka6.cpp",
}


def compile_cpp(source: Path, alias: str, index: int, work: Path,
                output: Path, env: dict[str, str]) -> dict[str, object]:
    relative = source.relative_to(ROOT).as_posix()
    copied = work / source.relative_to(ROOT)
    compile_source = work / alias
    compile_source.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(copied, compile_source)
    if sha256(copied) != sha256(compile_source):
        raise RuntimeError(f"compile alias changed source bytes: {relative}")
    obj_dir = work / "obj/cpp" / f"{index:03d}"
    obj_dir.mkdir(parents=True)
    alias_relative = compile_source.relative_to(work).as_posix()
    command = ["wine", str(RUNNER), "-e", "-x", "tcc", *FLAGS,
               f"-nobj/cpp/{index:03d}/", alias_relative]
    log = output / f"compile-{index:03d}.log"
    result = run(command, work, env, log)
    objects = sorted(obj_dir.glob("*.obj"))
    omf = describe_omf(objects[0].read_bytes()) if len(objects) == 1 else None
    primary, has_code, groups = code_group(objects[0]) if omf else ("none", False, ())
    return {
        "index": index,
        "source": relative,
        "source_sha256": sha256(copied),
        "compile_alias": alias,
        "compile_alias_sha256": sha256(compile_source),
        "compile_exit": result.returncode,
        "log": log.relative_to(output).as_posix(),
        "objects": [item.relative_to(work).as_posix() for item in objects],
        "object_sha256": sha256(objects[0]) if len(objects) == 1 else None,
        "omf_valid": bool(omf and omf["valid"]),
        "translator_comments": omf["translator_comments"] if omf else [],
        "primary_code_group": primary,
        "has_nonzero_code": has_code,
        "code_groups": list(groups),
    }


def assemble_asm(source: Path, index: int, work: Path, output: Path,
                 env: dict[str, str], subdir: str, log_prefix: str) -> dict[str, object]:
    source = source if source.is_absolute() else ROOT / source
    try:
        relative = source.relative_to(work).as_posix()
    except ValueError:
        relative = source.relative_to(ROOT).as_posix()
    obj = work / "obj" / subdir / f"{index:03d}.obj"
    obj.parent.mkdir(parents=True, exist_ok=True)
    source_win = relative.replace("/", "\\")
    object_win = obj.relative_to(work).as_posix().replace("/", "\\")
    command = [
        "wine", "cmd", "/d", "/c",
        r"set PATH=C:\\TASM50\\BIN;C:\\TC4\\BIN;%PATH%&&"
        + f"tasm32 /m /mx /kh32768 /t /dGAME=4 /dTH04_LARGE_PRODUCT=1 "
        f"{source_win} {object_win}",
    ]
    log = output / f"{log_prefix}-{index:03d}.log"
    result = run(command, work, env, log)
    omf = describe_omf(obj.read_bytes()) if obj.is_file() else None
    return {
        "index": index,
        "source": relative,
        "source_sha256": sha256(source),
        "object": obj.relative_to(work).as_posix() if obj.is_file() else None,
        "object_sha256": sha256(obj) if obj.is_file() else None,
        "assemble_exit": result.returncode,
        "omf_valid": bool(omf and omf["valid"]),
        "translator_comments": omf["translator_comments"] if omf else [],
        "log": log.relative_to(output).as_posix(),
    }


def assemble_state(source: Path, index: int, work: Path, output: Path,
                   env: dict[str, str]) -> dict[str, object]:
    return assemble_asm(source, index, work, output, env, "state", "assemble-state")


def link(work: Path, output: Path, objects: list[Path], env: dict[str, str],
         without_support: bool) -> dict[str, object]:
    bin_dir = work / "bin"
    bin_dir.mkdir(exist_ok=True)
    if not without_support:
        shutil.copy2(SUPPORT_LIB, bin_dir / "masters.lib")
    relative_objects = [str(item.relative_to(work)).replace("/", "\\") for item in objects]
    libraries = "emu.lib mathl.lib cl.lib" if without_support else \
        "bin\\masters.lib emu.lib mathl.lib cl.lib"
    response = work / "obj/main-native.@l"
    response.write_text(
        "-c -s -E c0l.obj " + " ".join(relative_objects)
        + ", bin\\main-native.exe, obj\\main-native.map, " + libraries + "\n",
        encoding="ascii",
    )
    command = ["wine", str(RUNNER), "-e", "-x", "tlink", "@obj\\main-native.@l"]
    log = output / "link.log"
    result = run(command, work, env, log)
    text = result.stdout + result.stderr
    errors = re.findall(r"^(?:Error|Fatal): (.+)$", text, re.MULTILINE)
    warnings = re.findall(r"^Warning: (.+)$", text, re.MULTILINE)
    undefined = re.findall(r"^Error: Undefined symbol (.+?) in module (.+)$", text, re.MULTILINE)
    duplicate = re.findall(r"^Error: (.+? defined in module .+? is duplicated in module .+)$",
                           text, re.MULTILINE)
    fixups = [item for item in errors if item.startswith("Fixup overflow")]
    groups = [item for item in errors if item.startswith("Group ") and "exceeds 64K" in item]
    exe = work / "bin/main-native.exe"
    map_file = work / "obj/main-native.map"
    mz: dict[str, object] | None = None
    if exe.is_file():
        try:
            parsed = parse_mz(exe.read_bytes())
            mz = {
                "sha256": sha256(exe),
                "size": exe.stat().st_size,
                "header_paragraphs": parsed.header.header_paragraphs,
                "relocations": len(parsed.relocations),
            }
        except Exception as exc:  # pragma: no cover - diagnostic receipt path
            mz = {"error": str(exc), "sha256": sha256(exe), "size": exe.stat().st_size}
    return {
        "command": command,
        "response": response.relative_to(work).as_posix(),
        "response_sha256": sha256(response),
        "log": log.relative_to(output).as_posix(),
        "exit": result.returncode,
        "errors": errors,
        "warnings": warnings,
        "undefined_symbols": [{"symbol": symbol, "module": module}
                              for symbol, module in undefined],
        "duplicate_errors": duplicate,
        "fixup_overflows": fixups,
        "group_overflows": groups,
        "map": map_file.relative_to(work).as_posix() if map_file.is_file() else None,
        "map_sha256": sha256(map_file) if map_file.is_file() else None,
        "mz": mz,
    }


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output-dir", type=Path)
    parser.add_argument("--without-support", action="store_true",
                        help="link TH04 objects with Borland system libraries only")
    parser.add_argument("--check-manifest", action="store_true",
                        help="audit the frozen MAIN routing manifest without building")
    parser.add_argument("--require-link", action="store_true",
                        help="return failure unless TLINK exits zero with no errors")
    args = parser.parse_args()
    if args.check_manifest:
        if args.output_dir or args.without_support or args.require_link:
            parser.error("--check-manifest cannot be combined with build options")
        result = audit()
        print(json.dumps({"artifact": "th04-main", "manifest": result}, sort_keys=True))
        return 0
    if args.output_dir is None:
        parser.error("--output-dir is required for a link diagnostic")
    output = args.output_dir.resolve()
    if output.exists() or not output.is_relative_to(PRIVATE):
        parser.error("output must be a new private directory")
    subprocess.run([sys.executable, "scripts/preflight.py"], cwd=ROOT, check=True,
                   stdout=subprocess.DEVNULL)
    subprocess.run([sys.executable, "scripts/attest_toolchain.py"], cwd=ROOT, check=True,
                   stdout=subprocess.DEVNULL)
    if sha256(RUNNER) != RUNNER_SHA256:
        raise RuntimeError("pinned TC4J runner identity drift")
    if not args.without_support and sha256(SUPPORT_LIB) != SUPPORT_SHA256:
        raise RuntimeError("pinned masters.lib identity drift")

    roots, included = cpp_roots()
    assembly_sources = asm_sources()
    state_source_names = {source.as_posix() for source in STATE_SOURCES}
    aliases = manifest_aliases()
    output.mkdir(parents=True)
    work = output / "source"
    shutil.copytree(ROOT / "src", work / "src")
    sprite_sources, sprite_asset_records = generate_sprite_sources(
        ROOT, work / "generated/sprites"
    )
    env = os.environ.copy()
    env.update(WINEPREFIX=str(ROOT / ".analysis/toolchain/wineprefix"),
               WINEDEBUG="-all", MSDOS_PATH=r"C:\TC4\BIN;C:\TASM50\BIN")

    # Historical aliases first; un-routed physical roots receive deterministic
    # short aliases. Their explicit pragma segment names remain intact.
    root_records: list[dict[str, object]] = []
    for index, source in enumerate(roots):
        relative = source.relative_to(ROOT).as_posix()
        alias = aliases.get(relative, EXTRA_ALIASES.get(
            relative, f"th04/u{index:03d}{source.suffix.lower()}"))
        root_records.append(compile_cpp(source, alias, index, work, output, env))
    state_records = [assemble_state(source, index, work, output, env)
                     for index, source in enumerate(STATE_SOURCES)]
    asm_records = [
        assemble_asm(source, index, work, output, env, "asm", "assemble-asm")
        for index, source in enumerate(assembly_sources)
        if source.relative_to(ROOT).as_posix() not in state_source_names
    ]
    sprite_records = [
        assemble_asm(source, index, work, output, env, "sprite", "assemble-sprite")
        for index, source in enumerate(sprite_sources)
    ]

    valid_cpp = [record for record in root_records
                 if record["compile_exit"] == 0 and record["omf_valid"]]
    valid_state = [record for record in state_records
                   if record["assemble_exit"] == 0 and record["omf_valid"]]
    valid_asm = [record for record in asm_records
                 if record["assemble_exit"] == 0 and record["omf_valid"]]
    valid_sprites = [record for record in sprite_records
                     if record["assemble_exit"] == 0 and record["omf_valid"]]
    # Keep each physical root once and order by group before TLINK.  This
    # prevents zero-length MAIN_03 state SEGDEFs from assigning later default
    # segments to the wrong group; it is a routing diagnostic, not exact order.
    priority = {"none": 0, "MAIN_01": 1, "MAIN_03": 2}
    ordered_records = sorted(
        valid_cpp,
        key=lambda item: (priority.get(str(item["primary_code_group"]), 3),
                          0 if item["has_nonzero_code"] else 1,
                          str(item["source"])),
    )
    anchor_records = [item for item in valid_asm
                      if item["source"] == LAYOUT_ANCHOR.as_posix()]
    if len(anchor_records) > 1:
        raise RuntimeError("layout anchor assembled more than once")
    other_asm = [item for item in valid_asm
                 if item["source"] != LAYOUT_ANCHOR.as_posix()]
    object_paths = [work / str(item["object"]) for item in anchor_records]
    object_paths.extend(work / str(item["objects"][0]) for item in ordered_records)
    object_paths.extend(work / str(item["object"]) for item in other_asm)
    object_paths.extend(work / str(item["object"]) for item in valid_state)
    object_paths.extend(work / str(item["object"]) for item in valid_sprites)
    link_result = link(work, output, object_paths, env, args.without_support)

    receipt = {
        "schema_version": 1,
        "observed_utc": datetime.now(timezone.utc).isoformat(timespec="seconds"),
        "artifact": "th04-main",
        "scope": "native MAIN physical-root link diagnostic",
        "runner_sha256": RUNNER_SHA256,
        "support_library": None if args.without_support else {
            "path": SUPPORT_LIB.relative_to(ROOT).as_posix(),
            "sha256": SUPPORT_SHA256,
        },
        "compiler_flags": list(FLAGS),
        "source_tree_sha256": source_tree_digest(roots),
        "assembly_tree_sha256": source_tree_digest(assembly_sources),
        "root_count": len(roots),
        "included_cpp_count": len(included),
        "source_exclusions": sorted(SOURCE_EXCLUSIONS),
        "assembly_exclusions": sorted(ASM_EXCLUSIONS),
        "roots_compile_pass": len(valid_cpp),
        "roots_compile_fail": len(root_records) - len(valid_cpp),
        "state_count": len(STATE_SOURCES),
        "state_assemble_pass": len(valid_state),
        "state_assemble_fail": len(state_records) - len(valid_state),
        "asm_count": len(asm_records),
        "asm_assemble_pass": len(valid_asm),
        "asm_assemble_fail": len(asm_records) - len(valid_asm),
        "asm_state_exclusions": sorted(state_source_names),
        "sprite_asset_records": sprite_asset_records,
        "sprite_sources": sprite_records,
        "sprite_assemble_pass": len(valid_sprites),
        "sprite_assemble_fail": len(sprite_records) - len(valid_sprites),
        "root_records": root_records,
        "state_records": state_records,
        "asm_records": asm_records,
        "link": link_result,
        "limit": (
            "Diagnostic only: scaffold th04_main.asm, product ASM owners beyond "
            "the eight state owners, final MZ/layout, relocation agreement, and "
            "PC-98 startup remain open. Sprite inputs are locally supplied "
            "reference BMPs replayed in a private tree; support-library symbols "
            "are calibration."
        ),
    }
    (output / "receipt.json").write_text(
        json.dumps(receipt, indent=2, sort_keys=True) + "\n", encoding="utf-8"
    )
    print(json.dumps({
        "artifact": "th04-main",
        "root_count": len(roots),
        "roots_compile_pass": len(valid_cpp),
        "state_assemble_pass": len(valid_state),
        "sprite_assemble_pass": len(valid_sprites),
        "link_exit": link_result["exit"],
        "undefined": len(link_result["undefined_symbols"]),
        "duplicates": len(link_result["duplicate_errors"]),
        "group_overflows": len(link_result["group_overflows"]),
        "fixup_overflows": len(link_result["fixup_overflows"]),
        "mz": bool(link_result["mz"]),
    }, sort_keys=True))
    if args.require_link:
        return 0 if link_result["exit"] == 0 and not link_result["errors"] else 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
