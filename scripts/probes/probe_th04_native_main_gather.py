#!/usr/bin/env python3
"""Compare the product-owned TH04 MAIN gather API with pinned headers."""

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
LOCAL_HEADER = ROOT / "src/main/gather.hpp"
REFERENCE_FILES = (
    "platform.h",
    "pc98.h",
    "th01/math/subpixel.hpp",
    "th01/main/playfld.hpp",
    "th01/rank.h",
    "th02/main/entity.hpp",
    "th02/main/playfld.hpp",
    "th02/main/scroll.hpp",
    "th02/sprites/cels.h",
    "th04/main/bullet/types.h",
    "th04/main/bullet/bullet.hpp",
    "th04/main/gather.hpp",
    "th04/main/playfld.hpp",
    "th04/main/rank.hpp",
    "th04/main/scroll.hpp",
    "th04/math/motion.hpp",
    "th04/sprites/cels.h",
)
FLAGS = ("-c", "-O", "-b-", "-3", "-Z", "-d", "-DGAME=4", "-ml")
TEST = r'''#include <stddef.h>
#include "th04/main/gather.hpp"

void probe_gather_template(void)
{
    gather_template_init();
    gather_template.center.x.v = TO_SP(12);
    gather_template.center.y.v = TO_SP(20);
    gather_template.velocity.x.v = TO_SP(1);
    gather_template.velocity.y.v = TO_SP(2);
    gather_template.radius.v = TO_SP(64);
    gather_template.ring_points = 8;
    gather_template.col = 9;
    gather_template.angle_delta = 2;
    gather_circles[0].flag = F_FREE;
    gather_circles[0].center.cur = gather_template.center;
    gather_circles[0].bullet_template.spawn_type = BST_GATHER_ONLY;
}

void probe_gather_entries(void)
{
    gather_add_bullets();
    gather_add_only();
    gather_add_only_3stack(0, static_cast<vc2>(7), static_cast<vc2>(9));
    gather_add_only_3stack(2, static_cast<vc2>(7), static_cast<vc2>(9));
    gather_add_only_3stack(4, static_cast<vc2>(7), static_cast<vc2>(9));
    gather_point_render(32, 16);
    gather_update();
    gather_render();
}

int probe_gather_layout(void)
{
    return (
        sizeof(entity_flag_t)
        + sizeof(PlayfieldPoint)
        + sizeof(PlayfieldMotion)
        + sizeof(gather_t)
        + sizeof(gather_template_t)
        + sizeof(gather_circles)
        + sizeof(gather_template)
        + GATHER_POINT_W + GATHER_POINT_H + GATHER_FRAMES
        + GATHER_COUNT + GATHER_CAP
    );
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
        reference_destination = reference / "tree" / relative
        reference_destination.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(ROOT / "_reference/ReC98" / relative, reference_destination)
        if relative_text != "th04/main/gather.hpp":
            local_destination = local / relative
            local_destination.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(ROOT / "_reference/ReC98" / relative, local_destination)
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
        "scope": "TH04 MAIN product-owned gather header ABI",
        "runner_sha256": RUNNER_SHA256,
        "compiler_flags": FLAGS,
        "harness_sha256": hashlib.sha256(TEST.encode("ascii")).hexdigest(),
        "local_header_sha256": sha(LOCAL_HEADER),
        "reference_header_sha256": sha(ROOT / "_reference/ReC98/th04/main/gather.hpp"),
        "reference": reference_result,
        "local": local_result,
        "passed": passed,
        "limit": (
            "Compiler-observed declaration and layout equivalence for the exercised "
            "TH04 gather API; the cold aggregate replay remains the staged "
            "code/layout gate and gather BSS ownership is separate."
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
