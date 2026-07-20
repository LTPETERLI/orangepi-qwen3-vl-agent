# Official Qwen3-VL Demo Stability Report

Date: 2026-07-20

## Decision

The official fixed-image demo stability gate is `PASS`. Thirty consecutive
Chinese-preset multimodal runs completed with valid non-empty responses, exit
code 0, and no kernel fault signature.

## Accepted Suite

- Prompt: official preset `[1] <image>这张图片中有什么？`
- Image: official `demo.jpg`
- Runs: 30 consecutive independent processes
- Output limit: 128 tokens
- Generated tokens: 127 per run
- Context: 4096
- NPU cores: 3
- Platform: `rk3588`
- Runtime: RKLLM 1.3.0 and application-private RKNN Runtime
- Model: Qwen3-VL-2B, RKLLM Toolkit 1.2.3, W8A8

All 30 responses were byte-identical under deterministic `top_k=1`. They were
coherent Chinese descriptions of the astronaut, lunar surface, Earth, green
bottle, and cooler. The 128-token limit truncated the description at a natural
continuation point, but each response contained substantially more than the
minimum 32-token validity threshold.

## Measurements

| Measurement | Minimum | Average | Maximum |
|---|---:|---:|---:|
| End-to-end duration | 18.277 s | 19.169 s | 20.146 s |
| Generation speed | 11.53 tok/s | 11.77 tok/s | 12.03 tok/s |
| Runtime peak memory | 3165.40 MB | 3165.53 MB | 3165.73 MB |

- Available memory before suite: 15985909760 bytes
- Available memory after suite: 15987548160 bytes
- Swap used after suite: 0 bytes
- New IOMMU fault, page fault, OOM, oops, or panic signatures: 0
- RKNPU IOMMU domain-switch messages: 120, expected operational messages

## Rejected Preliminary Suite

An earlier 30-run English-preset suite was technically stable but generated
only five tokens per answer: `Based on the image provided,`. It is retained as
diagnostic evidence but was not counted toward acceptance because the response
was incomplete. The corrected Chinese suite above is the acceptance basis.

## Evidence

- Accepted raw logs: `audit/model-diagnostic/stability-30-zh/`
- Rejected preliminary logs: `audit/model-diagnostic/stability-30/`
- First diagnostic: `audit/model-diagnostic/first-fixed-image/`

This result accepts the official fixed-image runtime baseline. It does not yet
accept RealSense capture, live camera-to-VLM integration, or long-duration
agent operation.
