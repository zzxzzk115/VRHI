# ============================================================================
# HelloTriangle Example
# ============================================================================

add_executable(HelloTriangle
    main.cpp
)

target_link_libraries(HelloTriangle
    PRIVATE
        VRHI::VRHI
)

# Set output directory
set_target_properties(HelloTriangle PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/examples
)

# Copy to examples directory on build
install(TARGETS HelloTriangle
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}/examples
)
