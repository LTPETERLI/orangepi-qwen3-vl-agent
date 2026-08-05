#include "image_utils.h"

#include <algorithm>
#include <cmath>
#include <cstring>

extern "C" int get_image_size(image_buffer_t* image) {
    if (!image) return 0;
    switch (image->format) {
        case IMAGE_FORMAT_GRAY8: return image->width * image->height;
        case IMAGE_FORMAT_RGB888: return image->width * image->height * 3;
        case IMAGE_FORMAT_RGBA8888: return image->width * image->height * 4;
        case IMAGE_FORMAT_YUV420SP_NV21:
        case IMAGE_FORMAT_YUV420SP_NV12: return image->width * image->height * 3 / 2;
    }
    return 0;
}

extern "C" int convert_image_with_letterbox(image_buffer_t* source,
                                               image_buffer_t* destination,
                                               letterbox_t* letterbox,
                                               char color) {
    if (!source || !destination || !letterbox || !source->virt_addr ||
        !destination->virt_addr || source->format != IMAGE_FORMAT_RGB888 ||
        destination->format != IMAGE_FORMAT_RGB888) {
        return -1;
    }

    const float scale = std::min(
        static_cast<float>(destination->width) / source->width,
        static_cast<float>(destination->height) / source->height);
    const int scaled_width = std::max(1, static_cast<int>(std::round(source->width * scale)));
    const int scaled_height = std::max(1, static_cast<int>(std::round(source->height * scale)));
    const int x_pad = (destination->width - scaled_width) / 2;
    const int y_pad = (destination->height - scaled_height) / 2;

    std::memset(destination->virt_addr, static_cast<unsigned char>(color),
                get_image_size(destination));
    for (int y = 0; y < scaled_height; ++y) {
        const int source_y = std::min(source->height - 1,
                                      static_cast<int>(y / scale));
        for (int x = 0; x < scaled_width; ++x) {
            const int source_x = std::min(source->width - 1,
                                          static_cast<int>(x / scale));
            const auto* input = source->virt_addr + (source_y * source->width + source_x) * 3;
            auto* output = destination->virt_addr +
                           ((y + y_pad) * destination->width + x + x_pad) * 3;
            output[0] = input[0];
            output[1] = input[1];
            output[2] = input[2];
        }
    }

    letterbox->x_pad = x_pad;
    letterbox->y_pad = y_pad;
    letterbox->scale = scale;
    return 0;
}

