#!/usr/bin/env python3
"""Run the TH04-local PAR INT 21h service on pinned real members."""

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
sys.path.insert(0, str(ROOT))
from scripts.probes.prepare_th04_maine_diagnostic_hdi import Fat12  # noqa: E402
from scripts.probes.probe_th04_pf_archive import ARCHIVES, parse_archive, u16, u32  # noqa: E402
from scripts.lib.pc98 import parse_mz  # noqa: E402

RUNNER = ROOT / "_reference/ReC98/bin/msdos.exe"
RUNNER_SHA256 = "f7f6cb0a3e816c5edb13112d327c1bddbf7463fe7bf9a005ca1eb5317751bd02"
SOURCES = ("src/shared/formats/pf_archive.cpp", "src/shared/memory/heap.cpp")
ASSEMBLY = ("src/shared/formats/pf_state.asm", "src/shared/formats/pf_int21.asm")
FLAGS = ("-c", "-I.", "-O", "-b-", "-3", "-Z", "-d", "-DGAME=4", "-DTH04P", "-ml")
MEMBERS = (("OP.DAT", "GAMEFT.BFT"), ("OP.DAT", "CONG10.PI"),
           ("OP.DAT", "CONG14.PI"), ("OP.DAT", "_ED000.TXT"),
           ("OP.DAT", "SCNUM2.BFT"), ("MAIN.DAT", "ST00.BFT"),
           ("MAIN.DAT", "EYE1.CDG"), ("MAIN.DAT", "EYE4.CDG"),
           ("MAIN.DAT", "MIKO.EFC"))

TEST = r'''#include <stdio.h>
#include <dos.h>
#include <io.h>
#include "src/shared/runtime/api.hpp"
extern "C" unsigned pferrno;
extern "C" unsigned bbufsiz;
struct Expected { const char *archive; const char *name; unsigned size; unsigned extra; unsigned long hash; };
static const Expected expected[] = {
@EXPECTED@
};
static int fail(int n) { printf("PF_FAIL_%d\n", n); return n; }
static int member(const Expected *e, int index)
{
    int h;
    unsigned got, total = 0;
    unsigned char buf[317];
    unsigned long hash = 2166136261UL;
    bbufsiz = index == 0 ? 512u : (e->archive[0] == 'O' ? 8192u : 4096u);
    pfstart((const unsigned char far *)e->archive);
    if (pferrno) return fail(10 + index);
    if (_dos_open("LOOSE.DAT", 0, &h)) return fail(20 + index);
    if (_dos_read(h, buf, 1, &got) || got != 1 || buf[0] != 0x42)
        return fail(30 + index);
    _dos_close(h);
    if (_dos_open(e->name, 0, &h)) return fail(40 + index);
    while (total < e->size) {
        unsigned want = e->size - total;
        if (want > sizeof(buf)) want = sizeof(buf);
        if (_dos_read(h, buf, want, &got) || got != want) return fail(50 + index);
        for (unsigned i = 0; i < got; i++) hash = (hash ^ buf[i]) * 16777619UL;
        total += got;
    }
    if (hash != e->hash) return fail(60 + index);
    if (_dos_read(h, buf, 2, &got) || got != e->extra) return fail(70 + index);
    if (lseek(h, 0L, SEEK_END) != (long)e->size) return fail(80 + index);
    if (lseek(h, -1L, SEEK_END) != (long)e->size - 1) return fail(90 + index);
    if (_dos_read(h, buf, 1, &got) || got != 1) return fail(100 + index);
    if (lseek(h, 0L, SEEK_SET) != 0L) return fail(110 + index);
    if (_dos_read(h, buf, 4, &got) || got != 4) return fail(120 + index);
    if (_dos_close(h)) return fail(130 + index);
    pfend();
    if (_dos_open("LOOSE.DAT", 0, &h)) return fail(140 + index);
    _dos_close(h);
    return 0;
}
int main(void)
{
    if (mem_assign_dos(4096) != 0) return fail(1);
    for (int i = 0; i < sizeof(expected) / sizeof(expected[0]); i++) {
        int result = member(&expected[i], i);
        if (result) return result;
    }
    if (mem_unassign() != 1) return fail(2);
    puts("PF_PASS");
    return 0;
}
'''


