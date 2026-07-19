# Kernel Build Report

## Result

The Orange Pi 5 Pro test kernel was cross-compiled successfully on the x86_64 development laptop. Nothing was installed on the Orange Pi eMMC.

| Item | Result |
| --- | --- |
| Kernel release | `6.1.99-rknpu098-test1` |
| Source branch | `orange-pi-6.1-rk35xx` |
| Source commit | `232ed4b97b65da2b7b647c4e3c496f8594b9f3f1` |
| Compiler | Arm GNU `11.2.1 20220111` |
| Target | AArch64, Orange Pi 5 Pro (`RK3588S`) |
| RKNPU driver | Built-in `0.9.8` |
| Modules | 772 |
| Deployment | Not performed; not authorized |

## Build Method

The board's running `6.1.43-rockchip-rk3588` configuration was used as the starting point. The test build sets:

```text
CONFIG_LOCALVERSION="-rknpu098-test1"
CONFIG_LOCALVERSION_AUTO=n
CONFIG_ROCKCHIP_RKNPU=y
CONFIG_ROCKCHIP_RKNPU_DEBUG_FS=y
CONFIG_ROCKCHIP_RKNPU_DRM_GEM=y
```

An initial out-of-tree (`O=...`) build was rejected because vendor GPU and Wi-Fi makefiles resolve some source-relative firmware and include paths from the output tree. All referenced files were present and tracked. The successful build therefore follows the Orange Pi manual's in-tree method while retaining the pinned compiler, source commit, and isolated release name.

## Artifacts

Artifacts are staged locally under:

```text
build/staging/6.1.99-rknpu098-test1/
```

This directory is excluded from Git. The verified boot artifact hashes are:

```text
cc3af5b38bfe01fd562e0c311630f7074f74f3684009222df2abb32deea5c20f  Image-6.1.99-rknpu098-test1
65b50995edb16892858413da4b506199ed20cd8d13535b1b8ff1e5b25ab765c6  config-6.1.99-rknpu098-test1
a8a0ab4e5596de89c73d486a6368f915596aec7fc71539f588af4e28983ab474  System.map-6.1.99-rknpu098-test1
fc6209c02ad2e77f916655b5408d214e89f4f2e5c4ddb52fd65fa3fde1b33629  rk3588s-orangepi-5-pro.dtb
```

The generated DTB reports:

```text
model: RK3588S OPi 5 Pro
compatible: rockchip,rk3588s-orangepi-5-pro rockchip,rk3588
```

## Safety Gate

Do not copy these artifacts to `/boot` yet. The current U-Boot script loads fixed paths (`/boot/Image`, `/boot/uInitrd`, and `/boot/dtb/${fdtfile}`) and provides no tested fallback menu. A deployment and recovery procedure must be reviewed before any eMMC write or reboot test.
