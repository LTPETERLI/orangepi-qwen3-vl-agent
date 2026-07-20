# First Official Qwen3-VL Demo Report

Date: 2026-07-20

## Result

The single user-authorized fixed-image diagnostic completed with exit code 0.
The deployment path is technically functional. This is not the 30-run
stability acceptance milestone.

## Stack

- Board: Orange Pi 5 Pro 16GB (`RK3588S`)
- Kernel: `6.1.99-rknpu098-test1`
- RKNPU driver: `0.9.8`
- RKLLM Runtime: `1.3.0`, application-private
- RKNN Runtime: bundled with `rknn-llm release-v1.3.0`, application-private
- Model format: RKLLM Toolkit `1.2.3`, W8A8
- Official source commit: `878f9361fd3afa7e167b7079918918f78d2c1c2a`
- Demo SHA256: `898a3e3a26fa87e659bba4a2d51b473b9fa28c7528fc68e8973539596aad54e7`
- Input: official `demo.jpg`, 448x448 vision input
- Parameters: 256 new tokens, context 4096, 3 NPU cores, platform `rk3588`

`ldd` resolved `librknnrt.so` and `librkllmrt.so` from
`/home/orangepi/qwen3_vl_2b_ws/runtime/rkllm-1.3.0/lib`. System Rockchip
library hashes remained unchanged after the build and diagnostic.

## Measurements

| Measurement | Result |
|---|---:|
| LLM load | 2723.89 ms |
| Vision model load | 867.51 ms |
| Vision inference | 2309.78 ms |
| Prefill | 180.16 tokens/s |
| Generation | 11.82 tokens/s |
| Runtime-reported peak memory | 3165.57 MB |
| Minimum observed system available memory | 12670713856 bytes |
| New kernel fault signatures | 0 |

The only new kernel messages were normal RKNPU IOMMU domain switches. No IOMMU
fault, page fault, OOM, killed process, oops, or panic was observed.

## Output Review

The answer correctly identified the astronaut, lunar surface, Earth, relaxed
pose, green bottle, and cooler. It incorrectly identified the visible
Carlsberg label as Coca-Cola. Runtime acceptance is `PASS`; single-image
semantic quality is `CONDITIONAL`.

## Gate Decision

- One-run official demo diagnostic: `PASS`
- Semantic spot check: `CONDITIONAL`
- Thirty-run stability acceptance: `PENDING`
- Further NPU execution: not authorized by this completed one-run approval

Raw evidence is stored under `audit/model-diagnostic/first-fixed-image/`.
