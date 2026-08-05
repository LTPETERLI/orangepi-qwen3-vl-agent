# Object Detection and Per-Object Depth Report

Date: 2026-08-05

## Result

The Orange Pi GUI now displays two live views:

- RGB with numbered YOLOv8 detection boxes;
- colorized depth aligned to RGB.

The raw point-cloud view and center-pixel distance were removed. A result area at
the lower left lists each detection's number, COCO class, confidence, and distance.

## Detection Stack

- Detector: Rockchip RKNN Model Zoo v2.3.2 YOLOv8n
- Model Zoo commit: `bad6c7334531becaf90a561988519b7bec34d0ab`
- Target: RK3588
- Quantization: INT8
- Toolkit: RKNN Toolkit2 2.3.2 in a project-private Python 3.10 environment
- Runtime: application-private `librknnrt.so` bundled with RKLLM 1.3.0
- Classes: COCO 80

## Distance Method

For every detected box, the preview process samples aligned RealSense depth from
the central 60 percent of the box, rejects invalid values and values outside
0.15-10.0 m, and reports the median. This is more robust than using the image
center or a single depth pixel.

## Live Validation

- GUI process remained running after launch.
- Detector process remained running and consumed NPU inference normally.
- Observed result: box `#1`, class `bottle`, confidence `47%`, distance `3.71 m`.
- No IOMMU fault, USB `error -71`, USB `error -110`, or repeated USB reset was
  observed during validation.
- Qwen3-VL inference was not started during this detector validation.

## Operational Behavior

The detector runs while the live preview is active. Sending a Qwen3-VL question
stops the preview first, which releases the detector's NPU context; the GUI restores
the detector preview after the multimodal model exits. This preserves serialized NPU
use between object detection and Qwen3-VL.
