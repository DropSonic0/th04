#!/usr/bin/env python3
"""Revalidate the MAINE regist_menu compiler frontier on the retained v489 baseline.

This is deliberately non-exact. It tests the two natural source families that
separately reproduce the two halves of the target zero-test topology:
- switch: preserves target JZ+JMP size/topology but lowers key_det through AX;
- if/goto: emits direct-memory CMP but collapses the redundant JMP, shortening
  the SCORE producer by two bytes.

The target-exact optimization_barrier form is retained only as a diagnostic
control and receives no authored-source credit.
"""
from __future__ import annotations

import argparse
from datetime import datetime, timezone
import hashlib
import json
from pathlib import Path
import shutil
import sys
import tempfile

ROOT = Path(__file__).resolve().parents[2]
PRIVATE = (ROOT / ".analysis").resolve()
sys.path[0:0] = [str(ROOT / "scripts"), str(ROOT / "scripts/probes")]

from compact_op_maine_snapshot import copy_compact_snapshot  # noqa: E402
from probe_th04_maine_segment_topology_v470 import tcc  # noqa: E402
from probe_th04_maine_staff_full_cpp_v478 import segment_bytes  # noqa: E402

SNAPSHOT = ROOT / ".analysis/gpt-web/v489-bgimage-hybrid-replay-003/a/maine/source"
REFERENCE_OBJ = SNAPSHOT / "obj/th04/scoreall.obj"
REFERENCE_SOURCE = SNAPSHOT / "th04/score86.cpp"

SCORE_SIZE = 0xB30
SCORE_SHA = "d8180ee4acb342cefe168337bb6dff4986b6bac625a4c614128c1b9430cec693"
REGIST_OFFSET = 0x6CB
REGIST_SIZE = 0x39C
REGIST_SHA = "e9e295118a623d0f1f0e9212c940cfa06c3e6e6740c099eab4644cbaac0b9f2c"
FRONTIER_OFFSET = 0x345
TARGET_FRONTIER = bytes.fromhex("833e0000007402eb05")
SWITCH_FRONTIER = bytes.fromhex("a100000bc07402eb05")
IF_GOTO_PREFIX = bytes.fromhex("833e0000007505c746fc0000c646f7")
IF_GOTO_SCORE_SIZE = 0xB2E
SWITCH_REGIST_DIFFS = [0x345, 0x346, 0x348, 0x349]

OLD = """\t\t\t\tif(key_det != INPUT_NONE) {
\t\t\t\t\toptimization_barrier();
\t\t\t\t} else {
\t\t\t\t\tinput_locked = 0;
\t\t\t\t}
\t\t\t\tinput_delay = 0;"""

VARIANTS = {
    "switch": """\t\t\t\tswitch(key_det) {
\t\t\t\tcase INPUT_NONE:
\t\t\t\t\tinput_locked = 0;
\t\t\t\t\tbreak;
\t\t\t\tdefault:
\t\t\t\t\tbreak;
\t\t\t\t}
\t\t\t\tinput_delay = 0;""",
    "if_goto": """\t\t\t\tif(key_det != INPUT_NONE) {
\t\t\t\t\tgoto input_delay_reset;
\t\t\t\t}
\t\t\t\tinput_locked = 0;
input_delay_reset:
\t\t\t\tinput_delay = 0;""",
}


