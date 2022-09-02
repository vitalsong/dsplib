include_guard(GLOBAL)

include(${CMAKE_CURRENT_LIST_DIR}/utils.cmake)

#--------------------------------------------------------------------------------
function(ConfigDefines package)
    set(prefix "${package}.defines")
    set(dep_list "${${prefix}.__keys__}")
    if ("${dep_list}" STREQUAL "")
        return()
    endif()
    
    foreach(target ${dep_list})
        if(NOT TARGET ${target})
            message(WARNING "${package}: Target ${target} not exist. Skip defines setup.")
            continue()
        endif()

        set(names "${${prefix}.${target}.__keys__}")
        foreach(name ${names})
            set(value "${${prefix}.${target}.${name}}")
            if(NOT "${value}" STREQUAL "")
                target_compile_definitions(${target} PRIVATE "${name}=${value}")
            endif()
        endforeach()

    endforeach()

endfunction(ConfigDefines)