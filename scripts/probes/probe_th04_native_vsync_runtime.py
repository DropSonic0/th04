#!/usr/bin/env python3
"""Exercise TH04's interrupt-vector lifecycle in a pinned DOS runner."""

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
PRIVATE = (ROOT / ".analysis/reconstruction/probes").resolve()
RUNNER = ROOT / "_reference/ReC98/bin/msdos.exe"
RUNNER_SHA256 = "f7f6cb0a3e816c5edb13112d327c1bddbf7463fe7bf9a005ca1eb5317751bd02"
SOURCE = ROOT / "src/shared/hardware/vsync_irq.asm"
TEST = r'''#include <dos.h>
#include <stdio.h>
#include "src/shared/runtime/api.hpp"

static volatile unsigned callback_count;

void pascal count_callback(void)
{
    callback_count++;
}

static int fail(int n)
{
    printf("VSYNC_FAIL_%d\n", n);
    return n;
}

int main(void)
{
    void interrupt (*old_irq)(...) = getvect(0x0A);
    void interrupt (*old_crt)(...) = getvect(0x18);
    vsync_start();
    if(getvect(0x0A) == old_irq || getvect(0x18) == old_crt) return fail(1);
    if(vsync_Count1 != 0 || vsync_Count2 != 0) return fail(2);
    vsync_proc_set(count_callback);
    asm int 0Ah;
    if(vsync_Count1 != 1 || vsync_Count2 != 1 || callback_count != 1) return fail(3);
    vsync_proc_reset();
    vsync_start();
    if(vsync_Count1 != 0 || vsync_Count2 != 0) return fail(4);
    asm int 0Ah;
    if(vsync_Count1 != 1 || vsync_Count2 != 1) return fail(5);
    vsync_end();
    if(getvect(0x0A) != old_irq || getvect(0x18) != old_crt) return fail(6);
    vsync_end();
    if(getvect(0x0A) != old_irq || getvect(0x18) != old_crt) return fail(7);
    vsync_start();
    vsync_end();
    if(getvect(0x0A) != old_irq || getvect(0x18) != old_crt) return fail(8);
    puts("VSYNC_PASS");
    return 0;
}
'''


def sha(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def run(command: list[str], work: Path, env: dict[str, str], log: Path) -> subprocess.CompletedProcess[str]:
    result = subprocess.run(command, cwd=work, env=env, capture_output=True,
                            text=True, timeout=240)
    log.write_text(json.dumps(command) + f"\nexit={result.returncode}\n"
                   + result.stdout + result.stderr, encoding="utf-8")
    return result


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output-dir", required=True, type=Path)
    args = parser.parse_args()
    output = args.output_dir.resolve()
    if output.exists() or not output.is_relative_to(PRIVATE):
        parser.error("output must be a new private directory")
    subprocess.run([sys.executable, "scripts/preflight.py"], cwd=ROOT, check=True,
                   stdout=subprocess.DEVNULL)
    subprocess.run([sys.executable, "scripts/attest_toolchain.py"], cwd=ROOT,
                   check=True, stdout=subprocess.DEVNULL)
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
    env.update(WINEPREFIX=str(ROOT / ".analysis/toolchain/wineprefix"), WINEDEBUG="-all",
               MSDOS_PATH=r"C:\TC4\BIN;C:\TASM50\BIN")

    asm = ("wine", "cmd", "/d", "/c",
           "set PATH=C:\\TASM50\\BIN;C:\\TC4\\BIN;%PATH%&&"
           "tasm32 /m /mx /kh32768 /t /dGAME=4 /dTH04_LARGE_PRODUCT=1 "
           "src\\shared\\hardware\\vsync_irq.asm obj\\vsync.obj")
    if run(list(asm), work, env, output / "assemble.log").returncode:
        raise RuntimeError("TASM failed; inspect assemble.log")
    compile_command = ["wine", str(RUNNER), "-e", "-x", "tcc", "-c", "-I.",
                       "-O", "-b-", "-3", "-Z", "-d", "-DGAME=4", "-DTH04P",
                       "-ml", "-nobj/", "test.cpp"]
    if run(compile_command, work, env, output / "compile.log").returncode:
        raise RuntimeError("TC4J failed; inspect compile.log")

    (work / "obj/link.rsp").write_text(
        "-c -s -E c0l.obj obj\\vsync.obj obj\\test.obj, "
        "bin\\vsynctest.exe, obj\\vsynctest.map, emu.lib mathl.lib cl.lib\n",
        encoding="ascii",
    )
    if run(["wine", str(RUNNER), "-e", "-x", "tlink", "@obj\\link.rsp"],
           work, env, output / "link.log").returncode:
        raise RuntimeError("TLINK failed; inspect link.log")
    exe = work / "bin/vsynctest.exe"
    result = run(["wine", str(RUNNER), "-e", "-x", "bin\\vsynctest.exe"],
                 work, env, output / "runtime.log")
    passed = result.returncode == 0 and "VSYNC_PASS" in result.stdout
    receipt = {
        "schema_version": 1,
        "observed_utc": datetime.now(timezone.utc).isoformat(timespec="seconds"),
        "source_sha256": sha(SOURCE),
        "harness_sha256": sha(test),
        "runner_sha256": RUNNER_SHA256,
        "object_sha256": sha(work / "obj/vsync.obj"),
        "mz_sha256": sha(exe),
        "runtime_log_sha256": sha(output / "runtime.log"),
        "returncode": result.returncode,
        "passed": passed,
    }
    (output / "receipt.json").write_text(json.dumps(receipt, indent=2, sort_keys=True) + "\n",
                                          encoding="utf-8")
    print(json.dumps({"passed": passed, "returncode": result.returncode,
                      "runtime_log": str(output / "runtime.log")}, sort_keys=True))
    return 0 if passed else 1


if __name__ == "__main__":
    raise SystemExit(main())
