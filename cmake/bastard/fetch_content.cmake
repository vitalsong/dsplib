# Fork of cmake FetchContent module

# Additional options:
# BASTARD_FETCH_SRC_DIR
# BASTARD_FETCH_BUILD_DIR
# BASTARD_FETCH_SUBBUILD_DIR

function(__BastardFetch_declareDetails contentName)

  string(TOLOWER ${contentName} contentNameLower)
  set(propertyName "_BastardFetch_${contentNameLower}_savedDetails")
  get_property(alreadyDefined GLOBAL PROPERTY ${propertyName} DEFINED)
  if(NOT alreadyDefined)
    define_property(GLOBAL PROPERTY ${propertyName}
      BRIEF_DOCS "Internal implementation detail of BastardFetch_Populate()"
      FULL_DOCS  "Details used by BastardFetch_Populate() for ${contentName}"
    )
    set_property(GLOBAL PROPERTY ${propertyName} ${ARGN})
  endif()

endfunction()


# Internal use, projects must not call this directly. It is
# intended for use by the BastardFetch_Declare() function.
#
# Retrieves details saved for the specified content in an
# earlier call to __BastardFetch_declareDetails().
function(__BastardFetch_getSavedDetails contentName outVar)

  string(TOLOWER ${contentName} contentNameLower)
  set(propertyName "_BastardFetch_${contentNameLower}_savedDetails")
  get_property(alreadyDefined GLOBAL PROPERTY ${propertyName} DEFINED)
  if(NOT alreadyDefined)
    message(FATAL_ERROR "No content details recorded for ${contentName}")
  endif()
  get_property(propertyValue GLOBAL PROPERTY ${propertyName})
  set(${outVar} "${propertyValue}" PARENT_SCOPE)

endfunction()


# Saves population details of the content, sets defaults for the
# SOURCE_DIR and BUILD_DIR.
function(BastardFetch_Declare contentName)

  set(options "")
  set(oneValueArgs SVN_REPOSITORY)
  set(multiValueArgs "")

  cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  unset(srcDirSuffix)
  unset(svnRepoArgs)
  if(ARG_SVN_REPOSITORY)
    # Add a hash of the svn repository URL to the source dir. This works
    # around the problem where if the URL changes, the download would
    # fail because it tries to checkout/update rather than switch the
    # old URL to the new one. We limit the hash to the first 7 characters
    # so that the source path doesn't get overly long (which can be a
    # problem on windows due to path length limits).
    string(SHA1 urlSHA ${ARG_SVN_REPOSITORY})
    string(SUBSTRING ${urlSHA} 0 7 urlSHA)
    set(srcDirSuffix "-${urlSHA}")
    set(svnRepoArgs  SVN_REPOSITORY ${ARG_SVN_REPOSITORY})
  endif()

  string(TOLOWER ${contentName} contentNameLower)
  __BastardFetch_declareDetails(
    ${contentNameLower}
    SOURCE_DIR "${BASTARD_FETCH_SRC_DIR}/${contentNameLower}"
    BINARY_DIR "${BASTARD_FETCH_BUILD_DIR}/${contentNameLower}"
    ${svnRepoArgs}
    # List these last so they can override things we set above
    ${ARG_UNPARSED_ARGUMENTS}
  )

endfunction()


#=======================================================================
# Set/get whether the specified content has been populated yet.
# The setter also records the source and binary dirs used.
#=======================================================================

# Internal use, projects must not call this directly. It is
# intended for use by the BastardFetch_Populate() function to
# record when BastardFetch_Populate() is called for a particular
# content name.
function(__BastardFetch_setPopulated contentName sourceDir binaryDir)

  string(TOLOWER ${contentName} contentNameLower)
  set(prefix "_BastardFetch_${contentNameLower}")

  set(propertyName "${prefix}_sourceDir")
  define_property(GLOBAL PROPERTY ${propertyName}
    BRIEF_DOCS "Internal implementation detail of BastardFetch_Populate()"
    FULL_DOCS  "Details used by BastardFetch_Populate() for ${contentName}"
  )
  set_property(GLOBAL PROPERTY ${propertyName} ${sourceDir})

  set(propertyName "${prefix}_binaryDir")
  define_property(GLOBAL PROPERTY ${propertyName}
    BRIEF_DOCS "Internal implementation detail of BastardFetch_Populate()"
    FULL_DOCS  "Details used by BastardFetch_Populate() for ${contentName}"
  )
  set_property(GLOBAL PROPERTY ${propertyName} ${binaryDir})

  set(propertyName "${prefix}_populated")
  define_property(GLOBAL PROPERTY ${propertyName}
    BRIEF_DOCS "Internal implementation detail of BastardFetch_Populate()"
    FULL_DOCS  "Details used by BastardFetch_Populate() for ${contentName}"
  )
  set_property(GLOBAL PROPERTY ${propertyName} True)

