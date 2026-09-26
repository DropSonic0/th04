#!/usr/bin/env python3
"""Replay MAINE cutscene EGC setup + masked box copy from maintained hybrid source.

Exact credit is artifact-local. The only non-ordinary mechanism is the
AX=value -> DX=port -> OUT DX,AX EGC register-write ordering (plus MOV AX,0 at
the address-register slot), independently observed as one complete 42-byte
setup core in TH02/TH03/TH04/TH05 release targets.
"""
from __future__ import annotations

import argparse
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

from compact_op_maine_snapshot import copy_compact_snapshot  # noqa: E402
from lib.omf import parse_omf  # noqa: E402
from lib.pc98 import parse_mz  # noqa: E402
from lib.targets import load_target_manifest, find_artifact, read_verified_artifact  # noqa: E402
from probe_th04_final_blocker_crossartifact import restore_diet  # noqa: E402
from probe_th04_maine_score_producers_v468 import RUNNER, RUNNER_SHA, run_checked  # noqa: E402
from probe_th04_maine_segment_topology_v470 import tcc  # noqa: E402
from probe_th04_maine_staff_full_cpp_v478 import segment_bytes  # noqa: E402
from replay_th04_scroll_driver_natural import fixup_locations  # noqa: E402
from replay_th04_shared_delay_measure import link_relevant_omf_sha  # noqa: E402
from replay_th04_zun_source_only import source_closure  # noqa: E402

SNAPSHOT = ROOT / ".analysis/gpt-web/v489-bgimage-hybrid-replay-003/a/maine/source"
TARGET = ROOT / ".analysis/reconstruction/diet-replay/v228-maine-target-roundtrip/a/restored.bin"

TARGET_SHA = "6b4547182b9d53d069c0e4efc33bdabb69065cb544bb187ced7b0f51918aa533"
BASE_EXE_SHA = "d3bdc485782a9fb953823155426ca7f0e6e8212d6bc0cdffaaa32f91df2dc90c"
BASE_MAP_SHA = "014d8dfdf31a2c42c39e76288f23842cff7bd84fb6534f6d46479ba5d8b0838e"
BASE_CUTSCENE_SOURCE_SHA = "49fbde7cc661dda1d8a290debe285d625f4d274abb9c68c42d28150c59afb760"
BASE_CUTSCENE_OBJECT_SHA = "1ada8b7c8539aaaefe56c13e2e2407256207e25bde7ba9c9d532f2bba12c8f7e"
BASE_CUTSCENE_CODE_SHA = "a7e160b07189e0752a71705ad4d96fda85ba91fefb014574fa93019880f779a1"
RELOCATIONS = 559

PRODUCER_START = 0xA292
PRODUCER_SIZE = 0xC3E

EGC_START = 0xA2D6
EGC_SIZE = 0x34
EGC_SHA = "1f0b71efadc4a80c8acced8ae69c7d527b00a34c8b107d5addb885bc36ee8825"
EGC_STANDALONE_SHA = "c77a97c01945b3393e6379984bca913bae8153219d3bfe201be1850877638036"
EGC_FIXUPS = [(3, 4)]

BOX_START = 0xA78F
BOX_SIZE = 0x86
BOX_SHA = "1a7ddf9c97c9fa1cb40368133d5b01e41cb359a40b516854d8754aea6735d012"
BOX_STANDALONE_SHA = "105e488821fba79f163654ff89f0e6ec02ef6c3ed18ded5cd8f540d97ca6c609"
BOX_FIXUPS = [(1, 99), (1, 84), (1, 49)]

EGC_SOURCE = ROOT / "src/maine/cutscene/egc_start_copy.cpp"
EGC_BODY = ROOT / "src/maine/cutscene/egc_start_copy.inl"
BOX_SOURCE = ROOT / "src/maine/cutscene/box_1_to_0_masked.cpp"
BOX_BODY = ROOT / "src/maine/cutscene/box_1_to_0_masked.inl"

WORD_CORE = bytes.fromhex(
    "b8f0ffbaa004ef"
    "b8ff00baa204ef"
    "b80031baa404ef"
    "b8ffffbaa804ef"
    "b80000baac04ef"
    "b80f00baae04ef"
)
WORD_CORE_SHA = "e3e761f7f926d17f1265e9dc32d665131a3ca4ae47fd415290d6f6e8db41cfb0"

