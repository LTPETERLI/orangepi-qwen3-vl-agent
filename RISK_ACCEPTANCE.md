# Risk Acceptance Record

Date: 2026-07-19

## Whole-Disk Backup Waiver

The user confirmed that the Orange Pi eMMC currently contains no project data that needs to be preserved and elected not to create a whole-disk image before later system-level work.

Accepted consequences:

- A failed kernel, driver, device-tree, boot, or image operation may make the current system unbootable.
- The existing Ubuntu installation, configuration, accounts, packages, audit files, and workspace may be lost.
- Recovery may require booting from SD/USB and completely reflashing eMMC.
- Recovery means rebuilding from documented source files and manifests; it does not mean restoring the current system byte-for-byte.
- Files that matter must remain mirrored on the laptop before any destructive action.

This waiver removes the whole-disk backup as a mandatory project gate. It does **not** authorize flashing, driver replacement, kernel changes, NPU model execution, or any other currently forbidden operation. Each destructive action still requires a concrete, reviewed plan and renewed user approval.

Status: `ACCEPTED_BY_USER`
