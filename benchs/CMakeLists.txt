cmake_minimum_required(VERSION 3.10)
project(dsplib-benchs LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_SOURCE_DIR}/cmake/CPM.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/FindFFTW.cmake)

cpmaddpackage(NAME benchmark
  GIT_REPOSITORY 
    "https://github.com/google/benchmark.git"
  VERSION 
    "1.7.0"
  OPTIONS
    "BENCHMARK_ENABLE_TESTING OFF"
    "HAVE_POSIX_REGEX OFF"
    "HAVE_STD_REGEX OFF"
    "BENCHMARK_ENABLE_INSTALL OFF"
    "BENCHMARK_INSTALL_DOCS OFF"
)

cpmaddpackage(NAME kissfft
  GIT_REPOSITORY 
    "https://github.com/mborgerding/kissfft.git"
  GIT_TAG 
    "131.1.0"
  OPTIONS
    "KISSFFT_DATATYPE double"
    "KISSFFT_TEST OFF"
    "KISSFFT_STATIC ON"
)

set(SOURCES
    main.cpp
    fft.cpp
    adaptive.cpp
    resample.cpp
    slice.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(${PROJECT_NAME} dsplib benchmark kissfft)
target_compile_definitions(${PROJECT_NAME} PRIVATE KISSFFT_SUPPORT)

FindFFTW()
if(TARGET FFTW:Double)
  target_link_libraries(${PROJECT_NAME} FFTW:Double)
  target_compile_definitions(${PROJECT_NAME} PRIVATE FFTW_SUPPORT)
endif()