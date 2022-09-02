include_guard(GLOBAL)

include(${CMAKE_CURRENT_LIST_DIR}/check_lang.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/check_deps.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/check_lib.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/check_options.cmake)

#--------------------------------------------------------------------------------
function(DefaultBinSources path _result)
    file(GLOB_RECURSE sources "${path}/*.c" "${path}/*.cpp" "${path}/*.cc" "${path}/*.h" "${path}/*.hpp")
    file(GLOB_RECURSE resources "${path}/*.ui" "${path}/*.qrc")
    set(sources ${sources} ${resources})
    set(${_result} ${sources} PARENT_SCOPE)
endfunction(DefaultBinSources)

#--------------------------------------------------------------------------------
# create single bin target
function(_CreateBinTarget section name path spec_src spec_dir def_lang)
    set(bin_lang "${${section}.lang}")
    if("${bin_lang}" STREQUAL "")
        set(bin_lang "${def_lang}")
    endif()

    if (NOT IS_DIRECTORY "${path}")
        set(sources "${path}")
        get_filename_component(private_dirs "${path}" DIRECTORY)
    else()
        # filter includes
        set(include_files "${${section}.include}")
        if ("${include_files}" STREQUAL "")
            DefaultBinSources(${path} include_files)
        else()
            AddDirPrefix("${include_files}" "${path}" include_files)
            file(GLOB_RECURSE include_files ${include_files})
        endif()

        set(private_dirs "${path}")
        if (NOT "${spec_dir}" STREQUAL "")
            list(APPEND private_dirs ${spec_dir})
        endif()

        if (NOT "${spec_src}" STREQUAL "")
            list(APPEND include_files ${spec_src})
        endif()

        # filter excludes
        set(exclude_files "${${section}.exclude}")
        AddDirPrefix("${exclude_files}" "${path}" exclude_files)
        file(GLOB_RECURSE exclude_files ${exclude_files})
        FilterList("${include_files}" "${exclude_files}" sources)
    endif()

    set(is_console ${${section}.console})
    if ("${is_console}" STREQUAL "false" AND WIN32)
        add_executable(${name} WIN32 ${sources})
    else()
        add_executable(${name} ${sources})
    endif()

    target_include_directories(${name} PRIVATE "${private_dirs}")
    SetLanguageProperty(${name} "${bin_lang}")

endfunction(_CreateBinTarget)

#--------------------------------------------------------------------------------
# type: bin|test|example
# default_dir: default sources dir for target
# use_test: use add_test() for target
function(CreateBinTagets type default_dir default_name use_test)
    set(keys "${${package}.${type}.__keys__}")
    if ("${keys}" STREQUAL "")
        return()
    endif()

    # check multisection
    set(section_list "")
    set(prefix ${package}.${type})
    if (NOT "${${prefix}.0.__keys__}" STREQUAL "")
        set(keys ${${prefix}.__keys__})
        foreach(section ${keys})
            list(APPEND section_list "${prefix}.${section}")
        endforeach()
    else()
        set(section_list "${prefix}")
    endif()

    LibTargetName(${package} lib_target)

    set(def_lang "${${package}.package.lang}")

    foreach(section ${section_list})
        set(bin_name "${${section}.name}")
        set(path_name "${${section}.path}")
        
        if ("${bin_name}" STREQUAL "" AND "${path_name}" STREQUAL "")
            set(bin_name "${default_name}")
            set(path_name "${default_dir}")
        elseif(NOT "${bin_name}" STREQUAL "" AND "${path_name}" STREQUAL "")
            set(path_name "${default_dir}/${bin_name}")
        elseif("${bin_name}" STREQUAL "" AND NOT "${path_name}" STREQUAL "")
            get_filename_component(file_name "${CMAKE_CURRENT_SOURCE_DIR}/${path_name}" NAME_WE)
            set(bin_name "${file_name}")
        endif()

        set(bin_path "${CMAKE_CURRENT_SOURCE_DIR}/${path_name}")
        if (NOT EXISTS "${bin_path}")
            message(FATAL_ERROR "${package}: Bin path error: ${bin_path}")
            return()
        endif()

        if (TARGET ${bin_name})
            message(FATAL_ERROR "${package}: Target <${bin_name}> already exist")
            return()
        endif()

        message("${package}: Create ${type} target: <${bin_name}>")
        if(NOT "${lib_target}" STREQUAL "")
            ImportOptions(${package} ${bin_name})
            _CreateBinTarget(${section} ${bin_name} ${bin_path} "" "" "${def_lang}")
            target_link_libraries(${bin_name} ${lib_target})
        else()
            set(specific_sources "${${package}_SPECIFIC_SOURCES}")
            set(specific_dirs "${${package}_SPECIFIC_DIRS}")
            ImportOptions(${package} ${bin_name})
            _CreateBinTarget(${section} ${bin_name} ${bin_path} "${specific_sources}" "${specific_dirs}" "${def_lang}")
            LinkSysLibraries(${package} ${bin_name})
            LinkDepsLibraries(${package} ${bin_name})
        endif()

        if (use_test)
            # include paths for dev-build
            target_include_directories(${bin_name} PRIVATE "lib" "${${package}_SPECIFIC_DIRS}")
            add_test(${bin_name} ${bin_name})
        endif()

    endforeach()
endfunction(CreateBinTagets)

#--------------------------------------------------------------------------------
function(SetupBinTargets package)
    CreateBinTagets(bin "bin" "${package}-app" False)
endfunction(SetupBinTargets)

#--------------------------------------------------------------------------------
function(SetupTestTargets package)
    CreateBinTagets(test "tests" "${package}-test" True)
endfunction(SetupTestTargets)

#--------------------------------------------------------------------------------
function(SetupExampleTargets package)
    CreateBinTagets(example "examples" "${package}-example" False)
endfunction(SetupExampleTargets)