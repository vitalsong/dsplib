project(dsplib-test LANGUAGES CXX)

include(${CMAKE_CURRENT_LIST_DIR}/../cmake/CPM.cmake)

option(DSPLIB_COVERAGE "Enable coverage compiler flags" OFF)

CPMAddPackage(NAME googletest
    GIT_REPOSITORY "https://github.com/google/googletest.git"
        VERSION 1.17.0
    OPTIONS 
        "BUILD_GMOCK OFF"
        "INSTALL_GTEST OFF"
        "gtest_force_shared_crt ON"  # fix msvc build
    EXCLUDE_FROM_ALL ON
)

file(GLOB_RECURSE TEST_SOURCES "*.cpp" "*.h")
add_executable(${PROJECT_NAME} ${TEST_SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_LIST_DIR} 
    ${CMAKE_CURRENT_LIST_DIR}/../lib
)

target_link_libraries(${PROJECT_NAME} PRIVATE dsplib GTest::gtest_main)

if (DSPLIB_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(STATUS "dsplib: coverage enabled")
    target_compile_options(dsplib PRIVATE -fprofile-instr-generate -fcoverage-mapping)
    target_compile_options(${PROJECT_NAME} PRIVATE -fprofile-instr-generate -fcoverage-mapping)
    target_link_libraries(${PROJECT_NAME} PRIVATE -fprofile-instr-generate)
endif()

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})
