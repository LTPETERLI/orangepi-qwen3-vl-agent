# Orange Pi 5 Pro 离线视觉语音智能体项目总计划

更新日期：2026-07-19  
电脑端工作空间：`/home/ubt/orangepai_ws`  
板端工作空间：`/home/orangepi/qwen3_vl_2b_ws`  
开发板地址：`orangepi@192.168.31.89`

## 1. 项目目标

在 Orange Pi 5 Pro 16GB 上构建完全离线运行的视觉语音智能体：

```text
离线语音输入
-> 问题理解与任务编排
-> RealSense RGB 场景理解
-> RealSense 深度测距
-> Qwen3-VL-2B 本地推理
-> 中文自然语言回答
-> 离线语音播放
```

运行时不得调用云端 API。图像、深度、音频、对话和日志只保存在本地。

## 2. 已确认硬件与环境

### 电脑端

| 项目 | 实际值 | 状态 |
|---|---|---|
| 系统 | Ubuntu 22.04.5 LTS x86_64 | PASS |
| CPU | Intel Core 7 240H，16 逻辑 CPU | PASS |
| 内存 | 30 GiB | CONDITIONAL |
| 磁盘 | 约 710 GB 可用 | PASS |
| GPU | Intel iGPU，无 NVIDIA/CUDA | CONDITIONAL |
| Python | 3.10.12 | PASS |
| 转换环境 | 尚未建立 | PENDING |

### Orange Pi

| 项目 | 实际值 | 状态 |
|---|---|---|
| 板卡 | Orange Pi 5 Pro 16GB | PASS |
| SoC | RK3588S | PASS |
| 架构 | AArch64 | PASS |
| 系统 | Ubuntu 22.04.5，Orange Pi 1.0.6 厂商镜像 | PASS |
| 内核 | `6.1.99-rknpu098-test1`，Orange Pi 6.1 厂商源码 | PASS |
| 系统介质 | 233 GiB eMMC，`/dev/mmcblk0` | PASS |
| RKNPU | 内核内置 `0.9.8` | PASS |
| 系统 RKNN/RKLLM 库 | 存在，但版本来源未确认 | UNKNOWN |
| 空闲存储 | 约 201 GB | PASS |
| 空闲内存 | 审计时约 13 GiB | PASS |

### RealSense

```yaml
camera:
  manufacturer: RealSense
  model: D455
  usb_vendor_id: "8086"
  usb_product_id: "0b5c"
  required_usb_speed: "5000M"
  current_usb_speed: "5000M"
  initial_stream:
    color: "640x480@15"
    depth: "640x480@15"
  firmware_update_allowed: false
```

D455 已在 Bus 006 以 SuperSpeed Gen 1 `5000M` 枚举，`/dev/video0` 至 `/dev/video5` 正常存在。业务接口统一使用：

```text
RealSenseCamera
RealSenseManager
realsense_config
```

不得把 D455 型号写死在业务接口名称中。

## 3. 已确认 Rockchip 基线

官方仓库：`https://github.com/airockchip/rknn-llm.git`

```yaml
rockchip:
  rknn_llm_tag: "release-v1.3.0"
  rknn_llm_commit: "878f9361fd3afa7e167b7079918918f78d2c1c2a"
  rkllm_toolkit: "1.3.0"
  rkllm_runtime_candidate: "1.3.0"
  rknn_toolkit2_minimum: "2.3.2"
  rknpu_required: ">=0.9.8"
  target_platform: "rk3588"
```

Qwen3-VL 初始参数：

```yaml
qwen3_vl:
  model: "Qwen3-VL-2B-Instruct"
  vision_resolution: [448, 448]
  quantization: "w8a8"
  context_length: 4096
  max_new_tokens: 128-256
  npu_cores: 3
```

Toolkit、Runtime、头文件、Demo 和模型文件格式必须来自同一 RKLLM SDK 版本，不允许混用。

