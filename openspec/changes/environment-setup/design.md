## Context

The current Orange Pi 5 Pro runs Ubuntu 20.04.6 with kernel 5.10.160 and built-in RKNPU 0.9.6. The D455 USB 3 gate has passed. Official RKLLM 1.3.0 source identity is verified, while the target model and an accepted RKNPU 0.9.8 board baseline are not yet established.

## Goals / Non-Goals

**Goals:** preserve durable context, pin all dependencies, reach an official multimodal demo baseline, validate D455 capture, and keep changes reversible.

**Non-goals:** automatic flashing, isolated replacement of a built-in NPU driver, system-wide runtime replacement, firmware updates, or premature application integration.

## Decisions

1. Git is the durable record. `main` is formal-only; `test/environment-setup` carries all pre-acceptance work.
2. `environment/toolchain.yaml` is the single machine-readable version and dependency manifest.
3. OpenSpec holds stable requirements and active change artifacts. `AGENTS.md` defines session recovery order.
4. Official Rockchip artifacts are validated before the Qengineering submodule is treated as an integration candidate.
5. RKNN, RKLLM, and librealsense deploy into versioned project-private prefixes.
6. Kernel/RKNPU changes require Orange Pi 5 Pro-specific boot assets and explicit user approval.

## Risks / Trade-offs

- A vendor Linux 6.1 image may still contain an older RKNPU driver; filename age is not evidence.
- Building RKNPU 0.9.8 into a kernel increases recovery risk and requires matching device-tree and boot artifacts.
- Third-party pre-converted models reduce conversion effort but add provenance and compatibility uncertainty.
- Skipping a full-disk backup accepts reinstallation risk; project state remains preserved on the laptop and GitHub.

## Rollback

Application-only work rolls back by selecting or deleting a versioned private prefix. Kernel, device-tree, bootloader, or image changes roll back by booting recovery media and reflashing a verified Orange Pi 5 Pro image. Git changes roll back through normal commits; `main` is not force-updated.

## Open Questions

- Which official Orange Pi 5 Pro image or reproducible kernel build provides RKNPU 0.9.8 or newer?
- What are the official model download hashes and exact matching RKNN runtime?
- Which librealsense release will be pinned for RSUSB validation?
