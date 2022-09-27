cmake_minimum_required(VERSION 3.10)
project(dsplib LANGUAGES CXX VERSION 0.25.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(DSPLIB_ASAN_ENABLED "Address sanitizer enabled" OFF)
option(DSPLIB_USE_FLOAT32 "Use float32 for base type dsplib::real_t" OFF)
option(DSPLIB_BUILD_TESTS "Build dsplib tests" OFF)
option(DSPLIB_BUILD_EXAMPLES "Build dsplib examples" OFF)
option(DSPLIB_POOL_ALLOCATOR "Use pool allocator for vectors" OFF)

file(GLOB_RECURSE DSPLIB_SOURCES 
    "lib/*.cpp"  
    "lib/*.h" 
    "include/*.h")

add_library(${PROJECT_NAME} ${DSPLIB_SOURCES})

target_include_directories(${PROJECT_NAME} 
    PUBLIC include
    PRIVATE lib)

# check root project
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_LIST_DIR}")
    set(DSPLIB_IS_ROOT ON)
endif()

# base type select
if (DSPLIB_USE_FLOAT32)
    message(STATUS "dsplib: base type of real_t: float32")
    target_compile_definitions(${PROJECT_NAME} PUBLIC "DSPLIB_USE_FLOAT32")
else()
    message(STATUS "dsplib: base type of real_t: float64")
endif()

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

if (DSPLIB_POOL_ALLOCATOR)
    message(STATUS "dsplib: use pool allocator")
    target_compile_definitions(dsplib PUBLIC "DSPLIB_POOL_ALLOCATOR")
endif()

# version header 
set(DSPLIB_VERSION_DIR "${CMAKE_BINARY_DIR}/include/${PROJECT_NAME}")
configure_file(cmake/version.h.in ${DSPLIB_VERSION_DIR}/version.h @ONLY)
target_include_directories(${PROJECT_NAME} PUBLIC ${DSPLIB_VERSION_DIR})
