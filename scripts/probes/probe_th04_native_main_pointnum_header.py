#!/usr/bin/env python3
"""Compare the product-owned TH04 MAIN point-number API with pinned headers."""

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
LOCAL_HEADER = ROOT / "src/main/pointnum/pointnum.hpp"
LOCAL_WRAPPER = ROOT / "src/main/include/th04/main/pointnum/pointnum.hpp"
REFERENCE_HEADER = ROOT / "_reference/ReC98/th04/main/pointnum/pointnum.hpp"
REFERENCE_FILES = (
    "platform.h",
    "pc98.h",
    "x86real.h",
    "decomp.hpp",
    "th01/main/playfld.hpp",
    "th01/math/subpixel.hpp",
    "th02/main/scroll.hpp",
    "th02/main/playfld.hpp",
    "th04/main/scroll.hpp",
    "th04/math/motion.hpp",
    "th04/main/playfld.hpp",
    "th04/main/pointnum/pointnum.hpp",
)
FLAGS = ("-c", "-O", "-b-", "-3", "-Z", "-d", "-DGAME=4", "-ml")
TEST = r'''#include "th04/main/pointnum/pointnum.hpp"

void probe_pointnum_state(void)
{
    unsigned char digits[POINTNUM_DIGITS];
    pointnum_t near *number = pointnums;
    pointnum_t near *alive = pointnums_alive[0];

    number->flag = 1;
    number->age = 0;
    number->center_cur.x.v = 0;
    number->center_prev_y.v = 0;
    number->digits_lebcd[0] = 0;
    number->width = 0;
    number->times_2 = false;
    pointnum_yellow_p = 0;
    pointnum_white_p = 0;
    pointnum_times_2 = false;
    pointnum_first_yellow_alive = alive;

    pointnum_digits_set((unsigned char near *)digits, 1234);
    pointnums_add_white(0, 0, 100);
    pointnums_add_yellow(0, 0, 200);
    pointnums_init();
    pointnums_invalidate();
    pointnums_update();
    pointnum_put_raw(0, 0);
    pointnums_render();
}

int probe_pointnum_layout(void)
{
    return sizeof(pointnum_t)
        + sizeof(pointnums)
        + sizeof(pointnums_alive)
        + sizeof(pointnum_first_yellow_alive)
        + sizeof(pointnum_yellow_p)
        + sizeof(pointnum_white_p)
        + sizeof(pointnum_times_2)
        + POINTNUM_POPUP_FRAMES
        + POINTNUM_FRAMES
        + POINTNUM_DIGITS
        + POINTNUM_YELLOW_COUNT
        + POINTNUM_WHITE_COUNT
        + POINTNUM_COUNT;
}
'''


def sha(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def semantic_omf_sha256(data: bytes) -> str:
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
        if record_type != 0x88:
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
        "scope": "TH04 MAIN point-number API header ABI",
        "runner_sha256": RUNNER_SHA256,
        "compiler_flags": FLAGS,
        "harness_sha256": hashlib.sha256(TEST.encode("ascii")).hexdigest(),
        "local_header_sha256": sha(LOCAL_HEADER),
        "local_wrapper_sha256": sha(LOCAL_WRAPPER),
        "reference_header_sha256": sha(REFERENCE_HEADER),
        "reference": reference_result,
        "local": local_result,
        "passed": passed,
        "limit": (
            "Compiler-observed pointnum structure, constants, external state, "
            "and entry ABIs only; pointnum DATA/BSS placement/order, sprite "
            "storage, standalone MAIN placement, and PC-98 startup remain "
            "outside this probe."
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
