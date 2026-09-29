#!/usr/bin/env python3
"""Compile every maintained TH04 MAIN C/C++ source in a product-only tree.

This is a compiler-only frontier.  It copies only the checked-in ``src`` tree,
uses the product include root, and gives each translation unit a short DOS
8.3 alias while preserving the original source path and digest in the receipt.
It does not assemble ASM owners, link an MZ, or claim standalone MAIN startup.
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
SOURCE_ROOTS = (ROOT / "src/main", ROOT / "src/shared")
SUFFIXES = {".c", ".cpp"}
FLAGS = (
    "-c", "-I.", "-Isrc/main/include", "-O", "-b-", "-3", "-Z", "-d",
    "-DGAME=4", "-DTH04P", "-ml", "-DBINARY='M'",
)

sys.path.insert(0, str(ROOT / "scripts"))
from lib.omf import describe_omf  # noqa: E402


def sha256_bytes(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def sha256(path: Path) -> str:
    return sha256_bytes(path.read_bytes())


def source_tree_digest(sources: list[Path]) -> str:
    value = hashlib.sha256()
    for source in sources:
        value.update(source.relative_to(ROOT).as_posix().encode("utf-8"))
        value.update(b"\0")
        value.update(bytes.fromhex(sha256(source)))
    return value.hexdigest()


def run(command: list[str], cwd: Path, env: dict[str, str], log: Path) -> int:
    result = subprocess.run(
        command,
        cwd=cwd,
        env=env,
        capture_output=True,
        text=True,
        timeout=240,
    )
    log.write_text(
        json.dumps(command) + f"\nexit={result.returncode}\n"
        + result.stdout + result.stderr,
        encoding="utf-8",
    )
    return result.returncode


def compile_source(
    source: Path,
    index: int,
    work: Path,
    output: Path,
    env: dict[str, str],
) -> dict[str, object]:
    relative = source.relative_to(ROOT)
    copied = work / relative
    alias = copied.with_name(f"u{index:03d}{source.suffix.lower()}")
    if alias.exists() or alias.is_symlink():
        raise RuntimeError(f"DOS alias collision: {alias}")
    shutil.copy2(copied, alias)
    if sha256(alias) != sha256(copied):
        raise RuntimeError(f"alias changed source bytes: {relative}")

    obj_dir = work / "obj" / f"{index:03d}"
    obj_dir.mkdir(parents=True)
    alias_relative = alias.relative_to(work).as_posix()
    command = [
        "wine", str(RUNNER), "-e", "-x", "tcc", *FLAGS,
        f"-nobj/{index:03d}/", alias_relative,
    ]
    log = output / f"compile-{index:03d}.log"
    compile_exit = run(command, work, env, log)
    objects = sorted(obj_dir.glob("*.obj"))
    omf = describe_omf(objects[0].read_bytes()) if len(objects) == 1 else None
    return {
        "index": index,
        "source": relative.as_posix(),
        "source_sha256": sha256(copied),
        "compile_alias": alias.relative_to(work).as_posix(),
        "compile_alias_sha256": sha256(alias),
        "compile_exit": compile_exit,
        "log": log.relative_to(output).as_posix(),
        "objects": [item.relative_to(work).as_posix() for item in objects],
        "object_sha256": sha256(objects[0]) if len(objects) == 1 else None,
        "omf_valid": bool(omf and omf["valid"]),
        "omf_record_counts": omf["record_counts"] if omf else None,
        "translator_comments": omf["translator_comments"] if omf else [],
    }


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output-dir", required=True, type=Path)
    parser.add_argument(
        "--require-all", action="store_true",
        help="return failure when any source does not produce one valid OMF",
    )
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
    if sha256(RUNNER) != RUNNER_SHA256:
        raise RuntimeError("pinned TC4J runner identity drift")

    sources = sorted(
        source
        for root in SOURCE_ROOTS
        for source in root.rglob("*")
        if source.is_file() and source.suffix.lower() in SUFFIXES
    )
    if not sources:
        raise RuntimeError("no product C/C++ sources found")

    output.mkdir(parents=True)
    work = output / "source"
    shutil.copytree(ROOT / "src", work / "src")
    env = os.environ.copy()
    env.update(
        WINEPREFIX=str(ROOT / ".analysis/toolchain/wineprefix"),
        WINEDEBUG="-all",
        MSDOS_PATH=r"C:\TC4\BIN",
    )
    records = [compile_source(source, index, work, output, env)
               for index, source in enumerate(sources)]
    compile_pass = sum(
        item["compile_exit"] == 0 and item["omf_valid"] for item in records
    )
    receipt = {
        "schema_version": 1,
        "observed_utc": datetime.now(timezone.utc).isoformat(timespec="seconds"),
        "artifact": "th04-main",
        "scope": "product-only maintained C/C++ source compiler frontier",
        "runner_sha256": RUNNER_SHA256,
        "compiler_flags": list(FLAGS),
        "source_roots": [root.relative_to(ROOT).as_posix() for root in SOURCE_ROOTS],
        "source_tree_sha256": source_tree_digest(sources),
        "source_count": len(sources),
        "compile_pass": compile_pass,
        "compile_fail": len(records) - compile_pass,
        "records": records,
        "link_performed": False,
        "mz_performed": False,
        "runtime_performed": False,
        "limit": (
            "Product-only C/C++ compiler frontier; ASM DATA/BSS owners, TLINK/MZ "
            "placement, relocation/layout, and PC-98 startup remain open."
        ),
    }
    (output / "receipt.json").write_text(
        json.dumps(receipt, indent=2, sort_keys=True) + "\n", encoding="utf-8"
    )
    print(json.dumps({
        "artifact": "th04-main",
        "source_count": len(sources),
        "compile_pass": compile_pass,
        "compile_fail": len(records) - compile_pass,
        "all_valid_omf": compile_pass == len(records),
    }, sort_keys=True))
    if args.require_all and compile_pass != len(records):
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
