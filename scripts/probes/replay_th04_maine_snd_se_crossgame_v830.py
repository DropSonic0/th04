#!/usr/bin/env python3
"""Replay TH04 MAINE shared sound functions from maintained cross-game hybrid source."""
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
from replay_th04_op_help_put import loose_segment_bytes  # noqa: E402
from replay_th04_op_snd_se_crossgame_v827 import (  # noqa: E402
    SOURCES, BASE_CODE_SHA, source_policy, crossgame_release_targets,
    install_sources, object_fixups, mask_fixups,
)

SNAPSHOT = ROOT / ".analysis/gpt-web/v489-bgimage-hybrid-replay-003/a/maine/source"
TARGET = ROOT / ".analysis/reconstruction/diet-replay/v228-maine-target-roundtrip/a/restored.bin"
TARGET_SHA = "6b4547182b9d53d069c0e4efc33bdabb69065cb544bb187ced7b0f51918aa533"
BASE_EXE_SHA = "d3bdc485782a9fb953823155426ca7f0e6e8212d6bc0cdffaaa32f91df2dc90c"
BASE_MAP_SHA = "014d8dfdf31a2c42c39e76288f23842cff7bd84fb6534f6d46479ba5d8b0838e"
RELOCATIONS = 559

PRODUCER_START = 0xD5A0
PRODUCER_SIZE = 0x86
PRODUCER_SHA = "c121583eb9fdf342d197ab65cb1c23129d7dba0243edf19c4ea18431361084a0"
PLAY_START = 0xD5A0
PLAY_SIZE = 0x39
PLAY_SHA = "f62d225fbdd7fc8aa2bbad4b0b96251617478720bc63503a51f91b38aafbb52e"
UPDATE_START = 0xD5DA
UPDATE_SIZE = 0x4C
UPDATE_SHA = "29186338ee7246d51f2168c7dda72b3f5205edcd0d5ae1997ca4ad4479a97fed"
MAP_OWNER = "0CC7:0930 0086 C=CODE   S=SHARED         G=(none)  M=th04/snd_se.cpp ACBP=28"


