# ============================================================================
# GLAD OpenGL 3.3 Loader
# ============================================================================

project(glad_gl33 LANGUAGES C)

# Create GLAD library
add_library(glad_gl33 STATIC
    src/glad.c
)

target_include_directories(glad_gl33
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Set properties
set_target_properties(glad_gl33 PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    C_STANDARD 11
    EXPORT_NAME gl33
)

# Disable strict warnings for GLAD (third-party code)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(glad_gl33 PRIVATE
        -Wno-pedantic
        -Wno-error
    )
endif()

# Add alias for consistency
add_library(glad::gl33 ALIAS glad_gl33)

# Export GLAD as part of VRHI installation
install(TARGETS glad_gl33
    EXPORT VRHITargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)
