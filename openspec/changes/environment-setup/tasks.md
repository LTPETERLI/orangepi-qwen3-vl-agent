## 1. Durable Context and Version Control

- [x] 1.1 Initialize Git and preserve `main` as the formal baseline.
- [x] 1.2 Create and push `test/environment-setup`.
- [x] 1.3 Add and validate `environment/toolchain.yaml`.
- [x] 1.4 Install project-pinned OpenSpec and Node toolchain.
- [x] 1.5 Add session recovery instructions and active OpenSpec artifacts.

## 2. Orange Pi Boot and RKNPU Baseline

- [x] 2.1 Download an official Orange Pi 5 Pro Jammy Desktop Linux 6.1.43 image, verify its archive, and verify the extracted image against the bundled SHA256 file.
- [x] 2.2 Obtain explicit approval before flashing or replacing boot artifacts.
- [x] 2.3 Verify board model, OS, kernel, device tree, boot media, and RKNPU version after migration.
- [ ] 2.4 If RKNPU remains below 0.9.8, design and review a 5 Pro-specific full-kernel integration path.

## 3. Official Multimodal Baseline

- [x] 3.1 Record official Qwen3-VL-2B RKNN/RKLLM model sources, sizes, and SHA256 hashes.
- [x] 3.2 Stage version-matched RKNN/RKLLM runtimes in an application-private prefix.
- [ ] 3.3 Obtain explicit approval to execute the official fixed-image demo.
- [ ] 3.4 Complete 30 consecutive accepted demo runs and record stability evidence.

## 4. RealSense Baseline

- [x] 4.1 Verify D455 identity and direct `5000M` USB connection.
- [ ] 4.2 Pin librealsense release, source commit, build options, and SHA256.
- [ ] 4.3 Build RSUSB backend into a project-private prefix without DKMS.
- [ ] 4.4 Validate aligned 640x480@15 RGB-D capture, reconnect, and endurance.

## 5. Integration and Promotion

- [ ] 5.1 Audit and fix the pinned Qwen3-VL-2B-NPU submodule integration path.
- [ ] 5.2 Complete the first camera-to-VLM workflow and offline test.
- [ ] 5.3 Update all OpenSpec evidence and toolchain states.
- [ ] 5.4 Obtain explicit user approval before merging to `main`.
