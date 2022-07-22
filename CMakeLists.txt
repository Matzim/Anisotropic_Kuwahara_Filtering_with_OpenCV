cmake_minimum_required(VERSION 3.0)
project(oil_paint_filtering VERSION 0.1.0 LANGUAGES CXX)

# set(CMAKE_BUILD_TYPE Release)
set(CMAKE_BUILD_TYPE Debug)


add_library(common_options INTERFACE)
target_compile_options(common_options INTERFACE
    "-Wall"
    "-Wextra"
    "-W"
    "-std=c++17"
    $<$<CONFIG:Release>: -Werror -O3 -march=native>
    $<$<CONFIG:Debug>: -g3 -Og>
)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_subdirectory(src)