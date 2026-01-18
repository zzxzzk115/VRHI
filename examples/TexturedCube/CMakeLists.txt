# ============================================================================
# TexturedCube Example
# ============================================================================

add_executable(TexturedCube
    main.cpp
)

target_link_libraries(TexturedCube
    PRIVATE
        VRHI::VRHI
        stb_image
)

target_include_directories(TexturedCube
    PRIVATE
        ${CMAKE_SOURCE_DIR}/external/stb
)

# Set output directory
set_target_properties(TexturedCube PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/examples
)

# Copy assets to build directory
add_custom_command(TARGET TexturedCube POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        ${CMAKE_BINARY_DIR}/bin/examples/assets
    COMMENT "Copying assets to build directory"
)

# Copy to examples directory on build
install(TARGETS TexturedCube
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}/examples
)

# Install assets
install(DIRECTORY ${CMAKE_SOURCE_DIR}/assets
    DESTINATION ${CMAKE_INSTALL_BINDIR}/examples
)
