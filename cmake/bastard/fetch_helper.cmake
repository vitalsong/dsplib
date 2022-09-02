include_guard(GLOBAL)

include(${CMAKE_CURRENT_LIST_DIR}/fetch_content.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/utils.cmake)

set(BASTARD_FETCH_UPDATES_DISCONNECTED ON CACHE BOOL "Disable/enable the update stage")
set(BASTARD_FETCH_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/.deps" CACHE PATH "FetchContent src deps dir")
set(BASTARD_FETCH_BUILD_DIR "${CMAKE_BINARY_DIR}/.deps" CACHE PATH "FetchContent build dir")
set(BASTARD_FETCH_SUBBUILD_DIR "${CMAKE_BINARY_DIR}/.deps" CACHE PATH "FetchContent sub-build dir")

#--------------------------------------------------------------------------------------------
function(GetGitCommit dir result)
    find_package(Git QUIET)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} log --pretty=format:%h -n 1
        WORKING_DIRECTORY ${dir}
        OUTPUT_VARIABLE commit
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE exit_code
    )

    if(NOT exit_code EQUAL "0")
        message(FATAL_ERROR "Failed to get origin URL for '${dir}'")
    endif()

    set(${result} ${commit} PARENT_SCOPE)
endfunction(GetGitCommit)

# ---------------------------------------------------------------------------------------
function(RelPathToAbs relpath out_var)
    find_package(Git QUIET)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} remote get-url origin
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE base_url
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE exit_code
    )

    if(NOT exit_code EQUAL "0")
        message(FATAL_ERROR "Failed to get origin URL for '${CMAKE_CURRENT_SOURCE_DIR}'")
    endif()
    
    string(REGEX MATCH "^git@" match ${base_url})
    if (NOT match STREQUAL "")
        # p.s: function get_filename_component don't allow symbol ":"
        string(FIND ${base_url} ":" symb_pos)
        string(REPLACE ":" "/" base_url ${base_url})
        get_filename_component(url ${relpath} ABSOLUTE BASE_DIR ${base_url})
        string(LENGTH ${url} len)
        string(SUBSTRING ${url} 0 ${symb_pos} host_name)
        MATH(EXPR symb_pos "${symb_pos}+1")
        string(SUBSTRING ${url} ${symb_pos} ${len} path_name)
        set(url "${host_name}:${path_name}")
    else()
        get_filename_component(url ${relpath} ABSOLUTE BASE_DIR ${base_url})
    endif()

    set(${out_var} ${url} PARENT_SCOPE)
endfunction(RelPathToAbs)

# ---------------------------------------------------------------------------------------
function(GetAbsoluteUrl url out_var)
    string(REGEX MATCH "^\.\./" match ${url})
    if (match STREQUAL "")
        set(${out_var} ${url} PARENT_SCOPE)
        return()
    endif()

    RelPathToAbs(${url} absolute_url)
    set(${out_var} "${absolute_url}" PARENT_SCOPE)
endfunction(GetAbsoluteUrl)

#--------------------------------------------------------------------------------------------
# Required Parameters:
# name - Dependency name
# url - Repository url
# tag - tag, branch or commit
function(DeclareHelper name url tag)
    GetAbsoluteUrl(${url} url)
    set(${name}_GIT_URL ${url} CACHE INTERNAL "Git URL for ${name}")

    set(declare_args
        ${name}
        GIT_REPOSITORY ${url}
        GIT_TAG        ${tag}
        GIT_PROGRESS   ON
    )

    BastardFetch_Declare(${declare_args})
endfunction(DeclareHelper)

#--------------------------------------------------------------------------------------------
function(CleanBuildDep dep_name)
    file(REMOVE_RECURSE "${CMAKE_BINARY_DIR}/.deps/${dep_name}")
    file(REMOVE_RECURSE "${CMAKE_BINARY_DIR}/.deps/${dep_name}-subbuild")
endfunction(CleanBuildDep)

#--------------------------------------------------------------------------------------------
function(PopulateHelper package name)
    BastardFetch_GetProperties(${name})
    if(NOT ${name}_POPULATED)
        message("Setting up ${name}...")

        # if cached local
        if(EXISTS "${BASTARD_FETCH_SRC_DIR}/${name}")
            message("${name}: Use local cache")
            set(${name}_SOURCE_DIR "${BASTARD_FETCH_SRC_DIR}/${name}")
            set(${name}_BINARY_DIR "${CMAKE_BINARY_DIR}/.deps/${name}")
        else()
            message("${name}: Fetch content...")
            CleanBuildDep(${name})
            BastardFetch_Populate(${name})
        endif()

        GetGitCommit(${${name}_SOURCE_DIR} rev)
        set(${name}_GIT_COMMIT ${rev} CACHE INTERNAL "Commit hash for ${name}")

        # revision check (only base dependencies)
        set(lock_rev "${${package}-lock.dependencies.${name}.rev}")
        if (NOT "${lock_rev}" STREQUAL "" AND NOT "${lock_rev}" STREQUAL "${rev}")
            message("WARNING: ${name} has a different cached and locked revision ${rev} != ${lock_rev}")
        endif()

        set(${name}_SOURCE_DIR ${${name}_SOURCE_DIR} PARENT_SCOPE)
        set(${name}_BINARY_DIR ${${name}_BINARY_DIR} PARENT_SCOPE)
        message("${name}: source dir ${${name}_SOURCE_DIR}")
        message("${name}: binary dir ${${name}_BINARY_DIR}")
    endif()
endfunction(PopulateHelper)
