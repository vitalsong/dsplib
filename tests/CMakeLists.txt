cmake_minimum_required(VERSION 3.11)
project(dsplib-tests LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)

include(${CMAKE_SOURCE_DIR}/cmake/fetch_helper.cmake)
FetchHelper(googletest https://github.com/google/googletest.git origin/master)

set(SOURCES
    main.cpp
    arr_cmpl_test.cpp
    arr_real_test.cpp
    math_test.cpp
    fft_test.cpp
    base_test.cpp
    dft_test.cpp
    utils_test.cpp
    slice_test.cpp
)

add_test(${PROJECT_NAME} ${PROJECT_NAME})
add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} dsplib gtest)
