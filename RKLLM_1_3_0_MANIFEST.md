# RKLLM 1.3.0 Manifest

## Source Identity

| Field | Value |
|---|---|
| Origin | `https://github.com/airockchip/rknn-llm.git` |
| Tag | `release-v1.3.0` |
| Annotated tag object | `5c93e267e6d4cd505d3d50c24f8327081dbf1de0` |
| Commit | `878f9361fd3afa7e167b7079918918f78d2c1c2a` |
| Expected short commit | `878f936` |
| Match | PASS |
| Checkout status | Detached HEAD at the exact release tag; clean |

## Toolkit Packages

| File | SHA256 |
|---|---|
| `requirements.txt` | `3eedb367f80ac0a0a34028f1986cfd3ec1f16960e75aaafcc501c4260b0d2e98` |
| `requirements_rwkv7.txt` | `bd0e0fbd57124832440c3a5f36f49a48dae52cc16be9ccd877215cde2eb8ce8b` |
| `rkllm_toolkit-1.3.0-cp39-cp39-linux_x86_64.whl` | `41d9999afc69c26eeef559a792a18a797dd417e1e3b722e928d8aecc960b073c` |
| `rkllm_toolkit-1.3.0-cp310-cp310-linux_x86_64.whl` | `2523e71f305963495c9fccc639dbcf8f9f49a9b8c6d6f6b355e58a3af076f753` |
| `rkllm_toolkit-1.3.0-cp311-cp311-linux_x86_64.whl` | `69bfc8529cfc28843a6dfcf85b1a06ad6bb89e20c258df41b99145aabc9108ae` |
| `rkllm_toolkit-1.3.0-cp312-cp312-linux_x86_64.whl` | `b20e9094b2cf55fd7dc055cd771eb7cc4251e32efe398081c11cc607f9bfea8f` |

No wheel was installed.

## Runtime, Header, Demo and Driver Artifact

| File | SHA256 |
|---|---|
| `rkllm-runtime/Linux/librkllm_api/aarch64/librkllmrt.so` | `6a9e4fc5324c68921c3a900340361e107af7599fe34dc8fa7759b2c5ae22a6e6` |
| `rkllm-runtime/Linux/librkllm_api/include/rkllm.h` | `80596a578f7f8e70df6eda1c2cbead3bfced14623a190258f2bd009a3d1f72cf` |
| `examples/multimodal_model_demo/deploy/src/main.cpp` | `e91782433ddeba1884593895f2536258453681cbe7500c24721eb02dc8b3b7c0` |
| `examples/multimodal_model_demo/deploy/src/image_enc.cc` | `3d725f230a684ab17284cf78d4bc3fff04df5b49c5a84ae2511671357863bf29` |
| `examples/multimodal_model_demo/deploy/src/image_enc.h` | `fefcd6ce2f306cce64617fe9fefe0fe38dbf4551064f7842bf43a599b5511f63` |
| `rknpu-driver/rknpu_driver_0.9.8_20241009.tar.bz2` | `188306a3eba7ca48c186fe2a39acf232dec8536484581e54e9b96c3f65677eba` |

The repository driver archive is evidence of SDK contents, not authorization to replace the Orange Pi built-in driver.

## Official Multimodal Baseline

- Supported model families include Qwen2-VL/Qwen3-VL, Qwen3/Qwen3.5, Gemma, InternVL, MiniCPM, SmolVLM, DeepSeekOCR, RWKV7, and others listed in the tagged README.
- Multimodal README minimums: `rkllm-toolkit>=1.3.0`, `rknn-toolkit2>=2.3.2`.
- Qwen3-VL vision export baseline: `448x448`.
- Official Qwen3-VL command recorded in the tag:

```bash
./demo demo.jpg ./qwen3-vl-2b_vision_rk3588.rknn \
  ./qwen3-vl-2b-instruct_w8a8_rk3588.rkllm \
  2048 4096 3 rk3588 \
  "<|vision_start|>" "<|vision_end|>" "<|image_pad|>"
```

This command was read, not executed. Project acceptance may later reduce `max_new_tokens` to 128-256 after confirming the actual demo argument contract.

Evidence: `audit/gate1a/rkllm/` and the local read-only checkout at `/home/ubt/qwen3-vl-offline-agent/vendor-src/rknn-llm`.
