cmake_minimum_required(VERSION 3.1)
project(dsplib LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)

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

#enable_testing()
if (CMAKE_TESTING_ENABLED)
    add_subdirectory(tests)
endif ()
