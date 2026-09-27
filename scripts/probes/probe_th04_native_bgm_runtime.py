#!/usr/bin/env python3
"""Replay pinned EFS parsing and TH04-local PC-98 beeper scheduling in DOS."""

from __future__ import annotations

import argparse
from datetime import datetime, timezone
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys
import tomllib

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT))
from scripts.probes.prepare_th04_maine_diagnostic_hdi import Fat12  # noqa: E402
from scripts.probes.probe_th04_pf_archive import ARCHIVES, parse_archive, u16, u32  # noqa: E402
from scripts.probes.probe_th04_native_pi_decode_runtime import (  # noqa: E402
    PRIVATE, RUNNER, RUNNER_SHA256, SUPPORT, SUPPORT_SHA256,
    audit_mz, compile_cpp, run, sha, sha_bytes,
)

PRODUCT = ("src/shared/sound/bgm_beep.cpp", "src/shared/memory/heap.cpp",
           "src/shared/formats/pf_archive.cpp")
ASSEMBLY = ("src/shared/sound/bgm_timer.asm", "src/shared/formats/pf_state.asm",
            "src/shared/formats/pf_int21.asm")
MEMBER = "MIKO.EFS"

REFERENCE = r'''#include <stdio.h>
#include <dos.h>
extern "C" int near pascal mem_assign_dos(unsigned);
extern "C" void __seg * near pascal hmem_allocbyte(unsigned);
extern "C" int near pascal bgm_read_sdata(const char near *);
extern "C" unsigned bgm_glb[];
extern "C" unsigned bgm_esound[];
static unsigned long step(unsigned long h, unsigned char v)
{
    return (h ^ v) * 16777619UL;
}
int main(void)
{
    if (mem_assign_dos(21000) != 0) return 1;
    for (unsigned i = 0; i < 16u; i++) {
        unsigned seg = (unsigned)hmem_allocbyte(514u);
        if (!seg) return 2;
        bgm_esound[i * 4u + 0u] = 0;
        bgm_esound[i * 4u + 1u] = seg;
        bgm_esound[i * 4u + 2u] = 0;
        bgm_esound[i * 4u + 3u] = seg;
    }
    bgm_glb[47] = 0; // snum at SGLB word 47 in pinned near-model member.
    int result = bgm_read_sdata("MIKO.EFS");
    unsigned count = bgm_glb[47];
    unsigned long hash = 2166136261UL, words = 0;
    for (unsigned effect = 0; effect < count && effect < 16u; effect++) {
        unsigned far *values = (unsigned far *)MK_FP(bgm_esound[effect * 4u + 3u], 0);
        for (unsigned at = 0; at < 257u; at++) {
            unsigned value = values[at];
            hash = step(step(hash, (unsigned char)value), (unsigned char)(value >> 8));
            words++;
            if (!value) break;
        }
    }
    printf("RET=%d COUNT=%u WORDS=%lu HASH=%08lX\n", result, count, words, hash);
    return result ? 3 : 0;
}
'''

PRODUCT_TEST = r'''#include <stdio.h>
#include <dos.h>
#include "src/shared/runtime/api.hpp"
extern "C" unsigned pferrno;
extern "C" unsigned far pascal bgm_tick(void);
static const unsigned lengths[15] = {@LENGTHS@};
static unsigned long step(unsigned long h, unsigned char v)
{
    return (h ^ v) * 16777619UL;
}
int main(void)
{
    void interrupt (*old_irq)(...) = getvect(8);
    if (mem_assign_dos(21000) != 0) return 1;
    pfstart((const unsigned char far *)"OP.DAT");
    if (pferrno || bgm_init(1024) != 0 || getvect(8) == old_irq) return 2;
    if (bgm_init(1024) != 0 || bgm_read_sdata("MIKO.EFS") != 0) return 3;
    if (bgm_sound(0) != -13 || bgm_sound(16) != -13) return 4;
    if (bgm_sound(1) != 0) return 5;
    for (unsigned tick = 0; tick < 3u; tick++) if (bgm_tick()) return 6;
    asm int 8; // The fourth tick must pass through the installed ISR.
    for (unsigned after_irq = 0; after_irq < 3u; after_irq++)
        if (bgm_tick()) return 7;
    if (bgm_tick() != 7168u) return 7;
    unsigned long hash = 2166136261UL, words = 0;
    for (unsigned effect = 1; effect <= 15u; effect++) {
        if (bgm_sound(effect) != 0) return 8;
        unsigned emitted = 0;
        for (unsigned effect_tick = 0; effect_tick < lengths[effect - 1u] * 6u + 40u; effect_tick++) {
            unsigned hz = bgm_tick();
            if (hz) {
                hash = step(step(hash, (unsigned char)hz), (unsigned char)(hz >> 8));
                if (++emitted == lengths[effect - 1u]) break;
            }
        }
        if (emitted != lengths[effect - 1u]) return 9;
        for (unsigned quiet = 0; quiet < 20u; quiet++) if (bgm_tick()) return 10;
        hash = step(step(hash, 0), 0);
        words += emitted + 1u;
    }
    if (hash != 0x@HASH@UL || words != @WORDS@UL) {
        printf("BGM_HASH=%08lX WORDS=%lu\n", hash, words);
        return 11;
    }
    if (bgm_read_sdata("BAD.EFS") != -11 || bgm_sound(1) != -13 ||
        bgm_read_sdata("MISSING.EFS") != -2) return 12;
    pfend();
    if (bgm_read_sdata("MIKO.EFS") != 0 || bgm_sound(15) != 0) return 13;
    bgm_finish();
    if (getvect(8) != old_irq) return 14;
    if (bgm_init(1024) != 0 || bgm_read_sdata("MIKO.EFS") != 0 ||
        bgm_sound(1) != 0) return 15;
    if (mem_unassign() != 1) return 16;
    for (unsigned final_tick = 0; final_tick < 4u; final_tick++) bgm_tick();
    bgm_finish();
    if (getvect(8) != old_irq) return 17;
    puts("BGM_PASS");
    return 0;
}
'''


