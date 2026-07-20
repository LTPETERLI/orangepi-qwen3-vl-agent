# Post-Kernel Activation Report

## Result

The Orange Pi 5 Pro booted `6.1.99-rknpu098-test1` successfully on the first activation attempt. Wi-Fi and SSH returned at `10.10.24.218`, and the eMMC root and boot partitions mounted read-write.

```text
Linux release: 6.1.99-rknpu098-test1
Board: RK3588S OPi 5 Pro
Root: /dev/mmcblk0p2 (ext4)
Boot: /dev/mmcblk0p1 (vfat)
RKNPU driver: v0.9.8
Modules: 772
```

No NPU model or runtime diagnostic was executed.

## RKNPU

The driver initialized in IOMMU mode and registered DRM `rknpu 0.9.8 20240828`. No IOMMU fault or page fault was observed. Debugfs reports:

```text
RKNPU driver: v0.9.8
```

The log also contains RKNPU MMIO request-region warnings, duplicate debugfs-directory warnings, and a PVTPLL voltage-selection warning. These did not prevent driver initialization, but they remain unresolved until an authorized runtime diagnostic and stability test are completed.

## System State

- Wi-Fi `UBT` is connected at `10.10.24.218/24`.
- The matching module tree is present and a sample module reports the expected AArch64 vermagic.
- Memory availability is normal after boot.
- `dnsmasq.service` failed because port 53 was already in use; Wi-Fi and SSH remain operational.
- Vendor kernel logs also contain display/video OPP regulator, unused PCIe link, touchscreen, audio, and PVTPLL warnings.

The raw evidence is stored in `audit/kernel-deployment/post-activation/POST_ACTIVATION_AUDIT.txt`.

## Acceptance

Boot and driver-version acceptance: **PASS**.

Final runtime and stability acceptance: **PENDING**. Do not promote this kernel to `main` and do not run an NPU model until separately authorized.

## Rollback

The original `6.1.43-rockchip-rk3588` boot files remain in the verified recovery archive. If the running system remains reachable, restore them with:

```bash
sudo /home/orangepi/qwen3_vl_2b_ws/backups/pre-kernel-6.1.99-rknpu098-test1/restore-kernel-6.1.43.sh
```

Review the verification output before rebooting. If the board cannot boot, use the prepared MaskROM recovery workflow.
