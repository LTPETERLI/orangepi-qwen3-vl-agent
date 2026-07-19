# Whole-Disk Backup and Restore Plan

This is a design only. None of the commands below were executed.

Status update: on 2026-07-19 the user waived whole-disk backup because the eMMC contains no project data requiring preservation. This plan is retained as an optional procedure and is not a mandatory gate. The active recovery strategy is full reflash and documented rebuild, with loss of the current system explicitly accepted.

## Preconditions

1. Prepare an Orange Pi 5 Pro-compatible recovery system on a separate SD card or proven USB boot device.
2. Use the laptop as the backup destination. It currently has about **709 GiB free**, enough for an uncompressed 233 GiB image plus hashes and logs.
3. Power off, insert recovery and backup media, then boot the recovery system.
4. Confirm the recovery root device is not `/dev/mmcblk1`.
5. Confirm `/dev/mmcblk1`, `p1`, `p2`, `boot0`, and `boot1` are not mounted.
6. Connect the recovery system and laptop to a stable local network, preferably wired Ethernet.
7. Reconfirm source and destination identities by size, serial, and path. A reversed source/destination destroys data.

## Safety Check

```bash
findmnt /
lsblk -o NAME,SIZE,TYPE,FSTYPE,MOUNTPOINT,MODEL,SERIAL
findmnt -rn -S /dev/mmcblk1
sudo umount /dev/mmcblk1p1 /dev/mmcblk1p2 2>/dev/null || true
findmnt -rn -S /dev/mmcblk1
```

Proceed only when the last command prints nothing and `findmnt /` points to the separate recovery medium.

## Backup to the Laptop

Create the destination on the laptop. This is a future execution command, not an action performed during planning:

```bash
BACKUP_DIR=/home/ubt/orangepi_backups/orangepi5pro-20260719
mkdir -p "$BACKUP_DIR"
df -h "$BACKUP_DIR"
```

From the laptop, pull the raw eMMC stream over SSH. Replace `RECOVERY_IP` and `RECOVERY_USER` with the separately booted recovery system values. The recovery user's password is read without echo and passed only to remote `sudo`; `dd` reads `/dev/mmcblk1`, not standard input:

```bash
read -rsp 'Recovery sudo password: ' RECOVERY_SUDO_PASSWORD; echo
printf '%s\n' "$RECOVERY_SUDO_PASSWORD" | \
  ssh RECOVERY_USER@RECOVERY_IP \
    'sudo -S dd if=/dev/mmcblk1 bs=16M iflag=fullblock status=none' | \
  dd of="$BACKUP_DIR/mmcblk1.img" bs=16M status=progress conv=fsync
unset RECOVERY_SUDO_PASSWORD
```

The command is valid only after `findmnt /` on the Orange Pi proves that the recovery system is not running from `mmcblk1` and all eMMC partitions are unmounted. If SSH disconnects or either command exits nonzero, discard the incomplete image and repeat; do not accept a short image.

## Progress Monitoring

- The laptop-side GNU `dd` displays received bytes through `status=progress`.
- In another laptop terminal, verify destination growth with `watch -n 5 ls -lh "$BACKUP_DIR/mmcblk1.img"`.
- Monitor network stability and laptop free space during the transfer. Disable suspend until completion.

## Integrity and Read Verification

```bash
ssh RECOVERY_USER@RECOVERY_IP 'sudo blockdev --getsize64 /dev/mmcblk1' \
  | tee "$BACKUP_DIR/source-size.txt"
stat -c '%s' "$BACKUP_DIR/mmcblk1.img" | tee "$BACKUP_DIR/image-size.txt"
ssh RECOVERY_USER@RECOVERY_IP 'sudo sha256sum /dev/mmcblk1' \
  | tee "$BACKUP_DIR/source-device.sha256"
sha256sum "$BACKUP_DIR/mmcblk1.img" | tee "$BACKUP_DIR/mmcblk1.img.sha256"
```

Accept only equal byte sizes and identical source/image SHA256 values. The image and hashes live on the laptop, never on `mmcblk1`. A second physical copy remains strongly recommended before any destructive system migration, but it is not available in the current hardware plan.

## Restore

Boot the separate recovery system again, leave all `mmcblk1` partitions unmounted, and obtain explicit user confirmation before writing. Stream the verified laptop image back over SSH:

```bash
sha256sum -c "$BACKUP_DIR/mmcblk1.img.sha256"
ssh RECOVERY_USER@RECOVERY_IP \
  'findmnt /; findmnt -rn -S /dev/mmcblk1; lsblk -o NAME,SIZE,TYPE,FSTYPE,MOUNTPOINT,MODEL,SERIAL'
read -rsp 'Recovery sudo password: ' RECOVERY_SUDO_PASSWORD; echo
cat "$BACKUP_DIR/mmcblk1.img" | \
  ssh RECOVERY_USER@RECOVERY_IP \
    "sudo -S sh -c 'dd of=/dev/mmcblk1 bs=16M oflag=direct status=progress conv=fsync'" \
  <<< "$RECOVERY_SUDO_PASSWORD"
unset RECOVERY_SUDO_PASSWORD
```

The exact restore transport command must be revalidated before execution because password input and binary image input must use separate channels. Prefer a recovery root login, a temporary restricted SSH key, or a root-owned restore script rather than combining both streams. Do not run the illustrative restore pipeline without a new review.

Power off cleanly, remove recovery media, and boot from eMMC. Verify hostname, OS, kernel, partitions, `/boot`, root UUID, SSH, NPU driver version, and audit/report hashes.

## Risks and Manual Confirmations

- User must identify `/dev/mmcblk1` and the external destination before backup.
- User must confirm the board is not running from `mmcblk1`.
- User must confirm laptop capacity, stable networking, stable power/cooling, and disabled suspend.
- Only one physical backup location is currently planned; this increases risk and must be acknowledged before destructive changes.
- User must explicitly authorize the restore write command.
- Backup does not by itself prove bootability; a controlled restore or spare-media test is the final proof.