def expected_effects(payload: bytes) -> tuple[list[int], int, str]:
    effects: list[list[int]] = []
    current: list[int] = []
    for line in payload.splitlines():
        for token in line.split(b";", 1)[0].split():
            if not token.isdigit():
                continue
            value = int(token)
            if value > 65535:
                raise ValueError("EFS value exceeds one word")
            if value == 0:
                effects.append(current)
                current = []
            else:
                current.append(value)
    if current or len(effects) != 15 or max(map(len, effects)) > 256:
        raise ValueError("pinned EFS shape drift")
    words = [value for effect in effects for value in (*effect, 0)]
    hash_value = 2166136261
    for value in words:
        for byte in (value & 255, value >> 8):
            hash_value = ((hash_value ^ byte) * 16777619) & 0xFFFFFFFF
    return [len(effect) for effect in effects], len(words), f"{hash_value:08X}"


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output-dir", required=True, type=Path)
    args = parser.parse_args()
    output = args.output_dir.resolve()
    if output.exists() or not output.is_relative_to(PRIVATE):
        parser.error("output must be a new private directory")
    subprocess.run([sys.executable, "scripts/preflight.py"], cwd=ROOT, check=True,
                   stdout=subprocess.DEVNULL)
    subprocess.run([sys.executable, "scripts/attest_toolchain.py"], cwd=ROOT, check=True,
                   stdout=subprocess.DEVNULL)
    if sha(RUNNER) != RUNNER_SHA256 or sha(SUPPORT) != SUPPORT_SHA256:
        raise ValueError("pinned DOS runner or historical library drift")
    config = tomllib.loads((ROOT / "config/runtime.toml").read_text())
    image = (ROOT / config["image"]["path"]).read_bytes()
    if len(image) != config["image"]["size"] or sha_bytes(image) != config["image"]["sha256"]:
        raise ValueError("pinned HDI drift")
    fat = Fat12(bytearray(image))
    genso = fat.find_entry([fat.root], b"GENSO      ")
    directory = [fat.cluster_offset(c) for c in fat.chain(u16(fat.image, genso + 26))]
    archive = ARCHIVES["op_end"]
    entry = fat.find_entry(directory, archive["fat_name"])
    blob = fat.file_bytes(u16(fat.image, entry + 26), u32(fat.image, entry + 28))
    if sha_bytes(blob) != archive["sha256"]:
        raise ValueError("pinned PAR drift")
    _, members = parse_archive(blob, "op_end", archive)
    fixture = members[MEMBER]
    lengths, words, expected_hash = expected_effects(fixture)

    output.mkdir(parents=True)
    env = os.environ.copy()
    env.update(WINEPREFIX=str(ROOT / ".analysis/toolchain/wineprefix"),
               WINEDEBUG="-all", MSDOS_PATH=r"C:\TC4\BIN;C:\TASM50\BIN")
    reference = output / "reference"
    reference.mkdir()
    (reference / "obj").mkdir()
    (reference / "bin").mkdir()
    shutil.copy2(SUPPORT, reference / "bin/masters.lib")
    (reference / MEMBER).write_bytes(fixture)
    (reference / "reference.cpp").write_text(REFERENCE, encoding="ascii")
    ref_obj = compile_cpp("reference.cpp", "-ms", reference, env,
                          output / "reference-compile.log", RUNNER, "reference")
    (reference / "obj/link.rsp").write_text(
        "-c -s -E c0s.obj " + ref_obj +
        ", bin\\reference.exe, obj\\reference.map, bin\\masters.lib emu.lib maths.lib cs.lib\n",
        encoding="ascii")
    if run(["wine", str(RUNNER), "-e", "-x", "tlink", "@obj\\link.rsp"],
           reference, env, output / "reference-link.log").returncode:
        raise RuntimeError("historical EFS oracle TLINK failed")
    historical = run(["wine", str(RUNNER), "-e", "-x", "bin\\reference.exe"],
                     reference, env, output / "reference-runtime.log")
    match = re.search(rb"RET=0 COUNT=(\d+) WORDS=(\d+) HASH=([0-9A-F]{8})",
                      historical.stdout)
    if (historical.returncode or not match or int(match.group(1)) != len(lengths)
            or int(match.group(2)) != words or match.group(3).decode() != expected_hash):
        raise RuntimeError("historical EFS oracle differs from pinned resource parser")

    work = output / "source"
    shutil.copytree(ROOT / "src", work / "src")
    (work / "obj").mkdir()
    (work / "bin").mkdir()
    (work / "OP.DAT").write_bytes(blob)
    (work / MEMBER).write_bytes(fixture)
    (work / "BAD.EFS").write_bytes(b"; no decimal effects\n")
    test = PRODUCT_TEST.replace("@LENGTHS@", ", ".join(map(str, lengths)))
    test = test.replace("@HASH@", expected_hash).replace("@WORDS@", str(words))
    (work / "test.cpp").write_text(test, encoding="ascii")
    objects = []
    for index, source in enumerate((*PRODUCT, "test.cpp")):
        objects.append(compile_cpp(source, "-ml", work, env,
                                   output / f"compile-{index}.log", RUNNER, f"c{index}"))
    for index, source in enumerate(ASSEMBLY):
        obj = f"obj\\a{index}.obj"
        command = ["wine", "cmd", "/d", "/c",
                   "set PATH=C:\\TASM50\\BIN;C:\\TC4\\BIN;%PATH%&&"
                   f"tasm32 /m /mx /kh32768 /t /dGAME=4 /dTH04_LARGE_PRODUCT=1 "
                   f"{source.replace('/', chr(92))} {obj}"]
        if run(command, work, env, output / f"assemble-{index}.log").returncode:
            raise RuntimeError(f"TASM failed on {source}")
        objects.append(obj)
    (work / "obj/link.rsp").write_text(
        "-c -s -E c0l.obj " + " ".join(objects) +
        ", bin\\bgmtest.exe, obj\\bgmtest.map, emu.lib mathl.lib cl.lib\n",
        encoding="ascii")
    if run(["wine", str(RUNNER), "-e", "-x", "tlink", "@obj\\link.rsp"],
           work, env, output / "link.log").returncode:
        raise RuntimeError("TH04-only BGM test TLINK failed")
    result = run(["wine", str(RUNNER), "-e", "-x", "bin\\bgmtest.exe"],
                 work, env, output / "runtime.log")
    relocation = audit_mz(work / "bin/bgmtest.exe")
    passed = result.returncode == 0 and b"BGM_PASS" in result.stdout
    receipt = {
        "schema_version": 1,
        "observed_utc": datetime.now(timezone.utc).isoformat(timespec="seconds"),
        "archive_sha256": sha_bytes(blob), "resource_sha256": sha_bytes(fixture),
        "source_sha256": {p: sha(ROOT / p) for p in (*PRODUCT, *ASSEMBLY)},
        "runner_sha256": RUNNER_SHA256, "historical_library_sha256": SUPPORT_SHA256,
        "historical_mz_sha256": sha(reference / "bin/reference.exe"),
        "historical_words": words, "historical_effect_count": len(lengths),
        "historical_hash": expected_hash,
        "test_mz_sha256": sha(work / "bin/bgmtest.exe"),
        "test_relocations": relocation,
        "runtime_log_sha256": sha(output / "runtime.log"),
        "runtime_exit": result.returncode, "passed": passed,
        "limit": "EFS data and manual scheduler ticks pass under DOS; PC-98 timer cadence and beeper waveform require hardware-emulator observation.",
    }
    (output / "receipt.json").write_text(json.dumps(receipt, indent=2, sort_keys=True) + "\n")
    print(json.dumps({"passed": passed, "effects": len(lengths), "words": words,
                      "hash": expected_hash, "relocations": relocation["relocations"]}, sort_keys=True))
    return 0 if passed else 1


if __name__ == "__main__":
    raise SystemExit(main())
