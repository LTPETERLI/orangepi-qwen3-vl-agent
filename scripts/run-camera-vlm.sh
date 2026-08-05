#!/usr/bin/env bash
set -euo pipefail

workspace=${ORANGEPI_WORKSPACE:-/home/orangepi/qwen3_vl_2b_ws}
realsense_prefix=${REALSENSE_PREFIX:-"${workspace}/runtime/librealsense-2.56.5-rsusb"}
rkllm_prefix=${RK_SDK_PREFIX:-"${workspace}/runtime/rkllm-1.3.0"}
keyframe_app=${REALSENSE_KEYFRAME_APP:-"${workspace}/app/realsense-keyframe"}
demo=${QWEN3_VL_DEMO:-"${workspace}/vendor-src/rknn-llm-878f936/examples/multimodal_model_demo/deploy/install/demo_Linux_aarch64/demo"}
model_dir=${QWEN3_VL_MODEL_DIR:-"${workspace}/models/qwen3-vl-2b/rkllm-model-zoo-1.2.3"}
vision_model=${QWEN3_VL_VISION_MODEL:-"${model_dir}/qwen3-vl-2b_vision_rk3588.rknn"}
language_model=${QWEN3_VL_LANGUAGE_MODEL:-"${model_dir}/qwen3-vl-2b-instruct_w8a8_rk3588.hf.rkllm"}
mode=capture-only

if [[ ${1:-} == "--execute-model" ]]; then
  mode=execute-model
  shift
elif [[ ${1:-} == "--capture-only" ]]; then
  shift
fi
if (($#)); then
  printf 'Usage: %s [--capture-only | --execute-model]\n' "$0" >&2
  exit 2
fi

for file in "$keyframe_app" "$realsense_prefix/lib/librealsense2.so.2.56.5"; do
  [[ -f $file ]] || { printf 'Required file not found: %s\n' "$file" >&2; exit 2; }
done

run_id=$(date +%Y%m%dT%H%M%S)
run_dir="${workspace}/logs/camera-vlm/${run_id}"
image="${run_dir}/keyframe.ppm"
mkdir -p "$run_dir"

LD_LIBRARY_PATH="${realsense_prefix}/lib" "$keyframe_app" --output "$image" \
  | tee "${run_dir}/capture.txt"
sha256sum "$image" | tee "${run_dir}/keyframe.sha256"

if [[ $mode == capture-only ]]; then
  printf 'Capture complete; model execution was not requested.\n'
  printf 'Run directory: %s\n' "$run_dir"
  exit 0
fi

for file in "$demo" "$rkllm_prefix/lib/librkllmrt.so" "$rkllm_prefix/lib/librknnrt.so" "$vision_model" "$language_model"; do
  [[ -f $file ]] || { printf 'Required file not found: %s\n' "$file" >&2; exit 2; }
done

printf 'Starting the approved offline camera-to-VLM session. Type exit to quit.\n'
export LD_LIBRARY_PATH="${rkllm_prefix}/lib"
exec "$demo" "$image" "$vision_model" "$language_model" 128 4096 3 rk3588
