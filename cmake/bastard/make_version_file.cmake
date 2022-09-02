# Generating version file from template

# Script variables:
# package_name - package name
# version - current version
# input_template - template file
# output_file - result file
# src_dir - work project dir

# template params:
# PACKAGE_NAME, VERSION, VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, GIT_COMMIT, GIT_COMMIT_URL

find_package(Git QUIET REQUIRED)

execute_process(
    COMMAND "${GIT_EXECUTABLE}" log --pretty=format:%h -n 1
    WORKING_DIRECTORY "${src_dir}"
    OUTPUT_VARIABLE GIT_COMMIT
    OUTPUT_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE rev_parse_exit_code
)

if (NOT rev_parse_exit_code EQUAL 0)
    set(GIT_COMMIT "UNKNOWN")
endif()

# Getting VERSION, VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH
set(VERSION ${version})
string(REGEX MATCH "^([0-9]+)\\.([0-9]+)\\.(([0-9]+))?$" valid_version "${version}")
if (NOT valid_version)
    message(WARNING "${version} is not a valid version")
    set(VERSION_MAJOR 0)
    set(VERSION_MINOR 0)
    set(VERSION_PATCH 0)
else()
    set(VERSION_MAJOR ${CMAKE_MATCH_1})
    set(VERSION_MINOR ${CMAKE_MATCH_2})
    if (CMAKE_MATCH_3)
        set(VERSION_PATCH ${CMAKE_MATCH_3})
    else()
        set(VERSION_PATCH 0)
    endif()
endif()


#get url for this commit
execute_process(
    COMMAND "${GIT_EXECUTABLE}" config --get remote.origin.url
    WORKING_DIRECTORY "${src_dir}"
    OUTPUT_VARIABLE GIT_COMMIT_URL
    OUTPUT_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE rev_parse_exit_code
)

if(NOT "${GIT_COMMIT_URL}" STREQUAL "")    
    string(REPLACE ".git" "" GIT_COMMIT_URL ${GIT_COMMIT_URL})
    string(REPLACE ":" "/" GIT_COMMIT_URL ${GIT_COMMIT_URL})
    string(REPLACE "git@" "http://" GIT_COMMIT_URL ${GIT_COMMIT_URL})
    set(GIT_COMMIT_URL "${GIT_COMMIT_URL}/-/commit/${GIT_COMMIT}")
    message("package \"${package_name}\" url: ${GIT_COMMIT_URL}")
endif()

# get commit description and date
execute_process(
    COMMAND "${GIT_EXECUTABLE}" log -1 --no-merges --pretty="%s|%ad" --date=format:"%Y-%m-%d %H:%M:%S"
    WORKING_DIRECTORY "${src_dir}"
    OUTPUT_VARIABLE GIT_COMMIT_DESCRIPTION
    OUTPUT_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE rev_parse_exit_code
)

if(NOT "${GIT_COMMIT_DESCRIPTION}" STREQUAL "")    
    string(LENGTH "${GIT_COMMIT_DESCRIPTION}" str_size)
    math(EXPR str_size "${str_size}-1")
    string(SUBSTRING "${GIT_COMMIT_DESCRIPTION}" 1 ${str_size} GIT_COMMIT_DESCRIPTION)
    string(FIND "${GIT_COMMIT_DESCRIPTION}" "|" split_pos REVERSE)
    string(SUBSTRING "${GIT_COMMIT_DESCRIPTION}" 0 ${split_pos} GIT_COMMIT_MESSAGE)
    math(EXPR split_pos "${split_pos} + 2")
    string(SUBSTRING "${GIT_COMMIT_DESCRIPTION}" ${split_pos} 19 GIT_COMMIT_DATE)
    string(REGEX REPLACE "\"" "_" GIT_COMMIT_MESSAGE "${GIT_COMMIT_MESSAGE}")
endif()

# Format PACKAGE_NAME
set(PACKAGE_NAME ${package_name})
string(REPLACE "-" "_" PACKAGE_NAME ${PACKAGE_NAME})
string(TOUPPER ${PACKAGE_NAME} PACKAGE_NAME)

# Generating an output file
configure_file("${input_template}" "${output_file}" @ONLY)
