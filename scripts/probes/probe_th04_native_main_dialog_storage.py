#!/usr/bin/env python3
"""Probe the maintained TH04 MAIN dialog DATA/BSS storage owners."""

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
from lib.omf import describe_omf, parse_omf  # noqa: E402
from lib.pc98 import parse_mz  # noqa: E402

PRIVATE = (ROOT / ".analysis/reconstruction/probes").resolve()
RUNNER = ROOT / "_reference/ReC98/bin/msdos.exe"
RUNNER_SHA256 = "f7f6cb0a3e816c5edb13112d327c1bddbf7463fe7bf9a005ca1eb5317751bd02"
TARGET = ROOT / ".analysis/targets/th04/main.exe"
TARGET_SHA256 = "077440a3c4e9ab52e72e9bae411276c47edc11995b5c2b83dfc83fbc039dc58b"
HEADER = ROOT / "src/main/dialog/state.hpp"
WRAPPER = ROOT / "src/main/include/th04/main/dialog/state.hpp"
STATE_SOURCE = ROOT / "src/main/dialog/state.asm"
DATA_SOURCE = ROOT / "src/main/dialog/data.asm"
TARGET_DATA_FILE_OFFSET = 0x243B2
TARGET_DATA = bytes.fromhex(
    "8888444422221111cccc666633339999"
    "eeee7777bbbbdddd"
)
FLAGS = ("-c", "-O", "-b-", "-3", "-Z", "-d", "-DGAME=4", "-ml")
TEST = r'''#include <stdio.h>
#include "th04/main/dialog/state.hpp"
#include "th04/main/dialog/dialog.hpp"
#include "th04/formats/dialog.hpp"

extern unsigned int BOX_TILES[3][4];

static bool near dialog_probe_update(void)
{
    return true;
}

static int fail(int number)
{
    printf("DIALOG_STORAGE_FAIL_%d\n", number);
    return number;
}

int main(void)
{
    if(sizeof(dialog_cursor_t) != 4 || sizeof(dialog_cursor) != 4 ||
       sizeof(dialog_side) != 2 || sizeof(std_update) != 2 ||
       sizeof(dialog_p) != 4 || sizeof(BOX_TILES) != 24) {
        return fail(1);
    }
    if(dialog_cursor.x || dialog_cursor.y || dialog_side ||
       std_update || dialog_p) {
        return fail(2);
    }
    if(BOX_TILES[0][0] != 0x8888 || BOX_TILES[0][3] != 0x1111 ||
       BOX_TILES[1][0] != 0xCCCC || BOX_TILES[1][3] != 0x9999 ||
       BOX_TILES[2][0] != 0xEEEE || BOX_TILES[2][3] != 0xDDDD) {
        return fail(3);
    }

    dialog_cursor.x = 320;
    dialog_cursor.y = 352;
    dialog_side = SIDE_BOSS;
    std_update = dialog_probe_update;
    dialog_p = reinterpret_cast<unsigned char far *>(0x1234UL);
    if(dialog_cursor.x != 320 || dialog_cursor.y != 352 ||
       dialog_side != SIDE_BOSS || std_update != dialog_probe_update ||
       !dialog_p) {
        return fail(4);
    }
    puts("DIALOG_STORAGE_PASS");
    return 0;
}
'''


