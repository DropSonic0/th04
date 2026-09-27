#!/usr/bin/env python3
"""Compile the current TH04 OP, MAINE, and shared C/C++ source without ReC98 headers.

This is a source-closure diagnostic. It does not link a product executable.
"""

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

RUNNER = ROOT / "_reference/ReC98/bin/msdos.exe"
RUNNER_SHA256 = "f7f6cb0a3e816c5edb13112d327c1bddbf7463fe7bf9a005ca1eb5317751bd02"
FLAGS = ("-c", "-I.", "-O", "-b-", "-3", "-Z", "-d", "-DGAME=4", "-ml")


def sha(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def run(command: list[str], work: Path, env: dict[str, str], log: Path) -> subprocess.CompletedProcess[str]:
    result = subprocess.run(command, cwd=work, env=env, capture_output=True,
                            text=True, timeout=180)
    log.write_text(json.dumps(command) + f"\nexit={result.returncode}\n"
                   + result.stdout + result.stderr, encoding="utf-8")
    return result


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output-dir", type=Path, required=True)
    args = parser.parse_args()
    output = args.output_dir.resolve()
    private = (ROOT / ".analysis/reconstruction/probes").resolve()
    if output.exists() or not output.is_relative_to(private):
        parser.error("output must be a new directory below .analysis/reconstruction/probes")
    subprocess.run([sys.executable, "scripts/preflight.py"], cwd=ROOT, check=True,
                   stdout=subprocess.DEVNULL)
    subprocess.run([sys.executable, "scripts/attest_toolchain.py"], cwd=ROOT, check=True,
                   stdout=subprocess.DEVNULL)
    if sha(RUNNER) != RUNNER_SHA256:
        raise RuntimeError("pinned DOS runner identity drift")

    output.mkdir(parents=True)
    work = output / "source"
    shutil.copytree(ROOT / "src", work / "src")
    source_paths = {
        group: sorted(path.relative_to(ROOT) for path in (ROOT / "src" / group).rglob("*")
                      if path.suffix in (".cpp", ".c"))
        for group in ("op", "maine", "shared")
    }
    env = os.environ.copy()
    env.update(WINEPREFIX=str(ROOT / ".analysis/toolchain/wineprefix"),
               WINEDEBUG="-all", MSDOS_PATH=r"C:\TC4\BIN;C:\TASM50\BIN")
    receipts = []
    for group, paths in source_paths.items():
        for index, source in enumerate(paths):
            source_str = source.as_posix()
            obj_dir = work / "obj" / group / f"{index:03d}"
            obj_dir.mkdir(parents=True)
            extra = ["-B"] if source_str == "src/shared/hardware/bgimage.cpp" else []
            if group in ("op", "maine"):
                extra.append("-DBINARY='O'" if group == "op" else "-DBINARY='E'")
            command = ["wine", str(RUNNER), "-e", "-x", "tcc", *extra,
                       *FLAGS, f"-nobj/{group}/{index:03d}/", source_str]
            log = output / f"compile-{group}-{index:03d}.log"
            result = run(command, work, env, log)
            if source_str == "src/shared/hardware/bgimage.cpp":
                generated = list(obj_dir.glob("*.asm"))
                if len(generated) != 1 or "Unable to execute command 'tasm.exe'" not in (result.stdout + result.stderr):
                    raise RuntimeError(f"BGIMAGE symbolic ASM generation failed; see {log}")
                asm_rel = generated[0].relative_to(work).as_posix().replace("/", "\\")
                obj_rel = generated[0].with_suffix(".obj").relative_to(work).as_posix().replace("/", "\\")
                assembly = ["wine", "cmd", "/d", "/c",
                            "set PATH=C:\\TASM50\\BIN;C:\\TC4\\BIN;%PATH%&&"
                            f"tasm32 /m /mx /kh32768 /t /dGAME=4 {asm_rel} {obj_rel}"]
                assembly_log = output / f"assemble-{group}-{index:03d}.log"
                if run(assembly, work, env, assembly_log).returncode:
                    raise RuntimeError(f"BGIMAGE assembly failed; see {assembly_log}")
            elif result.returncode:
                raise RuntimeError(f"TH04 source compilation failed: {source_str}; see {log}")
            objects = list(obj_dir.glob("*.obj"))
            if len(objects) != 1:
                raise RuntimeError(f"expected one OMF object for {source_str}; see {log}")
            omf = describe_omf(objects[0].read_bytes())
            expected = "Turbo Assembler  Version 5.0" if extra[:1] == ["-B"] else "TC86 Borland C++ 4.02"
            if not any(expected in comment for comment in omf["translator_comments"]):
                raise RuntimeError(f"unexpected OMF producer: {source_str}: {omf['translator_comments']}")
            receipts.append({"source": source_str, "source_sha256": sha(ROOT / source),
                             "object": objects[0].relative_to(work).as_posix(),
                             "object_sha256": sha(objects[0]),
                             "producer": omf["translator_comments"]})
    receipt = {"schema_version": 1,
               "observed_utc": datetime.now(timezone.utc).isoformat(timespec="seconds"),
               "scope": "TH04-owned OP, MAINE, and shared C/C++ compile closure only",
               "runner_sha256": RUNNER_SHA256, "flags": list(FLAGS),
               "sources": receipts,
               "limit": "Entry INL bodies, MAIN, ZUN, assembly objects, TLINK, MZ, relocation and runtime acceptance remain open."}
    (output / "receipt.json").write_text(json.dumps(receipt, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({group: len(paths) for group, paths in source_paths.items()}, sort_keys=True))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
