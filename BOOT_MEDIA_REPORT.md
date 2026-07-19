# Boot Media Report

## Determination

```text
Media type: eMMC
Current boot media: yes
Safe to create a whole-disk image while booted from it: no
Recommended recovery boot media: SD card; USB recovery media is the fallback
```

## Evidence

- `/` is mounted from `/dev/mmcblk1p2` and `/boot` from `/dev/mmcblk1p1`.
- `udevadm` reports `MMC_TYPE=MMC`, `ID_DRIVE_FLASH_MMC=1`, and device name `A3V014`.
- `/sys/block/mmcblk1/removable` is `0` and device type is `MMC`.
- `mmc extcsd read` succeeds and reports MMC 5.1, eMMC boot partitions, and a 233 GiB user area.
- `/dev/mmcblk1boot0` and `/dev/mmcblk1boot1` are present.

These signals distinguish the device from a removable SD card with sufficient confidence. Although the eMMC boot partition enable field is not set, the active Linux `/boot` and root filesystems are both on `mmcblk1`; therefore the running system is actively using this eMMC and must not image itself as the authoritative backup source.

## Recovery Choice

Use a separate SD card containing an Orange Pi 5 Pro-compatible recovery Linux image. Boot it and confirm that its root device is not `mmcblk1`. USB boot media is an alternative only after its boot path has been tested on this exact board. The backup destination must be a different physical device or the host computer.

Evidence: `audit/gate1a/boot-media/`.
