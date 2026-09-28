#!/usr/bin/env python3
"""Validate the product-owned TH04 MAIN scroll API and BSS owners."""

from __future__ import annotations

import argparse
from datetime import datetime, timezone
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys


ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
from lib.omf import describe_omf  # noqa: E402
from lib.pc98 import parse_mz  # noqa: E402

PRIVATE = (ROOT / ".analysis/reconstruction/probes").resolve()
RUNNER = ROOT / "_reference/ReC98/bin/msdos.exe"
RUNNER_SHA256 = "f7f6cb0a3e816c5edb13112d327c1bddbf7463fe7bf9a005ca1eb5317751bd02"
HEADER = ROOT / "src/main/scroll/scroll.hpp"
STATE_SOURCE = ROOT / "src/main/scroll/state.asm"
PAGE_SOURCE = ROOT / "src/main/scroll/page_state.asm"
REFERENCE_FILES = (
    "platform.h",
    "pc98.h",
    "th01/math/subpixel.hpp",
    "th02/main/scroll.hpp",
    "th04/main/scroll.hpp",
)
FLAGS = ("-c", "-O", "-b-", "-3", "-Z", "-d", "-ml")
HEADER_TEST = r'''#include "th04/main/scroll.hpp"

void probe_state(void)
{
    scroll_line = 1;
    scroll_subpixel_line.v = 2;
    scroll_speed.v = 3;
    scroll_line_on_page[0] = 4;
    scroll_line_on_page[1] = 5;
    scroll_last_delta.v = 6;
    scroll_active = true;
}

vram_y_t probe_calls(subpixel_t y)
{
    return (
        scroll_subpixel_y_to_vram_seg1(y) +
        scroll_subpixel_y_to_vram_seg3(y) +
        scroll_subpixel_y_to_vram_always(y)
    );
}

int probe_layout(void)
{
    return (
        sizeof(scroll_line) + sizeof(scroll_subpixel_line) +
        sizeof(scroll_speed) + sizeof(scroll_line_on_page) +
        sizeof(scroll_last_delta) + sizeof(scroll_active)
    );
}
'''
STORAGE_TEST = r'''#include <stdio.h>
#include "th04/main/scroll.hpp"

static int fail(int number)
{
    printf("SCROLL_FAIL_%d\n", number);
    return number;
}

int main(void)
{
    if(sizeof(scroll_line) != 2 || sizeof(scroll_subpixel_line) != 1 ||
       sizeof(scroll_speed) != 1 || sizeof(scroll_line_on_page) != 4 ||
       sizeof(scroll_last_delta) != 2 || sizeof(scroll_active) != 1) {
        return fail(1);
    }
    if(scroll_line || scroll_subpixel_line.v || scroll_speed.v ||
       scroll_line_on_page[0] || scroll_line_on_page[1] ||
       scroll_last_delta.v || scroll_active) {
        return fail(2);
    }

    scroll_line = 399;
    scroll_subpixel_line.v = 15;
    scroll_speed.v = 7;
    scroll_line_on_page[0] = 123;
    scroll_line_on_page[1] = 321;
    scroll_last_delta.v = 0x1234;
    scroll_active = true;
    if(scroll_line != 399 || scroll_subpixel_line.v != 15 ||
       scroll_speed.v != 7 || scroll_line_on_page[0] != 123 ||
       scroll_line_on_page[1] != 321 || scroll_last_delta.v != 0x1234 ||
       !scroll_active) {
        return fail(3);
    }
    puts("SCROLL_PASS");
    return 0;
}
'''


