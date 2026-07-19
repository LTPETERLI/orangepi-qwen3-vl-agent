## Why

The project needs a reproducible, auditable deployment baseline before Qwen3-VL-2B or RealSense integration can be accepted. Durable artifacts are also required so a new Codex window can recover decisions, versions, gates, and unfinished work without relying on chat history.

## What Changes

- Establish a version-locked host and Orange Pi toolchain manifest.
- Establish a compatible Orange Pi 5 Pro kernel, device-tree, and RKNPU baseline.
- Validate the official RKLLM 1.3.0 multimodal demo before integrating third-party code.
- Establish application-private RKNN/RKLLM runtimes and a user-space RealSense stack.
- Record every gate, artifact hash, test result, rollback path, and approval state in Git.
- Keep all work on `test/environment-setup` until the first complete workflow passes and the user approves promotion.

Non-goals for this change are unattended system flashing, replacing system Rockchip libraries, camera firmware updates, and merging to `main`.

## Capabilities

### New Capabilities

- `deployment-environment`: Reproducible and gated host/board environment for official Qwen3-VL inference and D455 capture.
- `durable-project-context`: Repository-owned context and progress that can be recovered across Codex windows.

### Modified Capabilities

None.

## Impact

This affects host tooling, Orange Pi OS/kernel selection, RKNPU/RKNN/RKLLM compatibility, model provenance, RealSense dependencies, deployment documentation, and Git workflow. Destructive board changes remain blocked until separately approved. Acceptance evidence consists of version output, SHA256 manifests, device identity, validation reports, and repeatable test results.
