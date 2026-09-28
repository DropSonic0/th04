#!/usr/bin/env python3
"""Compile and execute a DOS ABI probe for MAIN quit state."""

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
HEADER = ROOT / "src/main/include/th04/main/quit.hpp"
SOURCE = ROOT / "src/main/core/quit_state.asm"
FLAGS = (
    "-c", "-I.", "-Isrc/main/include", "-O", "-b-", "-3", "-Z", "-d",
    "-DGAME=4", "-DTH04P", "-ml",
)
TEST = r'''#include <stdio.h>
#include "th04/main/quit.hpp"

static int fail(int number)
{
    printf("QUIT_FAIL_%d\n", number);
    return number;
}

int main(void)
{
    if(sizeof(quit_t) != 1 || sizeof(quit) != 1) {
        return fail(1);
    }

    quit = Q_KEEP_RUNNING;
    if(quit != Q_KEEP_RUNNING) return fail(2);
    quit = Q_QUIT_TO_OP;
    if(quit != Q_QUIT_TO_OP) return fail(3);
    quit = Q_NEXT_STAGE;
    if(quit != Q_NEXT_STAGE) return fail(4);

    puts("QUIT_PASS");
    return 0;
}
'''


def sha(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


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
    work = output / "source"
    shutil.copytree(ROOT / "src", work / "src")
    (work / "obj").mkdir()
    (work / "bin").mkdir()
    test = work / "test.cpp"
    test.write_text(TEST, encoding="ascii")
    env = os.environ.copy()
    env.update(
        WINEPREFIX=str(ROOT / ".analysis/toolchain/wineprefix"),
        WINEDEBUG="-all",
        MSDOS_PATH=r"C:\TC4\BIN;C:\TASM50\BIN",
    )

    compile_result = run(
        ["wine", str(RUNNER), "-e", "-x", "tcc", *FLAGS, "-nobj/", "test.cpp"],
        work, env, output / "compile.log",
    )
    if compile_result.returncode:
        raise RuntimeError("TC4J failed; inspect compile.log")

    assemble_command = [
        "wine", "cmd", "/d", "/c",
        "set PATH=C:\\TASM50\\BIN;C:\\TC4\\BIN;%PATH%&&"
        "tasm32 /m /mx /kh32768 /t /dGAME=4 /dTH04_LARGE_PRODUCT=1 "
        "src\\main\\core\\quit_state.asm obj\\quit_state.obj",
    ]
    if run(assemble_command, work, env, output / "assemble.log").returncode:
        raise RuntimeError("TASM failed; inspect assemble.log")

    state_object = work / "obj/quit_state.obj"
    state_omf = describe_omf(state_object.read_bytes())
    if (
        not state_omf["valid"]
        or state_omf["record_counts"].get("PUBDEF") != 1
        or not any(
            "Turbo Assembler  Version 5.0" in comment
            for comment in state_omf["translator_comments"]
        )
    ):
        raise RuntimeError("unexpected quit-state OMF structure or producer")

    response = work / "obj/link.rsp"
    response.write_text(
        "-c -s -E c0l.obj obj\\quit_state.obj obj\\test.obj, "
        "bin\\quittest.exe, obj\\quittest.map, emu.lib mathl.lib cl.lib\n",
        encoding="ascii",
    )
    if run(
        ["wine", str(RUNNER), "-e", "-x", "tlink", "@obj\\link.rsp"],
        work, env, output / "link.log",
    ).returncode:
        raise RuntimeError("TLINK failed; inspect link.log")

    exe = work / "bin/quittest.exe"
    mz = parse_mz(exe.read_bytes())
    if not mz.valid:
        raise RuntimeError("linked quit-state DOS probe is not a valid MZ")
    result = run(
        ["wine", str(RUNNER), "-e", "-x", "bin\\quittest.exe"],
        work, env, output / "runtime.log",
    )
    passed = result.returncode == 0 and "QUIT_PASS" in result.stdout
    receipt = {
        "schema_version": 1,
        "observed_utc": datetime.now(timezone.utc).isoformat(timespec="seconds"),
        "scope": "TH04 MAIN quit enum and storage ABI",
        "header_sha256": sha(HEADER),
        "source_sha256": sha(SOURCE),
        "harness_sha256": sha(test),
        "runner_sha256": RUNNER_SHA256,
        "compiler_flags": FLAGS,
        "state_object_sha256": sha(state_object),
        "state_object_valid": state_omf["valid"],
        "state_object_pubdef_records": state_omf["record_counts"]["PUBDEF"],
        "state_object_producer": state_omf["translator_comments"],
        "test_object_sha256": sha(work / "obj/test.obj"),
        "mz_sha256": sha(exe),
        "mz_relocations": len(mz.relocations),
        "runtime_log_sha256": sha(output / "runtime.log"),
        "returncode": result.returncode,
        "passed": passed,
        "limit": (
            "Runtime-observed DOS ABI/storage behavior only; target BSS offset, "
            "complete MAIN linking, and PC-98 execution remain unverified."
        ),
    }
    (output / "receipt.json").write_text(
        json.dumps(receipt, indent=2, sort_keys=True) + "\n", encoding="utf-8"
    )
    print(json.dumps({"passed": passed, "returncode": result.returncode}, sort_keys=True))
    return 0 if passed else 1


if __name__ == "__main__":
    raise SystemExit(main())
