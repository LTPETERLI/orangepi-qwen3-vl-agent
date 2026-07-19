# USB 3 Retest Report

Initial audit: `2026-07-19T15:43:30+08:00`  
Successful retest: `2026-07-19T21:16:08+08:00`

## Result

**PASS**: after manual reconnection, the RealSense D455 enumerates directly on the USB 3 controller at the required `5000M`.

| Check | Actual | Required | Status |
|---|---|---|---|
| Device ID | `8086:0b5c` | `8086:0b5c` | PASS |
| Camera model | RealSense D455 | RealSense D455 | PASS |
| USB speed | Direct Bus 006 SuperSpeed Gen 1, all interfaces `5000M` | Direct `5000M` | PASS |
| Video nodes | `/dev/video0` through `/dev/video5` | Normal enumeration | PASS |
| Persistent D455 reset loop | Not observed | None | PASS |
| D455 `error -71` / `error -110` | Not observed | None | PASS |

The successful retest shows the old bus 1 D455 disconnect followed by a single SuperSpeed Gen 1 enumeration on bus 6. No D455 reset loop, `error -71`, or `error -110` appears. An `error -110` near boot belongs to the Wi-Fi/Bluetooth SDIO probe, not USB or D455.

## Connection Baseline

1. Keep D455 directly connected to this Orange Pi 5 Pro USB 3 port.
2. Keep the verified SuperSpeed cable and avoid hubs/adapters.
3. Do not update camera firmware.
4. Recheck `lsusb -t` after future cable or port changes.

The USB hardware gate is satisfied. This does not authorize camera SDK installation, NPU execution, backup, or flashing.

Evidence: initial `audit/gate1a/usb3/`; successful retest `audit/gate1a/usb3-retest-2/`.
