# ============================================================================
# Platform Detection Module
# Detects operating system and sets platform-specific variables
# ============================================================================

message(STATUS "Detecting platform...")

# ============================================================================
# Operating System Detection
# ============================================================================

if(WIN32)
    set(VRHI_PLATFORM_WINDOWS ON CACHE BOOL "Windows platform" FORCE)
    set(VRHI_PLATFORM_NAME "Windows")
    message(STATUS "Platform: Windows")
    
    # Windows-specific backend availability
    set(VRHI_BACKEND_D3D12_AVAILABLE ON)
    
    # Enable D3D12 by default on Windows
    if(NOT DEFINED VRHI_ENABLE_D3D12)
        set(VRHI_ENABLE_D3D12 ON)
    endif()
    
elseif(APPLE)
    set(VRHI_PLATFORM_MACOS ON CACHE BOOL "macOS platform" FORCE)
    set(VRHI_PLATFORM_NAME "macOS")
    message(STATUS "Platform: macOS")
    
    # macOS-specific backend availability
    set(VRHI_BACKEND_METAL_AVAILABLE ON)
    
    # Enable Metal by default on macOS
    if(NOT DEFINED VRHI_ENABLE_METAL)
        set(VRHI_ENABLE_METAL ON)
    endif()
    
    # Detect iOS
    if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
        set(VRHI_PLATFORM_IOS ON CACHE BOOL "iOS platform" FORCE)
        set(VRHI_PLATFORM_NAME "iOS")
        message(STATUS "Platform: iOS")
    endif()
    
elseif(UNIX)
    set(VRHI_PLATFORM_LINUX ON CACHE BOOL "Linux platform" FORCE)
    set(VRHI_PLATFORM_NAME "Linux")
    message(STATUS "Platform: Linux")
    
    # Detect Android
    if(ANDROID)
        set(VRHI_PLATFORM_ANDROID ON CACHE BOOL "Android platform" FORCE)
        set(VRHI_PLATFORM_NAME "Android")
        message(STATUS "Platform: Android")
        set(VRHI_BACKEND_GLES_AVAILABLE ON)
    endif()
endif()

# ============================================================================
# Processor Architecture Detection
# ============================================================================

if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm" OR CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
    set(VRHI_PLATFORM_ARM ON CACHE BOOL "ARM processor" FORCE)
    message(STATUS "Processor: ARM (${CMAKE_SYSTEM_PROCESSOR})")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64" OR CMAKE_SYSTEM_PROCESSOR MATCHES "AMD64")
    set(VRHI_PLATFORM_X64 ON CACHE BOOL "x64 processor" FORCE)
    message(STATUS "Processor: x64 (${CMAKE_SYSTEM_PROCESSOR})")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "i386" OR CMAKE_SYSTEM_PROCESSOR MATCHES "i686")
    set(VRHI_PLATFORM_X86 ON CACHE BOOL "x86 processor" FORCE)
    message(STATUS "Processor: x86 (${CMAKE_SYSTEM_PROCESSOR})")
endif()

# ============================================================================
# Backend Availability
# ============================================================================

# Vulkan is available on most platforms
set(VRHI_BACKEND_VULKAN_AVAILABLE ON)

# OpenGL desktop is available on Windows, Linux, and macOS
if(VRHI_PLATFORM_WINDOWS OR VRHI_PLATFORM_LINUX OR VRHI_PLATFORM_MACOS)
    set(VRHI_BACKEND_OPENGL_AVAILABLE ON)
endif()

# OpenGL ES is available on mobile platforms and embedded systems
if(VRHI_PLATFORM_ANDROID OR VRHI_PLATFORM_IOS OR VRHI_PLATFORM_ARM)
    set(VRHI_BACKEND_GLES_AVAILABLE ON)
endif()

# ============================================================================
# Platform-Specific Definitions
# ============================================================================

if(VRHI_PLATFORM_WINDOWS)
    add_compile_definitions(
        VRHI_PLATFORM_WINDOWS
        _CRT_SECURE_NO_WARNINGS
        NOMINMAX
        WIN32_LEAN_AND_MEAN
        _WIN32_WINNT=0x0A00  # Windows 10
    )
elseif(VRHI_PLATFORM_MACOS)
    add_compile_definitions(VRHI_PLATFORM_MACOS)
elseif(VRHI_PLATFORM_IOS)
    add_compile_definitions(VRHI_PLATFORM_IOS)
elseif(VRHI_PLATFORM_LINUX)
    add_compile_definitions(VRHI_PLATFORM_LINUX)
elseif(VRHI_PLATFORM_ANDROID)
    add_compile_definitions(VRHI_PLATFORM_ANDROID)
endif()

# Export platform information
set(VRHI_PLATFORM_DETECTED TRUE CACHE BOOL "Platform detection completed" FORCE)
