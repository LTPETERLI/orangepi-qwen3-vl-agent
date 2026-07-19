# Orange Pi Offline Agent Compatibility Report

Audit time: 2026-07-19T15:01:51+08:00  
Host: `ubt-ASUS-EXPERTBOOK-B1403CVA-P1463CVA`  
Board: `orangepi5pro` (`192.168.31.89`)  
Board workspace: `/home/orangepi/qwen3_vl_2b_ws`

## Executive Summary

The hardware and host are suitable for the project, but deployment must not begin yet. Three blocking compatibility gaps were found:

1. The board RKNPU driver is `0.9.6`, below the project baseline of `0.9.8`.
2. The project camera is **Intel RealSense D455** and is connected at USB 2.0 (`480M`) instead of SuperSpeed (`5000M`).
3. System `librknnrt.so` and `librkllmrt.so` exist, but their SDK/runtime versions cannot be established reliably from the binaries. They must not be overwritten or used as proof of 1.3.0 compatibility.

No package installation, upgrade, driver replacement, firmware update, or kernel modification was performed.

## Compatibility Matrix

| Item | Actual value | Recommended value | Status | Risk | Recommendation |
|---|---|---|---|---|---|
| Board model | Orange Pi 5 Pro | Orange Pi 5 Pro | PASS | Low | Keep board |
| SoC | Device tree: `RK3588S OPi 5 Pro` | RK3588S, conversion target `rk3588` | PASS | Low | Use `rk3588` as toolchain target |
| Memory | 15 GiB visible, 13 GiB available | 16 GB nominal, at least 2 GB reserved | PASS | Medium | Enforce 14 GB process ceiling during tests |
| Board storage | 233 GB `mmcblk1`; root has 201 GB free | Sufficient local storage | PASS | Medium | Confirm eMMC versus SD physically before imaging |
| Board OS | Ubuntu 20.04.6 vendor image | Prefer Ubuntu 22.04/24.04 vendor image | CONDITIONAL | Medium | Retain initially because vendor kernel/NPU integration matters more; validate official demo before considering migration |
| Kernel | `5.10.160-rockchip-rk3588`, vendor build 1.0.6 | Vendor-compatible Rockchip kernel | CONDITIONAL | High | Do not upgrade; preserve while evaluating driver/runtime compatibility |
| RKNPU driver | Built-in `0.9.6` | At least `0.9.8` | FAIL | High | Do not run the target model as an accepted baseline; investigate an official vendor image/update path only after full backup |
| NPU initialization | NPU initialized with IOMMU; no persistent IOMMU fault observed | Device initialized without persistent faults | CONDITIONAL | Medium | Validate with version-matched official demo after backup |
| `librknnrt` | `/usr/lib/librknnrt.so`, AArch64, SHA256 `0ebc1b408f897863a91a1b9ed60f3838a801386c7b1ef7c54d55ead624cd8347` | Runtime paired with selected RKNN SDK/model | UNKNOWN | High | Preserve the file; identify provenance or use an application-private version later |
| `librkllmrt` | `/usr/lib/librkllmrt.so`, AArch64, SHA256 `42c68f4cf840b4c83ecc0a956361a73ff91d90acf5f4464323bcb95d810af1eb` | RKLLM runtime 1.3.0 paired with toolkit/model/header/demo | UNKNOWN | High | Preserve the file; do not infer version from embedded strings |
| Host OS | Ubuntu 22.04.5 LTS x86_64 | Ubuntu 22.04 x86_64 | PASS | Low | Use as conversion host |
| Host memory | 30 GiB RAM, 2 GiB swap; only 11 GiB currently available | 32 GB suggested plus adequate swap | CONDITIONAL | Medium | Close memory-heavy applications; increase swap only in the approved implementation phase if conversion requires it |
| Host disk | 710 GB free | At least 30 GB free | PASS | Low | Sufficient for source models and artifacts |
| Host GPU/CUDA | Intel iGPU; no NVIDIA driver or CUDA | CUDA optional | CONDITIONAL | Medium | Plan CPU export unless a separate CUDA machine is selected; expect longer conversion time |
| Host Python | Python 3.10.12; no pip/Conda currently on PATH | Two isolated Python 3.10 environments | CONDITIONAL | Low | Install environment tooling only after plan approval |
| Host build tools | Git 2.34.1; GCC, G++, CMake, Git LFS and AArch64 cross compiler absent | Version-matched build/export tools | CONDITIONAL | Low | Install only the approved minimal set later |
| `rknn-toolkit2` | Not installed/audited | Candidate 2.3.2 | UNKNOWN | Medium | Select from official matching package metadata after approval |
| `rkllm-toolkit` | Not installed/audited | Candidate 1.3.0 | UNKNOWN | High | Keep toolkit/runtime/model/header/demo on one SDK version |
| Transformers | Not installed on host | Candidate 4.57.0 | UNKNOWN | Medium | Resolve from official requirements in isolated environment |
| Camera model | USB ID `8086:0b5c`: RealSense D455 | RealSense D455 | PASS | Low | Keep hardware-specific identity in configuration; use generic RealSense names in business interfaces |
| Camera USB speed | Camera behind hubs at `480M`; SuperSpeed root bus is unused | `5000M` SuperSpeed | FAIL | High | Move D455 to the board USB 3 port with a verified USB 3 cable, without a USB 2 hub |
| Camera V4L2 | Six video nodes, depth Z16 enumerates | Stable RGB-D capture | CONDITIONAL | Medium | Enumeration works, but streaming and synchronization are untested |
| librealsense | `rs-enumerate-devices` absent; no package detected | Stable user-space backend | UNKNOWN | Medium | Prefer RSUSB/user-space build; avoid DKMS and firmware update |
| Audio input | ES8388 ALSA capture and PulseAudio source enumerate | PCM capture verified | CONDITIONAL | Medium | Run a short record/playback acceptance test after approval |
| Audio output | HDMI/DP/ES8388 ALSA devices and PulseAudio sinks enumerate | PCM playback verified | CONDITIONAL | Low | Select ES8388 or intended output and test later |
| Board Python | Python 3.8.10, pip 20.0.2 | Board runtime isolated from host conversion environments | CONDITIONAL | Medium | Do not use board Python for model conversion; decide runtime environment after demo validation |
| Thermal/power idle state | About 48-49 C; no undervoltage/throttling message observed | Stable cooling and power | PASS | Medium | Recheck under NPU and camera load |

