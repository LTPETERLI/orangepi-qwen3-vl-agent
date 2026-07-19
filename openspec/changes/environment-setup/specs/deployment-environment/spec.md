## ADDED Requirements

### Requirement: Version-matched Rockchip stack
The deployment SHALL use an Orange Pi 5 Pro device tree, vendor-compatible kernel, RKNPU driver, RKNN runtime, RKLLM runtime, headers, demo, and models whose identities and compatibility are recorded in `environment/toolchain.yaml`.

#### Scenario: Stack is accepted for model diagnostics
- **WHEN** an NPU diagnostic is proposed
- **THEN** the board identity, kernel, device tree, RKNPU version, private runtime hashes, model hashes, and demo version are recorded
- **AND** RKNPU is at least version 0.9.8
- **AND** the user has explicitly approved model execution

#### Scenario: Candidate stack is incomplete
- **WHEN** any required component version or provenance is unknown
- **THEN** the affected capability remains `blocked` or `present_unverified`
- **AND** it SHALL NOT be presented as an accepted baseline

### Requirement: Reproducible host tools
Host-side tools SHALL be version-pinned and isolated from the Orange Pi system Python and system Rockchip libraries.

#### Scenario: A dependency changes
- **WHEN** a dependency is installed, removed, upgraded, downloaded, or replaced
- **THEN** `environment/toolchain.yaml` is updated in the same Git commit
- **AND** downloadable artifacts include source, target, version, size, and SHA256 when available

### Requirement: Reversible deployment
Application dependencies SHALL use project-private paths unless a separately approved system change is required.

#### Scenario: Runtime deployment
- **WHEN** RKNN or RKLLM runtime files are staged on the board
- **THEN** they are placed in a versioned application directory
- **AND** `/usr/lib/librknnrt.so` and `/usr/lib/librkllmrt.so` remain unchanged

### Requirement: Camera baseline
The capture stack SHALL identify the physical camera as RealSense D455 while keeping business interfaces model-neutral.

#### Scenario: Camera capture is eligible for SDK validation
- **WHEN** the D455 enumerates as `8086:0b5c` at `5000M` without repeated USB resets
- **THEN** a project-private RSUSB/user-space librealsense validation may be proposed
- **AND** DKMS and camera firmware updates remain prohibited
