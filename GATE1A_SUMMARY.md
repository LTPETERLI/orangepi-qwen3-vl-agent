# Gate 1A Summary

## 1. D455 USB 3 Status

**Yes.** After manual reconnection, device `8086:0b5c` and six video nodes enumerate directly on Bus 006 at `5000M`. No D455 USB reset loop, `error -71`, or `error -110` was observed.

## 2. Boot Media

`mmcblk1` is a 233 GiB **eMMC** device (MMC 5.1, non-removable, boot0/boot1 present). It supplies both `/boot` and `/`, so it is the current system medium.

## 3. Safe Whole-Disk Backup

**WAIVED BY USER.** The eMMC contains no project data requiring preservation. If system-level work fails, recovery will use a complete Orange Pi 5 Pro reflash and documented rebuild. Important documents remain mirrored on the laptop. `BACKUP_RESTORE_PLAN.md` is retained as an optional procedure.

## 4. RKLLM 1.3.0 Identity

**PASS.** `release-v1.3.0` resolves to `878f9361fd3afa7e167b7079918918f78d2c1c2a`; short hash `878f936` matches the plan. Files and hashes are recorded in `RKLLM_1_3_0_MANIFEST.md`. Nothing was installed, built, converted, or executed.

## 5. Official Orange Pi Upgrade Path

**UNKNOWN.** Official Orange Pi 5 Pro Ubuntu image families exist, but no accessible official evidence binds a concrete image and checksum to the Pro DTB and RKNPU `>=0.9.8`. No flash recommendation is made.

## 6. Still Forbidden

- Installing or upgrading packages, kernel, drivers, runtimes, or firmware.
- Replacing `/usr/lib/librknnrt.so` or `/usr/lib/librkllmrt.so`.
- Running an NPU model or any model conversion/build script.
- Executing `dd`/`ddrescue`, flashing an image, or modifying boot configuration.
- Updating D455 firmware or installing DKMS.
- Modifying `main.cpp` or creating business implementation code.

```yaml
rknpu:
  actual_version: "0.9.6"
  linkage: "built-in"
  final_acceptance: "FAIL"
  diagnostic_test_allowed: false
  upgrade_authorized: false
```

## 7. Next Recommendation

1. Preserve the verified direct D455 `5000M` cable and port arrangement.
2. Identify but do not yet write a compatible Orange Pi 5 Pro recovery image and record its official checksum and flashing procedure.
3. Obtain exact official Orange Pi 5 Pro image manifest/driver evidence or written vendor confirmation for RKNPU `>=0.9.8`.
4. Request a new approval before flashing, runtime diagnostics, or any NPU operation.

## 8. Next-Phase Readiness

**CONDITIONAL / NOT YET AUTHORIZED.** The USB3 hardware gate passes and the whole-disk backup gate is waived with data-loss risk accepted. Entry remains blocked because the Orange Pi 5 Pro vendor path for RKNPU `>=0.9.8` is unresolved. No flashing, driver change, private-runtime diagnostic, or NPU model run is authorized.

Gate 1A stops here for user review.
