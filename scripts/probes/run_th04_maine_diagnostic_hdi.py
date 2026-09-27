#!/usr/bin/env python3
"""Replay a private MAINE HDI probe under pinned DOSBox-X, with one X11 frame.

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
    args = parser.parse_args()
    prepared = args.prepared_dir.resolve()
    output = args.output_dir.resolve()
    if (not prepared.is_relative_to(PRIVATE) or not output.is_relative_to(PRIVATE)
            or output.exists() or args.frame_second < 1
            or args.time_limit <= args.frame_second + 2):
        parser.error("use private dirs and a frame before the time limit")
    prep_receipt_path = prepared / "receipt.json"
    prep = json.loads(prep_receipt_path.read_text(encoding="utf-8"))
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
                               stderr=subprocess.STDOUT, text=True)
    screenshot = output / "frame.png"
    try:
        time.sleep(args.frame_second)
        subprocess.run(["import", "-window", "root", str(screenshot)], check=True,
                       timeout=10, capture_output=True)
        log, _ = process.communicate(timeout=args.time_limit + 10)
    finally:
        if process.poll() is None:
            process.kill()
            process.communicate()
    (output / "boot.log").write_text(log, encoding="utf-8")
    if sha(source_image.read_bytes()) != prep["diagnostic_hdi_sha256"]:
        raise ValueError("prepared source HDI was modified")
    fs = Fat12(bytearray(image.read_bytes()))
    try:
        entry = fs.find_entry([fs.root], b"DIAG    TXT")
        marker = fs.file_bytes(u16(fs.image, entry + 26), u32(fs.image, entry + 28))
    except ValueError:
        marker = None
    expected_boot = runtime["primary"]["execution"]["boot_required_log_markers"]
    receipt = {
        "schema_version": 1,
        "observed_utc": datetime.now(timezone.utc).isoformat(),
        "scope": "diagnostic PC-98 boot frame; no MAINE execution or runtime acceptance implied",
        "prepared_receipt_sha256": sha(prep_receipt_path.read_bytes()),
        "prepared_hdi_sha256": prep["diagnostic_hdi_sha256"],
        "executed_hdi_sha256": sha(image.read_bytes()),
        "maine_source": prep["maine_source"],
        "startup": prep["startup"],
        "emulator_sha256": runtime["primary"]["binary_sha256"],
        "x11_config_sha256": sha(config.read_bytes()),
        "command": command,
        "frame_second": args.frame_second,
        "returncode": process.returncode,
        "missing_boot_markers": [item for item in expected_boot if item not in log],
        "diagnostic_marker_hex": marker.hex() if marker is not None else None,
        "boot_log_sha256": sha(log.encode()),
        "frame_sha256": sha(screenshot.read_bytes()),
    }
    (output / "receipt.json").write_text(json.dumps(receipt, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"receipt": str(output / "receipt.json"),
                      "returncode": process.returncode,
                      "marker_hex": receipt["diagnostic_marker_hex"]}, sort_keys=True))
    if process.returncode or receipt["missing_boot_markers"]:
        raise ValueError("diagnostic boot did not pass host smoke markers")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
