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

# Placeholder for future third-party dependencies
# Examples:
#   - Vulkan headers (header-only)
#   - GLAD (OpenGL loader)
#   - vk-bootstrap (Vulkan initialization helper)
#   - SDL2/SDL3 (optional window system)
#   - GLFW (optional window system)
#   - etc.

message(STATUS "External dependencies directory")
message(STATUS "  Third-party dependencies will be added here during development")

# Future dependencies will be added with add_subdirectory() or as interface libraries
# Example:
# add_subdirectory(vulkan-headers)
# add_subdirectory(glad)
# add_subdirectory(vk-bootstrap)

# Export empty list for now (will be populated as dependencies are added)
set(VRHI_EXTERNAL_TARGETS "" CACHE INTERNAL "List of external dependency targets")
