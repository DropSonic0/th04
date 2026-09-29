#!/usr/bin/env python3
"""Assemble the maintained TH04 MAIN DATA/BSS state owners.

This records the physical storage producers that are outside the historical
MAIN C/C++ source list.  It is deliberately limited to TASM OMF production:
it does not assign final DGROUP offsets, link MAIN, or claim startup behavior.
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
PRIVATE = (ROOT / ".analysis/reconstruction/probes").resolve()
RUNNER = ROOT / "_reference/ReC98/bin/msdos.exe"
RUNNER_SHA256 = "f7f6cb0a3e816c5edb13112d327c1bddbf7463fe7bf9a005ca1eb5317751bd02"
SOURCES = (
    Path("src/main/core/frame_state.asm"),
    Path("src/main/core/quit_state.asm"),
    Path("src/main/core/slowdown_state.asm"),
    Path("src/main/dialog/data.asm"),
    Path("src/main/dialog/script_state.asm"),
    Path("src/main/dialog/state.asm"),
    Path("src/main/scroll/page_state.asm"),
    Path("src/main/scroll/state.asm"),
)

sys.path.insert(0, str(ROOT / "scripts"))
from lib.omf import describe_omf  # noqa: E402


def sha256(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def run(command: list[str], cwd: Path, env: dict[str, str], log: Path) -> int:
    result = subprocess.run(
        command, cwd=cwd, env=env, capture_output=True, text=True, timeout=240
    )
    log.write_text(
        json.dumps(command) + f"\nexit={result.returncode}\n"
        + result.stdout + result.stderr,
        encoding="utf-8",
    )
    return result.returncode


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output-dir", required=True, type=Path)
    args = parser.parse_args()
    output = args.output_dir.resolve()
    if output.exists() or not output.is_relative_to(PRIVATE):
        parser.error("output must be a new private directory")
    if any(not (ROOT / source).is_file() for source in SOURCES):
        raise RuntimeError("one or more configured DATA/BSS owners are missing")

    subprocess.run(
        [sys.executable, "scripts/preflight.py"], cwd=ROOT, check=True,
        stdout=subprocess.DEVNULL,
    )
    subprocess.run(
        [sys.executable, "scripts/attest_toolchain.py"], cwd=ROOT, check=True,
        stdout=subprocess.DEVNULL,
    )
    if sha256(RUNNER) != RUNNER_SHA256:
        raise RuntimeError("pinned TC4J runner identity drift")

    output.mkdir(parents=True)
    work = output / "source"
    shutil.copytree(ROOT / "src", work / "src")
    (work / "obj").mkdir()
    env = os.environ.copy()
    env.update(
        WINEPREFIX=str(ROOT / ".analysis/toolchain/wineprefix"),
        WINEDEBUG="-all",
        MSDOS_PATH=r"C:\TC4\BIN;C:\TASM50\BIN",
    )

    records: list[dict[str, object]] = []
    for index, relative in enumerate(SOURCES):
        source = work / relative
        obj = work / "obj" / f"u{index:03d}.obj"
        dos_source = relative.as_posix().replace("/", "\\")
        dos_obj = obj.relative_to(work).as_posix().replace("/", "\\")
        command = [
            "wine", "cmd", "/d", "/c",
            r"set PATH=C:\TASM50\BIN;C:\TC4\BIN;%PATH%&&"
            + f"tasm32 /m /mx /kh32768 /t /dGAME=4 "
            f"/dTH04_LARGE_PRODUCT=1 {dos_source} {dos_obj}",
        ]
        log = output / f"assemble-{index:03d}.log"
        exit_code = run(command, work, env, log)
        omf = describe_omf(obj.read_bytes()) if obj.is_file() else None
        records.append({
            "index": index,
            "source": relative.as_posix(),
            "source_sha256": sha256(ROOT / relative),
            "object": obj.relative_to(work).as_posix() if obj.is_file() else None,
            "object_sha256": sha256(obj) if obj.is_file() else None,
            "assemble_exit": exit_code,
            "omf_valid": bool(omf and omf["valid"]),
            "omf_record_counts": omf["record_counts"] if omf else None,
            "translator_comments": omf["translator_comments"] if omf else [],
            "log": log.relative_to(output).as_posix(),
        })

    passed = sum(item["assemble_exit"] == 0 and item["omf_valid"]
                 for item in records)
    receipt = {
        "schema_version": 1,
        "observed_utc": datetime.now(timezone.utc).isoformat(timespec="seconds"),
        "artifact": "th04-main",
        "scope": "physical DATA/BSS state owners",
        "runner_sha256": RUNNER_SHA256,
        "assembler_flags": ["/m", "/mx", "/kh32768", "/t", "/dGAME=4",
                             "/dTH04_LARGE_PRODUCT=1"],
        "source_count": len(SOURCES),
        "assemble_pass": passed,
        "assemble_fail": len(SOURCES) - passed,
        "records": records,
        "link_performed": False,
        "mz_performed": False,
        "runtime_performed": False,
        "limit": (
            "Compiler-observed TASM OMF producers only; final DGROUP/BSS offsets, "
            "native MAIN TLINK/MZ placement, relocation/layout, and startup remain open."
        ),
    }
    (output / "receipt.json").write_text(
        json.dumps(receipt, indent=2, sort_keys=True) + "\n", encoding="utf-8"
    )
    print(json.dumps({
        "artifact": "th04-main",
        "source_count": len(SOURCES),
        "assemble_pass": passed,
        "assemble_fail": len(SOURCES) - passed,
    }, sort_keys=True))
    return 0 if passed == len(SOURCES) else 1


if __name__ == "__main__":
    raise SystemExit(main())
