# ============================================================================
# TexturedCube Example
# ============================================================================

add_executable(TexturedCube
    main.cpp
)

target_link_libraries(TexturedCube
    PRIVATE
        VRHI::VRHI
        glad_gl33
        stb_image
)

target_include_directories(TexturedCube
    PRIVATE
        ${CMAKE_SOURCE_DIR}/external/glad_gl33/include
        ${CMAKE_SOURCE_DIR}/external/stb
        ${CMAKE_SOURCE_DIR}/src/Backends/OpenGL33
)

# Set output directory
set_target_properties(TexturedCube PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/examples
)

# Copy to examples directory on build
install(TARGETS TexturedCube
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}/examples
)
