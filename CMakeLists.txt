cmake_minimum_required(VERSION 3.4)
project(dsplib LANGUAGES CXX VERSION 0.1.0)
add_compile_options(-Wall -Wextra -Wno-sign-compare)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(DSPLIB_USE_MATPLOT "Use matplotlib cpp tool." OFF)
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
)

add_library(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_compile_definitions(${PROJECT_NAME} PRIVATE _USE_MATH_DEFINES)

if (DSPLIB_USE_MATPLOT)
    set(BUILD_EXAMPLES OFF) 
    set(BUILD_TESTS OFF)
    add_subdirectory(external/matplot)
    target_link_libraries(${PROJECT_NAME} matplot)
endif()

if (DSPLIB_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    add_subdirectory(external/googletest)
    add_subdirectory(tests)
endif()
