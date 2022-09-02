include_guard(GLOBAL)

set(BASTARD_VERSION "0.16.8")
cmake_policy(VERSION 3.10)

# TODO: fix the problem of unloading sys-dependencies (for example Thread) without specifying the language
enable_language(C CXX)

cmake_policy(SET CMP0057 NEW)       # IN_LIST operator support (since 3.3)

include(${CMAKE_CURRENT_LIST_DIR}/fetch_helper.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/toml_parser.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/check_deps.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/check_options.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/check_defines.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/check_version.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/check_bin.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/check_lib.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/lock_generate.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/utils.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/hooks/check-git-hook.cmake)

# fix CMakeLists.cmake.in file search for fetch_content module
if("${_FETCH_CONTENT_FUNCTION_DIR}" STREQUAL "")
    set(_FETCH_CONTENT_FUNCTION_DIR ${BASTARD_DIR})
endif()

#--------------------------------------------------------------------------------
function(ConfigPackage package is_root)
    TomlParser("${CMAKE_CURRENT_SOURCE_DIR}/bastard.toml" ${package})
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/bastard.lock")
        TomlParser("${CMAKE_CURRENT_SOURCE_DIR}/bastard.lock" ${package}-lock)
    endif()

    # clear deps cache
    set("${package}_DEPS_LIST" "" CACHE INTERNAL "" FORCE)

    # setup before SetupDeps to exclude nested dev dependencies
    if (is_root)
        set("BASTARD_DEPS_LIST" "" CACHE INTERNAL "" FORCE)
        SetupDevDeps(${package})
    endif()

    SetupDeps(${package})

    FindSysPackage(${package})
    SetupLibTarget(${package})
    SetupBinTargets(${package})

    if (is_root)
        SetupTestTargets(${package})
        SetupExampleTargets(${package})
    endif()

    ConfigDefines(${package})
    CheckVersion(${package})

    if(is_root AND NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/bastard.lock")
        GenLockFile(${package})
    endif()
endfunction(ConfigPackage)

#--------------------------------------------------------------------------------
function(bastard_setup)

    if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/bastard.toml")
        message("File bastard.toml not exist")
        return()
    endif()

    GetPackageName(package_name)
    set(BASTARD_PACKAGE_NAME ${package_name} PARENT_SCOPE)

    GetPackageVersion(package_version)
    set(BASTARD_PACKAGE_VERSION ${package_version} PARENT_SCOPE)

    if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_LIST_DIR}")
        set(BASTARD_MAIN_PACKAGE ${package_name})
        message("Config main package <${package_name}>")
        CheckGitHooks(${CMAKE_CURRENT_SOURCE_DIR})
        ConfigPackage(${package_name} TRUE)
    else()
        message("Config sub package <${package_name}>")
        ConfigPackage(${package_name} FALSE)
    endif()

endfunction(bastard_setup)
