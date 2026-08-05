#include <librealsense2/rs.hpp>

#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <csignal>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "yolov8.h"

namespace {
constexpr int kWidth = 640;
constexpr int kHeight = 480;
constexpr int kChannels = 3;
constexpr int kDetectionInterval = 5;

std::atomic_bool running{true};
void stop_handler(int) { running = false; }

struct Detection {
    int index;
    std::string label;
    float confidence;
    image_rect_t box;
    float distance_m;
};

float median_depth(const rs2::depth_frame& depth, const image_rect_t& box) {
    const int width = std::max(1, box.right - box.left);
    const int height = std::max(1, box.bottom - box.top);
    const int left = std::clamp(box.left + width / 5, 0, kWidth - 1);
    const int right = std::clamp(box.right - width / 5, left + 1, kWidth);
    const int top = std::clamp(box.top + height / 5, 0, kHeight - 1);
    const int bottom = std::clamp(box.bottom - height / 5, top + 1, kHeight);

    std::vector<float> distances;
    distances.reserve(((right - left) / 2 + 1) * ((bottom - top) / 2 + 1));
    for (int y = top; y < bottom; y += 2) {
        for (int x = left; x < right; x += 2) {
            const float value = depth.get_distance(x, y);
            if (std::isfinite(value) && value >= 0.15F && value <= 10.0F) {
                distances.push_back(value);
            }
        }
    }
    if (distances.size() < 8) return 0.0F;
    const auto middle = distances.begin() + distances.size() / 2;
    std::nth_element(distances.begin(), middle, distances.end());
    return *middle;
}

void set_pixel(std::vector<std::uint8_t>& image, int x, int y,
               const std::array<std::uint8_t, 3>& color) {
    if (x < 0 || x >= kWidth || y < 0 || y >= kHeight) return;
    const std::size_t offset = static_cast<std::size_t>(y * kWidth + x) * kChannels;
    image[offset] = color[0];
    image[offset + 1] = color[1];
    image[offset + 2] = color[2];
}

void draw_box(std::vector<std::uint8_t>& image, const image_rect_t& box, int index) {
    static constexpr std::array<std::array<std::uint8_t, 3>, 6> colors{{
        {{255, 72, 72}}, {{80, 210, 120}}, {{65, 150, 255}},
        {{255, 195, 60}}, {{210, 90, 235}}, {{50, 215, 215}},
    }};
    const auto& color = colors[static_cast<std::size_t>(index - 1) % colors.size()];
    const int left = std::clamp(box.left, 0, kWidth - 1);
    const int right = std::clamp(box.right, 0, kWidth - 1);
    const int top = std::clamp(box.top, 0, kHeight - 1);
    const int bottom = std::clamp(box.bottom, 0, kHeight - 1);
    for (int thickness = 0; thickness < 3; ++thickness) {
        for (int x = left; x <= right; ++x) {
            set_pixel(image, x, top + thickness, color);
            set_pixel(image, x, bottom - thickness, color);
        }
        for (int y = top; y <= bottom; ++y) {
            set_pixel(image, left + thickness, y, color);
            set_pixel(image, right - thickness, y, color);
        }
    }
}

void emit_detections(const std::vector<Detection>& detections) {
    std::cerr << "detections=";
    for (std::size_t i = 0; i < detections.size(); ++i) {
        if (i) std::cerr << ';';
        const auto& detection = detections[i];
        std::cerr << detection.index << '|' << detection.label << '|'
                  << std::fixed << std::setprecision(3) << detection.confidence << '|'
                  << detection.box.left << '|' << detection.box.top << '|'
                  << detection.box.right << '|' << detection.box.bottom << '|'
                  << std::setprecision(2) << detection.distance_m;
    }
    std::cerr << '\n' << std::flush;
}
}  // namespace

int main() try {
    std::signal(SIGTERM, stop_handler);
    std::signal(SIGINT, stop_handler);
    std::signal(SIGPIPE, stop_handler);

    const char* enabled_value = std::getenv("YOLO_ENABLED");
    const bool detection_enabled = !enabled_value || std::string(enabled_value) != "0";
    const char* model_path = std::getenv("YOLO_MODEL_PATH");
    if (detection_enabled && (!model_path || !*model_path)) {
        throw std::runtime_error("YOLO_MODEL_PATH is not set");
    }

    rknn_app_context_t detector{};
    if (detection_enabled &&
        (init_post_process() != 0 || init_yolov8_model(model_path, &detector) != 0)) {
        throw std::runtime_error("failed to initialize YOLOv8 detector");
    }

    rs2::pipeline pipeline;
    rs2::config config;
    config.enable_stream(RS2_STREAM_COLOR, kWidth, kHeight, RS2_FORMAT_RGB8, 15);
    config.enable_stream(RS2_STREAM_DEPTH, kWidth, kHeight, RS2_FORMAT_Z16, 15);
    pipeline.start(config);
    rs2::align align_to_color(RS2_STREAM_COLOR);
    rs2::colorizer depth_colorizer;

    int frame_index = 0;
    std::vector<Detection> detections;
    while (running) {
        rs2::frameset frames = align_to_color.process(pipeline.wait_for_frames(5000));
        const rs2::video_frame color = frames.get_color_frame();
        const rs2::depth_frame depth = frames.get_depth_frame();
        if (!color || !depth) continue;

        if (detection_enabled && frame_index % kDetectionInterval == 0) {
            image_buffer_t input{};
            input.width = kWidth;
            input.height = kHeight;
            input.format = IMAGE_FORMAT_RGB888;
            input.size = kWidth * kHeight * kChannels;
            input.virt_addr = const_cast<unsigned char*>(
                static_cast<const unsigned char*>(color.get_data()));
            object_detect_result_list results{};
            if (inference_yolov8_model(&detector, &input, &results) == 0) {
                detections.clear();
                for (int i = 0; i < results.count; ++i) {
                    const auto& result = results.results[i];
                    detections.push_back({i + 1, coco_cls_to_name(result.cls_id), result.prop,
                                          result.box, median_depth(depth, result.box)});
                }
                emit_detections(detections);
            }
        } else if (detection_enabled) {
            for (auto& detection : detections) {
                detection.distance_m = median_depth(depth, detection.box);
            }
        }

        const auto* source = static_cast<const std::uint8_t*>(color.get_data());
        std::vector<std::uint8_t> annotated(source, source + kWidth * kHeight * kChannels);
        for (const auto& detection : detections) draw_box(annotated, detection.box, detection.index);
        const rs2::video_frame colorized_depth = depth_colorizer.colorize(depth);
        std::cout.write(reinterpret_cast<const char*>(annotated.data()), annotated.size());
        std::cout.write(static_cast<const char*>(colorized_depth.get_data()),
                        kWidth * kHeight * kChannels);
        std::cout.flush();
        if (!std::cout) break;
        ++frame_index;
    }

    pipeline.stop();
    if (detection_enabled) {
        release_yolov8_model(&detector);
        deinit_post_process();
    }
    return 0;
} catch (const rs2::error& error) {
    std::cerr << "error=" << error.what() << '\n';
    return 1;
} catch (const std::exception& error) {
    std::cerr << "error=" << error.what() << '\n';
    return 1;
}
