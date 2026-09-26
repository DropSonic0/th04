#!/usr/bin/env python3
"""Replay TH04 MAINE SND_LOAD from the maintained v828 hybrid source.

The source is shared with the accepted OP v828 reconstruction, but MAINE gets
independent credit only after its own cold compile/link, target slice, MAP, and
ordered-relocation checks.
"""
from __future__ import annotations

import argparse
from datetime import datetime, timezone
import hashlib
import json
from pathlib import Path
import subprocess
import sys
import tempfile

ROOT = Path(__file__).resolve().parents[2]
PRIVATE = (ROOT / ".analysis").resolve()
sys.path[0:0] = [str(ROOT / "scripts"), str(ROOT / "scripts/probes")]

from compact_op_maine_snapshot import copy_compact_snapshot  # noqa: E402
from lib.pc98 import parse_mz  # noqa: E402
from probe_th04_maine_score_producers_v468 import RUNNER, RUNNER_SHA, run_checked  # noqa: E402
from probe_th04_maine_segment_topology_v470 import tcc as tcc_maine  # noqa: E402
from probe_th04_maine_staff_full_cpp_v478 import segment_bytes  # noqa: E402
from replay_th04_op_snd_load_hybrid_v828 import (  # noqa: E402
    SOURCE, HANDLE, SOURCE_SHA, HANDLE_SHA, DIAG_CODE_SHA,
    source_policy, dialog_precedent, install_source,
)

SNAPSHOT = ROOT / ".analysis/gpt-web/v489-bgimage-hybrid-replay-003/a/maine/source"
TARGET = ROOT / ".analysis/reconstruction/diet-replay/v228-maine-target-roundtrip/a/restored.bin"

TARGET_SHA = "6b4547182b9d53d069c0e4efc33bdabb69065cb544bb187ced7b0f51918aa533"
BASE_EXE_SHA = "d3bdc485782a9fb953823155426ca7f0e6e8212d6bc0cdffaaa32f91df2dc90c"
BASE_MAP_SHA = "014d8dfdf31a2c42c39e76288f23842cff7bd84fb6534f6d46479ba5d8b0838e"
RELOCATIONS = 559

START = 0xD112
SIZE = 0xEA
MOV_OFFSET = 0xC1
MOV_START = START + MOV_OFFSET
TARGET_SLICE_SHA = "1bc7a9a7074f8249bfc68f27252aa98f5248e8d79e1565e07ee4626e4808a337"
BASE_SLICE_SHA = "6c5482e959ddbe94188c8c9ef1f6bd36ee1a776aa95adee3bc1659a270ab8dff"
NATURAL_CODE_SHA = "4d0e4b2577d371061f460eb53691694f4b76c23e198e88de6c9341a8a19cef40"
MAP_OWNER = "0CC7:04A2 00EA C=CODE   S=SHARED         G=(none)  M=th04/snd_load.cpp ACBP=28"


