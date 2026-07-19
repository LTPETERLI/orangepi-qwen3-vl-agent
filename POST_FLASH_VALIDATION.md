# Post-Flash Validation

Date: 2026-07-20
Target: Orange Pi 5 Pro eMMC at `192.168.31.89`

## Result

The official Orange Pi 1.0.6 Jammy Desktop Linux 6.1.43 image boots successfully from the 233 GiB eMMC and exposes SSH. Board, OS, kernel, boot media, and RKNPU identity were verified read-only.

| Check | Observed | Result |
|---|---|---|
| Device tree model | `RK3588S OPi 5 Pro` | PASS |
| OS | Orange Pi 1.0.6, Ubuntu 22.04.5 LTS Jammy | PASS |
| Kernel | `6.1.43-rockchip-rk3588`, build `#1.0.6` dated 2024-11-13 | PASS |
| Root filesystem | `/dev/mmcblk0p2`, ext4, 229.6 GiB | PASS |
| Boot filesystem | `/dev/mmcblk0p1`, vfat, 1 GiB | PASS |
| Physical boot media | 233 GiB eMMC with boot0/boot1 nodes | PASS |
| SSH | OpenSSH 8.9p1; address `192.168.31.89` | PASS |
| RKNPU | Built-in `0.9.6`, initialized with IOMMU | FAIL for final baseline |
| RKNPU kernel config | `CONFIG_ROCKCHIP_RKNPU=y`, debugfs and DRM GEM enabled | BUILT-IN CONFIRMED |
| Device tree file | `/boot/dtb-6.1.43-rockchip-rk3588/rockchip/rk3588s-orangepi-5-pro.dtb` | PASS |
| Board Python | Python 3.10.12; pip not installed | CONDITIONAL |
| System RKNN runtime | SHA256 `0ebc1b408f897863a91a1b9ed60f3838a801386c7b1ef7c54d55ead624cd8347` | PRESERVED |
| System RKLLM runtime | SHA256 `42c68f4cf840b4c83ecc0a956361a73ff91d90acf5f4464323bcb95d810af1eb` | PRESERVED |

## Findings

1. The official image confirms the manual: release 1.0.6 still provides RKNPU `0.9.6`, so RKNPU `0.9.8` must be integrated into a version-matched Orange Pi 5 Pro Linux 6.1 kernel.
2. The kernel reports `GPT: Use GNU Parted to correct GPT errors.` The image partitions were expanded to the eMMC capacity, but the backup GPT metadata requires inspection and repair before kernel deployment work.
3. RKNPU initializes in IOMMU mode without an observed IOMMU fault. Resource-region and duplicate-debugfs warnings exist and must be compared before/after the 0.9.8 kernel build.
4. Display/video OPP, absent PCIe link, OP-TEE, touchscreen, and Wi-Fi/BT firmware warnings are present. They do not prevent the current boot or SSH validation, but they remain baseline evidence rather than accepted fixes.
5. The image does not expose a matching kernel package through `dpkg-query`; exact source commit matching must therefore be established from the Orange Pi build source/history and the captured kernel config rather than inferred from a Debian package version.

## Gate

The official Linux 6.1 migration is complete. Kernel/RKNPU integration remains blocked until GPT metadata is inspected, the exact Orange Pi 5 Pro kernel source/config/DTB baseline is pinned, a recovery procedure is retained, and the user approves installing a test kernel.

No model was run, no system Rockchip library was replaced, and no RKNPU modification was performed during this validation.
