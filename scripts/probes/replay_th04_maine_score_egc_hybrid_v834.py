#!/usr/bin/env python3
"""Replay only MAINE SCORE EGC-start from maintained release-backed hybrid source."""
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
PRIVATE = (ROOT / '.analysis').resolve()
sys.path[0:0] = [str(ROOT / 'scripts'), str(ROOT / 'scripts/probes')]

from compact_op_maine_snapshot import copy_compact_snapshot
from lib.omf import parse_omf
from lib.pc98 import parse_mz
from lib.targets import load_target_manifest, find_artifact, read_verified_artifact
from probe_th04_final_blocker_crossartifact import restore_diet
from probe_th04_maine_score_producers_v468 import RUNNER, RUNNER_SHA, run_checked
from probe_th04_maine_segment_topology_v470 import tcc
from probe_th04_maine_staff_full_cpp_v478 import segment_bytes
from replay_th04_scroll_driver_natural import fixup_locations, omf_index
import replay_th04_maine_score_insert as prior

SOURCE = ROOT / 'src/maine/score/score_egc_start_copy.inl'
SOURCE_SHA = '8996baf43b7d981d14736641f3f4b83171a5e9064d35efa73a9c678ef58fb81e'
START = 0xCBB0
SIZE = 0x43
LOCAL = START - prior.SCORE_OWNER_OFFSET
TARGET_SHA = 'a433967aff2092e185268abd34ea0c17d46b63c552db89e1a2dd191425930f85'
FULL_CORE = bytes.fromhex(
    'b000e67cb007e66ab005e66ab080e67cb006e66a'
    'b8f0ffbaa004efb8ff00baa204efb80031baa404ef'
    'b8ffffbaa804efb80000baac04efb80f00baae04ef'
)
FULL_CORE_SHA = 'cc7836cd5c1f1cc3c3feb83ecadfb01a5e76bc6b1f0363d348dd64e0a7dc4a1f'
BASE_FIXUP_COUNT = 203
CAND_FIXUP_COUNT = 202
REMOVED_FIXUP = (1, 0xA9B)
CAND_MAP_SHA = 'bdda786f3f668ba4e7f2e0917248e8457db0a017db304ac69f4546b505f3fdbc'

FUNC_RE = re.compile(
    r'void near score_egc_start_copy\(void\)\n\{.*?\n\}\n\nvoid pascal near score_rect_copy\(',
    re.S,
)

