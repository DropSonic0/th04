#!/usr/bin/env python3
"""Compare the product-owned TH04 MAIN pattern table with the pinned header."""

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
LOCAL_HEADER = ROOT / "src/main/sprites/main_pat.hpp"
LOCAL_CELS = ROOT / "src/main/sprites/cels.hpp"
REFERENCE_FILES = (
    "th02/sprites/cels.h",
    "th04/sprites/cels.h",
    "th04/sprites/main_pat.h",
)
FLAGS = ("-c", "-O", "-b-", "-3", "-Z", "-d", "-DGAME=4", "-ml")
TEST = r'''#include "th04/sprites/main_pat.h"

main_patnum_t probe_enum_width = PAT_EXPLOSION_BIG;

int probe_cels[] = {
    ENEMY_CELS, ENEMY_KILL_CELS, SHOT_CELS, HITSHOT_CELS,
    BULLET_CLOUD_CELS, BULLET_DECAY_CELS, BULLET_ZAP_CELS,
    BULLET_D_CELS, BULLET_V_CELS, M4C_CELS, REIMU_ORB_CELS,
    YUUKA6_CHASECROSS_CELS
};

int probe_patterns[] = {
    PAT_EXPLOSION_BIG, PAT_ENEMY_KILL, PAT_ENEMY_KILL_last,
    PAT_CLOUD_BULLET16_BLUE, PAT_CLOUD_BULLET16_BLUE_last,
    PAT_CLOUD_BULLET16_RED, PAT_OPTION_REIMU, PAT_OPTION_MARISA,
    PAT_HITSHOT, PAT_HITSHOT_last, PAT_ITEM,
    PAT_BULLET16_N_OUTLINED_BALL_WHITE,
    PAT_BULLET16_N_OUTLINED_BALL_RED,
    PAT_BULLET16_N_OUTLINED_BALL_GREEN,
    PAT_BULLET16_N_OUTLINED_BALL_BLUE, PAT_BULLET16_N_STAR,
    PAT_BULLET16_N_BALL_BLUE, PAT_BULLET16_N_SMALL_BALL_YELLOW,
    PAT_BULLET16_N_CROSS_YELLOW, PAT_BULLET16_N_SMALL_BALL_RED,
    PAT_BULLET16_N_BALL_RED, PAT_BULLET16_N_HEART_BALL_RED,
    PAT_EXPLOSION_SMALL, PAT_SHOT_LASER_RING,
    PAT_SHOT_LASER_RING_last, PAT_BULLET_ZAP, PAT_BULLET_ZAP_last,
    PAT_BULLET16_D, PAT_BULLET16_D_BLUE, PAT_BULLET16_D_BLUE_last,
    PAT_BULLET16_D_YELLOW, PAT_BULLET16_D_YELLOW_last,
    PAT_DECAY_PELLET, PAT_DECAY_PELLET_last, PAT_DECAY_BULLET16,
    PAT_DECAY_BULLET16_last, PAT_STAGE, PAT_MIDBOSS4_STILL_LEFT,
    PAT_MIDBOSS4_STILL_LEFT_last, PAT_MIDBOSS4_STILL_RIGHT,
    PAT_MIDBOSS4_STILL_RIGHT_last, PAT_REIMU_ORB_BLUE,
    PAT_REIMU_ORB_BLUE_last, PAT_REIMU_ORB_YELLOW,
    PAT_REIMU_ORB_YELLOW_last, PAT_MARISA_BIT,
    PAT_YUUKA6_PARASOL_BACK_OPEN, PAT_YUUKA6_PARASOL_BACK_HALFOPEN,
    PAT_YUUKA6_PARASOL_BACK_HALFCLOSED,
    PAT_YUUKA6_PARASOL_BACK_CLOSED, PAT_YUUKA6_PARASOL_LEFT_PULL,
    PAT_YUUKA6_PARASOL_FORWARD_CLOSED,
    PAT_YUUKA6_PARASOL_FORWARD_OPEN, PAT_YUUKA6_PARASOL_SHIELD_0,
    PAT_YUUKA6_PARASOL_SHIELD_1, PAT_YUUKA6_PARASOL_SHIELD_2,
    PAT_YUUKA6_PARASOL_SHIELD_3, PAT_YUUKA6_PARASOL_LEFT,
    PAT_YUUKA6_PARASOL_LEFT_FORWARD_PULL,
    PAT_YUUKA6_PARASOL_SPIN_BACK_0, PAT_YUUKA6_PARASOL_SPIN_BACK_1,
    PAT_YUUKA6_PARASOL_SPIN_BACK_2, PAT_YUUKA6_PARASOL_SPIN_BACK_3,
    PAT_YUUKA6_PARASOL_SPIN_BACK_4, PAT_YUUKA6_PARASOL_SPIN_BACK_5,
    PAT_YUUKA6_PARASOL_SPIN_BACK_6, PAT_YUUKA6_PARASOL_SPIN_BACK_7,
    PAT_YUUKA6_PARASOL_SPIN_BACK_8, PAT_YUUKA6_PARASOL_SPIN_BACK_9,
    PAT_YUUKA6_VANISH_0, PAT_YUUKA6_VANISH_1,
    PAT_YUUKA6_VANISH_2, PAT_YUUKA6_VANISH_3,
    PAT_YUUKA6_CHASECROSS, PAT_YUUKA6_CHASECROSS_last,
    PAT_GENGETSU_TIPPING, PAT_STAGE_last, _main_patnum_t_FORCE_INT16
};
'''


