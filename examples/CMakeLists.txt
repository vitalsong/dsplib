cmake_minimum_required(VERSION 3.1)
project(dsplib-examples LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

include(${CMAKE_SOURCE_DIR}/cmake/fetch_helper.cmake)
FetchHelper(matplotplusplus https://github.com/alandefreitas/matplotplusplus.git origin/master)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} dsplib matplot)
