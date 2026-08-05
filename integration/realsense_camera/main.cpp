#include <librealsense2/rs.hpp>

#include <charconv>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <string>

namespace {

int parse_frame_count(int argc, char** argv) {
    if (argc == 1) return 150;
    if (argc != 3 || std::string(argv[1]) != "--frames") return -1;
    int frames = 0;
    const std::string value(argv[2]);
    const auto parsed = std::from_chars(value.data(), value.data() + value.size(), frames);
    if (parsed.ec != std::errc() || parsed.ptr != value.data() + value.size() || frames <= 0) return -1;
    return frames;
}

std::string info_or_unknown(const rs2::device& device, rs2_camera_info field) {
    return device.supports(field) ? device.get_info(field) : "unknown";
}

}  // namespace

int main(int argc, char** argv) try {
    const int target_frames = parse_frame_count(argc, argv);
    if (target_frames < 1) {
        std::cerr << "Usage: " << argv[0] << " [--frames COUNT]\n";
        return 2;
    }

    rs2::context context;
    const rs2::device_list devices = context.query_devices();
    if (devices.size() == 0) {
        std::cerr << "No RealSense camera detected through the RSUSB backend.\n";
        return 1;
    }

    const rs2::device device = devices.front();
    std::cout << "camera.name=" << info_or_unknown(device, RS2_CAMERA_INFO_NAME) << '\n'
              << "camera.serial=" << info_or_unknown(device, RS2_CAMERA_INFO_SERIAL_NUMBER) << '\n'
              << "camera.firmware=" << info_or_unknown(device, RS2_CAMERA_INFO_FIRMWARE_VERSION) << '\n'
              << "camera.usb=" << info_or_unknown(device, RS2_CAMERA_INFO_USB_TYPE_DESCRIPTOR) << '\n';

    rs2::pipeline pipeline(context);
    rs2::config config;
    config.enable_device(info_or_unknown(device, RS2_CAMERA_INFO_SERIAL_NUMBER));
    config.enable_stream(RS2_STREAM_COLOR, 640, 480, RS2_FORMAT_RGB8, 15);
    config.enable_stream(RS2_STREAM_DEPTH, 640, 480, RS2_FORMAT_Z16, 15);
    pipeline.start(config);

    rs2::align align_to_color(RS2_STREAM_COLOR);
    double previous_color_timestamp = -1.0;
    double previous_depth_timestamp = -1.0;
    int timestamp_regressions = 0;
    int valid_depth_frames = 0;
    double center_depth_sum = 0.0;

    for (int index = 0; index < target_frames; ++index) {
        rs2::frameset frames = pipeline.wait_for_frames(5000);
        frames = align_to_color.process(frames);
        const rs2::video_frame color = frames.get_color_frame();
        const rs2::depth_frame depth = frames.get_depth_frame();
        if (!color || !depth) {
            std::cerr << "Missing aligned color or depth frame at index " << index << '\n';
            pipeline.stop();
            return 1;
        }

        const double color_timestamp = color.get_timestamp();
        const double depth_timestamp = depth.get_timestamp();
        if (color_timestamp <= previous_color_timestamp || depth_timestamp <= previous_depth_timestamp) {
            ++timestamp_regressions;
        }
        previous_color_timestamp = color_timestamp;
        previous_depth_timestamp = depth_timestamp;

        const float center_depth = depth.get_distance(depth.get_width() / 2, depth.get_height() / 2);
        if (std::isfinite(center_depth) && center_depth > 0.0F) {
            ++valid_depth_frames;
            center_depth_sum += center_depth;
        }
    }
    pipeline.stop();

    std::cout << "capture.frames=" << target_frames << '\n'
              << "capture.color=640x480@15\n"
              << "capture.depth=640x480@15\n"
              << "capture.aligned=true\n"
              << "capture.timestamp_regressions=" << timestamp_regressions << '\n'
              << "capture.valid_center_depth_frames=" << valid_depth_frames << '\n';
    if (valid_depth_frames > 0) {
        std::cout << "capture.average_center_depth_m="
                  << center_depth_sum / valid_depth_frames << '\n';
    }
    return timestamp_regressions == 0 ? 0 : 1;
} catch (const rs2::error& error) {
    std::cerr << "RealSense error in " << error.get_failed_function() << ": " << error.what() << '\n';
    return 1;
} catch (const std::exception& error) {
    std::cerr << "Error: " << error.what() << '\n';
    return 1;
}
