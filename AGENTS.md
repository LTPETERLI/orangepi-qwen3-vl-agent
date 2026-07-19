# Repository Operating Context

Before making changes in a new session:

1. Run `git status --short --branch` and remain on `test/environment-setup` unless the user explicitly authorizes a merge to `main`.
2. Read `openspec/config.yaml`, `environment/toolchain.yaml`, and `PROJECT_MASTER_PLAN.md`.
3. Run `./scripts/openspec.sh list --json` and inspect every active change with `./scripts/openspec.sh status --change <id> --json`.
4. Treat `openspec/specs/` as the stable source of requirements and `openspec/changes/` as work in progress.
5. After any dependency, version, artifact, runtime, driver, kernel, model, or validation change, update `environment/toolchain.yaml` in the same commit.

Do not execute NPU models, flash an image, replace a kernel/driver/system library, update camera firmware, or merge into `main` without explicit user approval.

Use `./scripts/openspec.sh` rather than a globally installed `openspec`; the wrapper pins the project Node.js toolchain and restores TLS certificate verification.
