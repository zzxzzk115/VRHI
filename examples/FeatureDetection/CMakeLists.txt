# ============================================================================
# FeatureDetection Example
# ============================================================================

add_executable(FeatureDetection
    main.cpp
)

target_link_libraries(FeatureDetection
    PRIVATE
        VRHI::VRHI
)

# Set output directory
set_target_properties(FeatureDetection PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/examples
)

# Copy to examples directory on build
install(TARGETS FeatureDetection
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}/examples
)
