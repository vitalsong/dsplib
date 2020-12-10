cmake_minimum_required(VERSION 3.4)
project(dsplib LANGUAGES CXX VERSION 0.1.0)
add_compile_options(-Wall -Wextra -Wno-sign-compare)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(USE_MATPLOTLIB_TOOL "Use matplotlib cpp tool." OFF)
option(BUILD_EXAMPLES "Build all examples." OFF)

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

add_library(${PROJECT_NAME} STATIC ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_compile_definitions(${PROJECT_NAME} PRIVATE _USE_MATH_DEFINES)

if (USE_MATPLOTLIB_TOOL)
    target_include_directories(${PROJECT_NAME} PUBLIC tools)
    find_package(Python3 COMPONENTS Development NumPy)
    target_link_libraries(${PROJECT_NAME} Python3::Python Python3::NumPy)
endif()

if (BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    add_subdirectory(tests)
endif()
