# Orange Pi VLM Chat Integration

This is the project-owned text-only command-line entry point built against the
official RKLLM 1.3.0 API. It keeps RKLLM in the application-private runtime
prefix and does not copy it into `/usr/lib` or `/usr/local/lib`.

Build on the Orange Pi:

```bash
./scripts/build-vlm-chat.sh
```

The deployment step places the resulting executable at:

```text
/home/orangepi/qwen3_vl_2b_ws/app/orangepi-text-chat
```

Start a text-only session:

```bash
./scripts/run-vlm-chat.sh
```

Model execution remains a gated operation. Building the executable and using
`--help` do not initialize the model.
