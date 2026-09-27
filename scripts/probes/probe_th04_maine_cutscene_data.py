#!/usr/bin/env python3
"""Check TH04 MAINE cutscene masks against the attested target and TC4J OMF."""

from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
from lib.omf import describe_omf, parse_omf  # noqa: E402
from lib.pc98 import parse_mz  # noqa: E402

RESTORED = ROOT / ".analysis/reconstruction/diet-replay/v228-maine-target-roundtrip/a/restored.bin"
RESTORED_SHA = "6b4547182b9d53d069c0e4efc33bdabb69065cb544bb187ced7b0f51918aa533"
MASK_OFFSET = 0xEB3C  # Candidate MAP 0E53:060C, checked against decoded load.
GLYPH_OFFSET = 0xEB84  # Candidate MAP 0E53:0654.
EXPECTED_MASKS = bytes.fromhex(
    "0000111100004444 8888111122224444 aaaa5555aaaa5555 eeee7777bbbbdddd"
)


def sha(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--pi-object", type=Path, required=True)
    parser.add_argument("--state-object", type=Path, required=True)
    parser.add_argument("--output-dir", type=Path, required=True)
    args = parser.parse_args()
    private = (ROOT / ".analysis/reconstruction/probes").resolve()
    obj = args.pi_object.resolve()
    state = args.state_object.resolve()
    output = args.output_dir.resolve()
    if (not obj.is_relative_to(private) or not state.is_relative_to(private)
            or output.exists() or not output.is_relative_to(private)):
        parser.error("objects and new output must be below .analysis/reconstruction/probes")
    subprocess.run([sys.executable, "scripts/preflight.py"], cwd=ROOT, check=True,
                   stdout=subprocess.DEVNULL)

    raw = RESTORED.read_bytes()
    if sha(raw) != RESTORED_SHA:
        raise ValueError("pinned decoded MAINE identity drift")
    mz = parse_mz(raw)
    if not mz.valid:
        raise ValueError("decoded MAINE MZ invalid")
    target_masks = mz.program_image[MASK_OFFSET:MASK_OFFSET + 32]
    target_glyph = mz.program_image[GLYPH_OFFSET:GLYPH_OFFSET + 4]
    if target_masks != EXPECTED_MASKS or target_glyph != b"  \0\0":
        raise ValueError("cutscene target data or mapped boundary drift")

    data = obj.read_bytes()
    omf = describe_omf(data)
    if not omf["valid"] or not any("TC86 Borland C++ 4.02" in c
                                   for c in omf["translator_comments"]):
        raise ValueError("PI mask object producer or OMF integrity drift")
    records = parse_omf(data)
    ledatas = [record for record in records if record.record_type == 0xA0]
    if len(ledatas) != 1 or ledatas[0].data[3:] != target_masks:
        raise ValueError("PI mask LEDATA differs from the target's 32 bytes")
    if not any(record.record_type == 0x90 and b"_PI_MASKS" in record.data
               for record in records):
        raise ValueError("PI mask public missing")

    state_data = state.read_bytes()
    state_omf = describe_omf(state_data)
    if not state_omf["valid"] or not any("TC86 Borland C++ 4.02" in c
                                         for c in state_omf["translator_comments"]):
        raise ValueError("state object producer or OMF integrity drift")
    state_records = parse_omf(state_data)
    state_ledatas = [record for record in state_records if record.record_type == 0xA0]
    target_state = mz.program_image[0xEB5C:GLYPH_OFFSET + 4]
    if len(state_ledatas) != 1 or state_ledatas[0].data[3:] != target_state:
        raise ValueError("BOX_MASKS and CUTSCENE_KANJI LEDATA differs from target")
    required_publics = {"_BOX_MASKS": 0, "_CUTSCENE_KANJI": 40}
    found_publics = {}
    for record in state_records:
        if record.record_type != 0x90:
            continue
        payload = record.data
        if len(payload) < 6 or payload[:2] != b"\x01\x02":
            continue
        size = payload[2]
        if len(payload) != size + 6:
            raise ValueError("unexpected state PUBDEF framing")
        name = payload[3:3 + size].decode("ascii")
        found_publics[name] = int.from_bytes(payload[3 + size:5 + size], "little")
    if any(found_publics.get(name) != offset for name, offset in required_publics.items()):
        raise ValueError("cutscene state public offsets differ from semantic data owners")

    output.mkdir(parents=True)
    receipt = {
        "schema_version": 1,
        "artifact": "th04-maine",
        "evidence_classes": ["target-analysis", "compiler"],
        "target_restored_sha256": RESTORED_SHA,
        "pi_object_sha256": sha(data),
        "state_object_sha256": sha(state_data),
        "mask_location": "0E53:060C / decoded load 0xEB3C",
        "mask_bytes": 32,
        "mask_sha256": sha(target_masks),
        "glyph_location": "0E53:0654 / decoded load 0xEB84",
        "glyph_initial_hex": target_glyph.hex(),
        "state_named_bytes": len(target_state),
        "limit": "Checks target data and one compiled OMF owner, not linked placement or runtime rendering.",
    }
    (output / "receipt.json").write_text(json.dumps(receipt, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"mask_bytes": 32, "mask_raw_equal": True,
                      "state_named_bytes": len(target_state),
                      "glyph_initial_hex": target_glyph.hex()}, sort_keys=True))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
