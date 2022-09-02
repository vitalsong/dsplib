include_guard(GLOBAL)

include(${CMAKE_CURRENT_LIST_DIR}/fetch_helper.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/utils.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/check_options.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/specific_ctrl.cmake)

#--------------------------------------------------------------------------------
# get attributes for FetchContent
function(GitFetchParam package section name _uri _tag)

    # if lock version exist
    set(dep_key "${package}-lock.${section}.${name}")
    if("${${dep_key}.__keys__}" STREQUAL "")
        set(dep_key "${package}.${section}.${name}")
    endif()

    set(fetch_uri ${${dep_key}.git})
    set(fetch_tag "master")
    if (NOT "${${dep_key}.branch}" STREQUAL "")
        set(fetch_tag ${${dep_key}.branch})
    elseif(NOT "${${dep_key}.tag}" STREQUAL "")
        set(fetch_tag ${${dep_key}.tag})
    elseif(NOT "${${dep_key}.rev}" STREQUAL "")
        set(fetch_tag ${${dep_key}.rev})
    endif()
    set(${_uri} ${fetch_uri} PARENT_SCOPE)
    set(${_tag} ${fetch_tag} PARENT_SCOPE)
endfunction(GitFetchParam)

#--------------------------------------------------------------------------------
function(_UpdateDepsCache package target)
    AppendToCacheList(${package}_DEPS_LIST ${target})
    AppendToCacheList(BASTARD_DEPS_LIST ${target})
endfunction(_UpdateDepsCache)

#--------------------------------------------------------------------------------
function(SetupLocalTarget package name path)
    set(full_path "${CMAKE_CURRENT_SOURCE_DIR}/${path}")
    if (EXISTS "${full_path}/CMakeLists.txt")
        add_subdirectory("${full_path}" "${CMAKE_BINARY_DIR}/.deps/${name}" EXCLUDE_FROM_ALL)
        _UpdateDepsCache(${package} ${name})
    else()
        file(GLOB_RECURSE local_sources "${full_path}/*")
        # TODO: clear list before config
        foreach(fname ${local_sources})
            AppendToCacheList(${package}_SPECIFIC_SOURCES ${fname})
        endforeach()
        AppendToCacheList(${package}_SPECIFIC_DIRS ${full_path})
    endif()
endfunction(SetupLocalTarget)