CROSSGAME = {
    "th02-maine-smoke": {
        "offset": 0xC113,
        "map": ROOT / ".analysis/gpt-web/v401-master-vs-object-replay-001/a/source/obj/th02/maine.map",
        "module": "M=th02/staff.cpp",
        "public": "egc_start_copy()",
    },
    "th03-mainl-smoke": {
        "offset": 0xA19B,
        "map": ROOT / ".analysis/gpt-web/v401-master-vs-object-replay-001/a/source/obj/th03/mainl.map",
        "module": "M=th03/cutscene.cpp",
        "public": "egc_start_copy()",
    },
    "th05-main-smoke": {
        "offset": 0xBC42,
        "map": ROOT / ".analysis/gpt-web/v401-master-vs-object-replay-001/a/source/obj/th05/main.map",
        "module": "M=th05_main.asm",
        "public": "egc_start_copy_noframe()",
    },
    "th05-maine-smoke": {
        "offset": 0xA6D9,
        "map": ROOT / ".analysis/gpt-web/v401-master-vs-object-replay-001/a/source/obj/th05/maine.map",
        "module": "M=th05/cutscene.cpp",
        "public": "egc_start_copy()",
    },
}

CUTSCENE_EGC_OLD = (
    '#define egc_start_copy\tnear egc_start_copy\n'
    '#include "th01/hardware/egcstart.cpp"\n'
    '#undef egc_start_copy'
)
CUTSCENE_EGC_NEW = (
    '#include "th01/hardware/egc_impl.hpp"\n'
    '#include "src/maine/cutscene/egc_start_copy.inl"'
)
BOX_PATTERN = re.compile(
    r"void pascal near box_1_to_0_masked\(box_mask_t mask\)\n\{.*?\n\}\n\n"
    r"void near box_1_to_0_animate",
    re.S,
)
BOX_REPLACEMENT = (
    "extern const dot_rect_t(16, 4) BOX_MASKS[BOX_MASK_COUNT];\n"
    '#include "src/maine/cutscene/box_1_to_0_masked.inl"\n\n'
    "void near box_1_to_0_animate"
)


