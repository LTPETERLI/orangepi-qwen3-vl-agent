# Orange Pi 5 Pro Vendor Image Matrix

Research date: 2026-07-19.

## Decision

**UNKNOWN**: an Orange Pi official download page and Orange Pi 5 Pro manual exist, and the manual explicitly lists Ubuntu 20.04/22.04 with Linux 5.10. However, no accessible official manifest was found that binds a specific Orange Pi 5 Pro image filename, release date, checksum, exact Pro device tree, and RKNPU driver `>=0.9.8`. No image is recommended for flashing.

| Candidate | Official source | Image/release/kernel | Explicit 5 Pro support | RKNPU | DTB/checksum/size | Flash and recovery | Data impact | Status |
|---|---|---|---|---|---|---|---|---|
| Current vendor Ubuntu | Orange Pi-provided system currently installed | Ubuntu 20.04.6, vendor build 1.0.6, Linux `5.10.160-rockchip-rk3588` | Device tree identifies `RK3588S OPi 5 Pro` | Built-in `0.9.6` | Current DTB works; original image name, size and official checksum not recovered | Would require recovery image/whole-disk restore for rollback | Reflash clears current system | REJECTED for final NPU baseline |
| Official Orange Pi 5 Pro Ubuntu 20.04 image family | [Orange Pi 5 Pro official download page](https://www.orangepi.org/html/hardWare/computerAndMicrocontrollers/service-and-support/Orange-Pi-5-Pro.html) | Exact filename/date/size unavailable from accessible official metadata; manual lists Linux 5.10 | Yes, page is model-specific | Not stated; installed instance is 0.9.6 | Exact DTB and SHA256 unavailable | Orange Pi tools/SD flashing; recovery by full image restore | Reflash clears target | UNKNOWN |
| Orange Pi 5 Pro release 1.0.6 Ubuntu 22.04 Linux 6.1.43 | Orange Pi 5 Pro official image folder and local official manual v1.3-1, SHA256 `3c8d71b171d75043224db347d2026ea0fb6638242a48e363579587ce2689237b` | `Orangepi5pro_1.0.6_ubuntu_jammy_{server,desktop_xfce}_linux6.1.43.7z`, release date 2024-11-18 | Yes | Official manual records RKNPU `0.9.6`, below required `0.9.8` | 5 Pro-specific image family; compressed image official SHA256 still unavailable | Flash TF card first and validate; eMMC migration requires booting from TF/recovery media | Reflash clears target | REJECTED as final NPU baseline; POSSIBLE as 5 Pro Linux 6.1 build baseline |
| Images for Orange Pi 5 / 5 Plus / 5 Ultra | Other Orange Pi product pages | Various | No explicit 5 Pro match | Varies/unknown | Product-specific DTBs | Not applicable | High brick/data-loss risk | REJECTED |
| Generic Ubuntu ARM64 | Ubuntu generic downloads | Generic arm64 | No Orange Pi 5 Pro board adaptation | Unknown | No Orange Pi 5 Pro DTB/vendor stack | Generic installer | Destructive and unsupported | REJECTED |
| Mainline/edge and third-party images | Armbian/community/Joshua-Riek and others | Various | May claim 5 Pro, but not Orange Pi official | Varies | Third-party | Project-specific | Destructive | REJECTED by Gate 1A policy |

## User Reports

Search results include anecdotal reports of boot/setup problems with Orange Pi 5 Pro images and mixed stability experiences across Orange Pi 5-family images. These reports are useful risk signals but cannot establish driver versions or substitute for an official manifest.

## Required Evidence Before Any Recommendation

1. Exact image filename and release date from the Orange Pi 5 Pro official page.
2. Official SHA256 and compressed/uncompressed size.
3. The included kernel version and Orange Pi 5 Pro DTB path.
4. Verifiable RKNPU driver version `>=0.9.8` in that image.
5. Official flashing instructions and a tested recovery path.

Until all five are available, `upgrade_authorized` remains false and the current system remains untouched.
