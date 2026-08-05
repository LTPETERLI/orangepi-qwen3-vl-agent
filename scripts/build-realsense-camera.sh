#!/usr/bin/env bash
set -euo pipefail

project_root=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
prefix=${REALSENSE_PREFIX:-/home/orangepi/qwen3_vl_2b_ws/runtime/librealsense-2.56.5-rsusb}
rknn_prefix=${RKNN_PREFIX:-/home/orangepi/qwen3_vl_2b_ws/runtime/rkllm-1.3.0}
build_dir=${REALSENSE_CAMERA_BUILD_DIR:-"${project_root}/build/realsense-camera"}

cmake -S "${project_root}/integration/realsense_camera" -B "${build_dir}" \
  -DCMAKE_BUILD_TYPE=Release \
  -DREALSENSE_PREFIX="${prefix}" \
  -DRKNN_PREFIX="${rknn_prefix}"
cmake --build "${build_dir}" --parallel "$(nproc)"

printf 'Built: %s\n' "${build_dir}/realsense-camera-check"
