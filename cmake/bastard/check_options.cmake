include_guard(GLOBAL)

include(${CMAKE_CURRENT_LIST_DIR}/utils.cmake)

#--------------------------------------------------------------------------------
function(OptionToDefine package option value)

    set(name "${package}_${option}")
    string(TOUPPER "${name}" name)
    string(REPLACE "-" "_" name ${name})
    string(REPLACE "." "_" name ${name})

    if ("${value}" STREQUAL "" OR "${value}" STREQUAL "true" OR "${value}" STREQUAL "ON")
        set(param "-D${name}")
    elseif ("${value}" STREQUAL "false" OR "${value}" STREQUAL "OFF")
        # remove old define?
        return()
    else()
        set(param "-D${name}=${value}")
    endif()
    message("${package}: define from option: ${param}")
    add_definitions("${param}")
endfunction(OptionToDefine)

#--------------------------------------------------------------------------------
function(ImportOptions package name)
    set(prefix "${package}.options.${name}")
    set(keys "${${prefix}.__keys__}")
    if ("${keys}" STREQUAL "")
        return()
    endif()

    foreach(key ${keys})
        # TODO: check external options
        set(value "${${prefix}.${key}}")
        message("${package}: Set option ${key} = ${value}")

        if ("${value}" STREQUAL "true")
            set(cmake_value "ON")
        elseif("${value}" STREQUAL "false")
            set(cmake_value "OFF")
        else()
            set(cmake_value "${value}")
        endif()
        
        set("${key}" ${cmake_value} CACHE INTERNAL "${name}: ${key} = ${cmake_value}")

        # only when explicitly configured
        if ("${${package}.package.autogen-defines}" STREQUAL "true")
            OptionToDefine("${name}" "${key}" "${value}")
        endif()
    endforeach()
endfunction(ImportOptions)
