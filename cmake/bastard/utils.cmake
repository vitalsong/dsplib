include_guard(GLOBAL)

include(${CMAKE_CURRENT_LIST_DIR}/toml_parser.cmake)

#--------------------------------------------------------------------------------
# add item to cache list (removing duplicates and nulls)
function(AppendToCacheList name values)
    foreach(item "${values}")
        set(temp_list "${${name}}")
        list(APPEND temp_list ${item})
        list(REMOVE_DUPLICATES temp_list)
        list(REMOVE_ITEM temp_list "")
        set(${name} "${temp_list}" CACHE INTERNAL "" FORCE)
    endforeach()
endfunction(AppendToCacheList)

#--------------------------------------------------------------------------------
# get package name from config file
function(GetPackageName _name)
    TomlParser("${CMAKE_CURRENT_SOURCE_DIR}/bastard.toml" "temp")
    if ("${temp.package.name}" STREQUAL "")
        message(FATAL_ERROR "Package name is not defined for: ${CMAKE_CURRENT_SOURCE_DIR}/bastard.toml")
    endif()
    set(${_name} "${temp.package.name}" PARENT_SCOPE)
endfunction(GetPackageName)

#--------------------------------------------------------------------------------
# get package version from config file
function(GetPackageVersion _version)
    TomlParser("${CMAKE_CURRENT_SOURCE_DIR}/bastard.toml" "temp")
    set(${_version} "${temp.package.version}" PARENT_SCOPE)
endfunction(GetPackageVersion)

#--------------------------------------------------------------------------------
function(FilterList include exclude _result)
    foreach(ex_name ${exclude})
        list(REMOVE_ITEM include "${ex_name}")
    endforeach()
    set(${_result} ${include} PARENT_SCOPE)
endfunction(FilterList)

#--------------------------------------------------------------------------------
# file1.cpp file2.cpp | bin/app1 ==> bin/app1/file1.cpp bin/app1/file2.cpp
function(AddDirPrefix files prefix _result)
    set(result "")
    foreach(fname ${files})
        set(fname "${prefix}/${fname}")
        string(REPLACE "//" "/" fname ${fname})
        list(APPEND result "${fname}")
    endforeach()
    set(${_result} ${result} PARENT_SCOPE)
endfunction(AddDirPrefix)
