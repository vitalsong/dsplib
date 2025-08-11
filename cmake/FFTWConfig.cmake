# - Find the FFTW library
#
# Usage:
#   find_package(FFTW [REQUIRED] [QUIET] )
#     
# It sets the following variables:
#   FFTW_FOUND               ... true if fftw is found on the system
#   FFTW_LIBRARIES           ... full path to fftw library
#   FFTW_INCLUDES            ... fftw include directory
#
# The following variables will be checked by the function
#   FFTW_USE_STATIC_LIBS    ... if true, only static libraries are found
#   FFTW_ROOT               ... if set, the libraries are exclusively searched
#                               under this path
#   FFTW_LIBRARY            ... fftw library to use
#   FFTW_INCLUDE_DIR        ... fftw include directory
#
#If environment variable FFTWDIR is specified, it has same effect as FFTW_ROOT
if( NOT FFTW_ROOT AND ENV{FFTWDIR} )
  set( FFTW_ROOT $ENV{FFTWDIR} )
endif()

# Check if we can use PkgConfig
include(CMakeFindDependencyMacro)
find_dependency(PkgConfig)

#Determine from PKG
if( PKG_CONFIG_FOUND AND NOT FFTW_ROOT )
  pkg_check_modules( PKG_FFTW QUIET "fftw3" )
endif()

#Check whether to search static or dynamic libs
set( CMAKE_FIND_LIBRARY_SUFFIXES_SAV ${CMAKE_FIND_LIBRARY_SUFFIXES} )
if( ${FFTW_USE_STATIC_LIBS} )
  set( CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_STATIC_LIBRARY_SUFFIX} )
else()
  set( CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_SHARED_LIBRARY_SUFFIX} )
endif()

if( FFTW_ROOT )
  find_library(FFTW_LIB
    NAMES "fftw3"
    PATHS ${FFTW_ROOT}
    PATH_SUFFIXES "lib" "lib64"
    NO_DEFAULT_PATH
  )
  find_library(FFTWF_LIB
    NAMES "fftw3f"
    PATHS ${FFTW_ROOT}
    PATH_SUFFIXES "lib" "lib64"
    NO_DEFAULT_PATH
  )
  find_library(FFTWL_LIB
    NAMES "fftw3l"
    PATHS ${FFTW_ROOT}
    PATH_SUFFIXES "lib" "lib64"
    NO_DEFAULT_PATH
  )
  find_path(FFTW_INCLUDES
    NAMES "fftw3.h"
    PATHS ${FFTW_ROOT}
    PATH_SUFFIXES "include"
    NO_DEFAULT_PATH
  )
else()
  find_library(FFTW_LIB
    NAMES "fftw3"
    PATHS ${PKG_FFTW_LIBRARY_DIRS} ${LIB_INSTALL_DIR}
  )
  find_library(FFTWF_LIB
    NAMES "fftw3f"
    PATHS ${PKG_FFTW_LIBRARY_DIRS} ${LIB_INSTALL_DIR}
  )
  find_library(FFTWL_LIB
    NAMES "fftw3l"
    PATHS ${PKG_FFTW_LIBRARY_DIRS} ${LIB_INSTALL_DIR}
  )
  find_path(FFTW_INCLUDES
    NAMES "fftw3.h"
    PATHS ${PKG_FFTW_INCLUDE_DIRS} ${INCLUDE_INSTALL_DIR}
  )
endif()

set(FFTW_LIBRARIES ${FFTW_LIB} ${FFTWF_LIB})
if(FFTWL_LIB)
  set(FFTW_LIBRARIES ${FFTW_LIBRARIES} ${FFTWL_LIB})
endif()

 # create imported target
if(NOT TARGET FFTW::fftw3)
    add_library(FFTW::fftw3f UNKNOWN IMPORTED)
    set_target_properties(FFTW::fftw3f PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${FFTW_INCLUDES}"
        IMPORTED_LOCATION "${FFTWF_LIB}"
    )
    add_library(FFTW::fftw3 UNKNOWN IMPORTED)
    set_target_properties(FFTW::fftw3 PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${FFTW_INCLUDES}"
        IMPORTED_LOCATION "${FFTW_LIB}"
    )
    if (FFTWL_LIB)
      add_library(FFTW::fftw3l UNKNOWN IMPORTED)
      set_target_properties(FFTW::fftw3l PROPERTIES
          INTERFACE_INCLUDE_DIRECTORIES "${FFTW_INCLUDES}"
          IMPORTED_LOCATION "${FFTWL_LIB}"
      )
    endif()
endif()

set( CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES_SAV} )
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FFTW DEFAULT_MSG FFTW_INCLUDES FFTW_LIBRARIES)
mark_as_advanced(FFTW_INCLUDES FFTW_LIBRARIES FFTW_LIB FFTWF_LIB FFTWL_LIB)
