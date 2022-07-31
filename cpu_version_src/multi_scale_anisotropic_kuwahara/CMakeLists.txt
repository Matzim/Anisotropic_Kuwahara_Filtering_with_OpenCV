add_library(MultiAnisotropicKuwahara STATIC
    ImagePyramid.hh
    ImagePyramid.cpp
)

target_compile_features(MultiAnisotropicKuwahara PUBLIC cxx_std_17)
target_link_libraries(MultiAnisotropicKuwahara PRIVATE common_options ${OpenCV_LIBS})