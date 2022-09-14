add_subdirectory(multi_scale_anisotropic_kuwahara)

add_executable(main main.cpp MultithreadedVideoCapture.hh MultithreadedVideoCapture.cpp)
target_compile_features(main PUBLIC cxx_std_17)
target_link_libraries(main PRIVATE
    common_options ${OpenCV_LIBS} MultiAnisotropicKuwahara
)

find_package(OpenGL REQUIRED COMPONENTS OpenGL)
find_package(GLUT REQUIRED)

if (OpenGL_FOUND AND GLUT_FOUND)
    include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )
    add_executable(main_opengl main_opengl.cpp MultithreadedVideoCapture.hh MultithreadedVideoCapture.cpp)
    target_compile_features(main_opengl PUBLIC cxx_std_17)
    target_link_libraries(main_opengl PRIVATE
        common_options utilities ${OpenCV_LIBS} ${OpenGL_LIBS} ${GLUT_LIBS} 
    )
endif()
