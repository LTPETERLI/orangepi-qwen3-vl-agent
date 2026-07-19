# Hardware Baseline

The physical camera is RealSense D455. Business-facing interfaces must remain model-neutral and use `RealSenseCamera`, `RealSenseManager`, and `realsense_config`; do not encode `D455` in business interface names.

```yaml
camera:
  manufacturer: RealSense
  model: D455
  usb_vendor_id: "8086"
  usb_product_id: "0b5c"
  required_usb_speed: "5000M"
  initial_stream:
    color: "640x480@15"
    depth: "640x480@15"
  firmware_update_allowed: false

rknpu:
  actual_version: "0.9.6"
  linkage: "built-in"
  final_acceptance: "FAIL"
  diagnostic_test_allowed: false
  upgrade_authorized: false
```

RKNPU 0.9.6 is not an acceptable final stable-delivery baseline. No standalone driver replacement is permitted. After a verified full-disk backup, a version-matched private runtime may be proposed for an official-model diagnostic, but no NPU model may run without renewed user approval.