## 4. 当前状态

| Gate | 内容 | 状态 |
|---|---|---|
| Gate 0 | 初始环境只读审计 | COMPLETE |
| Gate 1A | 硬件收敛与恢复方案设计 | COMPLETE |
| USB3 | D455 直连 `5000M` | PASS |
| 启动介质 | eMMC 类型和启动状态确认 | PASS |
| RKLLM 1.3.0 | tag、commit、文件与哈希核验 | PASS |
| RKNPU | 内核内置 `0.9.8`，30 次官方 Demo 无驱动故障 | PASS |
| 厂商升级路径 | 未找到带完整版本证据的 5 Pro 官方镜像 | UNKNOWN |
| 整盘备份 | 用户确认无重要数据，整盘备份已豁免 | WAIVED |
| 模型运行 | 官方固定图片 Demo 连续 30 次有效回答 | PASS |

官方固定图片模型基线已通过。下一阶段为 D455 用户态采集基线，仍不得未经批准安装 DKMS、更新相机固件或执行主干合并。

## 5. 总体执行原则

1. 先核查，后决策，再准备恢复路径，最后实施。
2. 系统兼容性以厂商内核、设备树、RKNPU 驱动和 Runtime 组合为核心，不只看 Ubuntu 版本。
3. 用户已接受不做整盘备份的数据丢失风险；任何系统级变更仍须先明确完整重刷和重建路径，并再次批准。
4. Rockchip Runtime 使用应用私有目录，不覆盖系统库。
5. 官方预转换模型和官方 Demo 先于自行转换模型。
6. D455 优先使用用户态 RSUSB/libuvc 方案，不安装 DKMS。
7. 每个阶段必须有验收标准、日志、版本锁和回滚方法。
8. 用户必须在关键 Gate 明确确认后才能继续。

## 6. 禁止操作

未经新的阶段批准，不得执行：

```text
apt full-upgrade
apt dist-upgrade
do-release-upgrade
安装 generic/mainline/edge 内核
单独替换内置 RKNPU 驱动
覆盖 /usr/lib/librknnrt.so
覆盖 /usr/lib/librkllmrt.so
更新 D455 固件
安装 librealsense2-dkms
修改设备树或启动分区
运行 NPU 模型或模型转换脚本
从正在运行的 mmcblk1 对自身做权威整盘备份
```

## 7. 阶段一：重刷与重建准备

### 风险决定

用户确认 eMMC 没有需要保留的项目数据，决定不制作整盘镜像，并接受现有 Ubuntu、配置、软件包和板端工作空间可能全部丢失。风险记录见 `RISK_ACCEPTANCE.md`。

### 执行要求

1. 所有报告、清单、源码和配置必须保存在笔记本工作空间或 Git 中。
2. 找到明确支持 Orange Pi 5 Pro 的官方恢复镜像、SHA256、刷写工具和启动说明。
3. 记录从空白系统重建 Runtime、模型、相机、语音和服务的版本与步骤。
4. 在执行任何系统变更前再次审查清空范围、失败模式和重刷命令。
5. 每个破坏性操作必须重新获得用户批准。

### 验收

```text
项目文件已镜像到笔记本
官方恢复镜像身份和 SHA256 已确认
Orange Pi 5 Pro 重刷方法已记录
空白系统重建步骤可执行
用户已再次批准具体破坏性操作
```

可选整盘备份方案仍保留在 `BACKUP_RESTORE_PLAN.md`。

## 8. 阶段二：RKNPU 厂商兼容路径

目标是获得适用于 Orange Pi 5 Pro、RK3588S、16GB 的完整厂商栈，并明确包含：

```text
Orange Pi 5 Pro 专用镜像或内核
对应设备树
RKNPU >= 0.9.8
匹配的 RKNN/RKLLM Runtime 要求
官方镜像名称、发布日期、大小和 SHA256
刷写与恢复方法
```

