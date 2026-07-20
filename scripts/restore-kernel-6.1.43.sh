#!/usr/bin/env bash
set -euo pipefail

backup_dir="${1:-/home/orangepi/qwen3_vl_2b_ws/backups/pre-kernel-6.1.99-rknpu098-test1}"
archive="$backup_dir/boot-6.1.43-backup.tar.gz"
archive_manifest="$backup_dir/ARCHIVE_SHA256"
boot_manifest="$backup_dir/SHA256SUMS.absolute"

if [[ $EUID -ne 0 ]]; then
  echo "Run this script as root." >&2
  exit 1
fi

for path in "$archive" "$archive_manifest" "$boot_manifest"; do
  [[ -f "$path" ]] || { echo "Missing recovery file: $path" >&2; exit 1; }
done

expected_sha256="$(awk 'NR == 1 { print $1 }' "$archive_manifest")"
actual_sha256="$(sha256sum "$archive" | awk '{ print $1 }')"
[[ "$actual_sha256" == "$expected_sha256" ]] || {
  echo "Recovery archive SHA256 mismatch." >&2
  exit 1
}

tar -tzf "$archive" >/dev/null
tar -xzf "$archive" -C /
sync
sha256sum -c "$boot_manifest"

echo "Original 6.1.43 boot files restored. Review the output, then reboot manually."
