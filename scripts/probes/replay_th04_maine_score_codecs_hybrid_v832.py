#!/usr/bin/env python3
"""Cold-replay MAINE SCORE codecs with the v821 byte-ROR provenance.

Only the maintained decoder/encoder bodies receive credit. The surrounding
v489 scoreall.cpp owner remains replay scaffold so this backend does not promote
regist_menu, the SCORE EGC helper, or any other target-derived function.
"""
from __future__ import annotations

import argparse
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
from lib.omf import parse_omf  # noqa: E402
from lib.pc98 import parse_mz  # noqa: E402
from probe_th04_maine_score_producers_v468 import RUNNER, RUNNER_SHA, run_checked  # noqa: E402
from probe_th04_maine_segment_topology_v470 import tcc as tcc_maine  # noqa: E402
from probe_th04_maine_staff_full_cpp_v478 import segment_bytes  # noqa: E402
from replay_th04_scroll_driver_natural import fixup_locations  # noqa: E402
from replay_th04_zun_source_only import source_closure  # noqa: E402
from replay_th04_op_score_codecs_hybrid import (  # noqa: E402
    restore_crossgame_targets,
    th03_reference,
)

SNAPSHOT = ROOT / ".analysis/gpt-web/v489-bgimage-hybrid-replay-003/a/maine/source"
TARGET = ROOT / ".analysis/reconstruction/diet-replay/v228-maine-target-roundtrip/a/restored.bin"

TARGET_SHA = "6b4547182b9d53d069c0e4efc33bdabb69065cb544bb187ced7b0f51918aa533"
BASE_EXE_SHA = "d3bdc485782a9fb953823155426ca7f0e6e8212d6bc0cdffaaa32f91df2dc90c"
BASE_MAP_SHA = "014d8dfdf31a2c42c39e76288f23842cff7bd84fb6534f6d46479ba5d8b0838e"
RELOCATIONS = 559

SCORE_START = 0xC149
SCORE_SIZE = 0xB30
SCORE_CODE_SHA = "d8180ee4acb342cefe168337bb6dff4986b6bac625a4c614128c1b9430cec693"
SCORE_FIXUP_COUNT = 203

CODECS = {
    "decode": {
        "source": "src/maine/score/scoredec.cpp",
        "body": "src/maine/score/scoredec.inl",
        "source_sha256": "e6588f654bba3a84aa26852b68385e100b44c4a935138681420f710a80049d92",
        "body_sha256": "4d459c348791132ac5a6c33fa9be021aa5ab918928dc1121c67d4822bd94347a",
        "candidate_file": "th04/formats/scoredat/decode.cpp",
        "signature": "uint8_t pascal near scoredat_decode(void)\n",
        "offset": 0xC149,
        "relative": 0,
        "size": 0x58,
        "target_sha256": "31ea6a61abea7712e7ddd2ef8d9ee446b5947b514611252cba2d70c3930aff99",
    },
    "encode": {
        "source": "src/maine/score/scoreenc.cpp",
        "body": "src/maine/score/scoreenc.inl",
        "source_sha256": "b7cdb87cd70d2878f115abe0ac249f00ddafba32d7d6ba8fe3c5ff4fbf650d79",
        "body_sha256": "5bbf1ca6e6a8aba2af0cf0459a2d2b1c32f9b3e58e5fa3a808b5ad500ff9529b",
        "candidate_file": "th04/formats/scoredat/encode.cpp",
        "signature": "void pascal near scoredat_encode(void)\n",
        "offset": 0xC1A1,
        "relative": 0x58,
        "size": 0x65,
        "target_sha256": "c5c56e733842e6ba9b2d0448109939b2e04c8536b8495057425c4e787437c497",
    },
}


