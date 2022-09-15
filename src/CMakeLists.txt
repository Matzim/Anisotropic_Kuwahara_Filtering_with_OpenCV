add_subdirectory(anisotropic_kuwahara)
add_subdirectory(anisotropic_kuwahara_with_opengl)

add_executable(main main.cpp MultithreadedVideoCapture.hh MultithreadedVideoCapture.cpp)
target_compile_features(main PUBLIC cxx_std_17)
target_link_libraries(main PRIVATE
    common_options ${OpenCV_LIBS} MultiAnisotropicKuwahara
)

find_package(OpenGL)

if (OpenGL_FOUND)
    include_directories( ${OPENGL_INCLUDE_DIRS})
    add_executable(main_opengl main_opengl.cpp MultithreadedVideoCapture.hh MultithreadedVideoCapture.cpp)
    target_compile_features(main_opengl PUBLIC cxx_std_17)
    target_link_libraries(main_opengl PRIVATE
        common_options utilities ${OpenCV_LIBS} OpenGL::GL opengl_anisotropic_kuwahara
    )
endif()
