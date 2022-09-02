# https://github.com/vitalsong/bastard

if (NOT DEFINED BASTARD_DIR)
    set(BASTARD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmake/bastard")
endif()

include("${BASTARD_DIR}/bastard.cmake")
message("Bastard version: ${BASTARD_VERSION}")
