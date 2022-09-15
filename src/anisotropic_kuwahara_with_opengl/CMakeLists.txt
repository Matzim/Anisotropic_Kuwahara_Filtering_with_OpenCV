add_library(opengl_anisotropic_kuwahara STATIC
    program.cpp
    program.hh
    screen.hh
    screen.cpp
)

target_compile_features(opengl_anisotropic_kuwahara PUBLIC cxx_std_17)
target_link_libraries(opengl_anisotropic_kuwahara PRIVATE common_options OpenGL::GL GLEW ${OPENGL_LIBRARIES})