在没有这些证据时状态保持 `UNKNOWN`，不推荐刷机。不得使用只标注 Orange Pi 5、5 Plus 或 5 Ultra 的镜像代替 5 Pro 镜像。

如果需要迁移系统，必须先提出新的执行计划，说明目标镜像、驱动版本、设备树、清空范围和重建步骤，并再次获得用户批准。当前系统不承诺可恢复。

## 9. 阶段三：官方 Qwen3-VL Demo

仅在 RKNPU 基线解决且用户批准后执行。

### 私有目录

```text
/opt/offline-agent/
├── vendor/rkllm-1.3.0/
│   ├── bin/
│   ├── lib/
│   ├── include/
│   └── manifest/
├── models/qwen3-vl-2b/
├── test-images/
├── logs/
└── benchmarks/
```

不把 SDK 动态库复制到 `/usr/lib`。通过明确的 `LD_LIBRARY_PATH` 加载应用私有库，并使用 `ldd`/`LD_DEBUG=libs` 确认实际路径。

### 顺序

1. 获取与 `release-v1.3.0` 完全匹配的官方 Demo、Runtime、头文件和模型。
2. 建立来源、版本、文件大小和 SHA256 清单。
3. 检查二进制架构和动态库依赖。
4. 使用固定图片运行官方 Qwen3-VL-2B Demo。
5. 使用 20 张固定测试图进行 30 次连续问答。

### 验收

```text
连续运行 30 次无崩溃
无重复乱码或异常符号
无 IOMMU fault
无 OOM
峰值内存不超过 14GB
系统至少保留 2GB 内存
输出内容基本合理
```

官方 Demo 未通过时，禁止自行转换模型和集成 D455。

## 10. 阶段四：D455 用户态采集

### 原则

- 保持当前 `5000M` 端口、线缆和直连方式。
- 使用稳定版 librealsense 的用户态 RSUSB/libuvc 后端。
- 不安装 DKMS，不修改内核，不更新固件。
- 第一轮只启用 `640x480@15` 的彩色与深度流。

### 验收

```text
设备枚举正常
RGB 与深度时间戳持续更新
RGB-depth 对齐可用
连续采集 30 分钟无永久断流
无持续 USB reset
无大量丢帧
拔插后程序能够退出或恢复
```

之后扩展至 2 小时稳定性测试和 `640x480@30` 性能评估。

## 11. 阶段五：视觉与深度融合

Qwen3-VL 只处理 RGB 关键帧，深度由独立工具提供：

```text
get_depth_at_pixel(x, y)
get_region_median_depth(x1, y1, x2, y2)
get_nearest_obstacle()
deproject_pixel_to_point(x, y, depth)
get_depth_quality(region)
```

目标距离必须使用目标框中央区域的有效深度中位数，并报告有效像素比例。不得用单个中心像素直接代表目标距离，不得让 VLM 根据 RGB 猜测精确距离。

## 12. 阶段六：自行转换模型

只有官方模型无法满足分辨率、精度、量化数据或可复现供应链要求时才进入。

电脑端建立两个隔离的 Python 3.10 环境：

```text
qwen-vl-vision-export
qwen-vl-rkllm-export
```

视觉路径：

```text
Qwen3-VL safetensors
-> ONNX 448x448
-> RKNN，target=rk3588
```

语言路径：

```text
Qwen3-VL safetensors
-> RKLLM Toolkit 1.3.0
-> W8A8 RKLLM，3 NPU cores
```

安装版本必须以官方 requirements、wheel 元数据和 `pip check` 为准。两个环境分别生成冻结文件，任何官方脚本补丁必须单独提交、可撤销并保留 diff。

## 13. 最终软件架构

采用四进程架构：

```text
camera_worker
vlm_worker
speech_worker
agent_orchestrator
```

### camera_worker

