include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

function(ConfigInstallTarget TARGET)
    install(TARGETS ${TARGET}
        EXPORT ${PROJECT_NAME}Targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )

    install(EXPORT ${TARGET}Targets
        FILE ${TARGET}Targets.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${TARGET}
    )

    configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}Config.cmake"
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${TARGET}
    )

    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}Config.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${TARGET}
    )

    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}ConfigVersion.cmake"
        COMPATIBILITY SameMinorVersion
    )

    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}ConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${TARGET}
    )
endfunction(ConfigInstallTarget)
