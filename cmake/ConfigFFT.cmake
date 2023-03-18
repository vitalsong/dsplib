function(CheckFFTW3)

  find_package(PkgConfig)
  if(PKG_CONFIG_FOUND)
    pkg_check_modules(PKG_FFTW QUIET "fftw3")
  endif()

  find_library(FFTW_DOUBLE_LIB
    NAMES "fftw3"
    PATHS ${PKG_FFTW_LIBRARY_DIRS} ${LIB_INSTALL_DIR})

  find_library(FFTW_FLOAT_LIB
    NAMES "fftw3f"
    PATHS ${PKG_FFTW_LIBRARY_DIRS} ${LIB_INSTALL_DIR})

  find_path(FFTW_INCLUDE_DIRS
    NAMES "fftw3.h"
    PATHS ${PKG_FFTW_INCLUDE_DIRS} ${INCLUDE_INSTALL_DIR})

  if(FFTW_DOUBLE_LIB)
    add_library(FFTW:Double INTERFACE IMPORTED)
    set_target_properties(FFTW:Double
      PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${FFTW_INCLUDE_DIRS}"
                 INTERFACE_LINK_LIBRARIES "${FFTW_DOUBLE_LIB}")
  endif()

  if(FFTW_FLOAT_LIB)
    add_library(FFTW:Float INTERFACE IMPORTED)
    set_target_properties(FFTW:Float
      PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${FFTW_INCLUDE_DIRS}"
                 INTERFACE_LINK_LIBRARIES "${FFTW_FLOAT_LIB}")
  endif()
endfunction(CheckFFTW3)

function(ConfigFFTBackend TARGET USE_FLOAT32)
  CheckFFTW3()
  if(NOT USE_FLOAT32 AND TARGET FFTW:Double)
    target_link_libraries(${TARGET} FFTW:Double)
    target_compile_definitions(${TARGET} PRIVATE -DFFTW_DOUBLE)
    message(STATUS "FFT backend: fftw3-double")
  elseif(USE_FLOAT32 AND TARGET FFTW:Float)
    target_link_libraries(${TARGET} FFTW:Float)
    target_compile_definitions(${TARGET} PRIVATE -DFFTW_FLOAT)
    message(STATUS "FFT backend: fftw3-float")
  else()
    message(STATUS "FFT backend: default")
  endif()
endfunction(ConfigFFTBackend)
