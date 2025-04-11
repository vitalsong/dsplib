cmake_minimum_required(VERSION 3.10)
project(fftw-backend-example LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(dsplib-fftw fft.cpp)
target_link_libraries(dsplib-fftw PUBLIC dsplib FFTW:Double)

add_executable(${PROJECT_NAME} main.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC dsplib dsplib-fftw)