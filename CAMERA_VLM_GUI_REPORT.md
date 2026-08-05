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

- The left pane displays the live `RealSenseCamera` RGB stream and an aligned,
  colorized depth stream side by side at `640x480@15`, with continuously
  updated center depth.
- The right pane provides a question input, send button, answer log, history
  clear button, and model stop button.
- Launching the window starts the project-private RSUSB preview process but
  does not load an NPU model.
- Pressing Send freezes the most recent RGB frame into a PPM keyframe, stops
  the preview to release the camera, and then starts visual inference.
- The first submitted question starts the approved private RKLLM/RKNN stack
  through the official multimodal demo and a pseudo-terminal, so incremental
  output remains visible in the answer pane.
- Stopping the model automatically resumes live preview. This keeps the
  displayed frame and model image embedding consistent during each question.

## Validation

The Python source passes bytecode compilation on the laptop and Orange Pi. The
GUI process started successfully in the active XFCE `:0` session. The
`realsense-preview` process remained active at approximately 29% CPU, held the
five video interfaces through `usbfs`, and retained the `5000M` connection. Its
launch log was empty after the GdkPixbuf version declaration was corrected.

The preview protocol now carries three synchronized views: RGB, aligned
colorized depth, and an RGB-textured XYZ point cloud rendered from a fixed
oblique viewpoint. The three-view process remained active with an empty launch
log and `5000M` retained. Interactive rotation is not yet implemented.

No model question was submitted automatically. Visual question answering is
triggered only by the local user pressing Send. A full desktop screenshot was
not exported because it could expose unrelated screen content.