def sha(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()

def sha_file(path: Path) -> str:
    return sha(path.read_bytes())

def new_output(path: Path | None) -> Path:
    root = PRIVATE / 'reconstruction/probes'
    root.mkdir(parents=True, exist_ok=True)
    if path is None:
        return Path(tempfile.mkdtemp(prefix='v834-maine-score-egc-', dir=root))
    out = path.resolve()
    if out.exists() or out == root or not out.is_relative_to(root.resolve()):
        raise ValueError('output must be new below .analysis/reconstruction/probes')
    out.mkdir(parents=True)
    return out

def score_fixups(path: Path) -> list[tuple[int, int]]:
    result: list[tuple[int, int]] = []
    current: tuple[int, int] | None = None
    for rec in parse_omf(path.read_bytes()):
        if rec.record_type == 0xA0:
            seg, pos = omf_index(rec.data, 0)
            off = int.from_bytes(rec.data[pos:pos + 2], 'little')
            current = (seg, off)
        elif rec.record_type == 0x9C and current and current[0] == 4:
            result.extend((kind, current[1] + rel) for kind, rel in fixup_locations(rec.data))
    return result

def normalized_map(path: Path) -> str:
    return '\n'.join(
        line for line in path.read_text(encoding='cp437', errors='replace').splitlines()
        if '_address_0' not in line
    )

def source_policy() -> dict[str, object]:
    text = SOURCE.read_text()
    forbidden = ('__emit__', '_outportb_', 'keep_0', 'optimization_barrier', 'codestring')
    bad = [x for x in forbidden if x in text]
    if bad:
        raise ValueError(f'forbidden helper(s) in maintained SCORE EGC source: {bad}')
    if text.count('asm {') != 2:
        raise ValueError('expected exactly two bounded symbolic asm blocks')
    return {
        'path': str(SOURCE.relative_to(ROOT)),
        'sha256': sha_file(SOURCE),
        'bounded_asm_blocks': 2,
        'forbidden_helpers_absent': True,
    }

def crossgame_full_core(out: Path) -> dict[str, object]:
    if sha(FULL_CORE) != FULL_CORE_SHA:
        raise ValueError('full EGC core identity drift')
    manifest = load_target_manifest(ROOT / 'config/targets.toml')
    checks = {
        'th04-main': {
            'offset': 0xCBFA,
            'map': ROOT / '.analysis/gpt-web/v401-master-vs-object-replay-001/a/source/obj/th04/main.map',
            'module': 'M=th04_main.asm',
            'public': 'egc_start_copy_noframe()',
        },
        'th05-main-smoke': {
            'offset': 0xBC2E,
            'map': ROOT / '.analysis/gpt-web/v401-master-vs-object-replay-001/a/source/obj/th05/main.map',
            'module': 'M=th05_main.asm',
            'public': 'egc_start_copy_noframe()',
        },
    }
    rows = {}
    for aid, spec in checks.items():
        artifact = find_artifact(manifest, aid)
        packed = read_verified_artifact(ROOT, artifact)
        raw = packed
        if len(raw) >= 0x20 and raw[0x1C:0x20].lower() == b'diet':
            per = out / 'crossgame' / aid
            per.mkdir(parents=True, exist_ok=True)
            raw, _ = restore_diet(artifact, packed, per)
        mz = parse_mz(raw)
        if not mz.valid:
            raise ValueError(f'{aid}: invalid MZ')
        hits = []
        pos = 0
        while True:
            pos = mz.program_image.find(FULL_CORE, pos)
            if pos < 0:
                break
            hits.append(pos)
            pos += 1
        if hits != [spec['offset']]:
            raise ValueError(f'{aid}: full-core hits drift: {hits}')
        map_text = spec['map'].read_text(encoding='cp437', errors='replace')
        if spec['module'] not in map_text or spec['public'] not in map_text:
            raise ValueError(f'{aid}: MAP binding drift')
        rows[aid] = {
            'packed_sha256': sha(packed),
            'restored_sha256': sha(raw),
            'relocations': len(mz.relocations),
            'core_offset': hex(spec['offset']),
            'core_sha256': FULL_CORE_SHA,
            'map_module': spec['module'],
            'map_public': spec['public'],
        }
    return rows

def overlay(work: Path) -> str:
    dst = work / 'src/maine/score/score_egc_start_copy.inl'
    dst.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(SOURCE, dst)
    score = work / 'th04/score86.cpp'
    if sha_file(score) != prior.BASE_SCORE_SOURCE_SHA:
        raise ValueError('v489 score86.cpp identity drift')
    text = score.read_text()
    matches = list(FUNC_RE.finditer(text))
    if len(matches) != 1:
        raise ValueError(f'SCORE EGC replacement anchor drift: {len(matches)}')
    m = matches[0]
    score.write_text(
        text[:m.start()]
        + '#include "src/maine/score/score_egc_start_copy.inl"\n\n'
        + 'void pascal near score_rect_copy('
        + text[m.end():]
    )
    return sha_file(score)

def build_round(out: Path, label: str, target_mz, baseline_mz, base_code: bytes, base_fixups: list[tuple[int, int]], base_map_norm: str) -> dict[str, object]:
    work = out / label / 'maine/source'
    work.parent.mkdir(parents=True)
    compact = copy_compact_snapshot(prior.SNAPSHOT, work, 'maine')
    patched_sha = overlay(work)

    obj = work / 'obj/th04/scoreall.obj'
    obj.unlink()
    tcc(work, out, f'score-egc-{label}', 'th04/scoreall.cpp')
    code = segment_bytes(obj, 'SCORE_TEXT')
    if code != base_code or sha(code) != prior.BASE_SCORE_CODE_SHA:
        raise ValueError(f'{label}: SCORE CODE drift')
    helper = code[LOCAL:LOCAL + SIZE]
    if len(helper) != SIZE or sha(helper) != TARGET_SHA:
        raise ValueError(f'{label}: helper object bytes drift')

    fixes = score_fixups(obj)
    if len(base_fixups) != BASE_FIXUP_COUNT or len(fixes) != CAND_FIXUP_COUNT:
        raise ValueError(f'{label}: SCORE fixup count drift')
    missing = list((Counter(base_fixups) - Counter(fixes)).elements())
    extra = list((Counter(fixes) - Counter(base_fixups)).elements())
    if missing != [REMOVED_FIXUP] or extra:
        raise ValueError(f'{label}: unexpected OMF fixup delta: missing={missing}, extra={extra}')

    exe = work / 'bin/th04/maine.exe'
    mp = work / 'obj/th04/maine.map'
    exe.unlink(); mp.unlink()
    run_checked(['wine', str(RUNNER), '-e', '-x', 'tlink', r'@obj\th04\maine.@l'], work, out / f'link-{label}.log', timeout=300)
    linked = parse_mz(exe.read_bytes())
    if not linked.valid or sha_file(exe) != prior.BASE_EXE_SHA:
        raise ValueError(f'{label}: final MAINE EXE drift')
    if sha_file(mp) != CAND_MAP_SHA or normalized_map(mp) != base_map_norm:
        raise ValueError(f'{label}: MAP drift beyond _address_0 classification')
    if [x.linear for x in linked.relocations] != [x.linear for x in target_mz.relocations]:
        raise ValueError(f'{label}: final ordered relocation drift')
    if linked.program_image[START:START + SIZE] != target_mz.program_image[START:START + SIZE]:
        raise ValueError(f'{label}: linked helper target mismatch')
    producer = linked.program_image[prior.SCORE_OWNER_OFFSET:prior.SCORE_OWNER_OFFSET + prior.SCORE_OWNER_SIZE]
    target_producer = target_mz.program_image[prior.SCORE_OWNER_OFFSET:prior.SCORE_OWNER_OFFSET + prior.SCORE_OWNER_SIZE]
    if producer != target_producer:
        raise ValueError(f'{label}: linked SCORE producer target mismatch')
    diffs = [i for i,(a,b) in enumerate(zip(linked.program_image,target_mz.program_image)) if a != b]
    if diffs != [0xD1D3, 0xD1D4]:
        raise ValueError(f'{label}: unrelated target difference set drift: {diffs[:20]}')
    return {
        'compact_snapshot': compact,
        'patched_score86_sha256': patched_sha,
        'score_code_sha256': sha(code),
        'score_fixup_count': len(fixes),
        'removed_decomp_zero_fixup': [REMOVED_FIXUP[0], hex(REMOVED_FIXUP[1])],
        'helper_sha256': sha(helper),
        'linked_exe_sha256': sha_file(exe),
        'linked_map_sha256': sha_file(mp),
        'normalized_map_equal_v489': True,
        'ordered_relocations': len(linked.relocations),
        'producer_sha256': sha(producer),
        'raw_difference_counts': {'helper': 0, 'producer': 0},
        'program_difference_offsets_vs_target': [hex(x) for x in diffs],
    }

def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--output-dir', type=Path)
    args = ap.parse_args()
    out = new_output(args.output_dir)
    if sha_file(RUNNER) != RUNNER_SHA:
        raise ValueError('runner identity drift')
    if sha_file(SOURCE) != SOURCE_SHA:
        raise ValueError('maintained SCORE EGC source drift')
    if sha_file(prior.TARGET) != prior.TARGET_SHA:
        raise ValueError('target identity drift')
    if sha_file(prior.SNAPSHOT / 'bin/th04/maine.exe') != prior.BASE_EXE_SHA:
        raise ValueError('v489 baseline EXE drift')
    if sha_file(prior.SNAPSHOT / 'obj/th04/maine.map') != prior.BASE_MAP_SHA:
        raise ValueError('v489 baseline MAP drift')

    policy = source_policy()
    crossgame = crossgame_full_core(out)
    target_mz = parse_mz(prior.TARGET.read_bytes())
    baseline_mz = parse_mz((prior.SNAPSHOT / 'bin/th04/maine.exe').read_bytes())
    base_obj = prior.SNAPSHOT / 'obj/th04/scoreall.obj'
    base_code = segment_bytes(base_obj, 'SCORE_TEXT')
    if len(base_code) != prior.SCORE_OWNER_SIZE or sha(base_code) != prior.BASE_SCORE_CODE_SHA:
        raise ValueError('v489 SCORE CODE drift')
    base_fixups = score_fixups(base_obj)
    if len(base_fixups) != BASE_FIXUP_COUNT:
        raise ValueError('v489 SCORE fixup count drift')
    base_map_norm = normalized_map(prior.SNAPSHOT / 'obj/th04/maine.map')

    target_helper = target_mz.program_image[START:START + SIZE]
    if len(target_helper) != SIZE or sha(target_helper) != TARGET_SHA:
        raise ValueError('target SCORE EGC helper drift')
    if target_helper[3:3 + len(FULL_CORE)] != FULL_CORE:
        raise ValueError('target SCORE EGC full hardware core drift')

    builds = {
        label: build_round(out, label, target_mz, baseline_mz, base_code, base_fixups, base_map_norm)
        for label in ('a','b')
    }
    stable = lambda row: {k:v for k,v in row.items() if k != 'compact_snapshot'}
    if stable(builds['a']) != stable(builds['b']):
        raise ValueError('independent SCORE EGC cold rounds differ')

    receipt = {
        'schema_version': 1,
        'observed_utc': datetime.now(timezone.utc).isoformat(timespec='seconds'),
        'claim_scope': 'TH04 MAINE SCORE EGC-start artifact-local hybrid decoded exactness',
        'artifact': 'th04-maine',
        'source_policy': policy,
        'crossgame_full_hardware_core': crossgame,
        'boundary': {
            'payload_offset': hex(START),
            'size': SIZE,
            'target_sha256': TARGET_SHA,
            'producer_payload_offset': hex(prior.SCORE_OWNER_OFFSET),
            'producer_size': prior.SCORE_OWNER_SIZE,
        },
        'object_format_delta': {
            'v489_fixup_count': BASE_FIXUP_COUNT,
            'candidate_fixup_count': CAND_FIXUP_COUNT,
            'removed_fixup': {'kind': REMOVED_FIXUP[0], 'score_offset': hex(REMOVED_FIXUP[1])},
            'explanation': 'The removed SCORE+0xA9B fixup is exactly keep_0(0)\'s decomp-only _address_0 linker-zero immediate. CODE is unchanged; final MZ bytes and relocations are unchanged.',
            'map_delta': '_address_0 changes only from Abs to idle; all other MAP text is identical.',
        },
        'surrounding_scaffold': 'Pinned v489 regist_menu remains producer context only and receives no exact source credit.',
        'builds': builds,
        'limit': 'Decoded 67-byte helper only; packed-file and whole-MAINE exactness are not claimed.',
    }
    path = out / 'receipt.json'
    path.write_text(json.dumps(receipt, indent=2) + '\n')
    print(json.dumps({'receipt':str(path),'receipt_sha256':sha_file(path),'helper_sha256':TARGET_SHA},sort_keys=True))
    return 0

if __name__ == '__main__':
    raise SystemExit(main())
