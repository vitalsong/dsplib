# - Find the FFTW library
#
# Usage:
#   find_package(FFTW3 [REQUIRED] [QUIET] )
#     
# It sets the following variables:
#   FFTW3_FOUND              ... true if fftw is found on the system
#   FFTW3_LIBRARIES          ... full paths to found fftw libraries
#   FFTW3_INCLUDES           ... fftw include directory
#
# The following variables will be checked by the function
#   FFTW3_USE_STATIC_LIBS    ... if true, only static libraries are found
#   FFTW3_ROOT               ... if set, the libraries are exclusively searched under this path
#

#If environment variable FFTWDIR is specified, it has same effect as FFTW3_ROOT
if( NOT FFTW3_ROOT AND ENV{FFTWDIR} )
  set( FFTW3_ROOT $ENV{FFTWDIR} )
endif()

find_package(PkgConfig QUIET)

#Determine from PKG
if(PKG_CONFIG_FOUND AND NOT FFTW3_ROOT)
  pkg_check_modules(PKG_FFTW QUIET "fftw3")
endif()

#Check whether to search static or dynamic libs
set(CMAKE_FIND_LIBRARY_SUFFIXES_SAV ${CMAKE_FIND_LIBRARY_SUFFIXES})
if(FFTW3_USE_STATIC_LIBS)
  set(CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_STATIC_LIBRARY_SUFFIX})
else()
  set(CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_SHARED_LIBRARY_SUFFIX})
endif()

if(FFTW3_ROOT)
  find_library(FFTW_LIB
    NAMES "fftw3"
    PATHS ${FFTW3_ROOT}
    PATH_SUFFIXES "lib" "lib64"
    NO_DEFAULT_PATH
  )
  find_library(FFTWF_LIB
    NAMES "fftw3f"
    PATHS ${FFTW3_ROOT}
    PATH_SUFFIXES "lib" "lib64"
    NO_DEFAULT_PATH
  )
  find_library(FFTWL_LIB
    NAMES "fftw3l"
    PATHS ${FFTW3_ROOT}
    PATH_SUFFIXES "lib" "lib64"
    NO_DEFAULT_PATH
  )
  find_path(FFTW3_INCLUDES
    NAMES "fftw3.h"
    PATHS ${FFTW3_ROOT}
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
  find_path(FFTW3_INCLUDES
    NAMES "fftw3.h"
    PATHS ${PKG_FFTW_INCLUDE_DIRS} ${INCLUDE_INSTALL_DIR}
  )
endif()

set(FFTW3_LIBRARIES)

if (FFTW_LIB)
  list(APPEND FFTW3_LIBRARIES ${FFTW_LIB})
endif()

if (FFTWF_LIB)
  list(APPEND FFTW3_LIBRARIES ${FFTWF_LIB})
endif()

if (FFTWL_LIB)
  list(APPEND FFTW3_LIBRARIES ${FFTWL_LIB})
endif()

if (FFTW_LIB AND NOT TARGET FFTW3::fftw3)
  add_library(FFTW3::fftw3 UNKNOWN IMPORTED)
  set_target_properties(FFTW3::fftw3 PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${FFTW3_INCLUDES}"
      IMPORTED_LOCATION "${FFTW_LIB}"
  )
endif()

if (FFTWF_LIB AND NOT TARGET FFTW3::fftw3f)
  add_library(FFTW3::fftw3f UNKNOWN IMPORTED)
  set_target_properties(FFTW3::fftw3f PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${FFTW3_INCLUDES}"
      IMPORTED_LOCATION "${FFTWF_LIB}"
  )
endif()

if (FFTWL_LIB AND NOT TARGET FFTW3::fftw3l)
  add_library(FFTW3::fftw3l UNKNOWN IMPORTED)
  set_target_properties(FFTW3::fftw3l PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${FFTW3_INCLUDES}"
      IMPORTED_LOCATION "${FFTWL_LIB}"
  )
endif()

if(NOT TARGET fftw::fftw)
  add_library(fftw::fftw INTERFACE IMPORTED)

  if(TARGET FFTW3::fftw3)
    target_link_libraries(fftw::fftw INTERFACE FFTW3::fftw3)
  endif()

  if(TARGET FFTW3::fftw3f)
    target_link_libraries(fftw::fftw INTERFACE FFTW3::fftw3f)
  endif()

  if(TARGET FFTW3::fftw3l)
    target_link_libraries(fftw::fftw INTERFACE FFTW3::fftw3l)
  endif()
endif()

set(CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES_SAV})
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FFTW3 DEFAULT_MSG FFTW3_INCLUDES FFTW_LIB)