def sha(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def output_dir(path: Path | None) -> Path:
    if path is None:
        root = PRIVATE / "reconstruction/probes"
        root.mkdir(parents=True, exist_ok=True)
        return Path(tempfile.mkdtemp(prefix="v831-maine-regist-", dir=root))
    out = path.resolve()
    root = (PRIVATE / "reconstruction/probes").resolve()
    if out.exists() or out == root or not out.is_relative_to(root):
        raise ValueError("output must be new below .analysis/reconstruction/probes")
    out.mkdir(parents=True)
    return out


def compile_variant(out: Path, variant: str, replacement: str, label: str) -> dict[str, object]:
    work = out / variant / label / "maine/source"
    work.parent.mkdir(parents=True)
    copy_compact_snapshot(SNAPSHOT, work, "maine")

    source = work / "th04/score86.cpp"
    text = source.read_text()
    if text.count(OLD) != 1:
        raise ValueError("v489 regist source anchor drift")
    source.write_text(text.replace(OLD, replacement, 1))

    obj = work / "obj/th04/scoreall.obj"
    obj.unlink()
    tcc(work, out, f"{variant}-{label}", "th04/scoreall.cpp")
    score = segment_bytes(obj, "SCORE_TEXT")
    regist = score[REGIST_OFFSET:REGIST_OFFSET + REGIST_SIZE]
    return {
        "score_size": len(score),
        "score_sha256": sha(score),
        "regist_size": len(regist),
        "regist_sha256": sha(regist),
        "frontier_hex": regist[FRONTIER_OFFSET:FRONTIER_OFFSET + 15].hex(),
        "regist_difference_offsets": [],
    }


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--output-dir", type=Path)
    args = ap.parse_args()
    out = output_dir(args.output_dir)

    reference = segment_bytes(REFERENCE_OBJ, "SCORE_TEXT")
    if len(reference) != SCORE_SIZE or sha(reference) != SCORE_SHA:
        raise ValueError("v489 SCORE producer identity drift")
    ref_regist = reference[REGIST_OFFSET:REGIST_OFFSET + REGIST_SIZE]
    if len(ref_regist) != REGIST_SIZE or sha(ref_regist) != REGIST_SHA:
        raise ValueError("v489 regist_menu reference identity drift")
    if ref_regist[FRONTIER_OFFSET:FRONTIER_OFFSET + len(TARGET_FRONTIER)] != TARGET_FRONTIER:
        raise ValueError("target-exact frontier topology drift")
    if "optimization_barrier();" not in REFERENCE_SOURCE.read_text():
        raise ValueError("diagnostic barrier control source drift")

    builds: dict[str, dict[str, dict[str, object]]] = {}
    for variant, replacement in VARIANTS.items():
        builds[variant] = {}
        for label in ("a", "b"):
            row = compile_variant(out, variant, replacement, label)
            score_path = out / variant / label / "maine/source/obj/th04/scoreall.obj"
            score = segment_bytes(score_path, "SCORE_TEXT")
            regist = score[REGIST_OFFSET:REGIST_OFFSET + REGIST_SIZE]
            row["regist_difference_offsets"] = [
                i for i, (a, b) in enumerate(zip(regist, ref_regist)) if a != b
            ]
            builds[variant][label] = row

    for variant in VARIANTS:
        if builds[variant]["a"] != builds[variant]["b"]:
            raise ValueError(f"{variant}: A/B cold builds disagree")

    switch = builds["switch"]["a"]
    if (
        switch["score_size"] != SCORE_SIZE
        or switch["regist_size"] != REGIST_SIZE
        or switch["regist_difference_offsets"] != SWITCH_REGIST_DIFFS
        or bytes.fromhex(switch["frontier_hex"])[:len(SWITCH_FRONTIER)] != SWITCH_FRONTIER
    ):
        raise ValueError(f"switch frontier drift: {switch}")

    direct = builds["if_goto"]["a"]
    if (
        direct["score_size"] != IF_GOTO_SCORE_SIZE
        or bytes.fromhex(direct["frontier_hex"])[:len(IF_GOTO_PREFIX)] != IF_GOTO_PREFIX
    ):
        raise ValueError(f"if/goto frontier drift: {direct}")

    receipt = {
        "schema_version": 1,
        "observed_utc": datetime.now(timezone.utc).isoformat(timespec="seconds"),
        "claim_scope": "TH04 MAINE regist_menu current-v489 natural-source frontier; non-exact",
        "reference": {
            "snapshot": str(SNAPSHOT.relative_to(ROOT)),
            "score_size": SCORE_SIZE,
            "score_sha256": SCORE_SHA,
            "regist_offset": hex(REGIST_OFFSET),
            "regist_size": REGIST_SIZE,
            "regist_sha256": REGIST_SHA,
            "target_frontier_offset": hex(FRONTIER_OFFSET),
            "target_frontier_hex": TARGET_FRONTIER.hex(),
            "diagnostic_barrier_present": True,
        },
        "builds": builds,
        "conclusion": (
            "On the retained v489 producer, natural switch source preserves the target "
            "JZ+JMP topology and 924-byte regist_menu size but emits MOV/OR for key_det. "
            "Natural if/goto emits direct-memory CMP but TC4J removes the redundant JMP, "
            "shortening SCORE_TEXT by two bytes and shifting later code. The two target "
            "properties still do not co-occur in tested admissible source."
        ),
        "limit": (
            "No exact credit. optimization_barrier remains a decompilation/code-layout "
            "helper and is only a target-exact diagnostic control."
        ),
    }
    path = out / "receipt.json"
    path.write_text(json.dumps(receipt, indent=2) + "\n")
    print(json.dumps({
        "receipt": str(path),
        "receipt_sha256": sha(path.read_bytes()),
        "switch_differences": SWITCH_REGIST_DIFFS,
        "if_goto_score_size": IF_GOTO_SCORE_SIZE,
        "exact": False,
    }, sort_keys=True))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