def sha(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def semantic_omf_sha256(data: bytes) -> str:
    """Hash link semantics while ignoring Borland dependency comments."""

    digest = hashlib.sha256()
    offset = 0
    while offset < len(data):
        if offset + 3 > len(data):
            raise ValueError("truncated OMF record header")
        record_type = data[offset]
        size = int.from_bytes(data[offset + 1:offset + 3], "little")
        end = offset + 3 + size
        if end > len(data):
            raise ValueError("truncated OMF record")
        if record_type != 0x88:  # COMENT
            digest.update(data[offset:end])
        offset = end
    return digest.hexdigest()


def run(
    command: list[str], work: Path, env: dict[str, str], log: Path,
) -> subprocess.CompletedProcess[str]:
    result = subprocess.run(
        command, cwd=work, env=env, capture_output=True, text=True, timeout=240
    )
    log.write_text(
        json.dumps(command) + f"\nexit={result.returncode}\n"
        + result.stdout + result.stderr,
        encoding="utf-8",
    )
    return result


def compile_header(
    label: str,
    game: int,
    root: Path,
    includes: tuple[str, ...],
    env: dict[str, str],
    output: Path,
) -> dict[str, object]:
    object_dir = root / f"obj{game}"
    object_dir.mkdir()
    source = root / f"header{game}.cpp"
    source.write_text(HEADER_TEST, encoding="ascii")
    command = [
        "wine", str(RUNNER), "-e", "-x", "tcc", *FLAGS,
        f"-DGAME={game}", *(f"-I{include}" for include in includes),
        f"-nobj{game}/", source.name,
    ]
    result = run(command, root, env, output / f"compile-{label}-g{game}.log")
    if result.returncode:
        raise RuntimeError(f"TC4J {label} GAME={game} header compile failed")
    obj = object_dir / f"header{game}.obj"
    data = obj.read_bytes()
    omf = describe_omf(data)
    if not omf["valid"] or "TC86 Borland C++ 4.02" not in omf["translator_comments"]:
        raise RuntimeError(f"unexpected {label} GAME={game} OMF")
    return {
        "object_sha256": hashlib.sha256(data).hexdigest(),
        "semantic_omf_sha256": semantic_omf_sha256(data),
        "record_counts": omf["record_counts"],
        "dependency_paths": omf["dependency_paths"],
    }


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output-dir", required=True, type=Path)
    args = parser.parse_args()
    output = args.output_dir.resolve()
    if output.exists() or not output.is_relative_to(PRIVATE):
        parser.error("output must be a new private directory")

    subprocess.run(
        [sys.executable, "scripts/preflight.py"], cwd=ROOT, check=True,
        stdout=subprocess.DEVNULL,
    )
    subprocess.run(
        [sys.executable, "scripts/attest_toolchain.py"], cwd=ROOT, check=True,
        stdout=subprocess.DEVNULL,
    )
    if sha(RUNNER) != RUNNER_SHA256:
        raise ValueError("MS-DOS Player identity drift")

    output.mkdir(parents=True)
    reference = output / "reference"
    local = output / "local"
    reference.mkdir()
    local.mkdir()
    for relative_text in REFERENCE_FILES:
        relative = Path(relative_text)
        destination = reference / "tree" / relative
        destination.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(ROOT / "_reference/ReC98" / relative, destination)
    shutil.copytree(ROOT / "src", local / "src")

    env = os.environ.copy()
    env.update(
        WINEPREFIX=str(ROOT / ".analysis/toolchain/wineprefix"),
        WINEDEBUG="-all",
        MSDOS_PATH=r"C:\TC4\BIN;C:\TASM50\BIN",
    )

    header_results: dict[str, object] = {}
    header_passed = True
    for game in (4, 5):
        reference_result = compile_header(
            "reference", game, reference, ("tree",), env, output
        )
        local_result = compile_header(
            "local", game, local, ("src/main/include", "."), env, output
        )
        equal = (
            reference_result["semantic_omf_sha256"]
            == local_result["semantic_omf_sha256"]
        )
        header_passed &= equal
        header_results[str(game)] = {
            "reference": reference_result,
            "local": local_result,
            "semantic_equal": equal,
        }

    work = output / "storage"
    shutil.copytree(ROOT / "src", work / "src")
    (work / "obj").mkdir()
    (work / "bin").mkdir()
    test = work / "storage.cpp"
    test.write_text(STORAGE_TEST, encoding="ascii")
    compile_command = [
        "wine", str(RUNNER), "-e", "-x", "tcc", *FLAGS, "-DGAME=4",
        "-Isrc/main/include", "-I.", "-nobj/", test.name,
    ]
    if run(compile_command, work, env, output / "compile-storage.log").returncode:
        raise RuntimeError("TC4J scroll storage consumer failed")

    objects: dict[str, object] = {}
    for stem, source in (("state", STATE_SOURCE), ("page_state", PAGE_SOURCE)):
        dos_source = str(source.relative_to(ROOT)).replace("/", "\\")
        command = [
            "wine", "cmd", "/d", "/c",
            "set PATH=C:\\TASM50\\BIN;C:\\TC4\\BIN;%PATH%&&"
            f"tasm32 /m /mx /kh32768 /t {dos_source} obj\\{stem}.obj",
        ]
        if run(command, work, env, output / f"assemble-{stem}.log").returncode:
            raise RuntimeError(f"TASM {stem} failed")
        obj = work / "obj" / f"{stem}.obj"
        omf = describe_omf(obj.read_bytes())
        if not omf["valid"] or not any(
            "Turbo Assembler  Version 5.0" in comment
            for comment in omf["translator_comments"]
        ):
            raise RuntimeError(f"unexpected {stem} OMF structure or producer")
        objects[stem] = {
            "sha256": sha(obj),
            "record_counts": omf["record_counts"],
            "translator_comments": omf["translator_comments"],
        }

    response = work / "obj/link.rsp"
    response.write_text(
        "-c -s -E c0l.obj obj\\state.obj obj\\page_state.obj "
        "obj\\storage.obj, bin\\scroll.exe, obj\\scroll.map, "
        "emu.lib mathl.lib cl.lib\n",
        encoding="ascii",
    )
    if run(
        ["wine", str(RUNNER), "-e", "-x", "tlink", "@obj\\link.rsp"],
        work, env, output / "link.log",
    ).returncode:
        raise RuntimeError("TLINK scroll storage probe failed")

    exe = work / "bin/scroll.exe"
    mz = parse_mz(exe.read_bytes())
    if not mz.valid:
        raise RuntimeError("scroll storage probe is not a valid MZ")
    runtime = run(
        ["wine", str(RUNNER), "-e", "-x", "bin\\scroll.exe"],
        work, env, output / "runtime.log",
    )
    runtime_passed = runtime.returncode == 0 and "SCROLL_PASS" in runtime.stdout
    passed = header_passed and runtime_passed
    receipt = {
        "schema_version": 1,
        "observed_utc": datetime.now(timezone.utc).isoformat(timespec="seconds"),
        "scope": "TH04 MAIN scroll declaration and split BSS-owner ABI",
        "runner_sha256": RUNNER_SHA256,
        "compiler_flags": FLAGS,
        "header_sha256": sha(HEADER),
        "state_source_sha256": sha(STATE_SOURCE),
        "page_source_sha256": sha(PAGE_SOURCE),
        "header_harness_sha256": hashlib.sha256(
            HEADER_TEST.encode("ascii")
        ).hexdigest(),
        "storage_harness_sha256": sha(test),
        "header_results": header_results,
        "storage_objects": objects,
        "storage_test_object_sha256": sha(work / "obj/storage.obj"),
        "mz_sha256": sha(exe),
        "mz_relocations": len(mz.relocations),
        "runtime_log_sha256": sha(output / "runtime.log"),
        "runtime_returncode": runtime.returncode,
        "runtime_passed": runtime_passed,
        "passed": passed,
        "limit": (
            "Compiler-observed GAME4/GAME5 header ABI and runtime-observed DOS "
            "BSS behavior; target BSS offsets and complete MAIN execution remain open."
        ),
    }
    (output / "receipt.json").write_text(
        json.dumps(receipt, indent=2, sort_keys=True) + "\n", encoding="utf-8"
    )
    print(json.dumps({"passed": passed, "mz_sha256": sha(exe)}, sort_keys=True))
    return 0 if passed else 1


if __name__ == "__main__":
    raise SystemExit(main())
