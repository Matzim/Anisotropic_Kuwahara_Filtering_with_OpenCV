cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
project(image_abstraction VERSION 0.1.0 LANGUAGES CXX)

# set(CMAKE_BUILD_TYPE Release)
set(CMAKE_BUILD_TYPE Debug)


add_library(common_options INTERFACE)
target_compile_options(common_options INTERFACE
    "-Wall"
    "-Wextra"
    "-W"
    "-m64"
    $<$<CONFIG:Release>: -Werror -O3 -march=native -fopt-info-vec-optimized>
    $<$<CONFIG:Debug>: -g3 -Og>
)

set(OpenGL_GL_PREFERENCE LEGACY)

file(GLOB_RECURSE ALL_SOURCE_FILES *.cpp *.hh)
add_custom_target(
        clangformat
        COMMAND /usr/bin/clang-format
        -style=Google
        -i
        ${ALL_SOURCE_FILES}
)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_subdirectory(src)

