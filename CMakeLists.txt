cmake_minimum_required(VERSION 3.10)
project(dsplib LANGUAGES CXX VERSION 0.54.7)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(DSPLIB_USE_FLOAT32 "Use float32 for base type dsplib::real_t" OFF)
option(DSPLIB_NO_EXCEPTIONS "Use the abort() function instead throw" OFF)
set(DSPLIB_FFT_CACHE_SIZE "4" CACHE STRING "LRU cache size for FFT plans")

option(DSPLIB_BUILD_TESTS "Build dsplib tests" OFF)
option(DSPLIB_ASAN_ENABLED "Address sanitizer enabled" OFF)
option(DSPLIB_BUILD_EXAMPLES "Build dsplib examples" OFF)
option(DSPLIB_BUILD_BENCHS "Build dsplib benchs" OFF)

file(GLOB_RECURSE DSPLIB_SOURCES 
    "lib/*.cpp"  
    "lib/*.h" 
    "include/*.h")

add_library(${PROJECT_NAME} ${DSPLIB_SOURCES})

target_compile_definitions(${PROJECT_NAME} PRIVATE 
    "DSPLIB_FFT_CACHE_SIZE=${DSPLIB_FFT_CACHE_SIZE}")

target_include_directories(${PROJECT_NAME} 
    PUBLIC 
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
    PRIVATE lib
)

# check root project
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_LIST_DIR}")
    set(DSPLIB_IS_ROOT ON)
endif()

# base type select
if (DSPLIB_USE_FLOAT32)
    message(STATUS "dsplib: base type of real_t = float32")
else()
    message(STATUS "dsplib: base type of real_t = float64")
endif()

if (DSPLIB_NO_EXCEPTIONS)
    message(STATUS "dsplib: disable exceptions")
    target_compile_options(${PROJECT_NAME} PRIVATE -fno-exceptions)
endif()

# configure <dsplib/defs.h> file 
configure_file(cmake/defs.h.in ${CMAKE_BINARY_DIR}/dsplib/defs.h)

# check warnings
if (DSPLIB_IS_ROOT AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    message(STATUS "dsplib: warnings guard enabled")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wno-sign-compare)
endif()

# adress sanitizer
if (DSPLIB_IS_ROOT AND DSPLIB_ASAN_ENABLED)
    include(cmake/sanitizer.cmake)
    enable_address_sanitizer(TARGET ${PROJECT_NAME})
endif()

if (DSPLIB_IS_ROOT AND DSPLIB_BUILD_TESTS)
    add_subdirectory(tests)
endif()

if (DSPLIB_IS_ROOT AND DSPLIB_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if (DSPLIB_IS_ROOT AND DSPLIB_BUILD_BENCHS)
    add_subdirectory(benchs)
endif()

# install package
include(GNUInstallDirs)

install(TARGETS ${PROJECT_NAME})

install(FILES "include/dsplib.h" 
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(FILES "${CMAKE_BINARY_DIR}/dsplib/defs.h" 
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})

install(DIRECTORY include/${PROJECT_NAME}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING
    PATTERN "*.h"
)
