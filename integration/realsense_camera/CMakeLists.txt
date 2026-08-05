cmake_minimum_required(VERSION 3.16)
project(realsense_camera_check LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(REALSENSE_PREFIX "" CACHE PATH "Project-private librealsense prefix")
set(RKNN_PREFIX "" CACHE PATH "Project-private RKNN runtime prefix")
if(NOT REALSENSE_PREFIX)
  message(FATAL_ERROR "REALSENSE_PREFIX is required")
endif()
if(NOT RKNN_PREFIX)
  message(FATAL_ERROR "RKNN_PREFIX is required")
endif()

find_package(realsense2 REQUIRED CONFIG PATHS "${REALSENSE_PREFIX}/lib/cmake/realsense2" NO_DEFAULT_PATH)

add_executable(realsense-camera-check main.cpp)
target_compile_options(realsense-camera-check PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(realsense-camera-check PRIVATE realsense2::realsense2)
set_target_properties(realsense-camera-check PROPERTIES
  BUILD_RPATH "${REALSENSE_PREFIX}/lib"
  INSTALL_RPATH "$ORIGIN/../runtime/librealsense-2.56.5-rsusb/lib"
)

add_executable(realsense-keyframe keyframe.cpp)
target_compile_options(realsense-keyframe PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(realsense-keyframe PRIVATE realsense2::realsense2)
set_target_properties(realsense-keyframe PROPERTIES
  BUILD_RPATH "${REALSENSE_PREFIX}/lib"
  INSTALL_RPATH "$ORIGIN/../runtime/librealsense-2.56.5-rsusb/lib"
)

set(YOLO_VENDOR_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../object_detector/vendor/rockchip_yolov8")
find_library(RKNN_RUNTIME rknnrt REQUIRED PATHS "${RKNN_PREFIX}/lib" NO_DEFAULT_PATH)

add_executable(realsense-preview
  preview.cpp
  "${CMAKE_CURRENT_SOURCE_DIR}/../object_detector/image_utils.cpp"
  "${YOLO_VENDOR_DIR}/file_utils.c"
  "${YOLO_VENDOR_DIR}/postprocess.cc"
  "${YOLO_VENDOR_DIR}/yolov8.cc"
)
target_compile_options(realsense-preview PRIVATE -Wall -Wextra -Wpedantic)
target_include_directories(realsense-preview PRIVATE "${YOLO_VENDOR_DIR}")
target_link_libraries(realsense-preview PRIVATE realsense2::realsense2 "${RKNN_RUNTIME}" dl)
set_target_properties(realsense-preview PROPERTIES
  BUILD_RPATH "${REALSENSE_PREFIX}/lib;${RKNN_PREFIX}/lib"
  INSTALL_RPATH "$ORIGIN/../runtime/librealsense-2.56.5-rsusb/lib;$ORIGIN/../runtime/rkllm-1.3.0/lib"
)
