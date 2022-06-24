cmake_minimum_required(VERSION 3.10)
include(cmake/bastard_setup.cmake)
bastard_setup()
project(${BASTARD_PACKAGE_NAME})

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(${BASTARD_PACKAGE_NAME} PRIVATE -Wall -Wextra -Wno-sign-compare)
endif()

if (DSPLIB_ASAN_ENABLED)
    include(cmake/sanitizer.cmake)
    enable_address_sanitizer(TARGET dsplib)
endif()