def sha(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def sha_file(path: Path) -> str:
    return sha(path.read_bytes())


def new_output(path: Path | None) -> Path:
    if path is None:
        parent = PRIVATE / "reconstruction/probes"
        parent.mkdir(parents=True, exist_ok=True)
        return Path(tempfile.mkdtemp(prefix="maine-snd-se-v830-", dir=parent))
    out = path.resolve()
    root = PRIVATE / "reconstruction/probes"
    if out.exists() or out == root or not out.is_relative_to(root):
        raise ValueError("output must be new and below .analysis/reconstruction/probes")
    out.mkdir(parents=True)
    return out


def build_round(output: Path, label: str, target_mz, fixed: bytes, fixups: list[tuple[int, int]]) -> dict[str, object]:
    work = output / label / "maine/source"
    work.parent.mkdir(parents=True)
    compact = copy_compact_snapshot(SNAPSHOT, work, "maine")
    hashes = install_sources(work)
    if hashes != SOURCES:
        raise ValueError(f"{label}: copied maintained sound source identity drift")

    obj = work / "obj/th04/snd_se.obj"
    obj.unlink()
    tcc_maine(work, output, f"maine-snd-se-{label}", "th04/snd_se.cpp")
    code = loose_segment_bytes(obj, "SHARED")
    candidate_fixups = object_fixups(obj)
    if code != fixed or candidate_fixups != fixups:
        raise ValueError(f"{label}: maintained sound object/fixup drift")

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
        raise ValueError(f"{label}: invalid linked MAINE MZ")
    if sha_file(exe) != BASE_EXE_SHA or sha_file(map_path) != BASE_MAP_SHA:
        raise ValueError(f"{label}: linked MAINE EXE/MAP drift")
    if MAP_OWNER not in map_path.read_text(encoding="cp437", errors="replace"):
        raise ValueError(f"{label}: MAINE sound MAP owner drift")
    if [x.linear for x in linked.relocations] != [x.linear for x in target_mz.relocations]:
        raise ValueError(f"{label}: ordered relocation drift")

    producer = linked.program_image[PRODUCER_START:PRODUCER_START + PRODUCER_SIZE]
    play = linked.program_image[PLAY_START:PLAY_START + PLAY_SIZE]
    update = linked.program_image[UPDATE_START:UPDATE_START + UPDATE_SIZE]
    target_producer = target_mz.program_image[PRODUCER_START:PRODUCER_START + PRODUCER_SIZE]
    if producer != target_producer or sha(producer) != PRODUCER_SHA:
        raise ValueError(f"{label}: linked sound producer raw mismatch")
    if sha(play) != PLAY_SHA or sha(update) != UPDATE_SHA:
        raise ValueError(f"{label}: linked sound function raw mismatch")

    return {
        "compact_snapshot": compact,
        "source_sha256": hashes,
        "object_code_sha256": sha(code),
        "object_fixups": [list(x) for x in candidate_fixups],
        "linked_exe_sha256": sha_file(exe),
        "linked_map_sha256": sha_file(map_path),
        "ordered_relocations": len(linked.relocations),
        "producer_sha256": sha(producer),
        "SND_SE_PLAY_sha256": sha(play),
        "_snd_se_update_sha256": sha(update),
        "raw_difference_counts": {
            "producer": 0,
            "SND_SE_PLAY": 0,
            "_snd_se_update": 0,
        },
    }


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--output-dir", type=Path)
    args = ap.parse_args()
    output = new_output(args.output_dir)

    subprocess.run(
        [sys.executable, "scripts/preflight.py"],
        cwd=ROOT, capture_output=True, text=True, check=True,
    )
    if sha_file(RUNNER) != RUNNER_SHA:
        raise ValueError("pinned DOS runner drift")
    if sha_file(TARGET) != TARGET_SHA:
        raise ValueError("MAINE restored target identity drift")
    if sha_file(SNAPSHOT / "bin/th04/maine.exe") != BASE_EXE_SHA:
        raise ValueError("MAINE baseline EXE drift")
    if sha_file(SNAPSHOT / "obj/th04/maine.map") != BASE_MAP_SHA:
        raise ValueError("MAINE baseline MAP drift")

    policy = source_policy()
    target_mz = parse_mz(TARGET.read_bytes())
    if not target_mz.valid or len(target_mz.relocations) != RELOCATIONS:
        raise ValueError("MAINE target MZ/relocation drift")

    target_producer = target_mz.program_image[PRODUCER_START:PRODUCER_START + PRODUCER_SIZE]
    if sha(target_producer) != PRODUCER_SHA:
        raise ValueError("MAINE target sound producer drift")

    base_obj = SNAPSHOT / "obj/th04/snd_se.obj"
    fixed = loose_segment_bytes(base_obj, "SHARED")
    fixups = object_fixups(base_obj)
    if len(fixed) != PRODUCER_SIZE or sha(fixed) != BASE_CODE_SHA or len(fixups) != 20:
        raise ValueError("MAINE baseline sound object/fixup drift")
    if mask_fixups(target_producer, fixups) != fixed:
        raise ValueError("MAINE target fixed producer drift")

    crossgame = crossgame_release_targets(output, fixed, fixups)
    builds = {
        label: build_round(output, label, target_mz, fixed, fixups)
        for label in ("a", "b")
    }
    stable = lambda row: {k: v for k, v in row.items() if k != "compact_snapshot"}
    if stable(builds["a"]) != stable(builds["b"]):
        raise ValueError("independent MAINE sound cold rounds disagree")

    receipt = {
        "schema_version": 1,
        "observed_utc": datetime.now(timezone.utc).isoformat(timespec="seconds"),
        "claim_scope": "TH04 MAINE SND_SE_PLAY + snd_se_update artifact-local hybrid decoded exactness",
        "artifact": "th04-maine",
        "target_restored_sha256": TARGET_SHA,
        "sources": SOURCES,
        "source_policy": policy,
        "producer": {
            "payload_offset": hex(PRODUCER_START),
            "size": PRODUCER_SIZE,
            "target_sha256": PRODUCER_SHA,
            "fixed_sha256": BASE_CODE_SHA,
            "omf_fixup_count": len(fixups),
            "inter_function_padding": {
                "relative_offset": hex(PLAY_SIZE),
                "hex": "90",
                "authored_function_credit": False,
            },
        },
        "functions": {
            "SND_SE_PLAY": {
                "payload_offset": hex(PLAY_START),
                "size": PLAY_SIZE,
                "target_sha256": PLAY_SHA,
            },
            "_snd_se_update": {
                "payload_offset": hex(UPDATE_START),
                "size": UPDATE_SIZE,
                "target_sha256": UPDATE_SHA,
            },
        },
        "crossgame_release_targets": crossgame,
        "builds": builds,
        "conclusion": (
            "Two cold TC4.02/TLINK MAINE rounds reproduce both reviewed sound "
            "functions, the complete 0x86 producer, baseline EXE/MAP, and all "
            "559 ordered relocations raw-zero from maintained source."
        ),
        "limit": (
            "Artifact-local decoded-function exactness only. OP acceptance is not "
            "borrowed and packed MAINE.EXE exactness is not claimed."
        ),
    }
    path = output / "receipt.json"
    path.write_text(json.dumps(receipt, indent=2) + "\n")
    print(json.dumps({
        "receipt": str(path),
        "receipt_sha256": sha_file(path),
        "producer_sha256": PRODUCER_SHA,
        "SND_SE_PLAY_sha256": PLAY_SHA,
        "_snd_se_update_sha256": UPDATE_SHA,
        "ordered_relocations": RELOCATIONS,
    }, sort_keys=True))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
