cmake_minimum_required(VERSION 3.1)
project(dsplib-examples LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)

set(SOURCES
    spectrum.cpp
    )

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../tools/plot)
include_directories(/usr/include/python2.7)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} dsplib python2.7)
