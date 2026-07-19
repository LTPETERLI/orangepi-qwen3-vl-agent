# Revised Deployment Plan

This revision is based on the read-only audit performed on 2026-07-19. It intentionally stops before backup, installation, download, compilation, or system modification.

## Decisions

1. **Keep the current Ubuntu system?** Yes, provisionally. Keep Ubuntu 20.04.6 vendor image for the first compatibility investigation. Do not reinstall merely to reach Ubuntu 22.04/24.04. Reconsider only if no vendor-supported path can provide a compatible NPU stack.
2. **Keep the current kernel?** Yes, provisionally. Keep `5.10.160-rockchip-rk3588` and do not use `apt` to install a generic kernel. Any later driver remedy must be treated as a vendor image/kernel decision, not an isolated unverified module swap.
3. **Update the RKNPU driver?** An update is likely required because `0.9.6 < 0.9.8`, but no update is authorized yet. First identify the exact Orange Pi vendor image/update that supplies a newer integrated driver and confirm its matching device tree and runtime support. Never replace the built-in driver alone.
4. **Replace RKNN Runtime?** Not system-wide. Preserve `/usr/lib/librknnrt.so`. For official demo validation, use the SDK's application-private runtime and explicit `LD_LIBRARY_PATH`, after verifying its driver requirements.
5. **Replace RKLLM Runtime?** Not system-wide. Preserve `/usr/lib/librkllmrt.so`. Use an application-private, manifest-tracked runtime matching the selected SDK, model format, headers, and demo.
6. **RKLLM 1.3.0 or 1.2.3?** Keep 1.3.0 as the first candidate because the supplied project baseline requires its multimodal path. Do not fall back to 1.2.3 unless the official 1.3.0 demo proves incompatible and the complete 1.2.3 stack is evaluated as one versioned unit.
7. **Official model or self-conversion?** Official pre-converted Qwen3-VL-2B model and official AArch64 demo first. Self-conversion remains blocked until the official demo passes on a supported driver/runtime combination.
8. **D455 backend choice?** The project camera is D455. Use a user-space RSUSB/libuvc-oriented librealsense build as the first candidate. Keep business interfaces model-neutral (`RealSenseCamera`, `RealSenseManager`, `realsense_config`). Do not install `librealsense2-dkms`, patch the kernel, or update camera firmware.
9. **Cross-compilation or native compilation?** Use host cross-compilation for project C/C++ artifacts when practical, with small board-native builds allowed for official/sample validation. Model conversion remains exclusively on the x86_64 host.
10. **Rollback strategy?** The user waived a whole-disk image because eMMC contains no data requiring preservation. Recovery for system-level failure is a full reflash and documented rebuild. Keep important project documents mirrored on the laptop, use versioned application-private directories, leave system libraries untouched where possible, and track application changes in Git.

## Revised Sequence

### Gate 0: User Decisions

The user must confirm both points before any implementation:

- Use the connected **D455** as the project camera and require a direct `5000M` SuperSpeed connection.
- Approve investigating a vendor-supported RKNPU upgrade path. Whole-disk backup is waived; any destructive action still needs a concrete reflash/rebuild plan and renewed approval.

### Phase 1: Reflash and Rebuild Readiness

Whole-disk backup status: **WAIVED BY USER**. Before destructive work:

1. Mirror all project documents, manifests, audit results, and future source code to the laptop.
2. Identify an exact Orange Pi 5 Pro recovery image, official checksum, writing tool, and boot procedure.
3. Record how to rebuild packages, private runtimes, models, configuration, and services from manifests.
4. Obtain renewed approval before any operation that can erase or unboot the current system.

Recovery: remove application-private files for application-only changes; for kernel, driver, boot, or OS failure, reflash eMMC and rebuild. Preservation of the current installation is not promised.

### Phase 2: Resolve USB and Camera Identity

Gate 1A retest status: **PASS** on 2026-07-19. D455 enumerated directly as `8086:0b5c` at `5000M`; preserve this cable and port arrangement.

1. Connect the D455 directly to the Orange Pi USB 3 port using a verified SuperSpeed cable.
2. Require the camera branch in `lsusb -t` to report `5000M` before SDK work.
3. Re-run USB enumeration and inspect for resets or `error -71` events.
4. Do not update camera firmware.

Rollback: reconnect hardware to its original ports; no software state changes are needed.

### Phase 3: Establish a Supported Rockchip Baseline

1. Identify the exact Orange Pi image/build that provides RKNPU `>=0.9.8` for Orange Pi 5 Pro and its corresponding runtime expectations.
2. Compare this path with the current vendor image; document image source, checksum, release notes, driver, kernel, device tree, and recovery requirements.
3. Prefer a complete vendor-supported image/update path over replacing the built-in NPU module.
4. Present the concrete migration option to the user before applying it.

Recovery: full reflash using the reviewed Orange Pi 5 Pro image, followed by documented rebuild. Existing eMMC contents may be lost.

### Phase 4: Official Qwen3-VL Demo

1. Pin `rknn-llm` release v1.3.0 and verify the actual commit from the official repository at execution time.
2. Obtain official Qwen3-VL-2B vision and W8A8 language models plus the matching AArch64 demo, headers, `librknnrt`, and `librkllmrt`.
3. Store them under an application-private versioned directory; record URLs, dates, sizes, SHA256 hashes, tag, commit, target, quantization, context, and resolution.
4. Verify `file`, `ldd`, hashes, and actual loaded libraries before execution.
5. Run the official fixed-image demo at 448x448, context 4096, 3 NPU cores, and 128-256 output tokens.
6. Require 30 consecutive runs without crash, garbage output, IOMMU fault, or OOM before proceeding.

Rollback: remove or stop using the versioned application directory; system libraries remain unchanged.

### Phase 5: D455 User-Space Capture

1. Select a stable librealsense release after checking official release metadata and ARM64 build requirements.
2. Build/install in an isolated project prefix with RSUSB/user-space backend; avoid DKMS.
3. Start at depth and color 640x480 at 15 FPS.
4. Validate enumeration, timestamps, RGB-depth alignment, reconnect behavior, and at least 30 minutes of continuous capture before the two-hour milestone test.

Rollback: remove the project-private prefix and udev rule added by this phase; no kernel rollback should be necessary.

### Phase 6: Host Conversion Environments

Only if official models are inadequate:

1. Install the approved minimal host prerequisites and Git LFS.
2. Create separate Python 3.10 environments for vision and RKLLM export.
3. Resolve exact versions from official wheel metadata and requirements; run `pip check` and freeze both environments.
4. Export vision to RKNN at 448x448 and language to RKLLM W8A8 using a fully version-matched stack.

Rollback: delete only the named isolated environments and project artifacts; do not alter board system Python.

### Phase 7: Agent Milestones

Proceed in order: official image demo, stable RGB-D capture, camera keyframe to VLM, depth tools, optional detector/depth fusion, push-to-talk offline speech, then offline and endurance tests. Keep VLM models resident, separate depth measurements from visual-language inference, and serialize NPU workloads.

## Required Checks Before Each Phase

- Record goal, commands, modified files, installed versions, tests, unresolved issues, risks, rollback, and gate decision.
- Update `CHANGELOG_DEPLOYMENT.md` and `VERSIONS.lock` only after implementation is approved.
- Never run `apt full-upgrade`, `dist-upgrade`, `do-release-upgrade`, install a generic kernel, replace system Rockchip libraries, or patch camera firmware as an incidental step.

## Current Stop Point

The read-only audit and plan revision are complete. Whole-disk backup is waived. **Stop here and wait for user review.** No system modification or NPU execution begins without a new approval.
