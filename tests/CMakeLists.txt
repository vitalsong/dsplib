cmake_minimum_required(VERSION 3.10)
project(dsplib-test LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_SOURCE_DIR}/cmake/CPM.cmake)

CPMAddPackage(NAME googletest
    GIT_REPOSITORY "https://github.com/google/googletest.git"
    GIT_TAG release-1.12.1
    OPTIONS "BUILD_GMOCK OFF"
    "gtest_force_shared_crt ON"
)

file(GLOB_RECURSE SOURCES "*.cpp" "*.h")
add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC dsplib gtest)