endfunction()


# Set variables in the calling scope for any of the retrievable
# properties. If no specific properties are requested, variables
# will be set for all retrievable properties.
#
# This function is intended to also be used by projects as the canonical
# way to detect whether they should call BastardFetch_Populate()
# and pull the populated source into the build with add_subdirectory(),
# if they are using the populated content in that way.
function(BastardFetch_GetProperties contentName)

  string(TOLOWER ${contentName} contentNameLower)

  set(options "")
  set(oneValueArgs SOURCE_DIR BINARY_DIR POPULATED)
  set(multiValueArgs "")

  cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if(NOT ARG_SOURCE_DIR AND
     NOT ARG_BINARY_DIR AND
     NOT ARG_POPULATED)
    # No specific properties requested, provide them all
    set(ARG_SOURCE_DIR ${contentNameLower}_SOURCE_DIR)
    set(ARG_BINARY_DIR ${contentNameLower}_BINARY_DIR)
    set(ARG_POPULATED  ${contentNameLower}_POPULATED)
  endif()

  set(prefix "_BastardFetch_${contentNameLower}")

  if(ARG_SOURCE_DIR)
    set(propertyName "${prefix}_sourceDir")
    get_property(value GLOBAL PROPERTY ${propertyName})
    if(value)
      set(${ARG_SOURCE_DIR} ${value} PARENT_SCOPE)
    endif()
  endif()

  if(ARG_BINARY_DIR)
    set(propertyName "${prefix}_binaryDir")
    get_property(value GLOBAL PROPERTY ${propertyName})
    if(value)
      set(${ARG_BINARY_DIR} ${value} PARENT_SCOPE)
    endif()
  endif()

  if(ARG_POPULATED)
    set(propertyName "${prefix}_populated")
    get_property(value GLOBAL PROPERTY ${propertyName} DEFINED)
    set(${ARG_POPULATED} ${value} PARENT_SCOPE)
  endif()

endfunction()


#=======================================================================
# Performing the population
#=======================================================================

