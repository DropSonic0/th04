#!/usr/bin/env python3
"""Compile and execute a DOS ABI probe for MAIN slowdown state."""

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
HEADER = ROOT / "src/main/include/th04/main/slowdown.hpp"
STATE_SOURCE = ROOT / "src/main/core/slowdown_state.asm"
FRAME_SOURCE = ROOT / "src/main/core/frame_state.asm"
FLAGS = (
    "-c", "-I.", "-Isrc/main/include", "-O", "-b-", "-3", "-Z", "-d",
    "-DGAME=4", "-DTH04P", "-ml",
)
TEST = r'''#include <stdio.h>
#include "th04/main/slowdown.hpp"

static int fail(int number)
{
    printf("SLOWDOWN_FAIL_%d\n", number);
    return number;
}

int main(void)
{
    if(sizeof(turbo_mode) != 1 || sizeof(slowdown_factor) != 2) {
        return fail(1);
    }

    turbo_mode = false;
    slowdown_factor = 0x1234;
    if(turbo_mode != false || slowdown_factor != 0x1234) {
        return fail(2);
    }

    turbo_mode = true;
    slowdown_factor = 1;
    if(turbo_mode != true || slowdown_factor != 1) {
        return fail(3);
    }

    puts("SLOWDOWN_PASS");
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

    for stem in ("frame_state", "slowdown_state"):
        command = [
            "wine", "cmd", "/d", "/c",
            "set PATH=C:\\TASM50\\BIN;C:\\TC4\\BIN;%PATH%&&"
            "tasm32 /m /mx /kh32768 /t /dGAME=4 /dTH04_LARGE_PRODUCT=1 "
            f"src\\main\\core\\{stem}.asm obj\\{stem}.obj",
        ]
        if run(command, work, env, output / f"assemble-{stem}.log").returncode:
            raise RuntimeError(f"TASM failed for {stem}; inspect its log")

    frame_object = work / "obj/frame_state.obj"
    state_object = work / "obj/slowdown_state.obj"
    frame_omf = describe_omf(frame_object.read_bytes())
    state_omf = describe_omf(state_object.read_bytes())
    if (
        not frame_omf["valid"]
        or frame_omf["record_counts"].get("PUBDEF") != 10
        or not state_omf["valid"]
        or state_omf["record_counts"].get("PUBDEF") != 1
        or not all(
            any("Turbo Assembler  Version 5.0" in comment for comment in omf["translator_comments"])
            for omf in (frame_omf, state_omf)
        )
    ):
        raise RuntimeError("unexpected slowdown-state OMF structure or producer")

    response = work / "obj/link.rsp"
    response.write_text(
        "-c -s -E c0l.obj obj\\frame_state.obj obj\\slowdown_state.obj "
        "obj\\test.obj, bin\\slowtest.exe, obj\\slowtest.map, "
        "emu.lib mathl.lib cl.lib\n",
        encoding="ascii",
    )
    if run(
        ["wine", str(RUNNER), "-e", "-x", "tlink", "@obj\\link.rsp"],
        work, env, output / "link.log",
    ).returncode:
        raise RuntimeError("TLINK failed; inspect link.log")

    exe = work / "bin/slowtest.exe"
    mz = parse_mz(exe.read_bytes())
    if not mz.valid:
        raise RuntimeError("linked slowdown-state DOS probe is not a valid MZ")
    result = run(
        ["wine", str(RUNNER), "-e", "-x", "bin\\slowtest.exe"],
        work, env, output / "runtime.log",
    )
    passed = result.returncode == 0 and "SLOWDOWN_PASS" in result.stdout
    receipt = {
        "schema_version": 1,
        "observed_utc": datetime.now(timezone.utc).isoformat(timespec="seconds"),
        "scope": "TH04 MAIN slowdown declaration and turbo-mode storage ABI",
        "header_sha256": sha(HEADER),
        "state_source_sha256": sha(STATE_SOURCE),
        "frame_source_sha256": sha(FRAME_SOURCE),
        "harness_sha256": sha(test),
        "runner_sha256": RUNNER_SHA256,
        "compiler_flags": FLAGS,
        "state_object_sha256": sha(state_object),
        "state_object_valid": state_omf["valid"],
        "state_object_pubdef_records": state_omf["record_counts"]["PUBDEF"],
        "state_object_producer": state_omf["translator_comments"],
        "frame_object_sha256": sha(frame_object),
        "frame_object_pubdef_records": frame_omf["record_counts"]["PUBDEF"],
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
