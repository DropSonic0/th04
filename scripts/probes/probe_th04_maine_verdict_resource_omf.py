#!/usr/bin/env python3
"""Compare compiled verdict resource LEDATA with named decoded MAINE data."""

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
PUBLICS = {
    "grEASY", "aU_", "aBd", "aBu", "aBd_0", "aBu_0",
    "aB_b_b_b_b_b_b", "aUqiUx", "aNPiuU_", "aGGxi", "aGGaogcpi",
    "aGqbGatbrmcj", "aIlcSObcj", "aGagcgegai", "aUU_gagcgeganNv",
    "aLcnzvv", "aPicacovCj", "aVavVVSrso", "aTimes", "aTimes_0",
    "aPoint", "a_ude_txt", "aBhbhbhbhbhbhu_", "aPicacovVVcvsfT",
    "aUde_pi",
}


def sha(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--object", type=Path, required=True)
    parser.add_argument("--output-dir", type=Path, required=True)
    args = parser.parse_args()
    private = (ROOT / ".analysis/reconstruction/probes").resolve()
    obj = args.object.resolve()
    output = args.output_dir.resolve()
    if not obj.is_relative_to(private) or output.exists() or not output.is_relative_to(private):
        parser.error("object and new output must be below .analysis/reconstruction/probes")
    subprocess.run([sys.executable, "scripts/preflight.py"], cwd=ROOT, check=True,
                   stdout=subprocess.DEVNULL)
    raw = RESTORED.read_bytes()
    if sha(raw) != RESTORED_SHA:
        raise ValueError("pinned decoded MAINE identity drift")
    mz = parse_mz(raw)
    if not mz.valid:
        raise ValueError("decoded MAINE MZ invalid")
    image = mz.program_image
    # The target has a leading zero skill flag at 0E53:071A, a zero display
    # flag at 0E53:0743, and one trailing alignment byte. Those are separate
    # owners; the named 40-byte gaiji table and strings total 271 bytes.
    expected = image[0xEC4B:0xEC73] + image[0xEC74:0xED5B]
    data = obj.read_bytes()
    omf = describe_omf(data)
    if not omf["valid"] or not any("TC86 Borland C++ 4.02" in c
                                   for c in omf["translator_comments"]):
        raise ValueError("resource object producer or OMF integrity drift")
    records = parse_omf(data)
    ledatas = [record for record in records if record.record_type == 0xA0]
    if len(ledatas) != 1 or ledatas[0].data[3:] != expected:
        raise ValueError("resource LEDATA does not match 271 target-observed bytes")
    publics = []
    for record in records:
        if record.record_type != 0x90:
            continue
        payload = record.data
        if len(payload) < 4 or payload[0:2] != b"\x01\x02":
            raise ValueError("unexpected verdict resource PUBDEF framing")
        length = payload[2]
        if len(payload) != 3 + length + 3:
            raise ValueError("unexpected verdict resource PUBDEF body")
        name = payload[3:3 + length].decode("ascii")
        publics.append(name[1:] if name.startswith("_") else name)
    if set(publics) != PUBLICS or len(publics) != len(PUBLICS):
        raise ValueError("verdict resource public-name set drift")
    output.mkdir(parents=True)
    receipt = {
        "schema_version": 1,
        "artifact": "th04-maine",
        "evidence_class": "compiler",
        "target_restored_sha256": RESTORED_SHA,
        "object_sha256": sha(data),
        "target_named_bytes_sha256": sha(expected),
        "object_named_bytes_sha256": sha(ledatas[0].data[3:]),
        "named_bytes": len(expected),
        "public_count": len(publics),
        "limit": "Checks resource bytes and public names in one OMF object; linked addresses and runtime rendering remain untested.",
    }
    (output / "receipt.json").write_text(json.dumps(receipt, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"named_bytes": len(expected), "publics": len(publics),
                      "raw_equal": True}, sort_keys=True))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
