include_guard(GLOBAL)

#--------------------------------------------------------------------------------
function(SetLanguageProperty target lang)
    if ("${lang}" STREQUAL "")
        return()
    elseif("${lang}" STREQUAL "c++98")
        set_target_properties(${target} PROPERTIES
            CXX_STANDARD 98
            CXX_STANDARD_REQUIRED ON
            CXX_EXTENSIONS OFF
        )
    elseif("${lang}" STREQUAL "c++11")
        set_target_properties(${target} PROPERTIES
            CXX_STANDARD 11
            CXX_STANDARD_REQUIRED ON
            CXX_EXTENSIONS OFF
        )
    elseif("${lang}" STREQUAL "c++14")
        set_target_properties(${target} PROPERTIES
            CXX_STANDARD 14
            CXX_STANDARD_REQUIRED ON
            CXX_EXTENSIONS OFF
        )
    elseif("${lang}" STREQUAL "c++17")
        set_target_properties(${target} PROPERTIES
            CXX_STANDARD 17
            CXX_STANDARD_REQUIRED ON
            CXX_EXTENSIONS OFF
        )
    elseif("${lang}" STREQUAL "c++20")
        set_target_properties(${target} PROPERTIES
            CXX_STANDARD 20
            CXX_STANDARD_REQUIRED ON
            CXX_EXTENSIONS OFF
        )
    elseif("${lang}" STREQUAL "c90")
        set_target_properties(${target} PROPERTIES
            C_STANDARD 90
            C_STANDARD_REQUIRED ON
            C_EXTENSIONS OFF
        )
    elseif("${lang}" STREQUAL "c99")
        set_target_properties(${target} PROPERTIES
            C_STANDARD 99
            C_STANDARD_REQUIRED ON
            C_EXTENSIONS OFF
        )
    elseif("${lang}" STREQUAL "c11")
        set_target_properties(${target} PROPERTIES
            C_STANDARD 11
            C_STANDARD_REQUIRED ON
            C_EXTENSIONS OFF
        )
    else()
        message(FATAL_ERROR "${target}: Language name error ${lang}.\nPlease select: c++98 | c++11 | c++14 | c++17 | c++20 | c90 | c99 | c11")
    endif()
endfunction(SetLanguageProperty)