def sha(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def fnv(data: bytes) -> int:
    value = 2166136261
    for byte in data:
        value = ((value ^ byte) * 16777619) & 0xffffffff
    return value


def run(command: list[str], work: Path, env: dict[str, str], log: Path) -> subprocess.CompletedProcess[str]:
    result = subprocess.run(command, cwd=work, env=env, capture_output=True,
                            text=True, timeout=240)
    log.write_text(json.dumps(command) + f"\nexit={result.returncode}\n" + result.stdout
                   + result.stderr, encoding="utf-8")
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
        raise ValueError("MS-DOS Player drift")

    from scripts.probes.probe_th04_pf_archive import digest  # noqa: E402
    import tomllib
    runtime = tomllib.loads((ROOT / "config/runtime.toml").read_text())
    image = (ROOT / runtime["image"]["path"]).read_bytes()
    if len(image) != runtime["image"]["size"] or digest(image) != runtime["image"]["sha256"]:
        raise ValueError("pinned HDI drift")
    fat = Fat12(bytearray(image))
    genso = fat.find_entry([fat.root], b"GENSO      ")
    directory = [fat.cluster_offset(c) for c in fat.chain(u16(fat.image, genso + 26))]
    archives: dict[str, tuple[bytes, dict, dict[str, bytes]]] = {}
    for archive_id, expected in ARCHIVES.items():
        offset = fat.find_entry(directory, expected["fat_name"])
        blob = fat.file_bytes(u16(fat.image, offset + 26), u32(fat.image, offset + 28))
        if digest(blob) != expected["sha256"]:
            raise ValueError(f"{archive_id} archive drift")
        receipt, files = parse_archive(blob, archive_id, expected)
        archives["OP.DAT" if archive_id == "op_end" else "MAIN.DAT"] = blob, receipt, files

    output.mkdir(parents=True)
    work = output / "source"
    shutil.copytree(ROOT / "src", work / "src")
    (work / "obj").mkdir()
    (work / "bin").mkdir()
    for name, (blob, _, _) in archives.items():
        (work / name).write_bytes(blob)
    (work / "LOOSE.DAT").write_bytes(b"B")
    lines = []
    for archive, name in MEMBERS:
        _, receipt, files = archives[archive]
        record = next(m for m in receipt["members"] if m["name"] == name)
        lines.append(f'    {{"{archive}", "{name}", {len(files[name])}u, '
                     f'{record["expanded_size"] - record["declared_size"]}u, '
                     f'0x{fnv(files[name]):08X}UL}},')
    test = work / "test.cpp"
    test.write_text(TEST.replace("@EXPECTED@", "\n".join(lines)), encoding="ascii")
    env = os.environ.copy()
    env.update(WINEPREFIX=str(ROOT / ".analysis/toolchain/wineprefix"),
               WINEDEBUG="-all", MSDOS_PATH=r"C:\TC4\BIN;C:\TASM50\BIN")
    objects = []
    for index, name in enumerate((*SOURCES, "test.cpp")):
        before = set((work / "obj").glob("*.obj"))
        cmd = ["wine", str(RUNNER), "-e", "-x", "tcc", *FLAGS, "-nobj/", name]
        log = output / f"compile-{index}.log"
        if run(cmd, work, env, log).returncode:
            raise RuntimeError(f"TC4J failed: {log}")
        created = set((work / "obj").glob("*.obj")) - before
        if len(created) != 1:
            raise RuntimeError(f"missing object: {name}")
        objects.append("obj\\" + created.pop().name)
    for index, name in enumerate(ASSEMBLY):
        obj = f"obj\\pfasm{index}.obj"
        cmd = ["wine", "cmd", "/d", "/c",
               "set PATH=C:\\TASM50\\BIN;C:\\TC4\\BIN;%PATH%&&"
               f"tasm32 /m /mx /kh32768 /t /dGAME=4 /dTH04_LARGE_PRODUCT=1 "
               f"{name.replace('/', chr(92))} {obj}"]
        log = output / f"assemble-{index}.log"
        if run(cmd, work, env, log).returncode:
            raise RuntimeError(f"TASM failed: {log}")
        objects.append(obj)
    (work / "obj/link.rsp").write_text(
        "-c -s -E c0l.obj " + " ".join(objects) +
        ", bin\\pftest.exe, obj\\pftest.map, emu.lib mathl.lib cl.lib\n",
        encoding="ascii")
    if run(["wine", str(RUNNER), "-e", "-x", "tlink", "@obj\\link.rsp"],
           work, env, output / "link.log").returncode:
        raise RuntimeError("TLINK failed; inspect link.log")
    result = run(["wine", str(RUNNER), "-e", "-x", "bin\\pftest.exe"],
                 work, env, output / "runtime.log")
    passed = result.returncode == 0 and "PF_PASS" in result.stdout
    mz = parse_mz((work / "bin/pftest.exe").read_bytes())
    if not mz.valid:
        raise ValueError(f"test MZ invalid: {mz.errors}")
    sites = sorted(rel.linear for rel in mz.relocations)
    image = mz.program_image
    if (len(set(sites)) != len(sites) or any(site + 2 > len(image) for site in sites)
            or any(right < left + 2 for left, right in zip(sites, sites[1:]))):
        raise ValueError("test MZ relocation sites overlap or escape load image")
    paragraphs = (len(image) + 15) // 16
    values = [int.from_bytes(image[site:site + 2], "little") for site in sites]
    if any(value >= paragraphs for value in values):
        raise ValueError("test MZ relocation target outside load image")
    for segment in (0x2000, 0x6000):
        if any(value + segment > 0xffff for value in values):
            raise ValueError("test MZ relocation wraps DOS segment")
        mz.relocated_program_image(segment)
    receipt = {"schema_version": 1,
               "observed_utc": datetime.now(timezone.utc).isoformat(timespec="seconds"),
               "sources_sha256": {p: sha(ROOT / p) for p in (*SOURCES, *ASSEMBLY)},
               "harness_sha256": sha(test), "runner_sha256": RUNNER_SHA256,
               "mz_sha256": sha(work / "bin/pftest.exe"), "mz_relocations": len(sites),
               "load_segments_checked": [0x2000, 0x6000],
               "runtime_log_sha256": sha(output / "runtime.log"),
               "members": list(MEMBERS), "returncode": result.returncode, "passed": passed,
               "limit": "MS-DOS Player validates DOS service behavior; MAINE and PC-98 rendering are separate gates."}
    (output / "receipt.json").write_text(json.dumps(receipt, indent=2, sort_keys=True) + "\n")
    print(json.dumps({"passed": passed, "runtime_log": str(output / "runtime.log")}, sort_keys=True))
    return 0 if passed else 1


if __name__ == "__main__":
    raise SystemExit(main())
