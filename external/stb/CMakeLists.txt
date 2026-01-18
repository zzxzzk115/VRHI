# ============================================================================
# stb_image - Image Loading Library
# ============================================================================

project(stb LANGUAGES C)

# Create stb_image library
add_library(stb_image STATIC
    stb_image.c
)

target_include_directories(stb_image
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include/stb>
)

# Set properties
set_target_properties(stb_image PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    C_STANDARD 99
    EXPORT_NAME image
)

# Disable strict warnings for stb (third-party code)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(stb_image PRIVATE
        -Wno-pedantic
        -Wno-error
    )
elseif(MSVC)
    target_compile_options(stb_image PRIVATE
        /W0
    )
endif()

# Add alias for consistency
add_library(stb::image ALIAS stb_image)

# Export stb_image as part of VRHI installation
install(TARGETS stb_image
    EXPORT VRHITargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install headers
install(FILES stb_image.h
    DESTINATION include/stb
)
