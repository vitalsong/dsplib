cmake_minimum_required(VERSION 3.10)
include(cmake/bastard_setup.cmake)
bastard_setup()
project(dsplib)

option(DSPLIB_ASAN_ENABLED "Address sanitizer enabled" OFF)
option(DSPLIB_USE_FLOAT32 "Use float32 for base type dsplib::real_t" OFF)

# check root project
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_LIST_DIR}")
    set(DSPLIB_IS_ROOT ON)
endif()

# base type select
if (DSPLIB_USE_FLOAT32)
    message(STATUS "dsplib: base type of real_t: float32")
    target_compile_definitions(dsplib PUBLIC "DSPLIB_USE_FLOAT32")
else()
    message(STATUS "dsplib: base type of real_t: float64")
endif()

# check warnings
if (DSPLIB_IS_ROOT AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    message(STATUS "dsplib: warnings guard enabled")
    target_compile_options(dsplib PRIVATE -Wall -Wextra -Wno-sign-compare)
endif()

# adress sanitizer
if (DSPLIB_IS_ROOT AND DSPLIB_ASAN_ENABLED)
    include(cmake/sanitizer.cmake)
    enable_address_sanitizer(TARGET dsplib)
endif()