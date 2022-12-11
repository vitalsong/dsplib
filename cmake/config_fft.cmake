# -----------------------------------------------------------------------
function (CheckFFTW3)
        find_package(PkgConfig)
        if(PKG_CONFIG_FOUND)
                pkg_check_modules(PKG_FFTW QUIET "fftw3")
        endif()

        find_library(FFTW_DOUBLE_LIB
                NAMES "fftw3"
                PATHS ${PKG_FFTW_LIBRARY_DIRS} ${LIB_INSTALL_DIR}
        )

        find_library(FFTW_FLOAT_LIB
                NAMES "fftw3f"
                PATHS ${PKG_FFTW_LIBRARY_DIRS} ${LIB_INSTALL_DIR}
        )

        find_path(FFTW_INCLUDE_DIRS
                NAMES "fftw3.h"
                PATHS ${PKG_FFTW_INCLUDE_DIRS} ${INCLUDE_INSTALL_DIR}
        )

        if (FFTW_DOUBLE_LIB)
                set(FFTW_DOUBLE_LIB_FOUND TRUE PARENT_SCOPE)
                set(FFTW_LIBRARIES ${FFTW_LIBRARIES} ${FFTW_DOUBLE_LIB})
                add_library(FFT:FFTW3_DOUBLE INTERFACE IMPORTED)
                set_target_properties(FFT:FFTW3_DOUBLE
                        PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${FFTW_INCLUDE_DIRS}"
                        INTERFACE_LINK_LIBRARIES "${FFTW_DOUBLE_LIB}"
                )
        endif()
        
        if (FFTW_FLOAT_LIB)
                set(FFTW_FLOAT_LIB_FOUND TRUE PARENT_SCOPE)
                set(FFTW_LIBRARIES ${FFTW_LIBRARIES} ${FFTW_FLOAT_LIB})
                add_library(FFT:FFTW3_FLOAT INTERFACE IMPORTED)
                set_target_properties(FFT:FFTW3_FLOAT
                        PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${FFTW_INCLUDE_DIRS}"
                        INTERFACE_LINK_LIBRARIES "${FFTW_FLOAT_LIB}"
                )
        endif()
endfunction(CheckFFTW3)

# -----------------------------------------------------------------------
function(CheckNE10)     
        include(FindPackageHandleStandardArgs)
        if (NOT "${CMAKE_SYSTEM_PROCESSOR}" MATCHES "arm|aarch")
                return()
        endif()

        if (TARGET NE10)
                set(NE10_LIB_FOUND ON PARENT_SCOPE)
                add_library(FFT::NE10 ALIAS NE10)
                return()
        endif()
        
        if (DEFINED ENV{NE10_DIR})
                set(NE10_DIR "$ENV{NE10_DIR}")
        endif()

        find_path(NE10_INCLUDE_DIR
                "NE10.h"
                HINTS "${NE10_DIR}" "${NE10_DIR}/inc" "${NE10_DIR}/include"
                CMAKE_FIND_ROOT_PATH_BOTH
        )

        find_library(NE10_LIBRARY
                NAMES NE10
                HINTS "${NE10_DIR}" "${NE10_DIR}/lib"
                CMAKE_FIND_ROOT_PATH_BOTH
        )

        find_package_handle_standard_args(NE10
                NE10_INCLUDE_DIR
                NE10_LIBRARY
        )

        if (NOT NE10_FOUND)
                return()
        endif()

        try_compile(NE10_COMPILED
                "${CMAKE_BINARY_DIR}"
                "${CMAKE_SOURCE_DIR}/cmake/try_compile/ne10.cpp"
                LINK_LIBRARIES "${NE10_LIBRARY}"
        )

        if (NE10_COMPILED)
                add_library(FFT::NE10 INTERFACE IMPORTED)
                set_target_properties(FFT::NE10
                        PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${NE10_INCLUDE_DIRS}"
                        INTERFACE_LINK_LIBRARIES "${NE10_LIBRARY}"
                )
                set(NE10_LIB_FOUND ON PARENT_SCOPE)
        endif()
endfunction(CheckNE10)

# -----------------------------------------------------------------------
function(ConfigFFTBackend TARGET USE_FLOAT32)
        CheckFFTW3()
        CheckNE10()
        if (USE_FLOAT32 AND NE10_LIB_FOUND)
                target_link_libraries(${TARGET} FFT::NE10)
                target_compile_definitions(${TARGET} PRIVATE -DFFT_BACKEND_NE10)
                message(STATUS "FFT backend: NE10")
        elseif (NOT USE_FLOAT32 AND FFTW_DOUBLE_LIB_FOUND)
                target_link_libraries(${TARGET} FFT:FFTW3_DOUBLE)
                target_compile_definitions(${TARGET} PRIVATE -DFFT_BACKEND_FFTW3 -DFFTW_DOUBLE)
                message(STATUS "FFT backend: fftw3-double")
        elseif (USE_FLOAT32 AND FFTW_FLOAT_LIB_FOUND)
                target_link_libraries(${TARGET} FFT:FFTW3_FLOAT)
                target_compile_definitions(${TARGET} PRIVATE -DFFT_BACKEND_FFTW3 -DFFTW_FLOAT)
                message(STATUS "FFT backend: fftw3-float")
        else()
                message(STATUS "FFT backend: default")
        endif()
endfunction(ConfigFFTBackend)
