include_guard(GLOBAL)

#------------------------------------------------------------
function(GetValue line out_attr out_value)
    string(FIND ${line} "=" pos)
    string(LENGTH ${line} line_len)

    string(SUBSTRING ${line} 0 ${pos} attr)
    string(REPLACE " " "" attr ${attr})

    math(EXPR len "${line_len} - ${pos}")
    math(EXPR pos "${pos} + 1")
    string(SUBSTRING ${line} ${pos} ${len} value)
    string(STRIP ${value} value)

    string(SUBSTRING ${value} 0 1 symbol)
    if("${symbol}" STREQUAL "\"")
        string(LENGTH ${value} value_len)
        math(EXPR len "${value_len} - 2")
        string(SUBSTRING ${value} 1 ${len} value)
    endif()

    set(${out_attr} ${attr} PARENT_SCOPE)
    set(${out_value} ${value} PARENT_SCOPE)
endfunction(GetValue)

#------------------------------------------------------------
function(GetValueType value result)
    set(${result} "STRING" PARENT_SCOPE)

    #check if dict
    string(REGEX MATCH "\\{.+\\}" params ${value})
    if(NOT "${params}" STREQUAL "")
        set(${result} "DICT" PARENT_SCOPE)
        return()
    endif()

    #check if list
    string(REGEX MATCH "\\[.+\\]" params ${value})
    if(NOT "${params}" STREQUAL "")
        set(${result} "LIST" PARENT_SCOPE)
        return()
    endif()

endfunction(GetValueType)

#------------------------------------------------------------
function(ValueAsDict value prefix _keys)
    set(items "")
    set(keys "")
    GetValueType(${value} type)
    if("${type}" STREQUAL "DICT")
        string(REPLACE " " "" value ${value})
        string(REPLACE "{" "" items ${value})
        string(REPLACE "}" "" items "${items}")
        string(REPLACE "\"," "\";" items "${items}")
        string(REPLACE "]," "];" items "${items}")
        foreach(item ${items})
            string(REPLACE "\";" "\"," item "${item}") # TODO: ugly
            GetValue(${item} key val)
            set(key ${prefix}${key})
            list(APPEND keys ${key})
            set(${key} ${val} PARENT_SCOPE)
        endforeach()
    endif()

    set(${_keys} ${keys} PARENT_SCOPE)
endfunction(ValueAsDict)

#------------------------------------------------------------
function(ValueAsList value _items)
    set(items "")
    GetValueType(${value} type)
    if("${type}" STREQUAL "LIST")
        string(REPLACE "[" "" items ${value})
        string(REPLACE "]" "" items ${items})
        string(REPLACE "\"," "\"\;" items ${items})
        string(REPLACE "\"" "" items ${items})
        list(REMOVE_DUPLICATES items)
    endif()

    set(result "")
    foreach(item ${items})
        string(STRIP ${item} item)
        list(APPEND result ${item})
    endforeach()
    set(${_items} ${result} PARENT_SCOPE)
endfunction(ValueAsList)

#------------------------------------------------------------
function(TomlParser fname rprefix)
    file(READ ${fname} text)
    string(REPLACE "\n" ";" text_lines ${text})
    set(__headers__ "")
    foreach(line ${text_lines})

        # remove comment
        string(REGEX REPLACE "#.+" "" line ${line})
        string(STRIP "${line}" line)
        if("${line}" STREQUAL "")
            continue()
        endif()
        
        # search [section]
        string(REGEX MATCH "\\[[a-zA-Z0-9\\_\\.-]+\\]" match ${line})
        if(NOT "${match}" STREQUAL "")
            # new section - update key
            string(REPLACE "[" "" section ${line})
            string(REPLACE "]" "" section ${section})
            string(STRIP ${section} section)
            
            string(REPLACE "." ";" sub_sections ${section})

            # update headers
            list(GET sub_sections 0 header)
            list(APPEND __headers__ ${header})

            # check multisection []
            string(REGEX MATCH "\\[\\[" match ${line})
            if(NOT "${match}" STREQUAL "")
                # TODO: if a single section has been processed
                list(LENGTH ${header}.__keys__ len)
                list(APPEND ${header}.__keys__ ${len}) # example: bin.0, bin.1, bin.2
                set(last_sub ${header}.${len})
                set(section ${last_sub})
            else()
                set(last_sub ${header})
            endif()

            # update subgroups
            list(REMOVE_AT sub_sections 0)
            foreach(sub ${sub_sections})
                list(APPEND ${last_sub}.__keys__ ${sub})
                set(last_sub ${last_sub}.${sub})
            endforeach()
        else()
            GetValue(${line} key value)
            list(APPEND ${section}.__keys__ ${key})
        
            set(prefix ${section}.${key})
            GetValueType(${value} type)
            if("${type}" STREQUAL "DICT")
                ValueAsDict(${value} "" keys)
                set(${prefix}.__keys__ ${keys})
                foreach(key ${keys})
                    set(value ${${key}})
                    GetValueType(${value} type)
                    if("${type}" STREQUAL "DICT")
                        message(FATAL_ERROR "Too deep structure of toml")
                    elseif("${type}" STREQUAL "LIST")
                        ValueAsList(${value} items)
                        set(${prefix}.${key} ${items})
                    else()
                        set(${prefix}.${key} ${value})
                    endif()
                endforeach()
            elseif("${type}" STREQUAL "LIST")
                ValueAsList(${value} items)
                set(${prefix} "${items}")
            else()
                set(${prefix} ${value})
            endif()
        endif()
    endforeach()

    list(REMOVE_DUPLICATES __headers__)

    set(stack "${__headers__}")
    list(LENGTH stack stask_len)
    while (${stask_len} GREATER 0)
        
        math(EXPR idx "${stask_len} - 1")
        list(GET stack ${idx} key)
        list(REMOVE_AT stack ${idx})
        
        set(subkeys "${${key}.__keys__}")
        if(NOT "${subkeys}" STREQUAL "")
            foreach(subkey ${subkeys})
                list(APPEND stack ${key}.${subkey})
            endforeach()
            list(REMOVE_DUPLICATES subkeys)
            set(${rprefix}.${key}.__keys__ ${subkeys} PARENT_SCOPE)
        else()
            # end element found
            set(value ${${key}})
            set(${rprefix}.${key} ${value} PARENT_SCOPE)
        endif()
        
        list(LENGTH stack stask_len)
    endwhile()
    
    set("${rprefix}.__headers__" ${__headers__} PARENT_SCOPE)

endfunction(TomlParser)