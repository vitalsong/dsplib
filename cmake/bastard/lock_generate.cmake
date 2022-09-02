include_guard(GLOBAL)

include(${CMAKE_CURRENT_LIST_DIR}/utils.cmake)

#------------------------------------------------------------
function(ListAsValue items _result)
    set(result "")
    list(REMOVE_ITEM items "")
    foreach(item ${items})
        string(STRIP "${item}" item)
        if("${result}" STREQUAL "")
            set(result "\"${item}\"")
        else()
            set(result "${result}, \"${item}\"")
        endif()
    endforeach()
    set(result "[ ${result} ]")
    set(${_result} ${result} PARENT_SCOPE)
endfunction(ListAsValue)

#-----------------------------------------------------------------
# attrs format: package.name;package.version;dependencies.test_1;dependencies.test_2;
function(GenTomlText sections prefix _text)
    set(text "")
    foreach(section ${sections})
        set(text "${text}[${section}]\n")  
        foreach(key ${${prefix}.${section}.__keys__})
            set(value ${${prefix}.${section}.${key}})
            set(text "${text}${key} = ${value}\n")
        endforeach()
        set(text "${text}\n")
    endforeach()
    set(${_text} ${text} PARENT_SCOPE)
endfunction(GenTomlText)

#--------------------------------------------------------------------------------
function(GenLockFile package)
    message("Generate bastard.lock file")

    # rewrite dependencies section
    set(${package}.dependencies.__keys__ "")
    set(${package}.dev-dependencies.__keys__ "")

    foreach(dep_name ${BASTARD_DEPS_LIST})
        set(links "${${dep_name}_LINKS}")
        set(interface "${${dep_name}_INTERFACE}")
        set(commit "${${dep_name}_GIT_COMMIT}")
        set(url "${${dep_name}_GIT_URL}")
        set(section "${${dep_name}_SECTION}")

        if (url STREQUAL "")
            continue()
        endif()

        list(APPEND ${package}.${section}.__keys__ ${dep_name})

        if (NOT "${links}" STREQUAL "")
            ListAsValue("${links}" links_value)
            set(value "{ git = \"${url}\", rev = \"${commit}\", links = ${links_value} }")
        elseif(NOT "${interface}" STREQUAL "")
            set(value "{ git = \"${url}\", rev = \"${commit}\", interface = \"${interface}\" }")
        else()
            set(value "{ git = \"${url}\", rev = \"${commit}\" }")
        endif()
        set(${package}.${section}.${dep_name} ${value})
    endforeach()

    # set update sections
    set(update_sections "package")

    set(${package}.package.__keys__ "name" "cmake" "bastard")
    set(${package}.package.cmake ${CMAKE_VERSION})
    set(${package}.package.bastard ${BASTARD_VERSION})

    if(NOT "${${package}.dependencies.__keys__}" STREQUAL "")
        list(APPEND update_sections "dependencies")
    endif()

    if(NOT "${${package}.dev-dependencies.__keys__}" STREQUAL "")
        list(APPEND update_sections "dev-dependencies")
    endif()

    GenTomlText("${update_sections}" ${package} toml_text)
    file(WRITE "${CMAKE_BINARY_DIR}/bastard.lock" ${toml_text})
    file(COPY "${CMAKE_BINARY_DIR}/bastard.lock" DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}" FILE_PERMISSIONS OWNER_READ)
    file(REMOVE "${CMAKE_BINARY_DIR}/bastard.lock")
endfunction(GenLockFile)