def sha(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def semantic_omf_sha256(data: bytes) -> str:
    digest = hashlib.sha256()
    for record in parse_omf(data):
        if record.name != "COMENT":
            start = record.offset
            end = record.offset + 3 + record.length
            digest.update(data[start:end])
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


def ledata_payloads(data: bytes) -> list[bytes]:
    payloads: list[bytes] = []
    for record in parse_omf(data):
        if record.name not in {"LEDATA", "LEDATA32"}:
            continue
        if record.name == "LEDATA":
            if len(record.data) < 3:
                raise ValueError("short LEDATA record")
            payloads.append(record.data[3:])
        else:
            if len(record.data) < 5:
                raise ValueError("short LEDATA32 record")
            payloads.append(record.data[5:])
    return payloads


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
    if sha(TARGET) != TARGET_SHA256:
        # Fail closed rather than silently probing a different MAIN image.
        raise ValueError("TH04 MAIN target identity drift")

    output.mkdir(parents=True)
    work = output / "source"
    shutil.copytree(ROOT / "src", work / "src")
    (work / "obj").mkdir()
    (work / "bin").mkdir()
    # Keep the DOS-visible basename within 8.3 so TC4J does not silently
    # generate a different truncated object name.
    test = work / "dstore.cpp"
    test.write_text(TEST, encoding="ascii")
    env = os.environ.copy()
    env.update(
        WINEPREFIX=str(ROOT / ".analysis/toolchain/wineprefix"),
        WINEDEBUG="-all",
        MSDOS_PATH=r"C:\TC4\BIN;C:\TASM50\BIN",
    )

    compile_command = [
        "wine", str(RUNNER), "-e", "-x", "tcc", *FLAGS,
        "-Isrc/main/include", "-I.", "-nobj/", test.name,
    ]
    if run(compile_command, work, env, output / "compile.log").returncode:
        raise RuntimeError("TC4J dialog storage consumer failed")

    objects: dict[str, object] = {}
    for stem, source in (("dialog_state", STATE_SOURCE), ("dialog_data", DATA_SOURCE)):
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
            "semantic_omf_sha256": semantic_omf_sha256(obj.read_bytes()),
            "record_counts": omf["record_counts"],
            "translator_comments": omf["translator_comments"],
        }

    data_object = work / "obj/dialog_data.obj"
    if TARGET_DATA not in b"".join(ledata_payloads(data_object.read_bytes())):
        raise RuntimeError("dialog DATA LEDATA does not contain the expected mask")
    target_data = TARGET.read_bytes()[TARGET_DATA_FILE_OFFSET:
                                      TARGET_DATA_FILE_OFFSET + len(TARGET_DATA)]
    if target_data != TARGET_DATA:
        raise RuntimeError("target MAIN dialog DATA slice changed")

    response = work / "obj/link.rsp"
    response.write_text(
        "-c -s -E c0l.obj obj\\dialog_state.obj obj\\dialog_data.obj "
        "obj\\dstore.obj, bin\\dialogstorage.exe, "
        "obj\\dialogstorage.map, emu.lib mathl.lib cl.lib\n",
        encoding="ascii",
    )
    if run(
        ["wine", str(RUNNER), "-e", "-x", "tlink", "@obj\\link.rsp"],
        work, env, output / "link.log",
    ).returncode:
        raise RuntimeError("TLINK dialog storage probe failed")

    exe = work / "bin/dialogstorage.exe"
    mz = parse_mz(exe.read_bytes())
    if not mz.valid:
        raise RuntimeError("dialog storage probe is not a valid MZ")
    runtime = run(
        ["wine", str(RUNNER), "-e", "-x", "bin\\dialogstorage.exe"],
        work, env, output / "runtime.log",
    )
    runtime_passed = runtime.returncode == 0 and "DIALOG_STORAGE_PASS" in runtime.stdout
    passed = runtime_passed
    receipt = {
        "schema_version": 1,
        "observed_utc": datetime.now(timezone.utc).isoformat(timespec="seconds"),
        "scope": "TH04 MAIN dialog DATA/BSS storage owners",
        "runner_sha256": RUNNER_SHA256,
        "target_sha256": sha(TARGET),
        "compiler_flags": FLAGS,
        "header_sha256": sha(HEADER),
        "wrapper_sha256": sha(WRAPPER),
        "state_source_sha256": sha(STATE_SOURCE),
        "data_source_sha256": sha(DATA_SOURCE),
        "harness_sha256": hashlib.sha256(TEST.encode("ascii")).hexdigest(),
        "objects": objects,
        "target_data_file_offset": hex(TARGET_DATA_FILE_OFFSET),
        "target_data_sha256": hashlib.sha256(target_data).hexdigest(),
        "target_data_bytes": target_data.hex(),
        "mz_sha256": sha(exe),
        "mz_relocations": len(mz.relocations),
        "runtime_log_sha256": sha(output / "runtime.log"),
        "runtime_returncode": runtime.returncode,
        "runtime_passed": runtime_passed,
        "passed": passed,
        "limit": (
            "Runtime-observed isolated DOS storage ABI and target file-backed DATA "
            "slice only; target BSS offsets, complete MAIN placement, and PC-98 "
            "startup remain open."
        ),
    }
    (output / "receipt.json").write_text(
        json.dumps(receipt, indent=2, sort_keys=True) + "\n", encoding="utf-8"
    )
    print(json.dumps({"passed": passed, "mz_sha256": sha(exe)}, sort_keys=True))
    return 0 if passed else 1


if __name__ == "__main__":
    raise SystemExit(main())