#--------------------------------------------------------------------------------
function(_SetupDeps package section)
    set(prefix "${package}.${section}")
    set(dep_list "${${prefix}.__keys__}")
    if("${dep_list}" STREQUAL "")
        return()
    endif()

    foreach(dep_name ${dep_list})

        ImportOptions(${package} ${dep_name})

        # dirty check links (legacy)
        set(links "${${prefix}.${dep_name}.links}")
        set(links_exist False)
        foreach(lib ${links})
            if (TARGET ${lib})
                set(links_exist True)
            endif()
        endforeach()
        AppendToCacheList(${dep_name}_LINKS "${links}")

        # check package target or legacy target exist
        if (TARGET ${dep_name} OR links_exist)
            _UpdateDepsCache(${package} ${dep_name})
            continue()
        endif()

        # check system
        set(system_list ${${prefix}.${dep_name}.system})
        if("${system_list}" STREQUAL "")
            set(system_list "${${package}.package.system}")
        endif()

        SystemValid("${system_list}" system_valid)
        if (NOT system_valid)
            message("${package}: <${dep_name}> is not supported for ${CMAKE_SYSTEM_NAME}")
            continue()
        endif()

        # check processor
        set(proc_list ${${prefix}.${dep_name}.processor})
        if("${proc_list}" STREQUAL "")
            set(proc_list "${${package}.package.processor}")
        endif()

        ProcessorValid("${proc_list}" proc_valid)
        if (NOT proc_valid)
            message("${package}: <${dep_name}> is not supported for ${CMAKE_SYSTEM_PROCESSOR}")
            continue()
        endif()

        set(dep_keys "${${prefix}.${dep_name}.__keys__}")
        if ("git" IN_LIST dep_keys AND NOT "interface" IN_LIST dep_keys)
            #setup cmake dep
            GitFetchParam(${package} ${section} ${dep_name} fetch_uri fetch_tag)
            DeclareHelper(${dep_name} ${fetch_uri} ${fetch_tag})
            PopulateHelper(${package} ${dep_name})
            add_subdirectory(${${dep_name}_SOURCE_DIR} ${${dep_name}_BINARY_DIR} EXCLUDE_FROM_ALL)
            _UpdateDepsCache(${package} ${dep_name})
        elseif ("git" IN_LIST dep_keys AND "interface" IN_LIST dep_keys)
            #setup interface library dep
            GitFetchParam(${package} ${section} ${dep_name} fetch_uri fetch_tag)
            DeclareHelper(${dep_name} ${fetch_uri} ${fetch_tag})
            PopulateHelper(${package} ${dep_name})
            _UpdateDepsCache(${package} ${dep_name})
            set(local_path "${${prefix}.${dep_name}.interface}")
            set(include_path "${${dep_name}_SOURCE_DIR}/${local_path}")
            message("${dep_name}: Create interface library from ${include_path}")
            add_library(${dep_name} INTERFACE)
            target_include_directories(${dep_name} INTERFACE ${include_path})
            set(${dep_name}_INTERFACE ${local_path} CACHE INTERNAL "Local include path for ${dep_name}")
        elseif ("path" IN_LIST dep_keys)
            set(local_path "${${prefix}.${dep_name}.path}")
            SetupLocalTarget(${package} ${dep_name} ${local_path})
        else()
            message(WARNING "${dep_name}: Undefined dep type")
            continue()
        endif()
        
        # save section type
        set(${dep_name}_SECTION ${section} CACHE INTERNAL "Section type of dependency")
    endforeach()
endfunction(_SetupDeps)

#--------------------------------------------------------------------------------
function(SetupDeps package)
    _SetupDeps(${package} "dependencies")
endfunction(SetupDeps)

#--------------------------------------------------------------------------------
function(SetupDevDeps package)
    _SetupDeps(${package} "dev-dependencies")
endfunction(SetupDevDeps)

#--------------------------------------------------------------------------------
function(LinkSysLibraries package target)
    if ("${target}" STREQUAL "")
        return()
    endif()

    set(prefix "${package}.sys-dependencies")
    set(dep_list "${${prefix}.__keys__}")
    if("${dep_list}" STREQUAL "")
        return()
    endif()

    ImportOptions(${package} ${target})
    foreach(dep_name ${dep_list})
        set(links "${${prefix}.${dep_name}.links}")
        set(components "${${prefix}.${dep_name}.components}")
        if ("${links}" STREQUAL "")
            foreach(component ${components})
                target_link_libraries(${target} "${dep_name}::${component}")
            endforeach()
        else()
            foreach(link ${links})
                target_link_libraries(${target} "${dep_name}::${link}")
            endforeach()
        endif()
    endforeach()
endfunction(LinkSysLibraries)

#--------------------------------------------------------------------------------
function(FindSysPackage package)
    set(prefix "${package}.sys-dependencies")
    set(dep_list "${${prefix}.__keys__}")
    if("${dep_list}" STREQUAL "")
        return()
    endif()
    
    foreach(dep_name ${dep_list})
        message("${package}: Updating sys-deps ${dep_name}")
        set(components "${${prefix}.${dep_name}.components}")
        ImportOptions(${package} ${dep_name})
        if ("${components}" STREQUAL "")
            find_package(${dep_name} REQUIRED)
        else()
            find_package(${dep_name} COMPONENTS ${components} REQUIRED)
        endif()
    endforeach()
endfunction(FindSysPackage)
