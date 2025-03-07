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
  EXCLUDE_FROM_ALL ON
)

cpmaddpackage(NAME kissfft
  GIT_REPOSITORY 
    "https://github.com/mborgerding/kissfft.git"
  GIT_TAG 
    "131.1.0"
  OPTIONS
    "KISSFFT_DATATYPE double"
    "KISSFFT_TEST OFF"
    "KISSFFT_PKGCONFIG OFF"
    "KISSFFT_STATIC ON"
    "KISSFFT_TOOLS OFF"
  EXCLUDE_FROM_ALL ON
)

cpmaddpackage(NAME pocketfft_ext
  GIT_REPOSITORY 
    "https://github.com/mreineck/pocketfft.git"
  GIT_TAG 
    "0fa0ef591e38c2758e3184c6c23e497b9f732ffa"
  OPTIONS
    "DOWNLOAD_ONLY ON"
  EXCLUDE_FROM_ALL ON
)

add_library(pocketfft INTERFACE)
target_include_directories(pocketfft INTERFACE ${pocketfft_ext_SOURCE_DIR})
target_compile_definitions(pocketfft INTERFACE POCKETFFT_CACHE_SIZE=1 POCKETFFT_NO_MULTITHREADING=1)

set(SOURCES
    main.cpp
    fft.cpp
    ifft.cpp
    adaptive.cpp
    resample.cpp
    slice.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(${PROJECT_NAME} dsplib benchmark kissfft pocketfft)
target_compile_definitions(${PROJECT_NAME} PRIVATE KISSFFT_SUPPORT)

FindFFTW()
if(TARGET FFTW:Double)
  target_link_libraries(${PROJECT_NAME} FFTW:Double)
  target_compile_definitions(${PROJECT_NAME} PRIVATE FFTW_SUPPORT)
endif()