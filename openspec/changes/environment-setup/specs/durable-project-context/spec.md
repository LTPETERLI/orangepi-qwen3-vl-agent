## ADDED Requirements

### Requirement: Context recovery
The repository SHALL contain enough current context for a new Codex session to determine the active branch, stable requirements, active changes, dependency versions, safety gates, and next incomplete task.

#### Scenario: A new session begins
- **WHEN** an agent starts work without prior chat history
- **THEN** it reads `AGENTS.md`, `openspec/config.yaml`, `environment/toolchain.yaml`, and the active OpenSpec change
- **AND** it checks Git status before modifying files

### Requirement: Branch promotion control
Environment work SHALL remain on `test/environment-setup` until the complete first workflow passes and the user explicitly approves promotion.

#### Scenario: Partial milestone completes
- **WHEN** a tool, camera, driver, runtime, or demo milestone completes but the full workflow has not passed
- **THEN** changes are committed only to `test/environment-setup`
- **AND** `main` remains unchanged

### Requirement: Current task status
OpenSpec task checkboxes and validation status SHALL reflect observed work rather than planned intent.

#### Scenario: Evidence is missing
- **WHEN** a task lacks the required command output, hash, or test report
- **THEN** the task remains incomplete
- **AND** the missing evidence is stated in the active change
