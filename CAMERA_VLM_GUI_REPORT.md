# Camera VLM Desktop GUI Report

Date: 2026-08-05

## Deployment

The Orange Pi XFCE desktop already provided Python 3.10, GTK 3.24, PyGObject,
and Pillow. No new GUI package was installed.

```yaml
application: "/home/orangepi/qwen3_vl_2b_ws/app/camera_vlm_gui.py"
launcher: "/home/orangepi/qwen3_vl_2b_ws/bin/camera-vlm-gui"
desktop_entry: "/home/orangepi/Desktop/OrangePi-Qwen3-VL.desktop"
display: ":0"
```

## Behavior

- The left pane displays a current `RealSenseCamera` RGB keyframe and aligned
  center depth.
- The right pane provides a question input, send button, answer log, history
  clear button, and model stop button.
- Launching the window captures an RGB-D keyframe but does not load an NPU
  model.
- The first submitted question starts the approved private RKLLM/RKNN stack
  through the official multimodal demo and a pseudo-terminal, so incremental
  output remains visible in the answer pane.
- Refreshing the image requires stopping the current model session first. This
  keeps the displayed image and the model image embedding consistent.

## Validation

The Python source passes bytecode compilation on the laptop and Orange Pi. The
GUI process started successfully in the active XFCE `:0` session and generated
a 921615-byte RGB keyframe. Its launch log was empty after the GdkPixbuf version
declaration was corrected.

No model question was submitted automatically. Visual question answering is
triggered only by the local user pressing Send. A full desktop screenshot was
not exported because it could expose unrelated screen content.
