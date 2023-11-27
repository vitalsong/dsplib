cmake_minimum_required(VERSION 3.10)
project(dsplib LANGUAGES CXX VERSION 0.44.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(DSPLIB_USE_FLOAT32 "Use float32 for base type dsplib::real_t" OFF)
option(DSPLIB_NO_EXCEPTIONS "Use the abort() function instead throw" OFF)

set(DSPLIB_FFT_BACKEND "default" CACHE STRING "Select fft backend: [ne10, default]")
set(DSPLIB_FFT_CACHE_SIZE "4" CACHE STRING "LRU cache size for FFT plans")

option(DSPLIB_BUILD_TESTS "Build dsplib tests" OFF)
option(DSPLIB_ASAN_ENABLED "Address sanitizer enabled" OFF)
option(DSPLIB_BUILD_EXAMPLES "Build dsplib examples" OFF)
option(DSPLIB_BUILD_BENCHS "Build dsplib benchs" OFF)

# check root project
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_LIST_DIR}")
    set(DSPLIB_IS_ROOT ON)
endif()

# check ne10 support
if (DSPLIB_FFT_BACKEND STREQUAL "ne10" AND NOT DSPLIB_USE_FLOAT32)
    message(WARNING "NE10 backend only supported for float32 base type. Select `default`.")
    set(DSPLIB_FFT_BACKEND "default")
endif()

if (DSPLIB_FFT_BACKEND STREQUAL "ne10" AND NOT "${CMAKE_SYSTEM_PROCESSOR}" MATCHES "arm|aarch")
    message(WARNING "NE10 backend only supported for ARM processors. Select `default`.")
    set(DSPLIB_FFT_BACKEND "default")
endif()

if (DSPLIB_FFT_BACKEND STREQUAL "ne10")
    add_subdirectory(external/ne10-dsp)
endif()

file(GLOB DSPLIB_SOURCES 
    "lib/*.cpp"  
    "lib/resample/*.cpp"
)

if (DSPLIB_FFT_BACKEND STREQUAL "ne10")
    file(GLOB DSPLIB_FFT_SOURCES "lib/specific/ne10-fft/*.cpp")
else()
    file(GLOB DSPLIB_FFT_SOURCES "lib/specific/dsplib-fft/*.cpp")
endif()
list(APPEND DSPLIB_SOURCES ${DSPLIB_FFT_SOURCES})

message(STATUS "FFT backend: ${DSPLIB_FFT_BACKEND}")

add_library(${PROJECT_NAME} ${DSPLIB_SOURCES})

if (DSPLIB_FFT_BACKEND STREQUAL "ne10")
    target_link_libraries(${PROJECT_NAME} PRIVATE ne10dsp) #TODO: NE10:DSP alias
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE 
    "DSPLIB_FFT_CACHE_SIZE=${DSPLIB_FFT_CACHE_SIZE}")

target_include_directories(${PROJECT_NAME} 
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
    PRIVATE lib
)

# base type select
if (DSPLIB_USE_FLOAT32)
    message(STATUS "dsplib: base type of real_t: float32")
    target_compile_definitions(${PROJECT_NAME} PUBLIC "DSPLIB_USE_FLOAT32")
else()
    message(STATUS "dsplib: base type of real_t: float64")
endif()

if (DSPLIB_NO_EXCEPTIONS)
    message(STATUS "dsplib: disable exceptions")
    target_compile_options(${PROJECT_NAME} PRIVATE -fno-exceptions)
    target_compile_definitions(${PROJECT_NAME} PUBLIC "DSPLIB_NO_EXCEPTIONS")
else()
    message(STATUS "dsplib: enable exceptions")
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

if (DSPLIB_IS_ROOT AND DSPLIB_BUILD_BENCHS)
    add_subdirectory(benchs)
endif()

target_compile_definitions(${PROJECT_NAME} PUBLIC
    DSPLIB_VERSION="${CMAKE_PROJECT_VERSION}"
    DSPLIB_MAJOR_VERSION=${CMAKE_PROJECT_VERSION_MAJOR}
    DSPLIB_MINOR_VERSION=${CMAKE_PROJECT_VERSION_MINOR}
    DSPLIB_PATCH_VERSION=${CMAKE_PROJECT_VERSION_PATCH}
)

# install package
# FIXME: install problem with ne10dsp dependency
if (DSPLIB_FFT_BACKEND STREQUAL "default")
    include(cmake/InstallTools.cmake)

    ConfigInstallTarget(${PROJECT_NAME})

    install(FILES "include/dsplib.h" 
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

    install(DIRECTORY include/${PROJECT_NAME}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILES_MATCHING
        PATTERN "*.h"
    )
endif()
