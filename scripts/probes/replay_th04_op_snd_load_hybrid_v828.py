#!/usr/bin/env python3
"""Replay TH04 OP SND_LOAD from complete maintained hybrid source.

The complete 234-byte function is maintained C++ plus the already accepted
DS save/restore fragments and one two-byte MOV BX,AX register-direction
primitive.  That MOV encoding is independently corroborated by the accepted
TH04/TH05 dialog_face_unput_8 hybrid precedent (v394).
"""
from __future__ import annotations

import argparse
import csv
from datetime import datetime, timezone
import hashlib
import json
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile

ROOT = Path(__file__).resolve().parents[2]
PRIVATE = (ROOT / ".analysis").resolve()
sys.path[0:0] = [str(ROOT / "scripts"), str(ROOT / "scripts/probes")]

from compact_op_maine_snapshot import copy_compact_snapshot  # noqa: E402
from lib.pc98 import parse_mz  # noqa: E402
from lib.targets import find_artifact, load_target_manifest, read_verified_artifact  # noqa: E402
from probe_th04_final_blocker_crossartifact import restore_diet  # noqa: E402
from probe_th04_maine_score_producers_v468 import RUNNER, RUNNER_SHA, run_checked  # noqa: E402
from probe_th04_op_snd_load_provenance_v823 import (  # noqa: E402
    TARGET, TARGET_SHA, BASE_EXE_SHA, DIAG_EXE_SHA, BASE_MAP_SHA, RELOCATIONS,
    START, SIZE, MOV_OFFSET, MOV_START, TARGET_SLICE_SHA, DIAG_CODE_SHA,
)
from replay_th04_op_help_put import SNAPSHOT, loose_segment_bytes, tcc_op  # noqa: E402
from replay_th04_zun_source_only import source_closure  # noqa: E402

SOURCE = ROOT / "src/shared/sound/load.cpp"
HANDLE = ROOT / "src/shared/sound/load_handle_mov.inl"
SOURCE_SHA = "688dedb69a70c600aac8f6cbf55cc9d0ac91de2a6b2b0eb49a863f21d18c5c13"
HANDLE_SHA = "e083032b401f77683d6430f6621fdfeb1c6f6855565fdec89b8e365ba462b86a"

DIALOG_SOURCE = ROOT / "src/main/dialog/render_primitives.inl"
DIALOG_TH04_START = 0xD04E
DIALOG_TH05_START = 0x13ACE
DIALOG_SIZE = 0x4A
DIALOG_MOV_REL = 0x0A
DIALOG_COMMON = bytes.fromhex("8b460489c3")
DIALOG_DIFFS = [0x05, 0x06, 0x25, 0x26, 0x41]

MAP_OWNER = "0DA1:03BA 00EA C=CODE   S=SHARED         G=(none)  M=th04/snd_load.cpp ACBP=28"


