#!/usr/bin/env python3
"""Replay a private OP or MAINE HDI probe under pinned DOSBox-X.

Run under ``xvfb-run -a``. A frame and a DOS boot log are diagnostics only;
the OP-to-MAINE transition must be observed before claiming MAINE execution.
"""

from __future__ import annotations

import argparse
from datetime import datetime, timezone
import json
import os
from pathlib import Path
import shutil
import signal
import subprocess
import time
import tomllib

from prepare_th04_maine_diagnostic_hdi import Fat12, sha, u16, u32


ROOT = Path(__file__).resolve().parents[2]
PRIVATE = (ROOT / ".analysis/runtime/candidates").resolve()


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--prepared-dir", type=Path, required=True)
    parser.add_argument("--output-dir", type=Path, required=True)
    parser.add_argument("--frame-second", type=int, default=10)
    parser.add_argument("--time-limit", type=int, default=20)
    parser.add_argument(
        "--input-key",
        help="send one xdotool key to the DOSBox-X window before the frame",
    )
    parser.add_argument(
        "--input-second",
        type=float,
        help="host-second offset for --input-key (requires --input-key)",
    )
    parser.add_argument(
        "--input-event",
        action="append",
        default=[],
        metavar="KEY@SECOND",
        help="send a key at a host-second offset; repeat for a timeline",
    )
    args = parser.parse_args()
    input_specs = list(args.input_event)
    if (args.input_key is not None) != (args.input_second is not None):
        parser.error("--input-key and --input-second must be supplied together")
    if args.input_key is not None:
        input_specs.append(f"{args.input_key}@{args.input_second}")
    parsed_inputs = []
    for raw in input_specs:
        if "@" not in raw:
            parser.error("input events must use KEY@SECOND")
        key, second_text = raw.rsplit("@", 1)
        try:
            second = float(second_text)
        except ValueError:
            parser.error(f"invalid input-event offset: {raw}")
        if not key or second < 0 or second >= args.frame_second:
            parser.error("input events must be nonempty and before the frame")
        parsed_inputs.append((second, key))
    parsed_inputs.sort()
    prepared = args.prepared_dir.resolve()
    output = args.output_dir.resolve()
    if (not prepared.is_relative_to(PRIVATE) or not output.is_relative_to(PRIVATE)
            or output.exists() or args.frame_second < 1
            or args.time_limit <= args.frame_second + 2
            or any(second >= args.frame_second for second, _ in parsed_inputs)):
        parser.error("use private dirs and a frame before the time limit")
    prep_receipt_path = prepared / "receipt.json"
    prep = json.loads(prep_receipt_path.read_text(encoding="utf-8"))
    artifact = prep.get("artifact", "th04-maine")
    if artifact not in {"th04-main", "th04-maine", "th04-op", "th04-zun"}:
        raise ValueError(f"unsupported prepared artifact: {artifact}")
    source_image = prepared / "diagnostic.hdi"
    if sha(source_image.read_bytes()) != prep["diagnostic_hdi_sha256"]:
        raise ValueError("prepared HDI identity drift")

    runtime = tomllib.loads((ROOT / "config/runtime.toml").read_text(encoding="utf-8"))
    executable_name = shutil.which(runtime["primary"]["command"])
    if executable_name is None:
        raise ValueError("pinned DOSBox-X is unavailable")
    executable = Path(executable_name).resolve()
    if not executable.is_file() or sha(executable.read_bytes()) != runtime["primary"]["binary_sha256"]:
        raise ValueError("DOSBox-X binary identity drift")
    source_conf = ROOT / runtime["primary"]["config"]
    conf_bytes = source_conf.read_bytes()
    if sha(conf_bytes) != runtime["primary"]["config_sha256"]:
        raise ValueError("pinned headless configuration identity drift")
    old = b"videodriver       = dummy"
    if conf_bytes.count(old) != 1:
        raise ValueError("expected one pinned video driver setting")

    output.mkdir(parents=True)
    image = output / "execution.hdi"
    shutil.copyfile(source_image, image)
    config = output / "dosbox-x-x11.conf"
    config.write_bytes(conf_bytes.replace(old, b"videodriver       = x11"))
    command = [
        str(executable), "-defaultconf", "-defaultmapper", "-conf", str(config),
        "-fastlaunch", "-nogui", "-nomenu", "-exit", "-time-limit",
        str(args.time_limit), "-c", f'imgmount 2 "{image}" -t hdd -fs none',
        "-c", "boot -l c",
    ]
    env = os.environ.copy()
    env.update({
        "SDL_VIDEODRIVER": "x11", "SDL_AUDIODRIVER": "dummy",
        "XDG_CACHE_HOME": str(output / "cache"),
        "XDG_CONFIG_HOME": str(output / "config"),
        "XDG_DATA_HOME": str(output / "data"),
    })
    process = subprocess.Popen(command, cwd=ROOT, env=env, stdout=subprocess.PIPE,
                               stderr=subprocess.STDOUT, text=True,
                               start_new_session=True)
    screenshot = output / "frame.png"
    host_timeout = False
    input_events = []
    next_input = 0
    try:
        started = time.monotonic()
        while True:
            elapsed = time.monotonic() - started
            if (next_input < len(parsed_inputs)
                    and elapsed >= parsed_inputs[next_input][0]):
                requested_second, input_key = parsed_inputs[next_input]
                xdotool = shutil.which("xdotool")
                if xdotool is None:
                    raise ValueError("input events require xdotool")
                windows = subprocess.run(
                    [xdotool, "search", "--onlyvisible", "--name", "DOSBox-X"],
                    check=False, capture_output=True, text=True,
                ).stdout.split()
                if not windows:
                    raise ValueError("DOSBox-X window not found for --input-key")
                for window in windows:
                    subprocess.run(
                        [xdotool, "key", "--window", window, input_key],
                        check=True, capture_output=True, text=True,
                    )
                input_events.append({
                    "key": input_key,
                    "requested_second": requested_second,
                    "observed_second": elapsed,
                    "window_ids": windows,
                })
                next_input += 1
            remaining = args.frame_second - elapsed
            if remaining <= 0:
                break
            time.sleep(min(0.05, remaining))
        subprocess.run(["import", "-window", "root", str(screenshot)], check=True,
                       timeout=10, capture_output=True)
        try:
            log, _ = process.communicate(
                timeout=max(10, args.time_limit - args.frame_second + 10)
            )
        except subprocess.TimeoutExpired:
            host_timeout = True
            os.killpg(process.pid, signal.SIGKILL)
            log, _ = process.communicate(timeout=10)
    finally:
        if process.poll() is None:
            os.killpg(process.pid, signal.SIGKILL)
            process.communicate(timeout=10)
    (output / "boot.log").write_text(log, encoding="utf-8")
    if sha(source_image.read_bytes()) != prep["diagnostic_hdi_sha256"]:
        raise ValueError("prepared source HDI was modified")
    fs = Fat12(bytearray(image.read_bytes()))
    try:
        entry = fs.find_entry([fs.root], b"DIAG    TXT")
        marker = fs.file_bytes(u16(fs.image, entry + 26), u32(fs.image, entry + 28))
    except ValueError:
        marker = None
    try:
        game_dir = fs.find_entry([fs.root], b"GENSO      ")
        game_offsets = [fs.cluster_offset(cluster) for cluster in
                        fs.chain(u16(fs.image, game_dir + 26))]
        trace_entry = fs.find_entry(game_offsets, b"OPMARK  TXT")
        op_trace = fs.file_bytes(u16(fs.image, trace_entry + 26),
                                 u32(fs.image, trace_entry + 28))
    except ValueError:
        op_trace = None
    try:
        game_dir = fs.find_entry([fs.root], b"GENSO      ")
        game_offsets = [fs.cluster_offset(cluster) for cluster in
                        fs.chain(u16(fs.image, game_dir + 26))]
        cdg_entry = fs.find_entry(game_offsets, b"OPCDG   BIN")
        cdg_slots = fs.file_bytes(u16(fs.image, cdg_entry + 26),
                                  u32(fs.image, cdg_entry + 28))
    except ValueError:
        cdg_slots = None
    try:
        game_dir = fs.find_entry([fs.root], b"GENSO      ")
        game_offsets = [fs.cluster_offset(cluster) for cluster in
                        fs.chain(u16(fs.image, game_dir + 26))]
        mainhit_entry = fs.find_entry(game_offsets, b"MAINHIT TXT")
        mainhit = fs.file_bytes(u16(fs.image, mainhit_entry + 26),
                                u32(fs.image, mainhit_entry + 28))
    except ValueError:
        mainhit = None
    try:
        game_dir = fs.find_entry([fs.root], b"GENSO      ")
        game_offsets = [fs.cluster_offset(cluster) for cluster in
                        fs.chain(u16(fs.image, game_dir + 26))]
        input_entry = fs.find_entry(game_offsets, b"INPUT   BIN")
        input_trace = fs.file_bytes(u16(fs.image, input_entry + 26),
                                    u32(fs.image, input_entry + 28))
    except ValueError:
        input_trace = None
    try:
        game_dir = fs.find_entry([fs.root], b"GENSO      ")
        game_offsets = [fs.cluster_offset(cluster) for cluster in
                        fs.chain(u16(fs.image, game_dir + 26))]
        main_trace_entry = fs.find_entry(game_offsets, b"MAIN    BIN")
        main_trace = fs.file_bytes(u16(fs.image, main_trace_entry + 26),
                                   u32(fs.image, main_trace_entry + 28))
    except ValueError:
        main_trace = None
    try:
        game_dir = fs.find_entry([fs.root], b"GENSO      ")
        game_offsets = [fs.cluster_offset(cluster) for cluster in
                        fs.chain(u16(fs.image, game_dir + 26))]
        ems_trace_entry = fs.find_entry(game_offsets, b"EMS     BIN")
        ems_trace = fs.file_bytes(u16(fs.image, ems_trace_entry + 26),
                                  u32(fs.image, ems_trace_entry + 28))
    except ValueError:
        ems_trace = None
    expected_boot = runtime["primary"]["execution"]["boot_required_log_markers"]
    receipt = {
        "schema_version": 1,
        "observed_utc": datetime.now(timezone.utc).isoformat(),
        "scope": "diagnostic PC-98 boot frame; product execution needs a separate checkpoint",
        "artifact": artifact,
        "prepared_receipt_sha256": sha(prep_receipt_path.read_bytes()),
        "prepared_hdi_sha256": prep["diagnostic_hdi_sha256"],
        "executed_hdi_sha256": sha(image.read_bytes()),
        "artifact_source": prep.get("artifact_source", prep.get("maine_source")),
        "startup": prep["startup"],
        "emulator_sha256": runtime["primary"]["binary_sha256"],
        "x11_config_sha256": sha(config.read_bytes()),
        "command": command,
        "frame_second": args.frame_second,
        "input_events": input_events,
        "host_timeout": host_timeout,
        "returncode": process.returncode,
        "missing_boot_markers": [item for item in expected_boot if item not in log],
        "diagnostic_marker_hex": marker.hex() if marker is not None else None,
        "op_trace_marker_hex": op_trace.hex() if op_trace is not None else None,
        "op_cdg_slots_hex": cdg_slots.hex() if cdg_slots is not None else None,
        "mainhit_marker_hex": mainhit.hex() if mainhit is not None else None,
        "input_trace_hex": input_trace.hex() if input_trace is not None else None,
        "main_trace_hex": main_trace.hex() if main_trace is not None else None,
        "ems_trace_hex": ems_trace.hex() if ems_trace is not None else None,
        "boot_log_sha256": sha(log.encode()),
        "frame_sha256": sha(screenshot.read_bytes()),
    }
    if artifact == "th04-maine":
        receipt["maine_source"] = receipt["artifact_source"]
    (output / "receipt.json").write_text(json.dumps(receipt, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"receipt": str(output / "receipt.json"),
                      "returncode": process.returncode,
                      "marker_hex": receipt["diagnostic_marker_hex"]}, sort_keys=True))
    # A frame probe may stop the emulator after its checkpoint. The receipt
    # records this separately from an emulator crash or missing boot marker.
    if (process.returncode and not host_timeout) or receipt["missing_boot_markers"]:
        raise ValueError("diagnostic boot did not pass host smoke markers")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
