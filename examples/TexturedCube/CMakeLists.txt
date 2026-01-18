# ============================================================================
# TexturedCube Example
# ============================================================================

add_executable(TexturedCube
    main.cpp
)

target_link_libraries(TexturedCube
    PRIVATE
        VRHI::VRHI
)

# Set output directory
set_target_properties(TexturedCube PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/examples
)

# Copy to examples directory on build
install(TARGETS TexturedCube
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}/examples
)