def sha(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def sha_file(path: Path) -> str:
    return sha(path.read_bytes())


def new_output(path: Path | None) -> Path:
    if path is None:
        parent = PRIVATE / "reconstruction/probes"
        parent.mkdir(parents=True, exist_ok=True)
        return Path(tempfile.mkdtemp(prefix="op-snd-load-v828-", dir=parent))
    out = path.resolve()
    if out.exists() or not out.is_relative_to(PRIVATE):
        raise ValueError("output must be new and below .analysis")
    out.mkdir(parents=True)
    return out


def source_policy() -> dict[str, object]:
    if sha_file(SOURCE) != SOURCE_SHA or sha_file(HANDLE) != HANDLE_SHA:
        raise ValueError("maintained SND_LOAD source identity drift")

    source = SOURCE.read_text()
    handle = HANDLE.read_text()

    required_includes = (
        "load_prefix.inl",
        "load_push_ds.inl",
        "load_open.inl",
        "load_handle_mov.inl",
        "load_func.inl",
        "load_dispatch_read.inl",
        "load_pop_ds.inl",
        "load_tail.inl",
    )
    for marker in required_includes:
        if marker not in source:
            raise ValueError(f"SND_LOAD maintained composition drift: {marker}")

    for forbidden in ("__emit__", "#pragma codestring", "target_bytes", "db 0x"):
        if forbidden in source + handle:
            raise ValueError(f"SND_LOAD source contains byte-forcing surface: {forbidden}")

    if handle.count("asm { mov bx, ax; }") != 1:
        raise ValueError("bounded MOV BX,AX primitive drift")
    if "asm {" in source:
        raise ValueError("top-level SND_LOAD composition unexpectedly contains assembly")

    return {
        "ordinary_or_previously_accepted_source": [
            "prefix filename and mode logic",
            "DOS open",
            "function-parameter load",
            "driver dispatch and DOS read",
            "DOS close",
            "DS save/restore fragments accepted in v391",
        ],
        "new_symbolic_primitive": "one MOV BX,AX register-direction instruction",
        "no_emit": True,
        "no_codestring": True,
        "no_target_byte_arrays": True,
        "no_post_build_patch": True,
        "original_source_spelling_claimed": False,
    }


def dialog_precedent(output: Path) -> dict[str, object]:
    manifest = load_target_manifest(ROOT / "config/targets.toml")

    th04_art = find_artifact(manifest, "th04-main")
    th04_raw = read_verified_artifact(ROOT, th04_art)
    th04_mz = parse_mz(th04_raw)
    if not th04_mz.valid:
        raise ValueError("TH04 MAIN target invalid")

    th05_art = find_artifact(manifest, "th05-main-smoke")
    th05_packed = read_verified_artifact(ROOT, th05_art)
    restore_root = output / "dialog-precedent"
    restore_root.mkdir()
    if len(th05_packed) >= 0x20 and th05_packed[0x1C:0x20].lower() == b"diet":
        th05_raw, restore_meta = restore_diet(th05_art, th05_packed, restore_root)
    else:
        th05_raw = th05_packed
        restore_meta = {"restore_log_sha256": None}
    th05_mz = parse_mz(th05_raw)
    if not th05_mz.valid:
        raise ValueError("TH05 MAIN restored target invalid")

    a = th04_mz.program_image[DIALOG_TH04_START:DIALOG_TH04_START + DIALOG_SIZE]
    b = th05_mz.program_image[DIALOG_TH05_START:DIALOG_TH05_START + DIALOG_SIZE]
    if len(a) != DIALOG_SIZE or len(b) != DIALOG_SIZE:
        raise ValueError("dialog precedent extent drift")
    if a[7:12] != DIALOG_COMMON or b[7:12] != DIALOG_COMMON:
        raise ValueError("dialog MOV BX,AX precedent sequence drift")

    diffs = [i for i, (x, y) in enumerate(zip(a, b)) if x != y]
    if diffs != DIALOG_DIFFS:
        raise ValueError(f"dialog TH04/TH05 fixed-body drift: {diffs}")

    dialog_source = DIALOG_SOURCE.read_text()
    if "asm { mov bx, ax; }" not in dialog_source:
        raise ValueError("accepted dialog hybrid MOV primitive drift")

    evidence = {
        row["id"]: row
        for row in csv.DictReader((ROOT / "config/evidence.csv").open(newline=""))
        if row["id"] in {
            "ev-th04-main-dialog-render-crossgame-v382",
            "ev-th04-main-dialog-render-hybrid-provenance-v394",
            "ev-th04-main-dialog-render-raw-v394",
        }
    }
    if set(evidence) != {
        "ev-th04-main-dialog-render-crossgame-v382",
        "ev-th04-main-dialog-render-hybrid-provenance-v394",
        "ev-th04-main-dialog-render-raw-v394",
    } or any(row["result"] != "pass" for row in evidence.values()):
        raise ValueError("accepted v394 dialog precedent evidence drift")

    shutil.rmtree(restore_root)
    return {
        "th04_dialog_payload_offset": hex(DIALOG_TH04_START),
        "th05_dialog_payload_offset": hex(DIALOG_TH05_START),
        "body_size": DIALOG_SIZE,
        "common_mov_sequence_hex": DIALOG_COMMON.hex(),
        "mov_relative_offset": hex(DIALOG_MOV_REL),
        "only_body_difference_offsets": [hex(x) for x in diffs],
        "th04_body_sha256": sha(a),
        "th05_body_sha256": sha(b),
        "th05_restore_log_sha256": restore_meta["restore_log_sha256"],
        "accepted_precedent_evidence_ids": sorted(evidence),
        "finding": (
            "TH04 and TH05 release targets preserve the same BP-argument load followed by "
            "89 C3 inside the accepted dialog_face_unput_8 hybrid. v394 already treats "
            "this register-direction operation as a bounded cross-game-corroborated primitive."
        ),
    }


def install_source(work: Path) -> dict[str, str]:
    closure = source_closure(ROOT, ("src/shared/sound/load.cpp",))
    for rel in closure:
        src = ROOT / rel
        dst = work / rel
        dst.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(src, dst)
    (work / "th04/snd_load.cpp").write_text('#include "src/shared/sound/load.cpp"\n')
    return {rel: sha_file(ROOT / rel) for rel in closure}


def build_round(output: Path, label: str, target_mz, baseline_mz) -> dict[str, object]:
    work = output / label / "op/source"
    work.parent.mkdir(parents=True)
    compact = copy_compact_snapshot(SNAPSHOT, work, "op")
    source_hashes = install_source(work)

    obj = work / "obj/th04/snd_load.obj"
    obj.unlink()
    tcc_op(work, output, f"snd-load-hybrid-{label}", "th04/snd_load.cpp")
    code = loose_segment_bytes(obj, "SHARED")
    if len(code) != SIZE or sha(code) != DIAG_CODE_SHA:
        raise ValueError(f"{label}: maintained SND_LOAD object CODE drift")
    if code[MOV_OFFSET:MOV_OFFSET + 2] != bytes.fromhex("89c3"):
        raise ValueError(f"{label}: maintained MOV encoding drift")

    exe = work / "bin/th04/op.exe"
    map_path = work / "obj/th04/op.map"
    exe.unlink()
    map_path.unlink()
    run_checked(
        ["wine", str(RUNNER), "-e", "-x", "tlink", r"@obj\th04\op.@l"],
        work, output / f"link-{label}.log", timeout=300,
    )
    linked = parse_mz(exe.read_bytes())
    if not linked.valid:
        raise ValueError(f"{label}: invalid linked OP MZ")
    if sha_file(exe) != DIAG_EXE_SHA or sha_file(map_path) != BASE_MAP_SHA:
        raise ValueError(f"{label}: linked diagnostic OP identity drift")
    if [x.linear for x in linked.relocations] != [x.linear for x in target_mz.relocations]:
        raise ValueError(f"{label}: ordered relocation drift")
    map_text = map_path.read_text(encoding="cp437", errors="replace")
    if MAP_OWNER not in map_text:
        raise ValueError(f"{label}: SND_LOAD MAP owner drift")

    body = linked.program_image[START:START + SIZE]
    target_body = target_mz.program_image[START:START + SIZE]
    if body != target_body or sha(body) != TARGET_SLICE_SHA:
        raise ValueError(f"{label}: complete SND_LOAD target mismatch")

    if len(linked.program_image) != len(baseline_mz.program_image):
        raise ValueError(f"{label}: baseline program size drift")
    diffs = [
        i for i, (a, b) in enumerate(zip(linked.program_image, baseline_mz.program_image))
        if a != b
    ]
    if diffs != [MOV_START, MOV_START + 1]:
        raise ValueError(f"{label}: hybrid link changed unexpected program bytes: {diffs[:20]}")

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
        cwd=ROOT, capture_output=True, text=True, check=True,
    )
    if sha_file(RUNNER) != RUNNER_SHA:
        raise ValueError("pinned DOS runner drift")
    if sha_file(TARGET) != TARGET_SHA:
        raise ValueError("OP restored target identity drift")

    policy = source_policy()
    precedent = dialog_precedent(output)

    target_mz = parse_mz(TARGET.read_bytes())
    baseline_mz = parse_mz((SNAPSHOT / "bin/th04/op.exe").read_bytes())
    if (
        not target_mz.valid
        or not baseline_mz.valid
        or len(target_mz.relocations) != RELOCATIONS
        or len(baseline_mz.relocations) != RELOCATIONS
    ):
        raise ValueError("target/baseline OP MZ drift")
    target_body = target_mz.program_image[START:START + SIZE]
    if len(target_body) != SIZE or sha(target_body) != TARGET_SLICE_SHA:
        raise ValueError("OP target SND_LOAD identity drift")

    builds = {
        label: build_round(output, label, target_mz, baseline_mz)
        for label in ("a", "b")
    }
    stable = lambda row: {k: v for k, v in row.items() if k != "compact_snapshot"}
    if stable(builds["a"]) != stable(builds["b"]):
        raise ValueError("independent SND_LOAD hybrid cold rounds disagree")

    if sha_file(SOURCE) != SOURCE_SHA or sha_file(HANDLE) != HANDLE_SHA:
        raise ValueError("maintained SND_LOAD source changed during replay")

    receipt = {
        "schema_version": 1,
        "observed_utc": datetime.now(timezone.utc).isoformat(timespec="seconds"),
        "claim_scope": "TH04 OP SND_LOAD maintained hybrid decoded exactness",
        "artifact": "th04-op",
        "target_restored_sha256": TARGET_SHA,
        "boundary": {
            "payload_offset": hex(START),
            "size": SIZE,
            "target_sha256": TARGET_SLICE_SHA,
            "mov_relative_offset": hex(MOV_OFFSET),
            "mov_target_hex": "89c3",
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
            "Two cold TC4.02/TLINK rounds compile the complete maintained 234-byte "
            "SND_LOAD source to the target body raw-zero. Relative to the natural "
            "baseline, only the two MOV BX,AX encoding bytes change. The sole newly "
            "symbolic primitive is independently supported by the already accepted "
            "TH04/TH05 dialog_face_unput_8 register-direction precedent."
        ),
        "limit": (
            "Decoded-function exactness only. Literal original-source spelling and "
            "packed OP.EXE exactness are not claimed."
        ),
    }
    path = output / "receipt.json"
    path.write_text(json.dumps(receipt, indent=2) + "\n")
    print(json.dumps({
        "receipt": str(path),
        "receipt_sha256": sha_file(path),
        "snd_load_sha256": TARGET_SLICE_SHA,
        "object_code_sha256": DIAG_CODE_SHA,
        "linked_exe_sha256": DIAG_EXE_SHA,
        "dialog_common_mov": DIALOG_COMMON.hex(),
    }, sort_keys=True))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
