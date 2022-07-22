add_executable(main main.cpp)

target_compile_features(main PUBLIC cxx_std_17)
target_link_libraries(main PRIVATE common_options ${OpenCV_LIBS})