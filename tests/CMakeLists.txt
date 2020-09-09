cmake_minimum_required(VERSION 3.0)
project(dsplib-tests LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)

set(SOURCES
    main.cpp
    arr_cmpl_test.cpp
    arr_real_test.cpp
    math_test.cpp
    fft_test.cpp
    base_test.cpp
    dft_test.cpp
)

set(BUILD_GMOCK OFF)
add_subdirectory(googletests)
include_directories(googletests/googletest/include)

add_test(${PROJECT_NAME} ${PROJECT_NAME})
add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} dsplib gtest)
