add_library(utilities STATIC
    utils.hh
    utils.cpp
)

target_compile_features(utilities PUBLIC cxx_std_11)
target_link_libraries(utilities PRIVATE common_options ${OpenCV_LIBS})

add_library(MultiAnisotropicKuwahara STATIC
    anisotropic_kuwahara.cpp
    anisotropic_kuwahara.hh
    local_structure_estimation.cpp
    local_structure_estimation.hh
    ellipses.cpp
    ellipses.hh
    utils.cpp
    utils.hh
)

target_compile_features(MultiAnisotropicKuwahara PUBLIC cxx_std_17)
target_link_libraries(MultiAnisotropicKuwahara PRIVATE common_options ${OpenCV_LIBS})