def sha(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def sha_file(path: Path) -> str:
    return sha(path.read_bytes())


def new_output(path: Path | None) -> Path:
    if path is None:
        parent = PRIVATE / "reconstruction/probes"
        parent.mkdir(parents=True, exist_ok=True)
        return Path(tempfile.mkdtemp(prefix="maine-snd-load-v829-", dir=parent))
    out = path.resolve()
    root = PRIVATE / "reconstruction/probes"
    if out.exists() or out == root or not out.is_relative_to(root):
        raise ValueError("output must be new and below .analysis/reconstruction/probes")
    out.mkdir(parents=True)
    return out


def build_round(output: Path, label: str, target_mz, baseline_mz) -> dict[str, object]:
    work = output / label / "maine/source"
    work.parent.mkdir(parents=True)
    compact = copy_compact_snapshot(SNAPSHOT, work, "maine")
    source_hashes = install_source(work)

    obj = work / "obj/th04/snd_load.obj"
    obj.unlink()
    tcc_maine(work, output, f"maine-snd-load-{label}", "th04/snd_load.cpp")
    code = segment_bytes(obj, "SHARED")
    if len(code) != SIZE or sha(code) != DIAG_CODE_SHA:
        raise ValueError(f"{label}: maintained SND_LOAD object CODE drift")
    if code[MOV_OFFSET:MOV_OFFSET + 2] != bytes.fromhex("89c3"):
        raise ValueError(f"{label}: maintained MOV encoding drift")

    exe = work / "bin/th04/maine.exe"
    map_path = work / "obj/th04/maine.map"
    exe.unlink()
    map_path.unlink()
    run_checked(
        ["wine", str(RUNNER), "-e", "-x", "tlink", r"@obj\th04\maine.@l"],
        work,
        output / f"link-{label}.log",
        timeout=300,
    )

    linked = parse_mz(exe.read_bytes())
    if not linked.valid or len(linked.relocations) != RELOCATIONS:
        raise ValueError(f"{label}: linked MAINE MZ/relocation drift")
    if sha_file(map_path) != BASE_MAP_SHA:
        raise ValueError(f"{label}: MAINE MAP identity drift")
    if MAP_OWNER not in map_path.read_text(encoding="cp437", errors="replace"):
        raise ValueError(f"{label}: SND_LOAD MAP owner drift")

    target_relocs = [x.linear for x in target_mz.relocations]
    baseline_relocs = [x.linear for x in baseline_mz.relocations]
    linked_relocs = [x.linear for x in linked.relocations]
    if linked_relocs != target_relocs or linked_relocs != baseline_relocs:
        raise ValueError(f"{label}: ordered relocation drift")

    body = linked.program_image[START:START + SIZE]
    target_body = target_mz.program_image[START:START + SIZE]
    if body != target_body or sha(body) != TARGET_SLICE_SHA:
        raise ValueError(f"{label}: complete SND_LOAD target mismatch")

    if len(linked.program_image) != len(baseline_mz.program_image):
        raise ValueError(f"{label}: baseline program size drift")
    diffs = [
        i
        for i, (a, b) in enumerate(zip(linked.program_image, baseline_mz.program_image))
        if a != b
    ]
    if diffs != [MOV_START, MOV_START + 1]:
        raise ValueError(f"{label}: hybrid link changed unexpected bytes: {diffs[:20]}")

    return {
        "compact_snapshot": compact,
        "source_sha256": source_hashes,
        "object_code_size": len(code),
        "object_code_sha256": sha(code),
        "object_mov_hex": code[MOV_OFFSET:MOV_OFFSET + 2].hex(),
        "linked_exe_sha256": sha_file(exe),
        "linked_map_sha256": sha_file(map_path),
        "ordered_relocations": len(linked.relocations),
        "snd_load_sha256": sha(body),
        "snd_load_difference_count": 0,
        "program_difference_offsets_vs_natural_baseline": [hex(x) for x in diffs],
    }


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--output-dir", type=Path)
    args = ap.parse_args()
    output = new_output(args.output_dir)

    subprocess.run(
        [sys.executable, "scripts/preflight.py"],
        cwd=ROOT,
        capture_output=True,
        text=True,
        check=True,
    )
    if sha_file(RUNNER) != RUNNER_SHA:
        raise ValueError("pinned DOS runner drift")
    if sha_file(TARGET) != TARGET_SHA:
        raise ValueError("MAINE restored target identity drift")
    if sha_file(SNAPSHOT / "bin/th04/maine.exe") != BASE_EXE_SHA:
        raise ValueError("MAINE baseline EXE drift")
    if sha_file(SNAPSHOT / "obj/th04/maine.map") != BASE_MAP_SHA:
        raise ValueError("MAINE baseline MAP drift")
    if sha_file(SOURCE) != SOURCE_SHA or sha_file(HANDLE) != HANDLE_SHA:
        raise ValueError("maintained SND_LOAD source drift")

    policy = source_policy()
    precedent = dialog_precedent(output)

    target_mz = parse_mz(TARGET.read_bytes())
    baseline_mz = parse_mz((SNAPSHOT / "bin/th04/maine.exe").read_bytes())
    if (
        not target_mz.valid
        or not baseline_mz.valid
        or len(target_mz.relocations) != RELOCATIONS
        or len(baseline_mz.relocations) != RELOCATIONS
    ):
        raise ValueError("target/baseline MAINE MZ drift")

    target_body = target_mz.program_image[START:START + SIZE]
    baseline_body = baseline_mz.program_image[START:START + SIZE]
    if len(target_body) != SIZE or sha(target_body) != TARGET_SLICE_SHA:
        raise ValueError("MAINE target SND_LOAD identity drift")
    if len(baseline_body) != SIZE or sha(baseline_body) != BASE_SLICE_SHA:
        raise ValueError("MAINE baseline SND_LOAD identity drift")
    if target_body[MOV_OFFSET:MOV_OFFSET + 2] != bytes.fromhex("89c3"):
        raise ValueError("MAINE target MOV encoding drift")
    if baseline_body[MOV_OFFSET:MOV_OFFSET + 2] != bytes.fromhex("8bd8"):
        raise ValueError("MAINE baseline MOV encoding drift")

    base_obj = SNAPSHOT / "obj/th04/snd_load.obj"
    base_code = segment_bytes(base_obj, "SHARED")
    if len(base_code) != SIZE or sha(base_code) != NATURAL_CODE_SHA:
        raise ValueError("MAINE baseline SND_LOAD object CODE drift")

    builds = {
        label: build_round(output, label, target_mz, baseline_mz)
        for label in ("a", "b")
    }
    stable = lambda row: {k: v for k, v in row.items() if k != "compact_snapshot"}
    if stable(builds["a"]) != stable(builds["b"]):
        raise ValueError("independent MAINE SND_LOAD cold rounds disagree")

    receipt = {
        "schema_version": 1,
        "observed_utc": datetime.now(timezone.utc).isoformat(timespec="seconds"),
        "claim_scope": "TH04 MAINE SND_LOAD maintained hybrid decoded exactness",
        "artifact": "th04-maine",
        "target_restored_sha256": TARGET_SHA,
        "boundary": {
            "payload_offset": hex(START),
            "size": SIZE,
            "target_sha256": TARGET_SLICE_SHA,
            "natural_baseline_sha256": BASE_SLICE_SHA,
            "mov_relative_offset": hex(MOV_OFFSET),
            "mov_target_hex": "89c3",
            "mov_natural_hex": "8bd8",
        },
        "maintained_source": {
            "source": str(SOURCE.relative_to(ROOT)),
            "source_sha256": SOURCE_SHA,
            "handle_fragment": str(HANDLE.relative_to(ROOT)),
            "handle_fragment_sha256": HANDLE_SHA,
        },
        "source_policy": policy,
        "crossgame_register_direction_precedent": precedent,
        "builds": builds,
        "conclusion": (
            "Two cold TC4.02/TLINK MAINE rounds compile the complete maintained "
            "234-byte SND_LOAD source to the target body raw-zero. Relative to the "
            "natural MAINE baseline, only the two MOV BX,AX encoding bytes change; "
            "all 559 ordered relocations and MAP ownership stay unchanged."
        ),
        "limit": (
            "Artifact-local decoded-function exactness only. OP acceptance is not "
            "borrowed, and packed MAINE.EXE exactness is not claimed."
        ),
    }
    path = output / "receipt.json"
    path.write_text(json.dumps(receipt, indent=2) + "\n")
    print(json.dumps({
        "receipt": str(path),
        "receipt_sha256": sha_file(path),
        "snd_load_sha256": TARGET_SLICE_SHA,
        "object_code_sha256": DIAG_CODE_SHA,
        "ordered_relocations": RELOCATIONS,
        "linked_exe_sha256": builds["a"]["linked_exe_sha256"],
    }, sort_keys=True))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
