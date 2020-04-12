cmake_minimum_required(VERSION 3.4)
project(dsplib LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_compile_options(-fPIC)

set(SOURCES
    src/arr_cmplx.cpp
    src/arr_real.cpp
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
    )

add_library(${PROJECT_NAME} STATIC ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_compile_definitions(${PROJECT_NAME} PRIVATE _USE_MATH_DEFINES)

#enable_testing()
if (CMAKE_TESTING_ENABLED)
    add_subdirectory(tests)
endif ()
