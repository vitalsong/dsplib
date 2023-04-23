function(FindFFTW)

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
endfunction(FindFFTW)
