# RKLLM 1.3.0 Text Chat Integration Report

Date: 2026-08-05

## Result

The project-owned text chat entry point builds successfully on the Orange Pi 5
Pro and links to the application-private RKLLM 1.3.0 runtime. No NPU model was
executed during this step.

## Third-Party Audit

Pinned source:

```text
Qwen3-VL-2B-NPU
commit 03118c9887e6489c92d2bbc7ba4c772f12898094
```

That source does not compile against the verified RKLLM 1.3.0 header. It uses
the older callback signature, places image marker fields in `RKLLMParam`, and
uses the older flat multimodal image structure. The integration therefore does
not downgrade to its bundled runtime and does not copy its libraries into a
system directory.

## Accepted Text Path

```yaml
source: integration/vlm_chat
api: RKLLM 1.3.0
build_host: Orange Pi 5 Pro AArch64
compiler: g++ 11.4.0
runtime: /home/orangepi/qwen3_vl_2b_ws/runtime/rkllm-1.3.0/lib/librkllmrt.so
executable: /home/orangepi/qwen3_vl_2b_ws/app/orangepi-text-chat
sha256: 57b17d4c32bdbb3d2d234914c6ebe29cc0c489ddc07ddf8ea71d807dc0cbfd50
model_execution: not_performed
```

`file` identifies the executable as an AArch64 ELF binary. `ldd`, with the
private runtime path selected, resolves `librkllmrt.so` from the project
workspace and reports no missing dependencies. The `--help` path exits without
loading the model.

## User Command

After explicit model-execution approval:

```bash
ssh orangepi@192.168.31.89
/home/orangepi/qwen3_vl_2b_ws/bin/qwen3-vl-chat
```

Use `/clear` to clear conversation history and `/exit` to quit.

## Remaining Work

The pinned third-party multimodal implementation is rejected as a direct
RKLLM 1.3.0 integration path. Camera-to-VLM work must use the official 1.3.0
multimodal API and remains pending until the D455 is available.
