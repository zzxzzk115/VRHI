# ============================================================================
# VRHI Examples
# ============================================================================

message(STATUS "Configuring examples...")

# Examples are excluded from the default 'all' target
# Build with: cmake --build build --target <example_name>

# Example subdirectories
add_subdirectory(HelloTriangle)
add_subdirectory(FeatureDetection)
add_subdirectory(TexturedCube)

# Future examples
# add_subdirectory(ComputeShader)
# add_subdirectory(MultiBackend)

message(STATUS "Examples configured")
message(STATUS "  Use -DVRHI_BUILD_EXAMPLES=ON to enable examples")
