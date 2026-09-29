#!/usr/bin/env python3
"""Compile mapped TH04 MAIN owners in a pinned scaffold tree.

This is a compiler-only frontier. It materializes the pinned ReC98 headers and
``th04_main.asm`` environment, copies the maintained ``src`` tree, and builds
one object per unique direct/fused owner under its historical input basename.
It does not patch the target, link an MZ, or treat a successful object compile
as standalone MAIN readiness.
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
import tarfile
import tomllib

ROOT = Path(__file__).resolve().parents[2]
REFERENCE = ROOT / "_reference/ReC98"
RUNNER = REFERENCE / "bin/msdos.exe"
RUNNER_SHA256 = "f7f6cb0a3e816c5edb13112d327c1bddbf7463fe7bf9a005ca1eb5317751bd02"
REFERENCE_REVISION = "b6ba5b0a529edbb31efdf8c0e939263804f8ee47"
MANIFEST = ROOT / "config/native_main_sources.toml"
sys.path.insert(0, str(ROOT / "scripts"))
from lib.omf import describe_omf  # noqa: E402
from probe_th04_native_main_manifest import audit  # noqa: E402


def digest(path: Path) -> str:
    value = hashlib.sha256()
    with path.open("rb") as stream:
        for chunk in iter(lambda: stream.read(1 << 20), b""):
            value.update(chunk)
    return value.hexdigest()


def run(command: list[str], cwd: Path, env: dict[str, str], log: Path) -> int:
    result = subprocess.run(command, cwd=cwd, env=env, capture_output=True,
                            text=True, timeout=240)
    log.write_text(json.dumps(command) + f"\nexit={result.returncode}\n"
                   + result.stdout + result.stderr, encoding="utf-8")
    return result.returncode


def materialize_reference(work: Path, archive: Path) -> None:
    with archive.open("wb") as stream:
        subprocess.run(["git", "-C", str(REFERENCE), "archive", REFERENCE_REVISION],
                       stdout=stream, check=True)
    with tarfile.open(archive) as stream:
        try:
            stream.extractall(work, filter="data")
        except TypeError:  # pragma: no cover - older host Python
            stream.extractall(work)
    shutil.copytree(ROOT / "src", work / "src")


def unique_owners(data: dict[str, object]) -> list[dict[str, object]]:
    direct = data["direct_owners"]
    compile_aliases = data["compile_aliases"]
    fused = data["fused_owners"]
    seen: dict[str, int] = {}
    owners: list[dict[str, object]] = []
    for reference in data["reference_sources"]:
        if reference in direct:
            local = direct[reference]
            mode = "direct"
            physical_id = None
        elif reference in fused:
            entry = fused[reference]
            local = entry["local"]
            mode = "fused"
            physical_id = entry["physical_id"]
        else:
            continue
        if local in seen:
            owners[seen[local]]["references"].append(reference)
            continue
        seen[local] = len(owners)
        owners.append({
            "local": local,
            "compile_as": compile_aliases.get(reference, reference),
            "mode": mode,
            "physical_id": physical_id,
            "references": [reference],
        })
    return owners


def compile_owner(owner: dict[str, object], index: int, work: Path, output: Path,
                  environment: dict[str, str]) -> dict[str, object]:
    local = str(owner["local"])
    source = work / local
    compile_as = str(owner["compile_as"])
    compile_source = work / compile_as
    log = output / f"compile-{index:03d}.log"
    record = {**owner, "index": index, "source_present": source.is_file(),
              "compile_as": compile_as,
              "log": log.relative_to(output).as_posix()}
    if not source.is_file() or source.is_symlink():
        record.update({"compile_exit": None, "objects": [], "omf_valid": False})
        return record
    if compile_source.is_symlink():
        raise RuntimeError(f"historical compile path is a symlink: {compile_as}")
    compile_source.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(source, compile_source)
    if digest(compile_source) != digest(source):
        raise RuntimeError(f"historical compile path changed source bytes: {compile_as}")
    obj_dir = work / "obj/frontier" / f"{index:03d}"
    obj_dir.mkdir(parents=True)
    relative = compile_source.relative_to(work).as_posix()
    if compile_source.suffix.lower() == ".asm":
        object_path = obj_dir / "unit.obj"
        source_win = relative.replace("/", "\\")
        object_win = str(object_path.relative_to(work)).replace("/", "\\")
        command = [
            "wine", "cmd", "/d", "/c",
            r"set PATH=C:\TASM50\BIN;C:\TC4\BIN;%PATH%&&"
            + f"tasm32 /m /mx /kh32768 /t /dGAME=4 "
            f"/dTH04_LARGE_PRODUCT=1 {source_win} {object_win}",
        ]
    else:
        command = [
            "wine", str(RUNNER), "-e", "-x", "tcc",
            "-c", "-I.", "-O", "-b-", "-3", "-Z", "-d",
            "-DGAME=4", "-DTH04P", "-ml", "-DBINARY='M'",
            f"-nobj/frontier/{index:03d}/", relative,
        ]
    compile_exit = run(command, work, environment, log)
    objects = sorted(obj_dir.glob("*.obj"))
    omf = describe_omf(objects[0].read_bytes()) if len(objects) == 1 else None
    record.update({
        "compile_exit": compile_exit,
        "objects": [item.relative_to(work).as_posix() for item in objects],
        "omf_valid": bool(omf and omf["valid"]),
        "object_sha256": digest(objects[0]) if len(objects) == 1 else None,
        "source_sha256": digest(source),
        "compile_source_sha256": digest(compile_source),
    })
    return record


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output-dir", type=Path, required=True)
    parser.add_argument("--require-all", action="store_true",
                        help="return failure when any mapped owner does not produce a valid object")
    args = parser.parse_args()
    output = args.output_dir.resolve()
    private = (ROOT / ".analysis/reconstruction/probes").resolve()
    if output.exists() or not output.is_relative_to(private):
        parser.error("output must be a new directory below .analysis/reconstruction/probes")
    subprocess.run([sys.executable, "scripts/preflight.py"], cwd=ROOT, check=True,
                   stdout=subprocess.DEVNULL)
    subprocess.run([sys.executable, "scripts/attest_toolchain.py"], cwd=ROOT, check=True,
                   stdout=subprocess.DEVNULL)
    if digest(RUNNER) != RUNNER_SHA256:
        raise RuntimeError("pinned TC4J runner identity drift")
    if subprocess.check_output(["git", "-C", str(REFERENCE), "rev-parse", "HEAD"],
                               text=True).strip() != REFERENCE_REVISION:
        raise RuntimeError("pinned ReC98 revision drift")
    manifest_audit = audit()
    data = tomllib.loads(MANIFEST.read_text(encoding="utf-8"))
    owners = unique_owners(data)
    output.mkdir(parents=True)
    work = output / "source"
    work.mkdir()
    materialize_reference(work, output / "reference.tar")
    environment = os.environ.copy()
    environment.update(WINEPREFIX=str(ROOT / ".analysis/toolchain/wineprefix"),
                       WINEDEBUG="-all", MSDOS_PATH=r"C:\TC4\BIN")
    records = [compile_owner(owner, index, work, output, environment)
               for index, owner in enumerate(owners)]
    compile_pass = sum(bool(item["omf_valid"] and item["compile_exit"] == 0)
                       for item in records)
    receipt = {
        "schema_version": 1,
        "observed_utc": datetime.now(timezone.utc).isoformat(timespec="seconds"),
        "artifact": "th04-main",
        "reference_revision": REFERENCE_REVISION,
        "runner_sha256": RUNNER_SHA256,
        "manifest_sha256": digest(MANIFEST),
        "manifest_audit": manifest_audit,
        "unique_physical_sources": len(owners),
        "compile_pass": compile_pass,
        "compile_fail": len(records) - compile_pass,
        "records": records,
        "link_performed": False,
        "runtime_performed": False,
        "limit": "Compiler-only frontier; no TLINK/MZ/startup claim.",
    }
    (output / "receipt.json").write_text(json.dumps(receipt, indent=2, sort_keys=True) + "\n",
                                          encoding="utf-8")
    print(json.dumps({
        "artifact": "th04-main",
        "unique_physical_sources": len(owners),
        "compile_pass": compile_pass,
        "compile_fail": len(records) - compile_pass,
        "ready_for_native_link": bool(manifest_audit["ready_for_native_link"] and
                                       compile_pass == len(records)),
    }, sort_keys=True))
    if args.require_all and compile_pass != len(records):
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
