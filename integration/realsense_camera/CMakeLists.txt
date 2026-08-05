cmake_minimum_required(VERSION 3.16)
project(realsense_camera_check LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(REALSENSE_PREFIX "" CACHE PATH "Project-private librealsense prefix")
if(NOT REALSENSE_PREFIX)
  message(FATAL_ERROR "REALSENSE_PREFIX is required")
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
