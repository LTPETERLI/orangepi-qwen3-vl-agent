cmake_minimum_required(VERSION 3.16)

project(orangepi_vlm_chat LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(RK_SDK_PREFIX "" CACHE PATH "Application-private RKNN/RKLLM SDK prefix")
if(NOT RK_SDK_PREFIX)
  message(FATAL_ERROR "RK_SDK_PREFIX is required")
endif()

foreach(required_file
    "${RK_SDK_PREFIX}/include/rkllm.h"
    "${RK_SDK_PREFIX}/lib/librkllmrt.so")
  if(NOT EXISTS "${required_file}")
    message(FATAL_ERROR "Missing private SDK artifact: ${required_file}")
  endif()
endforeach()

find_package(Threads REQUIRED)

add_executable(orangepi-text-chat main.cpp)

target_include_directories(orangepi-text-chat PRIVATE
  "${RK_SDK_PREFIX}/include"
)

target_compile_options(orangepi-text-chat PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(orangepi-text-chat PRIVATE
  "${RK_SDK_PREFIX}/lib/librkllmrt.so"
  Threads::Threads
  dl
  gomp
)

set_target_properties(orangepi-text-chat PROPERTIES
  BUILD_RPATH "${RK_SDK_PREFIX}/lib"
  INSTALL_RPATH "$ORIGIN/../lib"
)
