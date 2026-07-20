# Kernel Deployment Preparation Report

## State

Preparation for `6.1.99-rknpu098-test1` is complete. The active boot selection has not changed and the board is still running `6.1.43-rockchip-rk3588`.

## Recovery Point

The current boot files are backed up on the board at:

```text
/home/orangepi/qwen3_vl_2b_ws/backups/pre-kernel-6.1.99-rknpu098-test1/
```

A verified copy also exists on the development laptop under:

```text
audit/kernel-deployment/pre-kernel-6.1.99-rknpu098-test1/
```

The recovery archive SHA256 is:

```text
18aa73356e15fafdb7b9a3f5fb7aa41d74bea86342e3998e267ed7588d03b1d6
```

The archive was listed successfully after transfer. `scripts/restore-kernel-6.1.43.sh` verifies the archive, restores the original boot files, verifies them again, and deliberately does not reboot.

## Prepared Files

- 772 modules installed under `/lib/modules/6.1.99-rknpu098-test1/` with a valid `depmod` index.
- `/boot/Image-6.1.99-rknpu098-test1`
- `/boot/vmlinuz-6.1.99-rknpu098-test1`
- `/boot/config-6.1.99-rknpu098-test1`
- `/boot/System.map-6.1.99-rknpu098-test1`
- `/boot/initrd.img-6.1.99-rknpu098-test1`
- `/boot/uInitrd-6.1.99-rknpu098-test1`
- `/boot/dtb-6.1.99-rknpu098-test1/rockchip/rk3588s-orangepi-5-pro.dtb`

The initramfs was generated with `mkinitramfs`, not `update-initramfs`. The vendor `/etc/initramfs/post-update.d/99-uboot` hook would otherwise replace the fixed `/boot/uInitrd` during preparation.

## Boot Readiness

The new configuration keeps initramfs, devtmpfs, MMC, Rockchip DesignWare MMC, DWCMSHC, EXT4, and RKNPU built into the kernel. The generated initramfs contains modules for the exact test release.

The fixed files `/boot/Image`, `/boot/uInitrd`, `/boot/dtb/rockchip/rk3588s-orangepi-5-pro.dtb`, `/boot/boot.cmd`, `/boot/boot.scr`, and `/boot/orangepiEnv.txt` still match the pre-deployment SHA256 manifest.

## Remaining Gate

Activation requires replacing the three fixed files loaded by the current U-Boot script. No automatic fallback entry has been demonstrated. Do not activate or reboot without explicit approval and a ready MaskROM recovery path.
