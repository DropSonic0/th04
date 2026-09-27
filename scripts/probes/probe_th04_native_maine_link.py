#!/usr/bin/env python3
"""Compile TH04-owned MAINE sources and report the native link frontier.

The optional external masters.lib is a pinned calibration input. An unresolved
link or a valid MZ is reported as evidence, never as product acceptance.
"""

from __future__ import annotations

import argparse
from datetime import datetime, timezone
import hashlib
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
from lib.omf import describe_omf, parse_omf  # noqa: E402
from lib.pc98 import parse_mz  # noqa: E402

RUNNER = ROOT / "_reference/ReC98/bin/msdos.exe"
RUNNER_SHA256 = "f7f6cb0a3e816c5edb13112d327c1bddbf7463fe7bf9a005ca1eb5317751bd02"
SUPPORT_LIB = ROOT / "_reference/ReC98/bin/masters.lib"
SUPPORT_SHA256 = "6be41dbcfcf4504977165ccc44443525a29a01f85a1580e6ad0c620bf802faf6"
# The MS-DOS command tail is bounded. A longer product define makes TC4J
# misread the longest MAINE source path's extension before compilation.
# TH04P selects C++ code grouping; ASM still uses TH04_LARGE_PRODUCT.
FLAGS = ("-c", "-I.", "-O", "-b-", "-3", "-Z", "-d", "-DGAME=4", "-DTH04P", "-ml")


