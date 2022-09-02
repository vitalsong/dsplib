include_guard(GLOBAL)

# ----------------------------------------------------------------------------
function(FileContains fname str _contains)
    file(READ ${fname} text)
    string(REGEX MATCH "${str}" match ${text})
    set("${_contains}" OFF PARENT_SCOPE)
    if(NOT "${match}" STREQUAL "")
        set("${_contains}" ON PARENT_SCOPE)
    endif()  
endfunction(FileContains)

# ----------------------------------------------------------------------------
function(UpdateHookFile path hook)
    set(hook_file "${path}/.git/hooks/${hook}")
    set(need_update ON)

    if (EXISTS "${hook_file}")
        FileContains("${hook_file}" "BASTARD" need_update)
    endif()

    if (need_update)
        file(COPY "${BASTARD_DIR}/hooks/${hook}" "${BASTARD_DIR}/hooks/bastard-${hook}"
            DESTINATION "${path}/.git/hooks"
            FILE_PERMISSIONS OWNER_EXECUTE OWNER_READ
        )
        message("Update hook: ${path}/.git/hooks/${hook}")
    endif()
endfunction(UpdateHookFile)

# ----------------------------------------------------------------------------
# check git client hooks for autotag from version
function(CheckGitHooks path)
    #check python3 exists
    execute_process(
        COMMAND python3 --version
        OUTPUT_VARIABLE python_version
        RESULT_VARIABLE exit_code
    )

    if (NOT exit_code EQUAL 0)
        message("Update git hooks error: python3 not found")
        return()
    endif()

    # check post-commit
    UpdateHookFile(${path} "post-commit")

    # check pre-commit
    UpdateHookFile(${path} "pre-commit")

endfunction(CheckGitHooks)
