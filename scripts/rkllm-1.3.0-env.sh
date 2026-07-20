#!/usr/bin/env bash

rkllm_root="/home/orangepi/qwen3_vl_2b_ws/runtime/rkllm-1.3.0"

if [[ ! -f "$rkllm_root/lib/librkllmrt.so" || ! -f "$rkllm_root/lib/librknnrt.so" ]]; then
  echo "RKLLM 1.3.0 private runtime is incomplete: $rkllm_root" >&2
  return 1 2>/dev/null || exit 1
fi

export RKLLM_RUNTIME_ROOT="$rkllm_root"
export LD_LIBRARY_PATH="$rkllm_root/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
export CPLUS_INCLUDE_PATH="$rkllm_root/include${CPLUS_INCLUDE_PATH:+:$CPLUS_INCLUDE_PATH}"