def sha(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def link_relevant_sha(path: Path) -> str:
    digest = hashlib.sha256()
    for record in parse_omf(path.read_bytes()):
        if record.record_type != 0x88:  # COMENT is not a linker input.
            digest.update(bytes([record.record_type]))
            digest.update(len(record.data).to_bytes(2, "little"))
            digest.update(record.data)
    return digest.hexdigest()


def run(command: list[str], work: Path, env: dict[str, str], log: Path) -> subprocess.CompletedProcess[str]:
    result = subprocess.run(command, cwd=work, env=env, capture_output=True,
                            text=True, timeout=240)
    log.write_text(json.dumps(command) + f"\nexit={result.returncode}\n"
                   + result.stdout + result.stderr, encoding="utf-8")
    return result


def assemble(source: Path, obj: Path, work: Path, env: dict[str, str], log: Path) -> None:
    src = str(source.relative_to(work)).replace("/", "\\")
    dst = str(obj.relative_to(work)).replace("/", "\\")
    command = ["wine", "cmd", "/d", "/c",
               "set PATH=C:\\TASM50\\BIN;C:\\TC4\\BIN;%PATH%&&"
               f"tasm32 /m /mx /kh32768 /t /dGAME=4 /dTH04_LARGE_PRODUCT=1 {src} {dst}"]
    if run(command, work, env, log).returncode:
        raise RuntimeError(f"TASM failed: {log}")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output-dir", type=Path, required=True)
    parser.add_argument("--without-support", action="store_true",
                        help="link only TH04 source plus pinned Borland system libraries")
    args = parser.parse_args()
    output = args.output_dir.resolve()
    private = (ROOT / ".analysis/reconstruction/probes").resolve()
    if output.exists() or not output.is_relative_to(private):
        parser.error("output must be a new directory below .analysis/reconstruction/probes")
    subprocess.run([sys.executable, "scripts/preflight.py"], cwd=ROOT, check=True,
                   stdout=subprocess.DEVNULL)
    subprocess.run([sys.executable, "scripts/attest_toolchain.py"], cwd=ROOT, check=True,
                   stdout=subprocess.DEVNULL)
    identities = [(RUNNER, RUNNER_SHA256)]
    if not args.without_support:
        identities.append((SUPPORT_LIB, SUPPORT_SHA256))
    for path, expected in identities:
        if sha(path) != expected:
            raise RuntimeError(f"pinned input identity drift: {path}")

    output.mkdir(parents=True)
    work = output / "source"
    shutil.copytree(ROOT / "src", work / "src")
    (work / "bin").mkdir()
    if not args.without_support:
        shutil.copy2(SUPPORT_LIB, work / "bin/masters.lib")
    (work / "obj/product").mkdir(parents=True)
    env = os.environ.copy()
    env.update(WINEPREFIX=str(ROOT / ".analysis/toolchain/wineprefix"),
               WINEDEBUG="-all", MSDOS_PATH=r"C:\TC4\BIN;C:\TASM50\BIN")

    sources = sorted(path.relative_to(ROOT) for group in ("maine", "shared")
                     for path in (ROOT / "src" / group).rglob("*")
                     if path.suffix in (".cpp", ".c"))
    asm_sources = sorted(path.relative_to(ROOT) for group in ("maine", "shared")
                         for path in (ROOT / "src" / group).rglob("*.asm"))
    object_paths: list[Path] = []
    records: list[dict[str, str]] = []
    for index, source in enumerate(sources):
        group = source.parts[1]
        obj_dir = work / "obj" / group / f"{index:03d}"
        obj_dir.mkdir(parents=True)
        extra = ["-B"] if source.as_posix() == "src/shared/hardware/bgimage.cpp" else []
        if group == "maine":
            extra.append("-DBINARY='E'")
        command = ["wine", str(RUNNER), "-e", "-x", "tcc", *extra, *FLAGS,
                   f"-nobj/{group}/{index:03d}/", source.as_posix()]
        log = output / f"compile-{index:03d}.log"
        result = run(command, work, env, log)
        if extra[:1] == ["-B"]:
            generated = list(obj_dir.glob("*.asm"))
            if len(generated) != 1 or "Unable to execute command 'tasm.exe'" not in result.stdout + result.stderr:
                raise RuntimeError(f"BGIMAGE ASM generation failed: {log}")
            obj = generated[0].with_suffix(".obj")
            assemble(generated[0], obj, work, env, output / f"assemble-generated-{index:03d}.log")
        elif result.returncode:
            raise RuntimeError(f"TC4J failed: {log}")
        objects = list(obj_dir.glob("*.obj"))
        if len(objects) != 1:
            raise RuntimeError(f"expected one object: {log}")
        obj = objects[0]
        omf = describe_omf(obj.read_bytes())
        producer = "Turbo Assembler  Version 5.0" if extra[:1] == ["-B"] else "TC86 Borland C++ 4.02"
        if not any(producer in comment for comment in omf["translator_comments"]):
            raise RuntimeError(f"unexpected OMF producer: {source}")
        object_paths.append(obj)
        records.append({"source": source.as_posix(), "source_sha256": sha(ROOT / source),
                        "object": obj.relative_to(work).as_posix(), "object_sha256": sha(obj),
                        "link_relevant_sha256": link_relevant_sha(obj)})

    for index, source in enumerate(asm_sources):
        obj_dir = work / "obj/asm" / f"{index:03d}"
        obj_dir.mkdir(parents=True)
        obj = obj_dir / "unit.obj"
        assemble(work / source, obj, work, env, output / f"assemble-{index:03d}.log")
        omf = describe_omf(obj.read_bytes())
        if not any("Turbo Assembler  Version 5.0" in comment for comment in omf["translator_comments"]):
            raise RuntimeError(f"unexpected ASM OMF producer: {source}")
        object_paths.append(obj)
        records.append({"source": source.as_posix(), "source_sha256": sha(ROOT / source),
                        "object": obj.relative_to(work).as_posix(), "object_sha256": sha(obj),
                        "link_relevant_sha256": link_relevant_sha(obj)})

    objlist = " ".join(str(path.relative_to(work)).replace("/", "\\") for path in object_paths)
    response = work / "obj/product/maine.@l"
    libraries = ("emu.lib mathl.lib cl.lib" if args.without_support else
                 "bin\\masters.lib emu.lib mathl.lib cl.lib")
    response.write_text("-c -s -E c0l.obj " + objlist
                        + ", bin\\maine-native.exe, obj\\product\\maine-native.map, "
                        + libraries + "\n", encoding="ascii")
    command = ["wine", str(RUNNER), "-e", "-x", "tlink", r"@obj\product\maine.@l"]
    link_log = output / "link.log"
    link = run(command, work, env, link_log)
    errors = re.findall(r"^Error: Undefined symbol (.+)$", link.stdout, re.M)
    warnings = re.findall(r"^Warning: (.+)$", link.stdout, re.M)
    exe = work / "bin/maine-native.exe"
    mz = parse_mz(exe.read_bytes()) if exe.exists() else None
    receipt = {
        "schema_version": 1,
        "observed_utc": datetime.now(timezone.utc).isoformat(timespec="seconds"),
        "scope": ("TH04-owned MAINE source graph without historical support library"
                  if args.without_support else
                  "TH04-owned MAINE source graph and diagnostic support-library link"),
        "runner_sha256": RUNNER_SHA256,
        "support_lib_sha256": None if args.without_support else SUPPORT_SHA256,
        "compiler_flags": list(FLAGS),
        "assembler_flags": ["/m", "/mx", "/kh32768", "/t", "/dGAME=4",
                            "/dTH04_LARGE_PRODUCT=1"],
        "objects": records,
        "response_sha256": sha(response),
        "link_log_sha256": sha(link_log),
        "link_exit": link.returncode,
        "unresolved": errors,
        "warnings": warnings,
        "mz_header": ({"sha256": sha(exe), "valid": mz.valid,
                "relocations": len(mz.relocations)} if mz else None),
        "link_complete": link.returncode == 0 and not errors and bool(mz and mz.valid),
        "limit": "This calibration link is not a standalone product or runtime acceptance; inspect every relocation and owner after it closes."
    }
    (output / "receipt.json").write_text(json.dumps(receipt, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"c_sources": len(sources), "asm_sources": len(asm_sources),
                      "objects": len(records), "link_exit": link.returncode,
                      "unresolved": len(errors), "warnings": len(warnings),
                      "mz_header_valid": mz.valid if mz else None,
                      "link_complete": receipt["link_complete"]}, sort_keys=True))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
