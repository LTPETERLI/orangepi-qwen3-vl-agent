#!/usr/bin/env bash
set -euo pipefail

project_root=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
sdk_prefix=${RK_SDK_PREFIX:-/home/orangepi/qwen3_vl_2b_ws/runtime/rkllm-1.3.0}
build_dir=${VLM_CHAT_BUILD_DIR:-"${project_root}/build/vlm-chat"}

cmake -S "${project_root}/integration/vlm_chat" -B "${build_dir}" \
  -DCMAKE_BUILD_TYPE=Release \
  -DRK_SDK_PREFIX="${sdk_prefix}"
cmake --build "${build_dir}" --parallel "$(nproc)"

printf 'Built: %s\n' "${build_dir}/orangepi-text-chat"
