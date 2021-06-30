cmake_minimum_required(VERSION 3.4)
project(dsplib LANGUAGES CXX VERSION 0.2.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(DSPLIB_BUILD_EXAMPLES "Build all examples." OFF)

set(SOURCES
    src/arr_cmplx.cpp
    src/arr_real.cpp
    src/slice.cpp
    src/awgn.cpp
    src/dft-tables.cpp
    src/fft.cpp
    src/fir.cpp
    src/hilbert.cpp
    src/ifft.cpp
    src/math.cpp
    src/window.cpp
    src/xcorr.cpp
    src/medfilt.cpp
    src/interp.cpp
    src/dft.cpp
    src/utils.cpp
    src/tuner.cpp
)

add_library(${PROJECT_NAME} ${SOURCES})

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wno-sign-compare)
endif()

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_compile_definitions(${PROJECT_NAME} PUBLIC _USE_MATH_DEFINES)

option(FETCHCONTENT_UPDATES_DISCONNECTED "" ON)

if (DSPLIB_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    add_subdirectory(tests)
endif()