def sha(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def semantic_omf_sha256(data: bytes) -> str:
    """Hash link semantics while ignoring dependency/comment records."""

    digest = hashlib.sha256()
    offset = 0
    while offset < len(data):
        if offset + 3 > len(data):
            raise ValueError("truncated OMF record header")
        record_type = data[offset]
        size = int.from_bytes(data[offset + 1:offset + 3], "little")
        end = offset + 3 + size
        if end > len(data):
            raise ValueError("truncated OMF record")
        if record_type != 0x88:  # COMENT
            digest.update(data[offset:end])
        offset = end
    return digest.hexdigest()


def compile_variant(
    label: str, root: Path, includes: tuple[str, ...], env: dict[str, str], output: Path
) -> dict[str, object]:
    (root / "obj").mkdir(parents=True)
    (root / "test.cpp").write_text(TEST, encoding="ascii")
    command = [
        "wine", str(RUNNER), "-e", "-x", "tcc", *FLAGS,
        *(f"-I{include}" for include in includes),
        "-nobj/", "test.cpp",
    ]
    result = subprocess.run(
        command, cwd=root, env=env, capture_output=True, text=True, timeout=240
    )
    (output / f"compile-{label}.log").write_text(
        json.dumps(command) + f"\nexit={result.returncode}\n"
        + result.stdout + result.stderr,
        encoding="utf-8",
    )
    if result.returncode:
        raise RuntimeError(f"TC4J failed; inspect compile-{label}.log")
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
    reference_result = compile_variant("reference", reference, ("tree",), env, output)
    local_result = compile_variant(
        "local", local, ("src/main/include", "."), env, output
    )
    passed = (
        reference_result["semantic_omf_sha256"]
        == local_result["semantic_omf_sha256"]
    )
    receipt = {
        "schema_version": 1,
        "observed_utc": datetime.now(timezone.utc).isoformat(timespec="seconds"),
        "scope": "TH04 MAIN product-owned pattern and cel constants",
        "runner_sha256": RUNNER_SHA256,
        "compiler_flags": FLAGS,
        "harness_sha256": hashlib.sha256(TEST.encode("ascii")).hexdigest(),
        "local_header_sha256": sha(LOCAL_HEADER),
        "local_cels_sha256": sha(LOCAL_CELS),
        "reference_header_sha256": sha(
            ROOT / "_reference/ReC98/th04/sprites/main_pat.h"
        ),
        "reference_cels_sha256": sha(
            ROOT / "_reference/ReC98/th04/sprites/cels.h"
        ),
        "reference": reference_result,
        "local": local_result,
        "passed": passed,
        "limit": (
            "Compiler-observed TH04 numeric-table equivalence; the cold aggregate "
            "replay remains the staged cross-game code/layout regression gate."
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