def sha(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def sha_file(path: Path) -> str:
    return sha(path.read_bytes())


def new_output(path: Path | None) -> Path:
    if path is None:
        root = PRIVATE / "reconstruction/probes"
        root.mkdir(parents=True, exist_ok=True)
        return Path(tempfile.mkdtemp(prefix="maine-score-codecs-v832-", dir=root))
    out = path.resolve()
    root = (PRIVATE / "reconstruction/probes").resolve()
    if out.exists() or out == root or not out.is_relative_to(root):
        raise ValueError("output must be new below .analysis/reconstruction/probes")
    out.mkdir(parents=True)
    return out


def object_fixups(obj: Path) -> list[tuple[int, int]]:
    return [
        item
        for record in parse_omf(obj.read_bytes())
        if record.record_type == 0x9C
        for item in fixup_locations(record.data)
    ]


def source_policy() -> dict[str, object]:
    for spec in CODECS.values():
        if sha_file(ROOT / spec["source"]) != spec["source_sha256"]:
            raise ValueError(f"maintained wrapper drift: {spec['source']}")
        if sha_file(ROOT / spec["body"]) != spec["body_sha256"]:
            raise ValueError(f"maintained body drift: {spec['body']}")

    dec = (ROOT / CODECS["decode"]["body"]).read_text()
    enc = (ROOT / CODECS["encode"]["body"]).read_text()
    required_dec = (
        "feedback = ((unsigned char *)&hi)[i + 1];",
        "_AL = hi.key2;",
        "asm { ror feedback, 3; }",
        "feedback ^= _AL;",
        "((unsigned char *)&hi)[i] =",
        "hi.key1 + feedback + ((unsigned char *)&hi)[i];",
    )
    required_enc = (
        "_AL = (unsigned char)hi.key2;",
        "asm { ror feedback, 3; }",
        "feedback ^= _AL;",
    )
    if any(x not in dec for x in required_dec) or any(x not in enc for x in required_enc):
        raise ValueError("maintained MAINE codec mechanism drift")
    for text in (dec, enc):
        lowered = text.lower()
        for forbidden in ("__emit__", "#pragma codestring", "target_bytes", "db 0x"):
            if forbidden in lowered:
                raise ValueError(f"codec source contains byte-forcing surface: {forbidden}")

    return {
        "symbolic_primitive": "single-byte ROR feedback,3",
        "crossgame_basis": "TH03 OP/MAINL release-target codec sites plus v821 provenance",
        "decoder_natural_type_mechanism": (
            "ordinary large-model unsigned-char pointer expression recovers the target "
            "AL=current / DL=key1 arithmetic allocation without another symbolic primitive"
        ),
        "no_emit": True,
        "no_codestring": True,
        "no_object_patch": True,
        "no_post_link_patch": True,
    }


def install_sources(work: Path) -> dict[str, str]:
    roots = tuple(spec["source"] for spec in CODECS.values())
    closure = source_closure(ROOT, roots)
    hashes = {}
    for rel in closure:
        src = ROOT / rel
        dst = work / rel
        dst.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(src, dst)
        hashes[rel] = sha_file(src)
    return hashes


def overlay_bodies(work: Path) -> None:
    for spec in CODECS.values():
        path = work / spec["candidate_file"]
        text = path.read_text()
        sig = spec["signature"]
        if text.count(sig) != 1:
            raise ValueError(f"candidate signature drift: {path}")
        prefix = text[:text.index(sig)]
        path.write_text(prefix + sig + f'#include "{spec["body"]}"\n')


def build_round(output: Path, label: str, target_mz, ref_code: bytes,
                ref_fixups: list[tuple[int, int]]) -> dict[str, object]:
    work = output / label / "maine/source"
    work.parent.mkdir(parents=True)
    compact = copy_compact_snapshot(SNAPSHOT, work, "maine")
    source_hashes = install_sources(work)
    overlay_bodies(work)

    obj = work / "obj/th04/scoreall.obj"
    obj.unlink()
    tcc_maine(work, output, f"score-codecs-{label}", "th04/scoreall.cpp")
    code = segment_bytes(obj, "SCORE_TEXT")
    fixups = object_fixups(obj)
    if code != ref_code or fixups != ref_fixups:
        raise ValueError(f"{label}: complete SCORE_TEXT object/fixup drift")

    exe = work / "bin/th04/maine.exe"
    map_path = work / "obj/th04/maine.map"
    exe.unlink()
    map_path.unlink()
    run_checked(
        ["wine", str(RUNNER), "-e", "-x", "tlink", r"@obj\th04\maine.@l"],
        work, output / f"link-{label}.log", timeout=300,
    )
    linked = parse_mz(exe.read_bytes())
    if not linked.valid or len(linked.relocations) != RELOCATIONS:
        raise ValueError(f"{label}: linked MAINE MZ drift")
    if sha_file(exe) != BASE_EXE_SHA or sha_file(map_path) != BASE_MAP_SHA:
        raise ValueError(f"{label}: MAINE EXE/MAP identity drift")
    if [x.linear for x in linked.relocations] != [x.linear for x in target_mz.relocations]:
        raise ValueError(f"{label}: ordered relocation drift")

    functions = {}
    for name, spec in CODECS.items():
        body = linked.program_image[spec["offset"]:spec["offset"] + spec["size"]]
        target_body = target_mz.program_image[spec["offset"]:spec["offset"] + spec["size"]]
        if body != target_body or sha(body) != spec["target_sha256"]:
            raise ValueError(f"{label}: {name} linked target mismatch")
        functions[name] = {
            "payload_offset": hex(spec["offset"]),
            "size": spec["size"],
            "sha256": sha(body),
            "raw_difference_count": 0,
        }

    return {
        "compact_snapshot": compact,
        "source_sha256": source_hashes,
        "score_text_size": len(code),
        "score_text_sha256": sha(code),
        "object_fixup_count": len(fixups),
        "object_fixups": [list(x) for x in fixups],
        "linked_exe_sha256": sha_file(exe),
        "linked_map_sha256": sha_file(map_path),
        "ordered_relocations": len(linked.relocations),
        "functions": functions,
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
    crossgame = restore_crossgame_targets(output)
    th03 = th03_reference()

    ref_obj = SNAPSHOT / "obj/th04/scoreall.obj"
    ref_code = segment_bytes(ref_obj, "SCORE_TEXT")
    ref_fixups = object_fixups(ref_obj)
    if len(ref_code) != SCORE_SIZE or sha(ref_code) != SCORE_CODE_SHA:
        raise ValueError("v489 SCORE_TEXT reference drift")
    if len(ref_fixups) != SCORE_FIXUP_COUNT:
        raise ValueError("v489 SCORE_TEXT fixup-count drift")

    target_mz = parse_mz(TARGET.read_bytes())
    if not target_mz.valid or len(target_mz.relocations) != RELOCATIONS:
        raise ValueError("MAINE target MZ/relocation drift")
    for spec in CODECS.values():
        body = target_mz.program_image[spec["offset"]:spec["offset"] + spec["size"]]
        if len(body) != spec["size"] or sha(body) != spec["target_sha256"]:
            raise ValueError("MAINE target codec identity drift")

    builds = {
        label: build_round(output, label, target_mz, ref_code, ref_fixups)
        for label in ("a", "b")
    }
    stable = lambda row: {k: v for k, v in row.items() if k != "compact_snapshot"}
    if stable(builds["a"]) != stable(builds["b"]):
        raise ValueError("independent MAINE codec cold rounds disagree")

    receipt = {
        "schema_version": 1,
        "observed_utc": datetime.now(timezone.utc).isoformat(timespec="seconds"),
        "claim_scope": "TH04 MAINE SCORE codec artifact-local hybrid decoded exactness",
        "artifact": "th04-maine",
        "target_restored_sha256": TARGET_SHA,
        "producer": {
            "payload_offset": hex(SCORE_START),
            "size": SCORE_SIZE,
            "reference_code_sha256": SCORE_CODE_SHA,
            "reference_fixup_count": SCORE_FIXUP_COUNT,
            "credit_scope": "only scoredat_decode and scoredat_encode",
        },
        "codecs": CODECS,
        "source_policy": policy,
        "crossgame_release_targets": crossgame,
        "th03_reference": th03,
        "builds": builds,
        "conclusion": (
            "Two cold TC4.02/TLINK MAINE rounds rebuild the complete v489 SCORE_TEXT "
            "object and 203 ordered fixups exactly, relink the retained MAINE EXE/MAP, "
            "preserve all 559 ordered MZ relocations, and reproduce both reviewed codec "
            "function slices raw-zero from maintained bodies. The decoder's former "
            "three-byte residual is explained by the ordinary large-model pointer "
            "expression; byte-ROR remains the sole symbolic primitive."
        ),
        "limit": (
            "Artifact-local decoded-function exactness for the two codec functions only. "
            "No credit is transferred to regist_menu or the SCORE EGC helper, and packed "
            "MAINE.EXE exactness is not claimed."
        ),
    }
    path = output / "receipt.json"
    path.write_text(json.dumps(receipt, indent=2) + "\n")
    print(json.dumps({
        "receipt": str(path),
        "receipt_sha256": sha_file(path),
        "score_text_sha256": SCORE_CODE_SHA,
        "decode_sha256": CODECS["decode"]["target_sha256"],
        "encode_sha256": CODECS["encode"]["target_sha256"],
        "ordered_relocations": RELOCATIONS,
    }, sort_keys=True))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
