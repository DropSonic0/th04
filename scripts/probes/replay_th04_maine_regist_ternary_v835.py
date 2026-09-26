#!/usr/bin/env python3
"""Replay MAINE regist_menu from ordinary conditional-expression C++.

v831 proved that switch and direct if/goto each reproduce only one half of the
target frontier. v835 uses an ordinary conditional expression whose semantics
are exactly the intended input-lock update and lets pinned TC4.02 naturally
emit the target direct-CMP / JZ / JMP CFG. No decompilation barrier, inline ASM,
register forcing, emitted bytes, or post-build patch is used for regist_menu.
"""
from __future__ import annotations

import argparse
from collections import Counter
from datetime import datetime, timezone
import hashlib
import json
from pathlib import Path
import re
import shutil
import sys
import tempfile

ROOT = Path(__file__).resolve().parents[2]
PRIVATE = (ROOT / ".analysis").resolve()
sys.path[0:0] = [str(ROOT / "scripts"), str(ROOT / "scripts/probes")]

from compact_op_maine_snapshot import copy_compact_snapshot
from lib.pc98 import parse_mz
from probe_th04_maine_score_producers_v468 import RUNNER, RUNNER_SHA, run_checked
from probe_th04_maine_segment_topology_v470 import tcc
from probe_th04_maine_staff_full_cpp_v478 import segment_bytes
import replay_th04_maine_score_egc_hybrid_v834 as egc
import replay_th04_maine_score_insert as prior

SOURCE = ROOT / "src/maine/score/regist_menu.inl"
SOURCE_SHA = "50dbda7f9f31a0ffa5fa6d5c053bb093e03ddc20ca044b59f9c53daeda0d424b"

START = 0xC814
SIZE = 0x39C
LOCAL = START - prior.SCORE_OWNER_OFFSET
TARGET_SHA = "7bef6c89de52462b75acb65a47dd919ce27eb38d5d920c082d73141e54cdad6b"
OBJECT_SLICE_SHA = "e9e295118a623d0f1f0e9212c940cfa06c3e6e6740c099eab4644cbaac0b9f2c"
FRONTIER_REL = 0x345
OBJECT_FRONTIER = bytes.fromhex("833e0000007402eb05")
TARGET_FRONTIER = bytes.fromhex("833e421b007402eb05")

REG_RE = re.compile(
    r"void near regist_menu\(void\)\n\{.*?\n\}\n\n\n"
    r'#include "src/maine/score/score_egc_start_copy\.inl"',
    re.S,
)