负责 D455 初始化、RGB-D 采集、对齐、帧缓存、深度工具和断连恢复。

### vlm_worker

基于官方 C++ Demo 改造，常驻加载视觉 RKNN 与语言 RKLLM，接收图片和问题并流式返回 Token。不得每轮重新加载模型。

### speech_worker

负责麦克风、VAD、离线 ASR、离线 TTS、半双工和回声规避。第一版采用按键说话。

### agent_orchestrator

Python 实现，负责对话状态、工具调用、指代、超时、降级、配置和日志。

### 通信与 NPU 调度

第一版使用 Unix Domain Socket 和 JSON 控制消息，大图像通过临时文件、共享内存或 mmap 传输。所有 NPU 任务使用全局锁串行调度，视觉问答期间暂停其他 RKNN 检测任务。

## 14. 项目里程碑

| 里程碑 | 目标 | 前置条件 |
|---|---|---|
| M0 | 审计、兼容性报告和修订计划 | 已完成 |
| M1 | 官方 Qwen3-VL 图片 Demo | RKNPU Gate 通过，重刷/重建路径已确认 |
| M2 | D455 RGB-D 稳定采集 | USB3 已通过，SDK Gate 待批准 |
| M3 | D455 RGB 关键帧进入 VLM | M1、M2 |
| M4 | 深度工具融合 | M3 |
| M5 | 可选 YOLO RKNN 与三维定位 | M4 |
| M6 | 离线 ASR/TTS 闭环 | M4 稳定 |
| M7 | 断网、重启与 8 小时稳定性验收 | 全部功能完成 |

## 15. 最终验收指标

```text
核心功能断网可用率：100%
模型启动成功率：100%
连续视觉问答：至少 30 次无崩溃
D455 连续采集：至少 2 小时无永久断流
整机连续运行：至少 8 小时无服务退出
视觉输入：448x448
上下文：4096
单次输出：128-256 Token
峰值内存：不超过 14GB
保留系统内存：至少 2GB
摄像头：5000M SuperSpeed
```

## 16. 回答可靠性规则

系统提示词必须要求：

1. 只描述当前图像中能够确认的内容。
2. 不清楚时明确表示无法确认。
3. 不根据 RGB 图像猜测精确距离。
4. 距离只使用深度工具结果。
5. 深度结果不可靠时不输出确定距离。
6. 不虚构物体、文字、人物或当前场景。
7. 回答简洁，适合语音播放。

## 17. 记录与交付规范

每阶段必须记录：

```text
1. 阶段目标
2. 实际命令
3. 修改文件
4. 安装软件和版本
5. 测试结果
6. 未解决问题
7. 风险
8. 回滚命令
9. 是否允许进入下一阶段
```

版本、依赖与工具链记录统一写入 `environment/toolchain.yaml`，系统和部署变更写入 `CHANGELOG_DEPLOYMENT.md`。每次安装、升级、下载、替换或验证状态变化后立即更新 YAML；所有下载文件、模型、工具链、动态库和镜像均记录来源、日期、大小与 SHA256。

## 18. 当前下一步

整盘备份已由用户豁免。当前只允许继续收敛官方恢复和 RKNPU 路径：

```text
确认 Orange Pi 5 Pro 官方恢复镜像、SHA256 和刷写方法
确认该镜像或完整厂商栈的 RKNPU 版本
保持全部项目文档与源码在笔记本端
为具体系统变更提交新的执行申请
```

未经用户再次确认，不格式化、不烧录、不替换驱动、不运行 NPU 模型。若系统变更失败，按已确认的官方镜像重刷，并从笔记本文档和清单重建。

RKNPU 状态保持：

```yaml
rknpu:
  actual_version: "0.9.6"
  linkage: "built-in"
  final_acceptance: "FAIL"
  diagnostic_test_allowed: false
  upgrade_authorized: false
```

项目当前停在 RKNPU 厂商路径确认与系统变更申请之前。
