# TH04-local BGM beeper service and MAINE link closure

2026-09-28 native product-build batch. `src/shared/sound/bgm_beep.cpp` owns
`BGM_INIT`, `BGM_FINISH`, `BGM_READ_SDATA`, `BGM_SOUND`, and the scheduler
entry `BGM_TICK`. `src/shared/sound/bgm_timer.asm` owns the PC-98 IRQ 8 vector,
PIT setup, PIC mask, and ISR. Both are TH04 product sources; neither includes
ReC98 headers or source. The implementation is semantic and is not a target
byte-match claim.

The pinned OP/ending PAR contains the 8,284-byte `MIKO.EFS` member, SHA-256
`12045fed57d7c5a07da0047ae13c6607cbc78155cfbf5baa719fc310131de607`.
It has 15 zero-terminated decimal-frequency sequences with 1,496 total words.
The longest sequence has 253 samples, inside the 256-sample effect buffer.
A separate small-model harness linked to the pinned historical `masters.lib`
reads that member as 15 effects and produces word-stream FNV-1a `010D3D58`.
The checked-in probe then runs the TH04-only large-model source with the same
resource through the local PAR hook and as a loose DOS file. Its test checks
effect selection, emitted Hertz sequence, missing/invalid files, repeated
initialization, and IRQ vector restoration. A software `INT 8` exercises the
installed ISR; the next emitted sample establishes that it reached the
scheduler. The test also follows MAINE's `mem_unassign()`-before-`bgm_finish()`
exit order. Active effects are copied into DGROUP before playback so the IRQ
never reads an already released heap segment. The isolated test MZ has 269
audited relocation sites at DOS load segments `0x2000` and `0x6000`. The
runtime receipt SHA-256 is
`7e7cd258d9bee9cdb55eebcb300f4f14ebc1f5d99cb8ea66be227cb741e0dfa6`.

## Far-call fixup counterexample

The first 130-object TH04-only MAINE link had zero unresolved names and zero
warnings in two cold rounds, and its MZ passed 655 static relocations. Yet the
unqualified TASM `call BGM_TICK` in the IRQ wrapper was linked as `PUSH CS` /
`CALL rel16` to `0703:2ABD`, while the MAP placed `BGM_TICK` at `0703:56CD`.
That MZ was **invalid for runtime use** despite a successful TLINK exit. The
expanded call-ABI gate caught the missing call to the public. Even `call far
ptr BGM_TICK` was relaxed to the same wrong relative instruction in the full
link; its small isolated test happened to pass. The source now encodes the
ordinary 8086 far-call opcode `9A` followed by symbolic `offset` and `seg`
fixups. The first isolated full-object relink produced `09C4:2ABD`, whose
linear address equals MAP `0703:56CD`, and the segment operand has an MZ
relocation. The audit compares normalized linear targets for this reason.
The final DOS `INT 8` probe and final-MZ call audit both pass. For later
games, include interrupt-to-C++ edges in the far-call
audit; neither link success nor an MZ relocation count detects a wrong
same-segment relative target.

The final TH04-only MAINE MZ links 130 source objects with zero unresolved
symbols and zero warnings. Its MZ has 656 valid relocation sites and nine
target segment values at the two checked DOS load segments. The call audit
checks 36 far-return owners, 148 relocated direct far calls, and one same-CS
far call; the IRQ's `BGM_TICK` edge is among the relocated far calls. The
candidate MAP records `BGM_TICK` at `0703:56CD`, `BGM_TIMER_START` at
`0703:6426`, and `BGM_TIMER_STOP` at `0703:6474`. These are **candidate MAP
addresses**, not target MAINE offsets. The first final no-support link,
relocation audit, and call-audit receipt SHA-256 values are respectively
`8724da0c3c6738b2cce9e8cc748122c06b0b80e13ebf3663318d48fa014966fa`,
`0491b876ac630ef51320b28d86b23195eab6f7ee4854ecee466881fdef86f613`,
and `4e184e69343030391e032cce4750ea9afac01adaae07056dfd8232786672c80a`.
The second cold link has the same MZ SHA-256
`6a20e946ce6ed8c6865887ba90fef31e4cbcb34937dd22e6829b30c21a5435f2`
and the same 130 link-relevant and timestamp-normalized OMF objects. Only
BGIMAGE's dependency timestamp comment changes its raw object hash. The
second link receipt SHA-256 is
`de18078dffdceb8cdc34b162b9dc6eb56260236200069e9b30c8b5f948e31263`.

The audited no-support MZ was placed in a disposable FAT12 copy of the pinned
HDI. The readback check found its exact 67,990 bytes in `GENSO/MAINE.EXE`;
the source HDI stayed hash-identical. A 20-second DOSBox-X PC-98 `GAME.BAT`
run displayed the OP `ZUN Soft / 東方 Project` splash and wrote only the
`START` diagnostic marker. It did **not** prove candidate MAINE execution.
Preparation and run receipt SHA-256 values are
`0031fc989b6e98b8d215f2b9ec84993a2110ef9a560e4535f9a5e509a90bae32`
and `bc168c158e083cb1e306915ccb9551fc548e6676ae9ee78478e07283a1990edc`.
The next runtime Oracle needs scripted input and a MAINE entry checkpoint;
sampling an OP splash frame cannot settle MAINE behavior.

## Replay and limits

Run these serially, using new directories below `.analysis/reconstruction/probes`:

```text
python3 scripts/probes/probe_th04_native_bgm_runtime.py --output-dir .analysis/reconstruction/probes/NEW-bgm-runtime
python3 scripts/probes/probe_th04_native_maine_link.py --without-support --output-dir .analysis/reconstruction/probes/NEW-maine-a
python3 scripts/probes/probe_th04_native_maine_link.py --without-support --output-dir .analysis/reconstruction/probes/NEW-maine-b
python3 scripts/probes/compare_th04_native_maine_link.py .analysis/reconstruction/probes/NEW-maine-a .analysis/reconstruction/probes/NEW-maine-b
python3 scripts/probes/audit_th04_native_maine_mz.py --link-receipt .analysis/reconstruction/probes/NEW-maine-a/receipt.json --output-dir .analysis/reconstruction/probes/NEW-maine-mz
python3 scripts/probes/audit_th04_native_maine_call_abi.py --link-receipt .analysis/reconstruction/probes/NEW-maine-a/receipt.json --output-dir .analysis/reconstruction/probes/NEW-maine-call
```

The DOS runner establishes EFS parsing, scheduler behavior, and software IRQ
entry under an emulated DOS environment. PC-98 PIT cadence, beeper waveform,
real MAINE entry, OP-to-MAINE handoff, and music-driver interaction still need
a recorded PC-98 runtime scenario. MAINE's native MZ is not a replacement for
the distributed packed executable until that scenario and packaging are
validated. MAIN's two nonexact function slices are outside this build batch.
