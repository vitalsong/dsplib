# link fftw3 to target and add macro FFT_BACKEND
function(ConfigFFTBackend TARGET USE_FLOAT32)

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

        # double lib
        if (FFTW_DOUBLE_LIB)
                set(FFTW_DOUBLE_LIB_FOUND TRUE)
                set(FFTW_LIBRARIES ${FFTW_LIBRARIES} ${FFTW_DOUBLE_LIB})
                add_library(FFTW::Double INTERFACE IMPORTED)
                set_target_properties(FFTW::Double
                        PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${FFTW_INCLUDE_DIRS}"
                        INTERFACE_LINK_LIBRARIES "${FFTW_DOUBLE_LIB}"
                )
        else()
                set(FFTW_DOUBLE_LIB_FOUND FALSE)
        endif()
        
        # float lib
        if (FFTW_FLOAT_LIB)
                set(FFTW_FLOAT_LIB_FOUND TRUE)
                set(FFTW_LIBRARIES ${FFTW_LIBRARIES} ${FFTW_FLOAT_LIB})
                add_library(FFTW::Float INTERFACE IMPORTED)
                set_target_properties(FFTW::Float
                        PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${FFTW_INCLUDE_DIRS}"
                        INTERFACE_LINK_LIBRARIES "${FFTW_FLOAT_LIB}"
                )
        else()
                set(FFTW_FLOAT_LIB_FOUND FALSE)
        endif()

        # link to target
        if (NOT USE_FLOAT32 AND FFTW_DOUBLE_LIB_FOUND)
                target_link_libraries(${TARGET} FFTW::Double)
                target_compile_definitions(${TARGET} PRIVATE -DFFT_BACKEND_FFTW3)
                message(STATUS "Use fft backend: fftw-double")
        elseif(USE_FLOAT32 AND FFTW_FLOAT_LIB_FOUND)
                target_link_libraries(${TARGET} FFTW::Float)
                target_compile_definitions(${TARGET} PRIVATE -DFFT_BACKEND_FFTW3_F)
                message(STATUS "Use fft backend: fftw-float")
        else()
                message(STATUS "Use fft backend: default")
        endif()
endfunction(ConfigFFTBackend)

