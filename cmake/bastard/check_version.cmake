include_guard(GLOBAL)

if (NOT DEFINED VERSION_SCRIPT_LOC)
    set(VERSION_SCRIPT_LOC "${CMAKE_CURRENT_LIST_DIR}")
endif()

# ---------------------------------------------------------------------------------------------------------
# Adds a header (or source file) generation with version, build time
# and commit information
# The following variables can be used in the template:
#  GIT_COMMIT, BUILD_TIMESTAMP,
#  VERSION, VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH.
function(GenVersionHeader package version input_template output_file)
    set(target_for_gen "${package}-version-header")
    add_custom_target(${target_for_gen} ALL
    COMMAND ${CMAKE_COMMAND}
        -Dpackage_name="${package}"
        -Dversion="${version}"
        -Dinput_template="${input_template}"
        -Doutput_file="${output_file}"
        -Dsrc_dir="${CMAKE_CURRENT_SOURCE_DIR}"
        -P "${VERSION_SCRIPT_LOC}/make_version_file.cmake"
    COMMENT "Create or update '${output_file}'")

    if (TARGET ${package})
        add_dependencies(${package} ${target_for_gen})
    endif()
endfunction(GenVersionHeader)

function(GenBuildInfoHeader package input_template output_file)
    set(target_for_gen "${package}-build-header")
    add_custom_target(${target_for_gen} ALL
    COMMAND ${CMAKE_COMMAND}
        -Dpackage_name="${package}"
        -Dinput_template="${input_template}"
        -Doutput_file="${output_file}"
        -Dsrc_dir="${CMAKE_CURRENT_SOURCE_DIR}"
        -Dcompiler_id=${CMAKE_CXX_COMPILER_ID}
        -Dcompiler_version=${CMAKE_CXX_COMPILER_VERSION}
        -P "${VERSION_SCRIPT_LOC}/make_build_info_file.cmake"
    COMMENT "Create or update '${output_file}'")

    if (TARGET ${package})
        add_dependencies(${package} ${target_for_gen})
    endif()
endfunction(GenBuildInfoHeader)

# ---------------------------------------------------------------------------------------------------------
# check target for generate package/version.h
function(CheckVersion package)
    set(package_version "${${package}.package.version}")
    if (NOT "${package_version}" STREQUAL "")
        set(input_dir "${VERSION_SCRIPT_LOC}")
        set(output_dir "${CMAKE_BINARY_DIR}/include/${package}")
        GenVersionHeader(${package} ${package_version} "${input_dir}/version.h.in" "${output_dir}/version.h")
        GenBuildInfoHeader(${package} "${input_dir}/build_info.h.in" "${output_dir}/build_info.h")

        # TODO: link only to target
        if (TARGET ${package})
            target_include_directories(${package} INTERFACE "${CMAKE_BINARY_DIR}/include")
        else()
            include_directories("${CMAKE_BINARY_DIR}/include")
        endif()
    endif()
endfunction(CheckVersion)