def sha(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def sha_file(path: Path) -> str:
    return sha(path.read_bytes())


def new_output(path: Path | None) -> Path:
    root = PRIVATE / "reconstruction/probes"
    root.mkdir(parents=True, exist_ok=True)
    if path is None:
        return Path(tempfile.mkdtemp(prefix="v835-maine-regist-", dir=root))
    out = path.resolve()
    if out.exists() or out == root or not out.is_relative_to(root.resolve()):
        raise ValueError("output must be new below .analysis/reconstruction/probes")
    out.mkdir(parents=True)
    return out


def source_policy() -> dict[str, object]:
    text = SOURCE.read_text()
    forbidden = (
        "optimization_barrier", "__emit__", "codestring", "asm {",
        "keep_0", "_outportb_",
    )
    bad = [x for x in forbidden if x in text]
    if bad:
        raise ValueError(f"forbidden helper(s) in maintained regist_menu: {bad}")
    exact_form = "(key_det != INPUT_NONE) ? input_locked : (input_locked = INPUT_NONE);"
    if text.count(exact_form) != 1:
        raise ValueError("maintained conditional-expression frontier drift")
    return {
        "path": str(SOURCE.relative_to(ROOT)),
        "sha256": sha_file(SOURCE),
        "ordinary_cpp_only": True,
        "frontier_expression": exact_form,
        "semantics": (
            "preserve input_locked for a new nonzero input; clear it once "
            "key_det returns to INPUT_NONE"
        ),
        "original_source_spelling_claimed": False,
    }


def overlay(work: Path) -> str:
    # First install the already-accepted v834 EGC helper so the complete current
    # SCORE owner is replayed. This leaves regist_menu as pinned v489 scaffold.
    egc.overlay(work)

    dst = work / "src/maine/score/regist_menu.inl"
    dst.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(SOURCE, dst)

    score = work / "th04/score86.cpp"
    text = score.read_text()
    matches = list(REG_RE.finditer(text))
    if len(matches) != 1:
        raise ValueError(f"regist_menu replacement anchor drift: {len(matches)}")
    m = matches[0]
    score.write_text(
        text[:m.start()]
        + '#include "src/maine/score/regist_menu.inl"\n\n\n'
        + '#include "src/maine/score/score_egc_start_copy.inl"'
        + text[m.end():]
    )
    return sha_file(score)


def build_round(
    out: Path,
    label: str,
    target_mz,
    base_code: bytes,
    base_fixups: list[tuple[int, int]],
    base_map_norm: str,
) -> dict[str, object]:
    work = out / label / "maine/source"
    work.parent.mkdir(parents=True)
    compact = copy_compact_snapshot(prior.SNAPSHOT, work, "maine")
    patched_sha = overlay(work)

    obj = work / "obj/th04/scoreall.obj"
    obj.unlink()
    tcc(work, out, f"regist-{label}", "th04/scoreall.cpp")
    code = segment_bytes(obj, "SCORE_TEXT")
    if code != base_code or sha(code) != prior.BASE_SCORE_CODE_SHA:
        raise ValueError(f"{label}: complete SCORE CODE drift")

    boss = code[LOCAL:LOCAL + SIZE]
    if len(boss) != SIZE or sha(boss) != OBJECT_SLICE_SHA:
        raise ValueError(f"{label}: regist_menu object bytes drift")
    if boss[FRONTIER_REL:FRONTIER_REL + len(OBJECT_FRONTIER)] != OBJECT_FRONTIER:
        raise ValueError(f"{label}: object frontier topology drift")

    fixes = egc.score_fixups(obj)
    missing = list((Counter(base_fixups) - Counter(fixes)).elements())
    extra = list((Counter(fixes) - Counter(base_fixups)).elements())
    if (
        len(fixes) != egc.CAND_FIXUP_COUNT
        or missing != [egc.REMOVED_FIXUP]
        or extra
    ):
        raise ValueError(
            f"{label}: OMF delta beyond accepted v834 helper: "
            f"missing={missing}, extra={extra}"
        )

    exe = work / "bin/th04/maine.exe"
    mp = work / "obj/th04/maine.map"
    exe.unlink()
    mp.unlink()
    run_checked(
        ["wine", str(RUNNER), "-e", "-x", "tlink", r"@obj\th04\maine.@l"],
        work,
        out / f"link-{label}.log",
        timeout=300,
    )

    linked = parse_mz(exe.read_bytes())
    if not linked.valid or sha_file(exe) != prior.BASE_EXE_SHA:
        raise ValueError(f"{label}: final MAINE EXE drift")
    if sha_file(mp) != egc.CAND_MAP_SHA or egc.normalized_map(mp) != base_map_norm:
        raise ValueError(f"{label}: MAP drift beyond accepted v834 _address_0 delta")
    if [x.linear for x in linked.relocations] != [x.linear for x in target_mz.relocations]:
        raise ValueError(f"{label}: final ordered relocation drift")

    linked_boss = linked.program_image[START:START + SIZE]
    target_boss = target_mz.program_image[START:START + SIZE]
    if linked_boss != target_boss or sha(linked_boss) != TARGET_SHA:
        raise ValueError(f"{label}: linked regist_menu target mismatch")
    if linked_boss[FRONTIER_REL:FRONTIER_REL + len(TARGET_FRONTIER)] != TARGET_FRONTIER:
        raise ValueError(f"{label}: linked target frontier drift")

    producer = linked.program_image[
        prior.SCORE_OWNER_OFFSET:prior.SCORE_OWNER_OFFSET + prior.SCORE_OWNER_SIZE
    ]
    target_producer = target_mz.program_image[
        prior.SCORE_OWNER_OFFSET:prior.SCORE_OWNER_OFFSET + prior.SCORE_OWNER_SIZE
    ]
    if producer != target_producer:
        raise ValueError(f"{label}: linked SCORE producer target mismatch")

    diffs = [
        i for i, (a, b) in enumerate(zip(linked.program_image, target_mz.program_image))
        if a != b
    ]
    if diffs != [0xD1D3, 0xD1D4]:
        raise ValueError(f"{label}: unrelated target difference set drift: {diffs[:20]}")

    return {
        "compact_snapshot": compact,
        "patched_score86_sha256": patched_sha,
        "score_code_sha256": sha(code),
        "score_fixup_count": len(fixes),
        "accepted_v834_removed_fixup": [egc.REMOVED_FIXUP[0], hex(egc.REMOVED_FIXUP[1])],
        "regist_object_sha256": sha(boss),
        "regist_linked_sha256": sha(linked_boss),
        "linked_exe_sha256": sha_file(exe),
        "linked_map_sha256": sha_file(mp),
        "normalized_map_equal_v489": True,
        "ordered_relocations": len(linked.relocations),
        "producer_sha256": sha(producer),
        "raw_difference_counts": {"regist_menu": 0, "producer": 0},
        "program_difference_offsets_vs_target": [hex(x) for x in diffs],
    }


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--output-dir", type=Path)
    args = ap.parse_args()
    out = new_output(args.output_dir)

    if sha_file(RUNNER) != RUNNER_SHA:
        raise ValueError("runner identity drift")
    if sha_file(SOURCE) != SOURCE_SHA:
        raise ValueError("maintained regist_menu source drift")
    if sha_file(prior.TARGET) != prior.TARGET_SHA:
        raise ValueError("target identity drift")
    if sha_file(prior.SNAPSHOT / "bin/th04/maine.exe") != prior.BASE_EXE_SHA:
        raise ValueError("v489 baseline EXE drift")
    if sha_file(prior.SNAPSHOT / "obj/th04/maine.map") != prior.BASE_MAP_SHA:
        raise ValueError("v489 baseline MAP drift")

    policy = source_policy()
    target_mz = parse_mz(prior.TARGET.read_bytes())
    base_obj = prior.SNAPSHOT / "obj/th04/scoreall.obj"
    base_code = segment_bytes(base_obj, "SCORE_TEXT")
    if len(base_code) != prior.SCORE_OWNER_SIZE or sha(base_code) != prior.BASE_SCORE_CODE_SHA:
        raise ValueError("v489 SCORE CODE drift")
    base_fixups = egc.score_fixups(base_obj)
    if len(base_fixups) != egc.BASE_FIXUP_COUNT:
        raise ValueError("v489 SCORE fixup count drift")
    base_map_norm = egc.normalized_map(prior.SNAPSHOT / "obj/th04/maine.map")

    target_boss = target_mz.program_image[START:START + SIZE]
    if len(target_boss) != SIZE or sha(target_boss) != TARGET_SHA:
        raise ValueError("target regist_menu identity drift")
    if target_boss[FRONTIER_REL:FRONTIER_REL + len(TARGET_FRONTIER)] != TARGET_FRONTIER:
        raise ValueError("target frontier bytes drift")

    builds = {
        label: build_round(out, label, target_mz, base_code, base_fixups, base_map_norm)
        for label in ("a", "b")
    }
    stable = lambda row: {k: v for k, v in row.items() if k != "compact_snapshot"}
    if stable(builds["a"]) != stable(builds["b"]):
        raise ValueError("independent regist_menu cold rounds differ")

    receipt = {
        "schema_version": 1,
        "observed_utc": datetime.now(timezone.utc).isoformat(timespec="seconds"),
        "claim_scope": "TH04 MAINE regist_menu ordinary-C++ decoded exactness",
        "artifact": "th04-maine",
        "source_policy": policy,
        "compiler_mechanism": {
            "v831_negative_frontier": (
                "switch preserves JZ+JMP but uses MOV/OR; direct if/goto emits "
                "direct CMP but folds the redundant JMP"
            ),
            "v835_resolution": (
                "an ordinary conditional expression with a discarded value "
                "naturally preserves the intermediate CFG block while TC4.02 "
                "emits no code for the nonzero result arm"
            ),
            "barrier_required": False,
            "inline_asm_required": False,
            "register_forcing_required": False,
        },
        "boundary": {
            "payload_offset": hex(START),
            "size": SIZE,
            "target_sha256": TARGET_SHA,
            "producer_payload_offset": hex(prior.SCORE_OWNER_OFFSET),
            "producer_size": prior.SCORE_OWNER_SIZE,
            "frontier_relative_offset": hex(FRONTIER_REL),
            "target_frontier_hex": TARGET_FRONTIER.hex(),
        },
        "surrounding_scaffold": (
            "v834 SCORE EGC source is included because it is already accepted; "
            "score_rect_copy remains pinned v489 producer context. Exact credit "
            "from this replay is limited to regist_menu."
        ),
        "builds": builds,
        "limit": (
            "Decoded-function exactness only. The maintained expression is a "
            "semantically equivalent compiler mechanism; literal original-source "
            "spelling is not claimed. Packed-file and whole-MAINE exactness are "
            "not claimed."
        ),
    }
    path = out / "receipt.json"
    path.write_text(json.dumps(receipt, indent=2) + "\n")
    print(json.dumps({
        "receipt": str(path),
        "receipt_sha256": sha_file(path),
        "regist_menu_sha256": TARGET_SHA,
        "score_code_sha256": prior.BASE_SCORE_CODE_SHA,
    }, sort_keys=True))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
