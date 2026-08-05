#include <librealsense2/rs.hpp>

#include <csignal>
#include <iostream>

namespace {
volatile std::sig_atomic_t running = 1;
void stop_handler(int) { running = 0; }
}  // namespace

int main() try {
    std::signal(SIGTERM, stop_handler);
    std::signal(SIGINT, stop_handler);
    std::signal(SIGPIPE, stop_handler);

    rs2::pipeline pipeline;
    rs2::config config;
    config.enable_stream(RS2_STREAM_COLOR, 640, 480, RS2_FORMAT_RGB8, 15);
    config.enable_stream(RS2_STREAM_DEPTH, 640, 480, RS2_FORMAT_Z16, 15);
    pipeline.start(config);
    rs2::align align_to_color(RS2_STREAM_COLOR);

    int frame_index = 0;
    while (running) {
        rs2::frameset frames = align_to_color.process(pipeline.wait_for_frames(5000));
        const rs2::video_frame color = frames.get_color_frame();
        const rs2::depth_frame depth = frames.get_depth_frame();
        if (!color || !depth) continue;

        std::cout.write(static_cast<const char*>(color.get_data()), 640 * 480 * 3);
        std::cout.flush();
        if (!std::cout) break;

        if (++frame_index % 5 == 0) {
            const float center_depth = depth.get_distance(320, 240);
            std::cerr << "depth=" << center_depth << '\n' << std::flush;
        }
    }
    pipeline.stop();
    return 0;
} catch (const rs2::error& error) {
    std::cerr << "error=" << error.what() << '\n';
    return 1;
} catch (const std::exception& error) {
    std::cerr << "error=" << error.what() << '\n';
    return 1;
}
