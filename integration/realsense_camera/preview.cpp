#include <librealsense2/rs.hpp>

#include <csignal>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <vector>

namespace {
volatile std::sig_atomic_t running = 1;
void stop_handler(int) { running = 0; }

std::vector<std::uint8_t> render_point_cloud(const rs2::points& points,
                                             const rs2::video_frame& color) {
    constexpr int width = 640;
    constexpr int height = 480;
    constexpr float yaw = 0.35F;
    constexpr float pitch = -0.12F;
    constexpr float focal = 390.0F;
    const float cos_yaw = std::cos(yaw);
    const float sin_yaw = std::sin(yaw);
    const float cos_pitch = std::cos(pitch);
    const float sin_pitch = std::sin(pitch);

    std::vector<std::uint8_t> image(width * height * 3, 12);
    std::vector<float> z_buffer(width * height, std::numeric_limits<float>::infinity());
    const rs2::vertex* vertices = points.get_vertices();
    const rs2::texture_coordinate* texture = points.get_texture_coordinates();
    const auto* rgb = static_cast<const std::uint8_t*>(color.get_data());

    for (std::size_t index = 0; index < points.size(); index += 2) {
        const rs2::vertex& point = vertices[index];
        if (point.z <= 0.1F || point.z > 8.0F) continue;

        const float x1 = cos_yaw * point.x + sin_yaw * point.z;
        const float z1 = -sin_yaw * point.x + cos_yaw * point.z;
        const float y2 = cos_pitch * point.y - sin_pitch * z1;
        const float z2 = sin_pitch * point.y + cos_pitch * z1;
        if (z2 <= 0.1F) continue;

        const int screen_x = static_cast<int>(width * 0.34F + focal * x1 / z2);
        const int screen_y = static_cast<int>(height * 0.47F + focal * y2 / z2);
        if (screen_x < 0 || screen_x >= width || screen_y < 0 || screen_y >= height) continue;
        const int pixel = screen_y * width + screen_x;
        if (z2 >= z_buffer[pixel]) continue;

        const int texture_x = static_cast<int>(texture[index].u * width + 0.5F);
        const int texture_y = static_cast<int>(texture[index].v * height + 0.5F);
        if (texture_x < 0 || texture_x >= width || texture_y < 0 || texture_y >= height) continue;
        const int source = (texture_y * width + texture_x) * 3;
        const int target = pixel * 3;
        z_buffer[pixel] = z2;
        image[target] = rgb[source];
        image[target + 1] = rgb[source + 1];
        image[target + 2] = rgb[source + 2];
    }
    return image;
}
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
    rs2::colorizer depth_colorizer;
    rs2::pointcloud point_cloud;

    int frame_index = 0;
    while (running) {
        rs2::frameset frames = align_to_color.process(pipeline.wait_for_frames(5000));
        const rs2::video_frame color = frames.get_color_frame();
        const rs2::depth_frame depth = frames.get_depth_frame();
        if (!color || !depth) continue;
        const rs2::video_frame colorized_depth = depth_colorizer.colorize(depth);
        point_cloud.map_to(color);
        const rs2::points points = point_cloud.calculate(depth);
        const std::vector<std::uint8_t> point_cloud_image = render_point_cloud(points, color);

        std::cout.write(static_cast<const char*>(color.get_data()), 640 * 480 * 3);
        std::cout.write(static_cast<const char*>(colorized_depth.get_data()), 640 * 480 * 3);
        std::cout.write(reinterpret_cast<const char*>(point_cloud_image.data()),
                        static_cast<std::streamsize>(point_cloud_image.size()));
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
