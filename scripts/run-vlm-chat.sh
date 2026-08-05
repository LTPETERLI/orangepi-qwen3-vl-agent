#!/usr/bin/env bash
set -euo pipefail

workspace=${ORANGEPI_WORKSPACE:-/home/orangepi/qwen3_vl_2b_ws}
app=${VLM_CHAT_APP:-"${workspace}/app/orangepi-text-chat"}
runtime=${RK_SDK_PREFIX:-"${workspace}/runtime/rkllm-1.3.0"}
models=${QWEN3_VL_MODEL_DIR:-"${workspace}/models/qwen3-vl-2b/rkllm-model-zoo-1.2.3"}
language=${QWEN3_VL_LANGUAGE_MODEL:-"${models}/qwen3-vl-2b-instruct_w8a8_rk3588.hf.rkllm"}

for file in "${app}" "${runtime}/lib/librkllmrt.so" "${language}"; do
  if [[ ! -f "${file}" ]]; then
    printf 'Required file not found: %s\n' "${file}" >&2
    exit 2
  fi
done

export LD_LIBRARY_PATH="${runtime}/lib${LD_LIBRARY_PATH:+:${LD_LIBRARY_PATH}}"
exec "${app}" --language "${language}" "$@"