def sha(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def sha_file(path: Path) -> str:
    return sha(path.read_bytes())


def new_output(path: Path | None) -> Path:
    root = PRIVATE / "reconstruction/probes"
    root.mkdir(parents=True, exist_ok=True)
    if path is None:
        return Path(tempfile.mkdtemp(prefix="v833-maine-cutscene-egc-", dir=root))
    out = path.resolve()
    if out.exists() or out == root or not out.is_relative_to(root.resolve()):
        raise ValueError("output must be new below .analysis/reconstruction/probes")
    out.mkdir(parents=True)
    return out


def object_fixups(path: Path) -> list[tuple[int, int]]:
    return [
        item
        for rec in parse_omf(path.read_bytes())
        if rec.record_type == 0x9C
        for item in fixup_locations(rec.data)
    ]


def mask_fixups(data: bytes, fixups: list[tuple[int, int]]) -> bytes:
    out = bytearray(data)
    for kind, offset in fixups:
        width = 2 if kind == 1 else 4
        out[offset:offset + width] = b"\0" * width
    return bytes(out)


def source_policy() -> dict[str, object]:
    rows = {}
    for path in (EGC_BODY, BOX_BODY):
        text = path.read_text()
        forbidden = ["__emit__", "codestring", "outport2", "keep_0", "optimization_barrier"]
        bad = [token for token in forbidden if token in text]
        if bad:
            raise ValueError(f"{path}: forbidden source helper(s): {bad}")
        rows[str(path.relative_to(ROOT))] = {
            "sha256": sha_file(path),
            "contains_inline_asm": "asm {" in text,
            "contains_target_byte_array": "bytes.fromhex" in text or "\\x" in text,
        }
    if rows[str(EGC_BODY.relative_to(ROOT))]["contains_target_byte_array"]:
        raise ValueError("EGC body embeds target bytes")
    if rows[str(BOX_BODY.relative_to(ROOT))]["contains_target_byte_array"]:
        raise ValueError("box body embeds target bytes")
    return rows


def crossgame_targets(out: Path) -> dict[str, object]:
    manifest = load_target_manifest(ROOT / "config/targets.toml")
    result = {}
    restore_root = out / "crossgame"
    restore_root.mkdir(parents=True, exist_ok=True)

    for artifact_id, spec in CROSSGAME.items():
        artifact = find_artifact(manifest, artifact_id)
        packed = read_verified_artifact(ROOT, artifact)
        raw = packed
        if len(raw) >= 0x20 and raw[0x1C:0x20].lower() == b"diet":
            per = restore_root / artifact_id
            per.mkdir(parents=True)
            raw, _meta = restore_diet(artifact, packed, per)

        mz = parse_mz(raw)
        if not mz.valid:
            raise ValueError(f"{artifact_id}: invalid restored MZ")
        image = mz.program_image
        hits = []
        pos = 0
        while True:
            pos = image.find(WORD_CORE, pos)
            if pos < 0:
                break
            hits.append(pos)
            pos += 1
        if hits != [spec["offset"]]:
            raise ValueError(f"{artifact_id}: EGC core hits drift: {hits}")

        map_text = spec["map"].read_text(encoding="cp437", errors="replace")
        if spec["module"] not in map_text or spec["public"] not in map_text:
            raise ValueError(f"{artifact_id}: MAP binding drift")

        result[artifact_id] = {
            "packed_sha256": sha(packed),
            "restored_sha256": sha(raw),
            "relocation_count": len(mz.relocations),
            "core_offset": hex(spec["offset"]),
            "core_sha256": WORD_CORE_SHA,
            "map_module": spec["module"],
            "map_public": spec["public"],
        }
    return result


def copy_closure(work: Path, source: str) -> None:
    for rel in source_closure(ROOT, (source,)):
        src = ROOT / rel
        dst = work / rel
        dst.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(src, dst)


def compile_standalone(
    work: Path,
    out: Path,
    label: str,
    source: str,
    target: bytes,
    expected_size: int,
    expected_sha: str,
    expected_fixups: list[tuple[int, int]],
) -> dict[str, object]:
    copy_closure(work, source)
    before = {p.name for p in (work / "obj/th04").glob("*.obj")}
    tcc(work, out, label, source)
    objs = [p for p in (work / "obj/th04").glob("*.obj") if p.name not in before]
    if len(objs) != 1:
        raise ValueError(f"{label}: expected one standalone object, got {[p.name for p in objs]}")
    obj = objs[0]
    code = segment_bytes(obj, "CUTSCENE_TEXT")
    fixups = object_fixups(obj)
    if len(code) != expected_size or sha(code) != expected_sha or fixups != expected_fixups:
        raise ValueError(f"{label}: standalone object drift")
    if mask_fixups(code, fixups) != mask_fixups(target, fixups):
        raise ValueError(f"{label}: standalone fixed bytes do not match target")
    obj.unlink()
    return {
        "code_size": len(code),
        "code_sha256": sha(code),
        "fixups": [list(x) for x in fixups],
        "fixed_sha256": sha(mask_fixups(code, fixups)),
    }


def overlay_cutscene(work: Path) -> str:
    shutil.copytree(ROOT / "src/shared", work / "src/shared", dirs_exist_ok=True)
    shutil.copytree(ROOT / "src/maine/cutscene", work / "src/maine/cutscene", dirs_exist_ok=True)

    source = work / "th03/cutscene/cutscene.cpp"
    if sha_file(source) != BASE_CUTSCENE_SOURCE_SHA:
        raise ValueError("v489 cutscene source identity drift")
    text = source.read_text()
    if text.count(CUTSCENE_EGC_OLD) != 1:
        raise ValueError("cutscene egc_start_copy include anchor drift")
    text = text.replace(CUTSCENE_EGC_OLD, CUTSCENE_EGC_NEW, 1)

    matches = list(BOX_PATTERN.finditer(text))
    if len(matches) != 1:
        raise ValueError(f"box replacement anchor drift: {len(matches)}")
    m = matches[0]
    text = text[:m.start()] + BOX_REPLACEMENT + text[m.end():]
    source.write_text(text)
    return sha_file(source)


def build_round(
    out: Path,
    label: str,
    target_mz,
    baseline_mz,
    baseline_code: bytes,
    baseline_fixups: list[tuple[int, int]],
) -> dict[str, object]:
    work = out / label / "maine/source"
    work.parent.mkdir(parents=True)
    compact = copy_compact_snapshot(SNAPSHOT, work, "maine")

    egc_target = target_mz.program_image[EGC_START:EGC_START + EGC_SIZE]
    box_target = target_mz.program_image[BOX_START:BOX_START + BOX_SIZE]
    standalone_egc = compile_standalone(
        work, out, f"egc-standalone-{label}",
        "src/maine/cutscene/egc_start_copy.cpp",
        egc_target, EGC_SIZE, EGC_STANDALONE_SHA, EGC_FIXUPS,
    )
    standalone_box = compile_standalone(
        work, out, f"box-standalone-{label}",
        "src/maine/cutscene/box_1_to_0_masked.cpp",
        box_target, BOX_SIZE, BOX_STANDALONE_SHA, BOX_FIXUPS,
    )

    patched_source_sha = overlay_cutscene(work)
    obj = work / "obj/th04/cutscene.obj"
    obj.unlink()
    tcc(work, out, f"cutscene-group-{label}", "th04/cutscene.cpp")
    code = segment_bytes(obj, "CUTSCENE_TEXT")
    fixups = object_fixups(obj)
    if code != baseline_code or fixups != baseline_fixups:
        raise ValueError(f"{label}: grouped CUTSCENE_TEXT object/fixups drift")

    exe = work / "bin/th04/maine.exe"
    map_path = work / "obj/th04/maine.map"
    exe.unlink()
    map_path.unlink()
    run_checked(
        ["wine", str(RUNNER), "-e", "-x", "tlink", r"@obj\th04\maine.@l"],
        work, out / f"link-{label}.log", timeout=300,
    )
    linked = parse_mz(exe.read_bytes())
    if not linked.valid or len(linked.relocations) != RELOCATIONS:
        raise ValueError(f"{label}: linked MAINE identity drift")
    if sha_file(exe) != BASE_EXE_SHA or sha_file(map_path) != BASE_MAP_SHA:
        raise ValueError(f"{label}: linked MAINE EXE/MAP drift")

    target_relocs = [x.linear for x in target_mz.relocations]
    if [x.linear for x in linked.relocations] != target_relocs:
        raise ValueError(f"{label}: ordered relocation drift")

    producer = linked.program_image[PRODUCER_START:PRODUCER_START + PRODUCER_SIZE]
    target_producer = target_mz.program_image[PRODUCER_START:PRODUCER_START + PRODUCER_SIZE]
    baseline_producer = baseline_mz.program_image[PRODUCER_START:PRODUCER_START + PRODUCER_SIZE]
    if producer != target_producer or producer != baseline_producer:
        raise ValueError(f"{label}: linked CUTSCENE_TEXT producer mismatch")

    egc = linked.program_image[EGC_START:EGC_START + EGC_SIZE]
    box = linked.program_image[BOX_START:BOX_START + BOX_SIZE]
    if egc != egc_target or sha(egc) != EGC_SHA:
        raise ValueError(f"{label}: egc_start_copy raw mismatch")
    if box != box_target or sha(box) != BOX_SHA:
        raise ValueError(f"{label}: box_1_to_0_masked raw mismatch")

    program_diffs = [
        i
        for i, (a, b) in enumerate(zip(linked.program_image, target_mz.program_image))
        if a != b
    ]
    if program_diffs != [0xD1D3, 0xD1D4]:
        raise ValueError(f"{label}: unrelated baseline difference set drift: {program_diffs[:20]}")

    return {
        "compact_snapshot": compact,
        "patched_cutscene_source_sha256": patched_source_sha,
        "standalone_egc": standalone_egc,
        "standalone_box": standalone_box,
        "group_object_sha256": sha_file(obj),
        "group_link_relevant_omf_sha256": link_relevant_omf_sha(obj),
        "group_code_size": len(code),
        "group_code_sha256": sha(code),
        "group_fixup_count": len(fixups),
        "group_fixups_equal_v489": True,
        "linked_exe_sha256": sha_file(exe),
        "linked_map_sha256": sha_file(map_path),
        "ordered_relocations": len(linked.relocations),
        "producer_sha256": sha(producer),
        "egc_start_copy_sha256": sha(egc),
        "box_1_to_0_masked_sha256": sha(box),
        "raw_difference_counts": {
            "producer": 0,
            "egc_start_copy": 0,
            "box_1_to_0_masked": 0,
        },
        "program_difference_offsets_vs_target": [hex(x) for x in program_diffs],
        "program_difference_note": "Only the retained v489 SND_LOAD MOV-encoding residual; unrelated to CUTSCENE_TEXT.",
    }


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--output-dir", type=Path)
    args = ap.parse_args()
    out = new_output(args.output_dir)

    if sha_file(RUNNER) != RUNNER_SHA:
        raise ValueError("pinned DOS runner drift")
    if sha_file(TARGET) != TARGET_SHA:
        raise ValueError("MAINE restored target identity drift")
    if sha_file(SNAPSHOT / "bin/th04/maine.exe") != BASE_EXE_SHA:
        raise ValueError("v489 baseline EXE drift")
    if sha_file(SNAPSHOT / "obj/th04/maine.map") != BASE_MAP_SHA:
        raise ValueError("v489 baseline MAP drift")
    if sha_file(SNAPSHOT / "th03/cutscene/cutscene.cpp") != BASE_CUTSCENE_SOURCE_SHA:
        raise ValueError("v489 cutscene source drift")
    if sha_file(SNAPSHOT / "obj/th04/cutscene.obj") != BASE_CUTSCENE_OBJECT_SHA:
        raise ValueError("v489 cutscene object drift")

    policy = source_policy()
    crossgame = crossgame_targets(out)

    target_mz = parse_mz(TARGET.read_bytes())
    baseline_mz = parse_mz((SNAPSHOT / "bin/th04/maine.exe").read_bytes())
    if not target_mz.valid or not baseline_mz.valid:
        raise ValueError("target/baseline MZ invalid")
    if len(target_mz.relocations) != RELOCATIONS or len(baseline_mz.relocations) != RELOCATIONS:
        raise ValueError("target/baseline relocation count drift")
    if [x.linear for x in target_mz.relocations] != [x.linear for x in baseline_mz.relocations]:
        raise ValueError("v489 baseline ordered relocations differ from target")

    baseline_obj = SNAPSHOT / "obj/th04/cutscene.obj"
    baseline_code = segment_bytes(baseline_obj, "CUTSCENE_TEXT")
    baseline_fixups = object_fixups(baseline_obj)
    if (
        len(baseline_code) != PRODUCER_SIZE
        or sha(baseline_code) != BASE_CUTSCENE_CODE_SHA
        or len(baseline_fixups) != 214
    ):
        raise ValueError("v489 CUTSCENE_TEXT object identity drift")

    target_producer = target_mz.program_image[PRODUCER_START:PRODUCER_START + PRODUCER_SIZE]
    baseline_producer = baseline_mz.program_image[PRODUCER_START:PRODUCER_START + PRODUCER_SIZE]
    if target_producer != baseline_producer:
        raise ValueError("v489 CUTSCENE_TEXT linked producer differs from target")

    builds = {
        label: build_round(out, label, target_mz, baseline_mz, baseline_code, baseline_fixups)
        for label in ("a", "b")
    }
    stable = lambda row: {
        k: v for k, v in row.items()
        if k not in {"compact_snapshot", "group_object_sha256"}
    }
    if stable(builds["a"]) != stable(builds["b"]):
        raise ValueError("independent v833 cold rounds disagree")

    receipt = {
        "schema_version": 1,
        "observed_utc": datetime.now(timezone.utc).isoformat(timespec="seconds"),
        "claim_scope": "TH04 MAINE cutscene EGC-start + masked-box artifact-local hybrid decoded exactness",
        "artifact": "th04-maine",
        "target_restored_sha256": TARGET_SHA,
        "source_policy": policy,
        "crossgame_word_register_core": {
            "size": len(WORD_CORE),
            "sha256": WORD_CORE_SHA,
            "semantics": "AX=value -> DX=port -> OUT DX,AX across six EGC copy registers",
            "targets": crossgame,
        },
        "producer": {
            "payload_offset": hex(PRODUCER_START),
            "size": PRODUCER_SIZE,
            "target_sha256": sha(target_producer),
            "object_code_sha256": BASE_CUTSCENE_CODE_SHA,
            "object_fixup_count": len(baseline_fixups),
        },
        "functions": {
            "egc_start_copy": {
                "payload_offset": hex(EGC_START),
                "size": EGC_SIZE,
                "target_sha256": EGC_SHA,
            },
            "box_1_to_0_masked": {
                "payload_offset": hex(BOX_START),
                "size": BOX_SIZE,
                "target_sha256": BOX_SHA,
            },
        },
        "builds": builds,
        "conclusion": (
            "Two cold MAINE rounds compile the maintained cutscene EGC sources to the "
            "same complete v489 CUTSCENE_TEXT CODE and all 214 OMF fixups, then link "
            "the exact v489 EXE/MAP with all 559 ordered relocations. Both reviewed "
            "functions and the complete CUTSCENE_TEXT producer are raw-zero against target."
        ),
        "limit": (
            "Artifact-local decoded-function exactness only. The SCORE EGC helper remains "
            "separately blocked because replacing its keep_0(0) source loses a real "
            "_address_0 OMF fixup. Packed-file exactness is not claimed."
        ),
    }
    path = out / "receipt.json"
    path.write_text(json.dumps(receipt, indent=2) + "\n")
    print(json.dumps({
        "receipt": str(path),
        "receipt_sha256": sha_file(path),
        "producer_sha256": sha(target_producer),
        "egc_start_copy_sha256": EGC_SHA,
        "box_1_to_0_masked_sha256": BOX_SHA,
        "group_fixup_count": len(baseline_fixups),
        "ordered_relocations": RELOCATIONS,
    }, sort_keys=True))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
