# ============================================================================
# External Dependencies
# ============================================================================
#
# This directory contains third-party dependencies required by VRHI.
# All dependencies are self-contained within this directory to ensure:
#   - Complete control over dependency versions
#   - Offline builds without network access
#   - No git submodules or external package managers required
#
# Dependencies will be added here as needed during development.
# See the documentation for details on which dependencies are included.
#
# ============================================================================

message(STATUS "External dependencies directory")

# ============================================================================
# GoogleTest (for unit testing)
# ============================================================================
if(VRHI_BUILD_TESTS)
    message(STATUS "  Adding GoogleTest")
    
    # Prevent GoogleTest from overriding our compiler/linker options
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    
    # Disable installation of GoogleTest
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    
    # Disable building gmock if not needed
    set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
    
    # Add GoogleTest
    add_subdirectory(googletest-1.17.0)
    
    # Add to external targets list
    list(APPEND VRHI_EXTERNAL_TARGETS gtest gtest_main)
endif()

# Placeholder for future third-party dependencies
# Examples:
#   - Vulkan headers (header-only)
#   - GLAD (OpenGL loader)
#   - vk-bootstrap (Vulkan initialization helper)
#   - SDL2/SDL3 (optional window system)
#   - GLFW (optional window system)
#   - etc.

# Future dependencies will be added with add_subdirectory() or as interface libraries
# Example:
# add_subdirectory(vulkan-headers)
# add_subdirectory(glad)
# add_subdirectory(vk-bootstrap)

# Export list of external targets
set(VRHI_EXTERNAL_TARGETS "${VRHI_EXTERNAL_TARGETS}" CACHE INTERNAL "List of external dependency targets")
