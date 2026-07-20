# Qwen3-VL-2B Model Staging Evidence

Captured: 2026-07-20

## Target

- Board: Orange Pi 5 Pro 16GB (`RK3588S`)
- Kernel: `6.1.99-rknpu098-test1`
- Private model directory: `/home/orangepi/qwen3_vl_2b_ws/models/qwen3-vl-2b/rkllm-model-zoo-1.2.3`
- Model execution: not authorized and not performed

## Artifacts

| Artifact | Source | Size (bytes) | SHA256 | State |
|---|---|---:|---|---|
| `qwen3-vl-2b_vision_rk3588.rknn` | Rockchip model zoo Filez, `1.2.3/RK3588/Qwen3-VL-2B` | 850488413 | `3d707ef5dbf0e420ac48e57b5bf0ed6c0fd1d5d048c29d81e1b5a8d051ab7ea8` | Verified official download |
| `qwen3-vl-2b-instruct_w8a8_rk3588.hf.rkllm` | `JiahaoLi/Qwen3-VL-RK3588` Hugging Face mirror | 2375022956 | `d5474340221fc495c70e1ec2c7dafc4ebf88292ce466db7e771e3a20b99cf21f` | Transfer verified; official Filez hash comparison pending |

The laptop and board sizes and SHA256 values matched exactly after SCP. The
language model remains `present_unverified` because byte identity with the
official Rockchip Filez artifact has not yet been established.

The retained official Filez partial download contains 862830136 bytes. A
byte-for-byte `cmp` against the beginning of the complete Hugging Face mirror
file returned success. This strongly supports that the mirror contains the
same artifact, but it does not establish the identity of the unobserved suffix
or replace a full official SHA256 comparison.

## Safety State

- No model binary was opened by RKNN or RKLLM Runtime.
- No NPU workload was executed.
- No system library was replaced.
- OpenSpec task 3.1 remains incomplete until both artifacts have official
  source provenance and recorded hashes.
