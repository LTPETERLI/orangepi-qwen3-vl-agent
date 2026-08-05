# RealSense RSUSB Baseline Report

Date: 2026-08-05

## Pinned Stack

```yaml
librealsense:
  version: "2.56.5"
  source: "https://github.com/realsenseai/librealsense.git"
  commit: "38a41441971387197193ad3aeae3cefe6a11f2cb"
  git_archive_sha256: "babf60fe36d260b56f23d80b3abb5039c4119fc80b74afdb4205fc8507481125"
  backend: "RSUSB"
  prefix: "/home/orangepi/qwen3_vl_2b_ws/runtime/librealsense-2.56.5-rsusb"
nlohmann_json:
  version: "3.11.3"
  commit: "9cca280a4d0ccf0c08f47a99aa71d1b0e52f8d03"
  git_archive_sha256: "758268d6b3c0fdcba8ec1d6f0ff35a59b2453f6c468ba4c075a42964d065472b"
libusb_dev:
  version: "2:1.0.25-1ubuntu2"
```

The build disables examples, graphical examples, tools, Python bindings, unit
tests, DDS, and CUDA. `FORCE_RSUSB_BACKEND=ON` is set. A recorded build patch,
`patches/librealsense-2.56.5-offline-json.patch`, makes CMake consume the pinned
JSON source instead of cloning it during configuration.

No DKMS package, kernel patch, camera firmware update, or system librealsense
library was installed. The recommended firmware resource compiled by the SDK
was not executed. The observed camera firmware remains `5.15.1.55`.

## Runtime Artifacts

```yaml
librealsense2_sha256: "34f7990a001c300247b9838c4e9a8d64a337351bf4fa6fcf8d275a8bb851d4c9"
camera_check_sha256: "de868841303146f66e8d7c3b926195debaf832b8e713493ce161fa7bab90aa87"
udev_rule_sha256: "21dd6bc40a117c9bb9dc93d78c9f77f2f74fae9f9eba75aa4714b25939d7532a"
```

The official `99-realsense-libusb.rules` file is installed at
`/etc/udev/rules.d/99-realsense-libusb.rules`. It is required because the
original USB node allowed non-root users to read but not control the camera.

## Validation

The project-owned `RealSenseCamera` check uses model-neutral naming and the
private RSUSB library.

First capture:

```text
camera: Intel RealSense D455
serial: 254722302521
USB descriptor: 3.2
color: 640x480@15
depth: 640x480@15
aligned frames: 150/150
timestamp regressions: 0
valid center depth frames: 150/150
average center depth: 3.95299 m
exit code: 0
```

Reconnect capture:

```text
aligned frames: 30/30
timestamp regressions: 0
valid center depth frames: 30/30
average center depth: 3.95667 m
exit code: 0
```

After each RSUSB process exits, kernel `uvcvideo` reattaches and `lsusb -t`
again reports `5000M`. No USB reset, error `-71`, or error `-110` was observed.
The kernel prints unsupported UVC format notices during reattachment; these did
not prevent RSUSB capture.

## Remaining Gate

The user shortened the continuous capture test due to a 22:10 time limit. It
ran for 451 seconds before a user-requested termination. The full 30-minute and
physical unplug/replug checks are therefore waived for this session rather than
claimed as complete.

A subsequent aligned keyframe capture succeeded at 22:08:46. The RGB PPM has
SHA256 `96e665dfb38ca079c5e5143b338f20942472d7b601237d3511ed84060d0ee4d3`
and its aligned center depth was `1.173 m`.
