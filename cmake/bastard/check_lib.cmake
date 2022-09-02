include_guard(GLOBAL)

include(${CMAKE_CURRENT_LIST_DIR}/check_lang.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/check_deps.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/check_options.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/utils.cmake)

#--------------------------------------------------------------------------------
function(LibTargetName package _name)
    if (NOT "lib" IN_LIST ${package}.__headers__)
        return()
    endif()

    set(name "${${package}.lib.name}")
    if("${name}" STREQUAL "")
        set(name ${package})
    endif()

    set(${_name} ${name} PARENT_SCOPE)
endfunction(LibTargetName)

#--------------------------------------------------------------------------------
function(DefaultLibSources _result)
    set(path1 "${path}/lib")
    set(path2 "${path}/include")
    file(GLOB_RECURSE sources "${path1}/*.c" "${path1}/*.cc" "${path1}/*.cpp" "${path1}/*.h" "${path1}/*.hpp" "${path2}/*.h" "${path2}/*.hpp")
    file(GLOB_RECURSE resources "${path1}/*.ui" "${path1}/*.qrc")
    set(sources ${sources} ${resources})
    set(${_result} ${sources} PARENT_SCOPE)
endfunction(DefaultLibSources)

#--------------------------------------------------------------------------------
function(GetLibType package _type)
    set(lib_type "${${package}.lib.type}")
    # create the interface library only as a dependency
    # because dev-deps links to the main package lib
    if ("${lib_type}" STREQUAL "interface" AND "${BASTARD_MAIN_PACKAGE}" STREQUAL "${package}")
        set(lib_type "static")
    endif()
    set(${_type} ${lib_type} PARENT_SCOPE)
endfunction(GetLibType)

#--------------------------------------------------------------------------------
function(CreateLibTarget package name path)
    ImportOptions(${package} ${name})

    # filter includes
    set(include_files "${${package}.lib.include}")
    if ("${include_files}" STREQUAL "")
        DefaultLibSources(include_files)
    else()
        AddDirPrefix("${include_files}" "${path}/lib" include_files)
        list(APPEND include_files "${path}/include/*.h" "${path}/include/*.hpp")
        file(GLOB_RECURSE include_files ${include_files})
    endif()

    set(public_dirs "${path}/include")
    set(private_dirs "${path}/lib")

    # add specific sources
    set(specific_sources "${${package}_SPECIFIC_SOURCES}")
    if (NOT "${specific_sources}" STREQUAL "")
        list(APPEND include_files ${specific_sources})
    endif()

    # add specific dirs
    set(specific_dirs "${${package}_SPECIFIC_DIRS}")
    if (NOT "${specific_dirs}" STREQUAL "")
        list(APPEND private_dirs ${specific_dirs})
    endif()

    # filter excludes
    set(exclude_files "${${package}.lib.exclude}")
    AddDirPrefix("${exclude_files}" "${path}/lib" exclude_files)
    file(GLOB_RECURSE exclude_files ${exclude_files})
    FilterList("${include_files}" "${exclude_files}" sources)

    set(lib_lang "${${package}.lib.lang}")
    set(def_lang "${${package}.package.lang}")
    if("${lib_lang}" STREQUAL "")
        set(lib_lang "${def_lang}")
    endif()

    GetLibType(${package} lib_type)
    if("${lib_type}" STREQUAL "shared")
        SetDepsLibraryPIC(${package})
        add_library(${name} SHARED ${sources})
        target_include_directories(${name} PUBLIC "${public_dirs}")
        target_include_directories(${name} PRIVATE "${private_dirs}")
        SetLanguageProperty(${name} "${lib_lang}")
        LinkSysLibraries(${package} "${lib_name}")
        LinkDepsLibraries(${package} "${lib_name}")
    elseif("${lib_type}" STREQUAL "interface")
        # TODO: warning if exists cpp/c source files
        add_library(${name} INTERFACE)
        target_include_directories(${name} INTERFACE "${public_dirs}")
        LinkDepsLibraries(${package} "${lib_name}")
    else()
        add_library(${name} ${sources})
        target_include_directories(${name} PUBLIC "${public_dirs}")
        target_include_directories(${name} PRIVATE "${private_dirs}")
        SetLanguageProperty(${name} "${lib_lang}")
        LinkSysLibraries(${package} "${lib_name}")
        LinkDepsLibraries(${package} "${lib_name}")
    endif()
endfunction(CreateLibTarget)

#--------------------------------------------------------------------------------
# linking the current package dependencies to the target
function(LinkDepsLibraries package target)
    set(deps "${${package}_DEPS_LIST}")
    GetLibType(${package} lib_type)
    foreach(dep_name ${deps})
        set(links "${${dep_name}_LINKS}")
        if ("${links}" STREQUAL "")
            set(links "${dep_name}")
        endif()
        if ("${lib_type}" STREQUAL "interface")
            target_link_libraries(${target} INTERFACE ${links})
        else()
            target_link_libraries(${target} ${links})
        endif()
    endforeach()
endfunction(LinkDepsLibraries)

#--------------------------------------------------------------------------------
# set POSITION_INDEPENDENT_CODE for all deps
function(SetDepsLibraryPIC package)
    set(section_list "${package}.dependencies" "${package}.dev-dependencies")
    foreach(section ${section_list})
        set(dep_list "${${section}.__keys__}")
        foreach(dep_name ${dep_list})
            set_target_properties(${dep_name} PROPERTIES POSITION_INDEPENDENT_CODE ON)
        endforeach()
    endforeach()
endfunction(SetDepsLibraryPIC)

#--------------------------------------------------------------------------------
function(SetupLibTarget package)
    LibTargetName(${package} lib_name)
    if("${lib_name}" STREQUAL "")
        message("${package}: No lib target exists")
        return()
    endif()

    if (NOT "${${package}.lib.0.__keys__}" STREQUAL "")
        message(FATAL_ERROR "${package}: Only one library target per package")
        return()
    endif()

    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/include" AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/lib")
        message(FATAL_ERROR "${package}: Lib structure is not valid")
        return()
    endif()

    message("${package}: Create lib target <${lib_name}>")
    CreateLibTarget(${package} ${lib_name} "${CMAKE_CURRENT_SOURCE_DIR}")

endfunction(SetupLibTarget)
