include_guard(GLOBAL)

#---------------------------------------------------------------------
function(SystemValid system_list _valid)
    set(system_valid FALSE)

    # empty is valid
    if ("${system_list}" STREQUAL "")
        set(system_valid TRUE)
    endif()

    set(win_aliases "win" "win32" "windows")
    set(unix_aliases "unix" "linux")

    string(TOLOWER ${CMAKE_SYSTEM_NAME} system_name)
    foreach(system ${system_list})
        string(TOLOWER ${system} system)
        if (system IN_LIST win_aliases AND WIN32)
            set(system_valid TRUE)
            break()
        elseif(system IN_LIST unix_aliases AND UNIX)
            set(system_valid TRUE)
            break()
        elseif("${system}" STREQUAL "${system_name}")
            set(system_valid TRUE)
            break()
        endif()
    endforeach()

    set(${_valid} ${system_valid} PARENT_SCOPE)
endfunction(SystemValid)

#---------------------------------------------------------------------
function(ProcessorValid proc_list _valid)
    set(proc_valid FALSE)

    # empty is valid
    if ("${proc_list}" STREQUAL "")
        set(proc_valid TRUE)
    endif()

    foreach(processor ${proc_list})
        if("${processor}" STREQUAL "${CMAKE_SYSTEM_PROCESSOR}")
            set(proc_valid TRUE)
            break()
        endif()
    endforeach()

    set(${_valid} ${proc_valid} PARENT_SCOPE)
endfunction(ProcessorValid)
