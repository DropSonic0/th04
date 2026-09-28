#!/usr/bin/env python3
"""Compare the product-owned TH04 sound header ABI with the pinned scaffold."""

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

PRIVATE = (ROOT / ".analysis/reconstruction/probes").resolve()
RUNNER = ROOT / "_reference/ReC98/bin/msdos.exe"
RUNNER_SHA256 = "f7f6cb0a3e816c5edb13112d327c1bddbf7463fe7bf9a005ca1eb5317751bd02"
LOCAL_HEADER = ROOT / "src/main/include/th04/snd/snd.h"
SHARED_HEADER = ROOT / "src/shared/sound/api.hpp"
REFERENCE_FILES = (
    "platform.h",
    "defconv.h",
    "game/pf.h",
    "libs/kaja/kaja.h",
    "th02/snd/snd.h",
    "th03/snd/snd.h",
    "th04/snd/snd.h",
)
FLAGS = ("-c", "-O", "-b-", "-3", "-Z", "-d", "-DGAME=4", "-ml")
TEST = r'''#include "th04/snd/snd.h"

typedef int (pascal *determine_modes_t)(int, int);
typedef void (pascal *snd_load_t)(const char[PF_FN_LEN], snd_load_func_t);

determine_modes_t determine_modes_p = snd_determine_modes;
snd_load_t snd_load_p = snd_load;

int probe_snd_constants(void)
{
    return (
        sizeof(snd_bgm_mode_t) + (sizeof(snd_se_mode_t) << 2) + PF_FN_LEN +
        SND_BGM_OFF + SND_BGM_FM26 + SND_BGM_FM86 + SND_BGM_MIDI +
        SND_SE_OFF + SND_SE_FM + SND_SE_BEEP +
        SND_LOAD_SONG + SND_LOAD_SE
    );
}

bool probe_snd_bgm_active(void)
{
    return snd_bgm_active();
}

bool16 probe_snd_se_active(void)
{
    return snd_se_active();
}

bool probe_snd_bgm_is_fm(void)
{
    return snd_bgm_is_fm();
}

int16_t probe_snd_kaja(void)
{
    return snd_kaja_func(KAJA_GET_VOLUME, 7);
}

void probe_snd_force(void)
{
    snd_se_play_force(4);
}

unsigned char snd_se_mode;
snd_bgm_mode_t snd_bgm_mode;

extern "C" {
int pascal snd_determine_modes(int req_bgm_mode, int req_se_mode)
{
    return req_bgm_mode + req_se_mode;
}

void pascal snd_load(const char fn[PF_FN_LEN], snd_load_func_t func)
{
    (void)fn;
    (void)func;
}

int16_t pascal snd_kaja_interrupt(int16_t ax)
{
    return ax;
}

void snd_se_reset(void) {}
void pascal snd_se_play(int new_se) { (void)new_se; }
void snd_se_update(void) {}
}
'''


def sha(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def semantic_omf_sha256(data: bytes) -> str:
    """Hash link semantics, ignoring comments and PUBDEF record ordering."""

    digest = hashlib.sha256()
    public_records: list[bytes] = []
    offset = 0
    while offset < len(data):
        if offset + 3 > len(data):
            raise ValueError("truncated OMF record header")
        record_type = data[offset]
        size = int.from_bytes(data[offset + 1:offset + 3], "little")
        end = offset + 3 + size
        if end > len(data):
            raise ValueError("truncated OMF record")
        record = data[offset:end]
        if record_type in {0x90, 0x91}:  # PUBDEF / PUBDEF32
            public_records.append(record)
        elif record_type != 0x88:  # COMENT
            digest.update(record)
        offset = end
    for record in sorted(public_records):
        digest.update(record)
    return digest.hexdigest()


def run(command: list[str], work: Path, env: dict[str, str], log: Path) -> None:
    result = subprocess.run(
        command, cwd=work, env=env, capture_output=True, text=True, timeout=240
    )
    log.write_text(
        json.dumps(command) + f"\nexit={result.returncode}\n"
        + result.stdout + result.stderr,
        encoding="utf-8",
    )
    if result.returncode:
        raise RuntimeError(f"TC4J failed; inspect {log.name}")


def compile_variant(
    label: str, root: Path, include: str, env: dict[str, str], output: Path
) -> dict[str, object]:
    (root / "obj").mkdir(parents=True)
    source = root / "test.cpp"
    source.write_text(TEST, encoding="ascii")
    run(
        [
            "wine", str(RUNNER), "-e", "-x", "tcc", *FLAGS,
            f"-I{include}", "-I.", "-nobj/", "test.cpp",
        ],
        root,
        env,
        output / f"compile-{label}.log",
    )
    obj = root / "obj/test.obj"
    data = obj.read_bytes()
    omf = describe_omf(data)
    if not omf["valid"] or omf["module_name"] != "test.cpp":
        raise RuntimeError(f"unexpected {label} OMF structure")
    return {
        "object_sha256": hashlib.sha256(data).hexdigest(),
        "semantic_omf_sha256": semantic_omf_sha256(data),
        "record_counts": omf["record_counts"],
        "translator_comments": omf["translator_comments"],
        "dependency_paths": omf["dependency_paths"],
    }


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output-dir", required=True, type=Path)
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
    if sha(RUNNER) != RUNNER_SHA256:
        raise ValueError("MS-DOS Player identity drift")

    output.mkdir(parents=True)
    reference = output / "reference"
    local = output / "local"
    reference.mkdir()
    local.mkdir()
    for relative_text in REFERENCE_FILES:
        relative = Path(relative_text)
        destination = reference / "tree" / relative
        destination.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(ROOT / "_reference/ReC98" / relative, destination)
    shutil.copytree(ROOT / "src", local / "src")

    env = os.environ.copy()
    env.update(
        WINEPREFIX=str(ROOT / ".analysis/toolchain/wineprefix"),
        WINEDEBUG="-all",
        MSDOS_PATH=r"C:\TC4\BIN;C:\TASM50\BIN",
    )
    reference_result = compile_variant(
        "reference", reference, "tree", env, output
    )
    local_result = compile_variant(
        "local", local, "src/main/include", env, output
    )
    passed = (
        reference_result["semantic_omf_sha256"]
        == local_result["semantic_omf_sha256"]
    )
    receipt = {
        "schema_version": 1,
        "observed_utc": datetime.now(timezone.utc).isoformat(timespec="seconds"),
        "scope": "TH04 MAIN product-owned sound header compiler ABI",
        "runner_sha256": RUNNER_SHA256,
        "compiler_flags": FLAGS,
        "harness_sha256": hashlib.sha256(TEST.encode("ascii")).hexdigest(),
        "local_header_sha256": sha(LOCAL_HEADER),
        "shared_header_sha256": sha(SHARED_HEADER),
        "reference_header_sha256": sha(ROOT / "_reference/ReC98/th04/snd/snd.h"),
        "reference": reference_result,
        "local": local_result,
        "passed": passed,
        "limit": (
            "Compiler-observed declaration equivalence for the exercised sound ABI; "
            "the cold aggregate replay remains the code/layout regression gate."
        ),
    }
    (output / "receipt.json").write_text(
        json.dumps(receipt, indent=2, sort_keys=True) + "\n", encoding="utf-8"
    )
    print(json.dumps({
        "passed": passed,
        "semantic_omf_sha256": local_result["semantic_omf_sha256"],
    }, sort_keys=True))
    return 0 if passed else 1


if __name__ == "__main__":
    raise SystemExit(main())
