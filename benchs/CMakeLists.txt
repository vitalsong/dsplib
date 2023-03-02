cmake_minimum_required(VERSION 3.10)
project(dsplib-benchs LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_SOURCE_DIR}/cmake/CPM.cmake)

cpmaddpackage(NAME benchmark
  GIT_REPOSITORY "https://github.com/google/benchmark.git"
  VERSION 1.7.0
  OPTIONS
  "BENCHMARK_ENABLE_TESTING OFF"
  "HAVE_POSIX_REGEX OFF"
  "HAVE_STD_REGEX OFF"
  "BENCHMARK_ENABLE_INSTALL OFF"
  "BENCHMARK_INSTALL_DOCS OFF"
)

set(SOURCES
    main.cpp
    adaptive.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(${PROJECT_NAME} dsplib benchmark)