## Evidence and Risks

### NPU

The built-in driver reports `version: 0.9.6`. Boot logs show successful IOMMU mode and driver initialization, plus power-model/debugfs warnings. There is no observed repeating IOMMU fault, but this does not override the project requirement of driver `>=0.9.8`.

### Camera

The connected unit is unambiguously a D455 (`8086:0b5c`). It is routed through a USB 2.0 hub and enumerates at `480M`. V4L2 exposes `/dev/video0` through `/dev/video5`, but `librealsense` tools are absent and no RGB-depth stability test has been run.

Business-facing camera interfaces must use the generic names `RealSenseCamera`, `RealSenseManager`, and `realsense_config`; the D455 identity belongs in hardware configuration and manifests rather than interface names.

### Runtime Libraries

Only one system copy of each Rockchip runtime was found. Both are AArch64 ELF libraries. The library hashes are recorded above, but no package provenance, manifest, matching headers, or trustworthy version metadata was found. Loading a Qwen3-VL model against these files would therefore be an uncontrolled compatibility test.

### Existing Workspace

The board workspace existed and contained `main.cpp`; the file is empty (SHA256 of an empty file). No implementation was overwritten.

## Audit Artifacts

- Host audit: `audit/host`, `audit/usb`
- Mirrored board audit: `audit/board`
- Board-native audit: `/home/orangepi/qwen3_vl_2b_ws/audit`

## Gate Decision

**M0 is not yet approved for implementation.** The audit and revised plan are complete, but the user must first confirm the camera identity change and approve `REVISED_PLAN.md`. No installation phase is authorized.
