cmake_minimum_required(VERSION 3.0)
project(tests LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)

set(SOURCES
    main.cpp
    arr_cmpl_test.cpp
    arr_real_test.cpp
    math_test.cpp
    )

#set(BUILD_GMOCK OFF)
add_subdirectory(googletests)
include_directories(googletests/googletest/include)
add_test(${PROJECT_NAME} "dsplib-tests")

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} dsplib gtest)
