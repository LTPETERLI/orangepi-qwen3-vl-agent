#!/usr/bin/env python3
import argparse

from rknn.api import RKNN


def main():
    parser = argparse.ArgumentParser(description="Convert Rockchip's optimized YOLOv8n ONNX")
    parser.add_argument("onnx")
    parser.add_argument("dataset")
    parser.add_argument("output")
    args = parser.parse_args()

    rknn = RKNN(verbose=False)
    try:
        if rknn.config(
            mean_values=[[0, 0, 0]],
            std_values=[[255, 255, 255]],
            target_platform="rk3588",
        ) != 0:
            raise RuntimeError("RKNN configuration failed")
        if rknn.load_onnx(model=args.onnx) != 0:
            raise RuntimeError("ONNX loading failed")
        if rknn.build(do_quantization=True, dataset=args.dataset) != 0:
            raise RuntimeError("RKNN INT8 build failed")
        if rknn.export_rknn(args.output) != 0:
            raise RuntimeError("RKNN export failed")
    finally:
        rknn.release()


if __name__ == "__main__":
    main()
