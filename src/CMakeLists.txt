add_subdirectory(anisotropic_kuwahara)

add_executable(main main.cpp MultithreadedVideoCapture.hh MultithreadedVideoCapture.cpp)
target_compile_features(main PUBLIC cxx_std_17)
target_link_libraries(main PRIVATE
    common_options ${OpenCV_LIBS} MultiAnisotropicKuwahara
)

find_package(OpenGL)
find_package(GLEW)
find_package(GLUT)

if (OpenGL_FOUND AND GLEW_FOUND AND GLUT_FOUND)
    include_directories( ${OPENGL_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})
    add_subdirectory(anisotropic_kuwahara_with_opengl)
    add_executable(main_opengl main_opengl.cpp MultithreadedVideoCapture.hh MultithreadedVideoCapture.cpp)
    target_compile_features(main_opengl PUBLIC cxx_std_11)
    target_link_libraries(main_opengl PRIVATE
        common_options utilities ${OpenCV_LIBS} OpenGL::GL GLEW ${OPENGL_LIBRARIES} GLUT::GLUT opengl_anisotropic_kuwahara
    )
    configure_file(simple_display.shd simple_display.shd COPYONLY)
    configure_file(vertex_shader.shd vertex_shader.shd COPYONLY)
    configure_file(anisotropic_kuwahara_shader.shd anisotropic_kuwahara_shader.shd COPYONLY)
endif()