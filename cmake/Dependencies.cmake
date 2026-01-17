# ============================================================================
# Dependencies Module
# Handles finding and configuring external dependencies
# ============================================================================

message(STATUS "Configuring dependencies...")

# ============================================================================
# System Dependencies
# ============================================================================

# Threads (required for most platforms)
find_package(Threads REQUIRED)

# ============================================================================
# Graphics API Dependencies
# ============================================================================

# Vulkan SDK (optional, for Vulkan backend)
if(VRHI_ENABLE_VULKAN)
    # Try to find Vulkan SDK
    find_package(Vulkan QUIET)
    
    if(Vulkan_FOUND)
        message(STATUS "Vulkan SDK found: ${Vulkan_INCLUDE_DIRS}")
        set(VRHI_HAS_VULKAN_SDK ON)
    else()
        message(STATUS "Vulkan SDK not found - will use header-only Vulkan support")
        set(VRHI_HAS_VULKAN_SDK OFF)
    endif()
endif()

# OpenGL (system provided, for OpenGL backend)
if(VRHI_ENABLE_OPENGL)
    find_package(OpenGL QUIET)
    
    if(OpenGL_FOUND)
        message(STATUS "OpenGL found: ${OPENGL_LIBRARIES}")
        set(VRHI_HAS_OPENGL ON)
    else()
        message(WARNING "OpenGL not found - OpenGL backend may not work")
        set(VRHI_HAS_OPENGL OFF)
    endif()
endif()

# Direct3D 12 (Windows only)
if(VRHI_ENABLE_D3D12 AND VRHI_PLATFORM_WINDOWS)
    # D3D12 is part of Windows SDK, no find_package needed
    set(VRHI_HAS_D3D12 ON)
    message(STATUS "Direct3D 12 support enabled (Windows SDK)")
endif()

# Metal (macOS/iOS only)
if(VRHI_ENABLE_METAL AND (VRHI_PLATFORM_MACOS OR VRHI_PLATFORM_IOS))
    # Metal is part of macOS/iOS SDK
    set(VRHI_HAS_METAL ON)
    message(STATUS "Metal support enabled (Apple SDK)")
endif()

# ============================================================================
# Window System Dependencies
# ============================================================================

# SDL2
if(VRHI_WINDOW_SDL2)
    find_package(SDL2 QUIET)
    if(SDL2_FOUND)
        message(STATUS "SDL2 found")
        set(VRHI_HAS_SDL2 ON)
    else()
        message(STATUS "SDL2 not found - will be added as external dependency later")
        set(VRHI_HAS_SDL2 OFF)
    endif()
endif()

# SDL3
if(VRHI_WINDOW_SDL3)
    find_package(SDL3 QUIET)
    if(SDL3_FOUND)
        message(STATUS "SDL3 found")
        set(VRHI_HAS_SDL3 ON)
    else()
        message(STATUS "SDL3 not found - will be added as external dependency later")
        set(VRHI_HAS_SDL3 OFF)
    endif()
endif()

# GLFW
if(VRHI_WINDOW_GLFW)
    # Always use bundled GLFW from external/ to avoid conflicts
    message(STATUS "GLFW not found - will be added as external dependency later")
    set(VRHI_HAS_GLFW OFF)
endif()

# EGL (Linux/Android)
if(VRHI_WINDOW_EGL)
    find_package(EGL QUIET)
    if(EGL_FOUND)
        message(STATUS "EGL found")
        set(VRHI_HAS_EGL ON)
    else()
        message(STATUS "EGL not found - will use system EGL")
        set(VRHI_HAS_EGL OFF)
    endif()
endif()

# ============================================================================
# Linux-Specific Dependencies
# ============================================================================

if(VRHI_PLATFORM_LINUX)
    # X11 for window management on Linux
    find_package(X11)
    if(X11_FOUND)
        message(STATUS "X11 found")
        set(VRHI_HAS_X11 ON)
    endif()
    
    # Wayland (optional)
    find_package(Wayland QUIET)
    if(Wayland_FOUND)
        message(STATUS "Wayland found")
        set(VRHI_HAS_WAYLAND ON)
    endif()
endif()

# ============================================================================
# Validation and Debug Tools
# ============================================================================

if(VRHI_ENABLE_VALIDATION)
    if(VRHI_ENABLE_VULKAN AND Vulkan_FOUND)
        # Check for Vulkan validation layers
        message(STATUS "Vulkan validation layers will be used if available")
    endif()
endif()

# ============================================================================
# Export Variables
# ============================================================================

# Export dependency status for use in other CMakeLists.txt files
set(VRHI_DEPENDENCIES_CONFIGURED TRUE CACHE BOOL "Dependencies configured" FORCE)

message(STATUS "Dependencies configured")
