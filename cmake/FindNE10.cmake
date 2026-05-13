# - Find the NE10 library
#
# Usage:
#   find_package(NE10 [REQUIRED] [QUIET] )
#     
# It sets the following variables:
#   NE10_FOUND               ... true if ne10 is found on the system
#   NE10_LIBRARIES           ... full path to ne10 library
#   NE10_INCLUDES            ... ne10 include directory
#
# The following variables will be checked by the function
#   NE10_ROOT               ... if set, the libraries are exclusively searched under this path
#
#If environment variable NE10DIR is specified, it has same effect as NE10_ROOT
if( NOT NE10_ROOT AND ENV{NE10DIR} )
  set( NE10_ROOT $ENV{NE10DIR} )
endif()

# search only static lib
set( CMAKE_FIND_LIBRARY_SUFFIXES_SAV ${CMAKE_FIND_LIBRARY_SUFFIXES} )
set( CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_STATIC_LIBRARY_SUFFIX} )

if( NE10_ROOT )
  find_library(NE10_LIB
    NAMES "NE10" "ne10"
    PATHS ${NE10_ROOT}
    PATH_SUFFIXES "lib" "lib64"
    NO_DEFAULT_PATH
  )
  find_path(NE10_INCLUDES
    NAMES "NE10.h"
    PATHS ${NE10_ROOT}
    PATH_SUFFIXES "include"
    NO_DEFAULT_PATH
  )
else()
  find_library(NE10_LIB
    NAMES "NE10" "ne10"
    PATHS ${LIB_INSTALL_DIR}
  )
  find_path(NE10_INCLUDES
    NAMES "NE10.h"
    PATHS ${INCLUDE_INSTALL_DIR}
  )
endif()

set(CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES_SAV})

if (TARGET NE10)
  if(NOT TARGET ne10::ne10)
    add_library(ne10::ne10 INTERFACE IMPORTED)
    target_link_libraries(ne10::ne10 INTERFACE NE10)
  endif()
  set(NE10_LIBRARIES ne10::ne10)
endif()

if(NE10_LIB AND NE10_INCLUDES AND NOT TARGET ne10::ne10)
  add_library(ne10::ne10 UNKNOWN IMPORTED)
  set_target_properties(ne10::ne10 PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${NE10_INCLUDES}"
      IMPORTED_LOCATION "${NE10_LIB}"
  )
  set(NE10_LIBRARIES ne10::ne10)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(NE10 DEFAULT_MSG NE10_LIBRARIES)
