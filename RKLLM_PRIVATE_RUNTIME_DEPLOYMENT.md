# RKLLM 1.3.0 Private Runtime Deployment

## Source Identity

```text
Repository: https://github.com/airockchip/rknn-llm.git
Tag: release-v1.3.0
Commit: 878f9361fd3afa7e167b7079918918f78d2c1c2a
```

The repository was clean and checked out at the exact release tag.

## Board Location

The runtime is deployed without privilege under:

```text
/home/orangepi/qwen3_vl_2b_ws/runtime/rkllm-1.3.0/
```

It contains application-private libraries, API headers, official multimodal demo source, its README, build metadata, and the official fixed demo image. No model files and no compiled demo executable are present.

## Runtime Hashes

```text
6a9e4fc5324c68921c3a900340361e107af7599fe34dc8fa7759b2c5ae22a6e6  lib/librkllmrt.so
d31fc19c85b85f6091b2bd0f6af9d962d5264a4e410bfb536402ec92bac738e8  lib/librknnrt.so
80596a578f7f8e70df6eda1c2cbead3bfced14623a190258f2bd009a3d1f72cf  include/rkllm.h
```

Both libraries are AArch64 ELF64 shared objects. All `NEEDED` libraries resolve on the board, including `libgomp.so.1` for RKLLM.

## Isolation

The system runtimes were not replaced. Their hashes remained unchanged before and after deployment:

```text
0ebc1b408f897863a91a1b9ed60f3838a801386c7b1ef7c54d55ead624cd8347  /usr/lib/librknnrt.so
42c68f4cf840b4c83ecc0a956361a73ff91d90acf5f4464323bcb95d810af1eb  /usr/lib/librkllmrt.so
```

Use the explicit environment file only for selected application commands:

```bash
source /home/orangepi/qwen3_vl_2b_ws/runtime/rkllm-1.3.0/rkllm-1.3.0-env.sh
```

No shell profile, linker configuration, `/usr/lib`, or system environment was modified.

## Remaining Work

The official demo CMake file expects its original repository-relative RKLLM and OpenCV layout. Compilation therefore remains blocked until the build dependency layout is deliberately staged or adapted. No compilation, runtime API call, model download, or model execution was performed in this phase.
