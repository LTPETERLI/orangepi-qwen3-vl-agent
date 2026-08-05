#include <librealsense2/rs.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

namespace {

struct Options {
    std::filesystem::path output;
    int warmup_frames = 30;
};

void usage(const char* program) {
    std::cerr << "Usage: " << program << " --output IMAGE.ppm [--warmup-frames 30]\n";
}

bool parse_args(int argc, char** argv, Options& options) {
    for (int index = 1; index < argc; ++index) {
        const std::string arg(argv[index]);
        if (arg == "--output" && index + 1 < argc) {
            options.output = argv[++index];
        } else if (arg == "--warmup-frames" && index + 1 < argc) {
            try {
                options.warmup_frames = std::stoi(argv[++index]);
            } catch (...) {
                return false;
            }
        } else {
            return false;
        }
    }
    return !options.output.empty() && options.warmup_frames >= 0;
}

void save_rgb_ppm(const rs2::video_frame& color, const std::filesystem::path& output) {
    std::filesystem::create_directories(output.parent_path());
    std::ofstream stream(output, std::ios::binary | std::ios::trunc);
    if (!stream) throw std::runtime_error("Could not open output image: " + output.string());
    stream << "P6\n" << color.get_width() << ' ' << color.get_height() << "\n255\n";
    stream.write(static_cast<const char*>(color.get_data()),
                 static_cast<std::streamsize>(color.get_width()) * color.get_height() * 3);
    if (!stream) throw std::runtime_error("Could not write output image: " + output.string());
}

}  // namespace

int main(int argc, char** argv) try {
    Options options;
    if (!parse_args(argc, argv, options)) {
        usage(argv[0]);
        return 2;
    }

    rs2::pipeline pipeline;
    rs2::config config;
    config.enable_stream(RS2_STREAM_COLOR, 640, 480, RS2_FORMAT_RGB8, 15);
    config.enable_stream(RS2_STREAM_DEPTH, 640, 480, RS2_FORMAT_Z16, 15);
    const rs2::pipeline_profile profile = pipeline.start(config);
    const rs2::device device = profile.get_device();
    rs2::align align_to_color(RS2_STREAM_COLOR);

    rs2::frameset aligned;
    for (int index = 0; index <= options.warmup_frames; ++index) {
        aligned = align_to_color.process(pipeline.wait_for_frames(5000));
    }
    const rs2::video_frame color = aligned.get_color_frame();
    const rs2::depth_frame depth = aligned.get_depth_frame();
    if (!color || !depth) throw std::runtime_error("Aligned RGB-D keyframe is incomplete");

    save_rgb_ppm(color, options.output);
    const float center_depth = depth.get_distance(depth.get_width() / 2, depth.get_height() / 2);
    pipeline.stop();

    std::cout << "keyframe.path=" << options.output << '\n'
              << "keyframe.color=640x480@15\n"
              << "keyframe.depth=640x480@15\n"
              << "keyframe.aligned=true\n"
              << "keyframe.center_depth_m=" << center_depth << '\n';
    if (device.supports(RS2_CAMERA_INFO_SERIAL_NUMBER)) {
        std::cout << "camera.serial=" << device.get_info(RS2_CAMERA_INFO_SERIAL_NUMBER) << '\n';
    }
    return 0;
} catch (const rs2::error& error) {
    std::cerr << "RealSense error in " << error.get_failed_function() << ": " << error.what() << '\n';
    return 1;
} catch (const std::exception& error) {
    std::cerr << "Error: " << error.what() << '\n';
    return 1;
}
