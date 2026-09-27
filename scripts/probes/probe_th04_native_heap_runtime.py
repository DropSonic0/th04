#!/usr/bin/env python3
"""Run a small DOS heap lifecycle program against the TH04-local owner."""

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
SOURCE = ROOT / "src/shared/memory/heap.cpp"
FLAGS = ("-c", "-I.", "-O", "-b-", "-3", "-Z", "-d", "-DGAME=4", "-DTH04P", "-ml")
TEST = r'''#include <stdio.h>
#include "src/shared/runtime/api.hpp"

static int fail(int number)
{
    printf("HEAP_FAIL_%d\n", number);
    return number;
}

int main(void)
{
    if(mem_assign_dos(512) != 0) return fail(1);
    void __seg *a = hmem_allocbyte(16);
    void __seg *b = hmem_allocbyte(64);
    void __seg *c = hmem_allocbyte(16);
    if(!a || !b || !c || a == b || b == c || a == c) return fail(2);
    unsigned char far *pa = (unsigned char far *)MK_FP(a, 0);
    unsigned char far *pb = (unsigned char far *)MK_FP(b, 0);
    unsigned char far *pc = (unsigned char far *)MK_FP(c, 0);
    pa[0] = 0xA1;
    pb[0] = 0xB2;
    pc[0] = 0xC3;
    if(pa[0] != 0xA1 || pb[0] != 0xB2 || pc[0] != 0xC3) return fail(3);
    hmem_free(b);
    void __seg *d = hmem_allocbyte(16);
    if(d != b) return fail(4);
    void __seg *tail = hmem_allocbyte(32);
    if((unsigned)tail != (unsigned)b + 2u) return fail(11);
    unsigned char far *pt = (unsigned char far *)MK_FP(tail, 0);
    pt[31] = 0xD4;
    if(pt[31] != 0xD4 || pa[0] != 0xA1 || pc[0] != 0xC3) return fail(12);
    hmem_free(tail);
    hmem_free(c);
    hmem_free(d);
    hmem_free(d);
    hmem_free(a);
    void __seg *e = hmem_allocbyte(8000);
    if(!e) return fail(5);
    unsigned char far *pe = (unsigned char far *)MK_FP(e, 0);
    pe[0] = 0x5A;
    pe[7999] = 0xA5;
    if(pe[0] != 0x5A || pe[7999] != 0xA5) return fail(6);
    hmem_free(e);
    if(mem_unassign() != 1 || mem_unassign() != 1) return fail(7);
    if(mem_assign_dos(512) != 0) return fail(8);
    void __seg *f = hmem_allocbyte(16);
    if(!f) return fail(9);
    hmem_free(f);
    if(mem_unassign() != 1) return fail(10);
    puts("HEAP_PASS");
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
    env.update(WINEPREFIX=str(ROOT / ".analysis/toolchain/wineprefix"),
               WINEDEBUG="-all", MSDOS_PATH=r"C:\TC4\BIN;C:\TASM50\BIN")
    for name in ("src/shared/memory/heap.cpp", "test.cpp"):
        command = ["wine", str(RUNNER), "-e", "-x", "tcc", *FLAGS,
                   "-nobj/", name]
        log = output / ("compile-heap.log" if name.startswith("src/") else "compile-test.log")
        if run(command, work, env, log).returncode:
            raise RuntimeError(f"TC4J failed: {log}")
    response = work / "obj/link.rsp"
    response.write_text(
        "-c -s -E c0l.obj obj\\heap.obj obj\\test.obj, "
        "bin\\heaptest.exe, obj\\heaptest.map, emu.lib mathl.lib cl.lib\n",
        encoding="ascii",
    )
    link = run(["wine", str(RUNNER), "-e", "-x", "tlink", "@obj\\link.rsp"],
               work, env, output / "link.log")
    if link.returncode:
        raise RuntimeError("TLINK failed; inspect link.log")
    exe = work / "bin/heaptest.exe"
    result = run(["wine", str(RUNNER), "-e", "-x", "bin\\heaptest.exe"],
                 work, env, output / "runtime.log")
    passed = result.returncode == 0 and "HEAP_PASS" in result.stdout
    receipt = {
        "schema_version": 1,
        "observed_utc": datetime.now(timezone.utc).isoformat(timespec="seconds"),
        "source_sha256": sha(SOURCE),
        "harness_sha256": sha(test),
        "runner_sha256": RUNNER_SHA256,
        "compiler_flags": FLAGS,
        "heap_object_sha256": sha(work / "obj/heap.obj"),
        "test_object_sha256": sha(work / "obj/test.obj"),
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
