#---------------------------------------------------
# Basic setup file for bastard package manager
# This file is the minimum and sufficient requirement for bastard support
#---------------------------------------------------

# if the package manager has already been configured
if(NOT ${BASTARD_DIR} STREQUAL "")
    include("${BASTARD_DIR}/bastard.cmake")
    return()
endif()

set(BASTARD_SETUP_VERSION "0.3.2")
set(BASTARD_GIT_URI "http://github.com/vitalsong/bastard.git")
set(BASTARD_CACHE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/.deps/bastard")

#---------------------------------------------------
# if the bastard was cached
if (EXISTS "${BASTARD_CACHE_DIR}/cmake/bastard.cmake")
    message("Using bastard from cache: ${BASTARD_CACHE_DIR}")
    set(BASTARD_DIR "${BASTARD_CACHE_DIR}/cmake")
    include("${BASTARD_DIR}/bastard.cmake")
    message("Bastard version: ${BASTARD_VERSION}")
    return()
endif()

#---------------------------------------------------
# load bastard from server
message("Trying to load bastard from server...")
message("Bastard uri: ${BASTARD_GIT_URI}")

#---------------------------------------------------
# check lock bastard version
if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/bastard.lock)
    file(READ ${CMAKE_CURRENT_SOURCE_DIR}/bastard.lock bastard_config)
elseif (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/bastard.toml)
    file(READ ${CMAKE_CURRENT_SOURCE_DIR}/bastard.toml bastard_config)
endif()

string(REGEX MATCH "[ ]?bastard[ ]?=[ ]?[\"]?([0-9]+\.[0-9]+\.[0-9]+)[\"]?" bastard_match ${bastard_config})
set(bastard_version ${CMAKE_MATCH_1})

if ("${bastard_version}" STREQUAL "")
    set(git_cmd git clone --depth 1 ${BASTARD_GIT_URI} ${BASTARD_CACHE_DIR})
else()
    set(git_cmd git clone --depth 1 --branch "v${bastard_version}" ${BASTARD_GIT_URI} ${BASTARD_CACHE_DIR})
endif()

execute_process(COMMAND ${git_cmd} RESULT_VARIABLE exit_code ERROR_QUIET)

if(exit_code EQUAL 0)
    message("The bastard has been loaded: ${BASTARD_CACHE_DIR}")
    set(BASTARD_DIR "${BASTARD_CACHE_DIR}/cmake")
else()
    message(FATAL_ERROR "Failed to load bastard from server")
endif()

#---------------------------------------------------
# include bastard.cmake
include("${BASTARD_DIR}/bastard.cmake")
message("Bastard version: ${BASTARD_VERSION}")
