#!/usr/bin/env bash
set -euo pipefail

workspace=${ORANGEPI_WORKSPACE:-/home/orangepi/qwen3_vl_2b_ws}
export DISPLAY=${DISPLAY:-:0}
export XAUTHORITY=${XAUTHORITY:-/home/orangepi/.Xauthority}
exec python3 "${workspace}/app/camera_vlm_gui.py"
