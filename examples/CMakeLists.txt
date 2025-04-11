project(examples)

include(${CMAKE_SOURCE_DIR}/cmake/FindFFTW.cmake)

FindFFTW()

if (DSPLIB_EXCLUDE_FFT AND TARGET FFTW:Double)
    add_subdirectory(fftw-backend)
else()
    add_subdirectory(basics)
endif()