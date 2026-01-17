# ============================================================================
# Compiler Options Module
# Configures compiler warnings, optimizations, and flags
# ============================================================================

message(STATUS "Configuring compiler options...")

# ============================================================================
# Warning Levels
# ============================================================================

if(MSVC)
    # MSVC warnings
    add_compile_options(
        /W4                 # Warning level 4
        /WX                 # Treat warnings as errors
        /permissive-        # Standards conformance mode
        /Zc:__cplusplus     # Enable updated __cplusplus macro
        /utf-8              # UTF-8 source and execution charset
    )
    
    # Disable specific warnings
    add_compile_options(
        /wd4201             # nonstandard extension used: nameless struct/union
        /wd4100             # unreferenced formal parameter (common in interface implementations)
    )
else()
    # GCC/Clang warnings
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        -Werror             # Treat warnings as errors
        -Wno-unused-parameter  # Allow unused parameters in interface implementations
    )
    
    # Additional warnings for better code quality
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        add_compile_options(
            -Wno-c++98-compat
            -Wno-c++98-compat-pedantic
        )
    endif()
endif()

# ============================================================================
# Optimization Flags
# ============================================================================

if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    if(MSVC)
        add_compile_options(
            /O2                 # Maximize speed
            /GL                 # Whole program optimization
            /Oi                 # Generate intrinsic functions
        )
        add_link_options(/LTCG)  # Link-time code generation
    else()
        add_compile_options(
            -O3                 # Maximum optimization
            -march=native       # Optimize for current processor (can be disabled for portability)
        )
        
        # Enable Link Time Optimization (LTO)
        if(NOT APPLE)
            add_compile_options(-flto)
            add_link_options(-flto)
        endif()
    endif()
elseif(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    if(MSVC)
        add_compile_options(/O1 /Os)  # Minimize size
    else()
        add_compile_options(-Os)      # Optimize for size
    endif()
endif()

# ============================================================================
# Debug Flags
# ============================================================================

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    # Define debug macro
    add_compile_definitions(VRHI_DEBUG=1)
    
    if(MSVC)
        add_compile_options(
            /Od                 # Disable optimization
            /Zi                 # Debug information format
            /RTC1               # Runtime checks
        )
    else()
        add_compile_options(
            -O0                 # No optimization
            -g3                 # Maximum debug info
        )
        
        # Enable sanitizers on non-Windows platforms
        if(NOT WIN32 AND NOT APPLE)
            option(VRHI_ENABLE_ASAN "Enable AddressSanitizer" ON)
            option(VRHI_ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)
            
            if(VRHI_ENABLE_ASAN)
                add_compile_options(-fsanitize=address)
                add_link_options(-fsanitize=address)
                message(STATUS "AddressSanitizer enabled")
            endif()
            
            if(VRHI_ENABLE_UBSAN)
                add_compile_options(-fsanitize=undefined)
                add_link_options(-fsanitize=undefined)
                message(STATUS "UndefinedBehaviorSanitizer enabled")
            endif()
        endif()
    endif()
else()
    # Release builds: define NDEBUG to disable assertions
    add_compile_definitions(NDEBUG)
endif()

# ============================================================================
# Platform-Specific Flags
# ============================================================================

if(VRHI_PLATFORM_WINDOWS)
    # Windows-specific flags
    if(MSVC)
        # Enable multi-processor compilation
        add_compile_options(/MP)
        
        # Generate PDB files even for release builds
        if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
            add_compile_options(/Zi)
            add_link_options(/DEBUG /OPT:REF /OPT:ICF)
        endif()
    endif()
    
elseif(VRHI_PLATFORM_MACOS OR VRHI_PLATFORM_IOS)
    # Apple-specific flags
    add_compile_options(
        -fno-common
        -fvisibility=hidden
    )
    
    # Set deployment target
    if(VRHI_PLATFORM_MACOS)
        set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Minimum macOS deployment version")
    endif()
    
elseif(VRHI_PLATFORM_LINUX)
    # Linux-specific flags
    add_compile_options(
        -fvisibility=hidden
        -fPIC
    )
    
    # Enable colored diagnostics
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        add_compile_options(-fcolor-diagnostics)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        add_compile_options(-fdiagnostics-color=always)
    endif()
endif()

# ============================================================================
# C++ Feature Checks
# ============================================================================

# Ensure C++23 support
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "19.30")
        message(FATAL_ERROR "MSVC 2022 (19.30) or later is required for C++23 support")
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "14.0")
        message(FATAL_ERROR "GCC 14.0 or later is required for full C++23 support")
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "AppleClang")
    # Apple Clang has different versioning - std::expected support starts from Xcode 15.3 (Apple Clang 15.0.0)
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "15.0")
        message(FATAL_ERROR "Apple Clang 15.0 or later (Xcode 15.3+) is required for C++23 support (std::expected)")
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "19.0")
        message(FATAL_ERROR "Clang 19.0 or later is required for full C++23 support")
    endif()
endif()

# ============================================================================
# Profiling Support
# ============================================================================

if(VRHI_ENABLE_PROFILING)
    add_compile_definitions(VRHI_ENABLE_PROFILING=1)
    message(STATUS "Profiling support enabled")
endif()

# ============================================================================
# Export Compile Commands (for IDEs and tools)
# ============================================================================

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "Export compile commands" FORCE)

message(STATUS "Compiler options configured for ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