# The value of contentName will always have been lowercased by the caller.
# All other arguments are assumed to be options that are understood by
# ExternalProject_Add(), except for QUIET and SUBBUILD_DIR.
function(__BastardFetch_directPopulate contentName)

  set(options
      QUIET
  )
  set(oneValueArgs
      SUBBUILD_DIR
      SOURCE_DIR
      BINARY_DIR
      # We need special processing if DOWNLOAD_NO_EXTRACT is true
      DOWNLOAD_NO_EXTRACT
      # Prevent the following from being passed through
      CONFIGURE_COMMAND
      BUILD_COMMAND
      INSTALL_COMMAND
      TEST_COMMAND
      # We force both of these to be ON since we are always executing serially
      # and we want all steps to have access to the terminal in case they
      # need input from the command line (e.g. ask for a private key password)
      # or they want to provide timely progress. We silently absorb and
      # discard these if they are set by the caller.
      USES_TERMINAL_DOWNLOAD
      USES_TERMINAL_UPDATE
  )
  set(multiValueArgs "")

  cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if(NOT ARG_SUBBUILD_DIR)
    message(FATAL_ERROR "Internal error: SUBBUILD_DIR not set")
  elseif(NOT IS_ABSOLUTE "${ARG_SUBBUILD_DIR}")
    set(ARG_SUBBUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/${ARG_SUBBUILD_DIR}")
  endif()

  if(NOT ARG_SOURCE_DIR)
    message(FATAL_ERROR "Internal error: SOURCE_DIR not set")
  elseif(NOT IS_ABSOLUTE "${ARG_SOURCE_DIR}")
    set(ARG_SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/${ARG_SOURCE_DIR}")
  endif()

  if(NOT ARG_BINARY_DIR)
    message(FATAL_ERROR "Internal error: BINARY_DIR not set")
  elseif(NOT IS_ABSOLUTE "${ARG_BINARY_DIR}")
    set(ARG_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/${ARG_BINARY_DIR}")
  endif()

  # Ensure the caller can know where to find the source and build directories
  # with some convenient variables. Doing this here ensures the caller sees
  # the correct result in the case where the default values are overridden by
  # the content details set by the project.
  set(${contentName}_SOURCE_DIR "${ARG_SOURCE_DIR}" PARENT_SCOPE)
  set(${contentName}_BINARY_DIR "${ARG_BINARY_DIR}" PARENT_SCOPE)

  # The unparsed arguments may contain spaces, so build up ARG_EXTRA
  # in such a way that it correctly substitutes into the generated
  # CMakeLists.txt file with each argument quoted.
  unset(ARG_EXTRA)
  foreach(arg IN LISTS ARG_UNPARSED_ARGUMENTS)
    set(ARG_EXTRA "${ARG_EXTRA} \"${arg}\"")
  endforeach()

  if(ARG_DOWNLOAD_NO_EXTRACT)
    set(ARG_EXTRA "${ARG_EXTRA} DOWNLOAD_NO_EXTRACT YES")
    set(__BASTARD_FETCH_COPY_FILE
"
ExternalProject_Get_Property(${contentName}-populate DOWNLOADED_FILE)
get_filename_component(dlFileName \"\${DOWNLOADED_FILE}\" NAME)

ExternalProject_Add_Step(${contentName}-populate copyfile
  COMMAND    \"${CMAKE_COMMAND}\" -E copy_if_different
             \"<DOWNLOADED_FILE>\" \"${ARG_SOURCE_DIR}\"
  DEPENDEES  patch
  DEPENDERS  configure
  BYPRODUCTS \"${ARG_SOURCE_DIR}/\${dlFileName}\"
  COMMENT    \"Copying file to SOURCE_DIR\"
)
")
  else()
    unset(__BASTARD_FETCH_COPY_FILE)
  endif()

  # Hide output if requested, but save it to a variable in case there's an
  # error so we can show the output upon failure. When not quiet, don't
  # capture the output to a variable because the user may want to see the
  # output as it happens (e.g. progress during long downloads). Combine both
  # stdout and stderr in the one capture variable so the output stays in order.
  if (ARG_QUIET)
    set(outputOptions
        OUTPUT_VARIABLE capturedOutput
        ERROR_VARIABLE  capturedOutput
    )
  else()
    set(capturedOutput)
    set(outputOptions)
    message(STATUS "Populating ${contentName}")
  endif()

  if(CMAKE_GENERATOR)
    set(generatorOpts "-G${CMAKE_GENERATOR}")
    if(CMAKE_GENERATOR_PLATFORM)
      list(APPEND generatorOpts "-A${CMAKE_GENERATOR_PLATFORM}")
    endif()
    if(CMAKE_GENERATOR_TOOLSET)
      list(APPEND generatorOpts "-T${CMAKE_GENERATOR_TOOLSET}")
    endif()

    if(CMAKE_MAKE_PROGRAM)
      list(APPEND generatorOpts "-DCMAKE_MAKE_PROGRAM:FILEPATH=${CMAKE_MAKE_PROGRAM}")
    endif()

  else()
    # Likely we've been invoked via CMake's script mode where no
    # generator is set (and hence CMAKE_MAKE_PROGRAM could not be
    # trusted even if provided). We will have to rely on being
    # able to find the default generator and build tool.
    unset(generatorOpts)
  endif()

  # Create and build a separate CMake project to carry out the population.
  # If we've already previously done these steps, they will not cause
  # anything to be updated, so extra rebuilds of the project won't occur.
  # Make sure to pass through CMAKE_MAKE_PROGRAM in case the main project
  # has this set to something not findable on the PATH.
  configure_file("${_FETCH_CONTENT_FUNCTION_DIR}/CMakeLists.cmake.in"
                 "${ARG_SUBBUILD_DIR}/CMakeLists.txt")
  execute_process(
    COMMAND ${CMAKE_COMMAND} ${generatorOpts} .
    RESULT_VARIABLE result
    ${outputOptions}
    WORKING_DIRECTORY "${ARG_SUBBUILD_DIR}"
  )
  if(result)
    if(capturedOutput)
      message("${capturedOutput}")
    endif()
    message(FATAL_ERROR "CMake step for ${contentName} failed: ${result}")
  endif()
  execute_process(
    COMMAND ${CMAKE_COMMAND} --build .
    RESULT_VARIABLE result
    ${outputOptions}
    WORKING_DIRECTORY "${ARG_SUBBUILD_DIR}"
  )
  if(result)
    if(capturedOutput)
      message("${capturedOutput}")
    endif()
    message(FATAL_ERROR "Build step for ${contentName} failed: ${result}")
  endif()

endfunction()


option(BASTARD_FETCH_FULLY_DISCONNECTED   "Disables all attempts to download or update content and assumes source dirs already exist")
option(BASTARD_FETCH_UPDATES_DISCONNECTED "Enables UPDATE_DISCONNECTED behavior for all content population")
option(BASTARD_FETCH_QUIET                "Enables QUIET option for all content population" ON)

# Populate the specified content using details stored from
# an earlier call to BastardFetch_Declare().
function(BastardFetch_Populate contentName)

  if(NOT contentName)
    message(FATAL_ERROR "Empty contentName not allowed for BastardFetch_Populate()")
  endif()

  string(TOLOWER ${contentName} contentNameLower)

  if(ARGN)
    # This is the direct population form with details fully specified
    # as part of the call, so we already have everything we need
    __BastardFetch_directPopulate(
      ${contentNameLower}
      SUBBUILD_DIR "${BASTARD_FETCH_SUBBUILD_DIR}/${contentNameLower}-subbuild"
      SOURCE_DIR   "${BASTARD_FETCH_SRC_DIR}/${contentNameLower}"
      BINARY_DIR   "${BASTARD_FETCH_BUILD_DIR}/${contentNameLower}"
      ${ARGN}  # Could override any of the above ..._DIR variables
    )

    # Pass source and binary dir variables back to the caller
    set(${contentNameLower}_SOURCE_DIR "${${contentNameLower}_SOURCE_DIR}" PARENT_SCOPE)
    set(${contentNameLower}_BINARY_DIR "${${contentNameLower}_BINARY_DIR}" PARENT_SCOPE)

    # Don't set global properties, or record that we did this population, since
    # this was a direct call outside of the normal declared details form.
    # We only want to save values in the global properties for content that
    # honours the hierarchical details mechanism so that projects are not
    # robbed of the ability to override details set in nested projects.
    return()
  endif()

  # No details provided, so assume they were saved from an earlier call
  # to BastardFetch_Declare(). Do a check that we haven't already
  # populated this content before in case the caller forgot to check.
  BastardFetch_GetProperties(${contentName})
  if(${contentNameLower}_POPULATED)
    message(FATAL_ERROR "Content ${contentName} already populated in ${${contentNameLower}_SOURCE_DIR}")
  endif()

  string(TOUPPER ${contentName} contentNameUpper)
  set(BASTARD_FETCH_SOURCE_DIR_${contentNameUpper}
      "${BASTARD_FETCH_SOURCE_DIR_${contentNameUpper}}"
      CACHE PATH "When not empty, overrides where to find pre-populated content for ${contentName}")

  if(BASTARD_FETCH_SOURCE_DIR_${contentNameUpper})
    # The source directory has been explicitly provided in the cache,
    # so no population is required
    set(${contentNameLower}_SOURCE_DIR "${BASTARD_FETCH_SOURCE_DIR_${contentNameUpper}}")
    set(${contentNameLower}_BINARY_DIR "${BASTARD_FETCH_BUILD_DIR}/${contentNameLower}")

  elseif(BASTARD_FETCH_FULLY_DISCONNECTED)
    # Bypass population and assume source is already there from a previous run
    set(${contentNameLower}_SOURCE_DIR "${BASTARD_FETCH_SRC_DIR}/${contentNameLower}")
    set(${contentNameLower}_BINARY_DIR "${BASTARD_FETCH_BUILD_DIR}/${contentNameLower}")

  else()
    # Support both a global "disconnect all updates" and a per-content
    # update test (either one being set disables updates for this content).
    option(BASTARD_FETCH_UPDATES_DISCONNECTED_${contentNameUpper}
           "Enables UPDATE_DISCONNECTED behavior just for population of ${contentName}")
    if(BASTARD_FETCH_UPDATES_DISCONNECTED OR
       BASTARD_FETCH_UPDATES_DISCONNECTED_${contentNameUpper})
      set(disconnectUpdates True)
    else()
      set(disconnectUpdates False)
    endif()

    if(BASTARD_FETCH_QUIET)
      set(quietFlag QUIET)
    else()
      unset(quietFlag)
    endif()

    __BastardFetch_getSavedDetails(${contentName} contentDetails)
    if("${contentDetails}" STREQUAL "")
      message(FATAL_ERROR "No details have been set for content: ${contentName}")
    endif()

    __BastardFetch_directPopulate(
      ${contentNameLower}
      ${quietFlag}
      UPDATE_DISCONNECTED ${disconnectUpdates}
      SUBBUILD_DIR "${BASTARD_FETCH_SUBBUILD_DIR}/${contentNameLower}-subbuild"
      SOURCE_DIR   "${BASTARD_FETCH_SRC_DIR}/${contentNameLower}"
      BINARY_DIR   "${BASTARD_FETCH_BUILD_DIR}/${contentNameLower}"
      # Put the saved details last so they can override any of the
      # the options we set above (this can include SOURCE_DIR or
      # BUILD_DIR)
      ${contentDetails}
    )
  endif()

  __BastardFetch_setPopulated(
    ${contentName}
    ${${contentNameLower}_SOURCE_DIR}
    ${${contentNameLower}_BINARY_DIR}
  )

  # Pass variables back to the caller. The variables passed back here
  # must match what BastardFetch_GetProperties() sets when it is called
  # with just the content name.
  set(${contentNameLower}_SOURCE_DIR "${${contentNameLower}_SOURCE_DIR}" PARENT_SCOPE)
  set(${contentNameLower}_BINARY_DIR "${${contentNameLower}_BINARY_DIR}" PARENT_SCOPE)
  set(${contentNameLower}_POPULATED  True PARENT_SCOPE)

endfunction()

# Arguments are assumed to be the names of dependencies that have been
# declared previously and should be populated. It is not an error if
# any of them have already been populated (they will just be skipped in
# that case). The command is implemented as a macro so that the variables
# defined by the BastardFetch_GetProperties() and BastardFetch_Populate()
# calls will be available to the caller.
macro(BastardFetch_MakeAvailable)

  foreach(contentName IN ITEMS ${ARGV})
    string(TOLOWER ${contentName} contentNameLower)
    BastardFetch_GetProperties(${contentName})
    if(NOT ${contentNameLower}_POPULATED)
      BastardFetch_Populate(${contentName})

      # Only try to call add_subdirectory() if the populated content
      # can be treated that way. Protecting the call with the check
      # allows this function to be used for projects that just want
      # to ensure the content exists, such as to provide content at
      # a known location. We check the saved details for an optional
      # SOURCE_SUBDIR which can be used in the same way as its meaning
      # for ExternalProject. It won't matter if it was passed through
      # to the ExternalProject sub-build, since it would have been
      # ignored there.
      set(__fc_srcdir "${${contentNameLower}_SOURCE_DIR}")
      __BastardFetch_getSavedDetails(${contentName} contentDetails)
      if("${contentDetails}" STREQUAL "")
        message(FATAL_ERROR "No details have been set for content: ${contentName}")
      endif()
      cmake_parse_arguments(__fc_arg "" "SOURCE_SUBDIR" "" ${contentDetails})
      if(NOT "${__fc_arg_SOURCE_SUBDIR}" STREQUAL "")
        string(APPEND __fc_srcdir "/${__fc_arg_SOURCE_SUBDIR}")
      endif()

      if(EXISTS ${__fc_srcdir}/CMakeLists.txt)
        add_subdirectory(${__fc_srcdir} ${${contentNameLower}_BINARY_DIR})
      endif()

      unset(__fc_srcdir)
    endif()
  endforeach()

endmacro()
