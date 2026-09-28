#!/usr/bin/env python3
"""Cold-link TH04's source-only Tiny-model ZUN resident COM component."""

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
import tomllib


ROOT = Path(__file__).resolve().parents[2]
PRIVATE = (ROOT / ".analysis/reconstruction/probes").resolve()
RUNNER = ROOT / "_reference/ReC98/bin/msdos.exe"
RUNNER_SHA256 = "f7f6cb0a3e816c5edb13112d327c1bddbf7463fe7bf9a005ca1eb5317751bd02"
MANIFEST = ROOT / "config/native_zun_resident_sources.toml"
sys.path.insert(0, str(ROOT / "scripts"))
from lib.omf import describe_omf, parse_omf  # noqa: E402


def sha(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def link_relevant_sha(data: bytes) -> str:
    digest = hashlib.sha256()
    for record in parse_omf(data):
        if record.record_type != 0x88:  # Timestamp-bearing COMMENT is not linked.
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


def load_manifest() -> tuple[list[str], list[str]]:
    data = tomllib.loads(MANIFEST.read_text(encoding="utf-8"))
    if (set(data) != {"schema_version", "artifact", "c_sources", "asm_sources"}
            or data["schema_version"] != 1 or data["artifact"] != "th04-zun-resident"):
        raise ValueError("invalid ZUN resident source manifest")
    for key, suffixes in (("c_sources", {".cpp", ".c"}), ("asm_sources", {".asm"})):
        names = data[key]
        if not names or len(names) != len(set(names)):
            raise ValueError(f"empty or duplicate {key}")
        for name in names:
            path = Path(name)
            if (path.is_absolute() or ".." in path.parts or path.suffix not in suffixes
                    or path.parts[:2] not in {("src", "zun"), ("src", "shared")}
                    or not (ROOT / path).is_file()):
                raise ValueError(f"invalid source: {name}")
    return data["c_sources"], data["asm_sources"]


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output-dir", required=True, type=Path)
    args = parser.parse_args()
    output = args.output_dir.resolve()
    if output.exists() or not output.is_relative_to(PRIVATE):
        parser.error("use a new private probe directory")
    c_sources, asm_sources = load_manifest()
    subprocess.run([sys.executable, "scripts/preflight.py"], cwd=ROOT,
                   check=True, stdout=subprocess.DEVNULL)
    subprocess.run([sys.executable, "scripts/attest_toolchain.py"], cwd=ROOT,
                   check=True, stdout=subprocess.DEVNULL)
    if sha(RUNNER.read_bytes()) != RUNNER_SHA256:
        raise ValueError("pinned DOS runner identity drift")

    output.mkdir(parents=True)
    work = output / "source"
    shutil.copytree(ROOT / "src", work / "src")
    (work / "obj").mkdir()
    (work / "bin").mkdir()
    for name in c_sources:
        path = work / name
        path.write_bytes(path.read_text(encoding="utf-8").encode("cp932"))
    env = os.environ.copy()
    env.update(WINEPREFIX=str(ROOT / ".analysis/toolchain/wineprefix"),
               WINEDEBUG="-all", MSDOS_PATH=r"C:\TC4\BIN;C:\TASM50\BIN")
    objects = []
    records = []
    for index, name in enumerate(c_sources):
        command = ["wine", str(RUNNER), "-e", "-x", "tcc", "-c", "-I.",
                   "-O", "-b-", "-3", "-Z", "-d", "-DGAME=4", "-mt",
                   "-nobj/", name]
        log = output / f"compile-{index:02d}.log"
        if run(command, work, env, log).returncode:
            raise RuntimeError(f"TC4J failed: {log}")
        obj = work / "obj" / (Path(name).stem + ".obj")
        if not obj.is_file():
            raise RuntimeError(f"missing compiler object: {obj}")
        describe_omf(obj.read_bytes())
        objects.append(obj)
        records.append({"source": name, "source_sha256": sha((ROOT / name).read_bytes()),
                        "object_sha256": sha(obj.read_bytes()),
                        "link_relevant_sha256": link_relevant_sha(obj.read_bytes())})

    for index, name in enumerate(asm_sources):
        obj = work / "obj" / f"asm{index:02d}.obj"
        command = ["wine", "cmd", "/d", "/c",
                   "set PATH=C:\\TASM50\\BIN;C:\\TC4\\BIN;%PATH%&&"
                   f"tasm32 /m /mx /kh32768 /t {name.replace('/', chr(92))} "
                   f"obj\\{obj.name}"]
        log = output / f"assemble-{index:02d}.log"
        if run(command, work, env, log).returncode:
            raise RuntimeError(f"TASM failed: {log}")
        if not obj.is_file():
            raise RuntimeError(f"missing assembler object: {obj}")
        describe_omf(obj.read_bytes())
        objects.append(obj)
        records.append({"source": name, "source_sha256": sha((ROOT / name).read_bytes()),
                        "object_sha256": sha(obj.read_bytes()),
                        "link_relevant_sha256": link_relevant_sha(obj.read_bytes())})

    response = ("-c -s -t c0t.obj "
                + " ".join("obj\\" + path.name for path in objects)
                + ", bin\\res_huma.com, obj\\res_huma.map, ct.lib\r\n")
    (work / "obj/link.rsp").write_bytes(response.encode("ascii"))
    link_log = output / "link.log"
    linked = run(["wine", str(RUNNER), "-e", "-x", "tlink", "@obj\\link.rsp"],
                 work, env, link_log)
    combined = linked.stdout + linked.stderr
    problems = re.findall(r"^.*(?:Error:|Fatal:|Warning:|Undefined symbol).*$",
                          combined, flags=re.MULTILINE | re.IGNORECASE)
    com = work / "bin/res_huma.com"
    map_path = work / "obj/res_huma.map"
    complete = (linked.returncode == 0 and not problems and com.is_file()
                and map_path.is_file() and 0 < com.stat().st_size <= 0xFF00)
    receipt = {
        "schema_version": 1,
        "observed_utc": datetime.now(timezone.utc).isoformat(timespec="seconds"),
        "scope": "TH04-only ZUN Tiny-model resident component link; no packed launcher claim",
        "artifact": "th04-zun",
        "manifest_sha256": sha(MANIFEST.read_bytes()),
        "runner_sha256": RUNNER_SHA256,
        "c_sources": len(c_sources), "asm_sources": len(asm_sources),
        "objects": records,
        "link_response": response,
        "link_exit": linked.returncode,
        "problems": problems,
        "link_complete": complete,
        "com_size": com.stat().st_size if com.is_file() else None,
        "com_sha256": sha(com.read_bytes()) if com.is_file() else None,
        "map_sha256": sha(map_path.read_bytes()) if map_path.is_file() else None,
    }
    (output / "receipt.json").write_text(json.dumps(receipt, indent=2) + "\n",
                                          encoding="utf-8")
    print(json.dumps({"link_complete": complete, "c_sources": len(c_sources),
                      "asm_sources": len(asm_sources), "problems": problems,
                      "com_size": receipt["com_size"]}, sort_keys=True))
    return 0 if complete else 1


if __name__ == "__main__":
    raise SystemExit(main())
