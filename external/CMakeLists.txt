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

# ============================================================================
# GLAD OpenGL Loaders
# ============================================================================
if(VRHI_ENABLE_OPENGL)
    message(STATUS "  Adding GLAD OpenGL 3.3")
    add_subdirectory(glad_gl33)
    list(APPEND VRHI_EXTERNAL_TARGETS glad_gl33)
    
    # Future: Add other GLAD versions as needed
    # add_subdirectory(glad_gl46)
endif()

# ============================================================================
# GLFW (Window System)
# ============================================================================
if(VRHI_WINDOW_GLFW)
    message(STATUS "  Adding GLFW 3.4")
    
    # Configure GLFW options
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    
    # Platform-specific GLFW options
    if(UNIX AND NOT APPLE)
        set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
        set(GLFW_BUILD_X11 ON CACHE BOOL "" FORCE)
    endif()
    
    # Enable GLFW install so it can be exported with VRHI
    set(GLFW_INSTALL ON CACHE BOOL "" FORCE)
    
    add_subdirectory(glfw-3.4)
    
    # GLFW has some warnings with strict compiler settings, disable Werror for it
    # Only apply GCC/Clang-specific flags to avoid MSVC errors
    if(TARGET glfw)
        if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
            target_compile_options(glfw PRIVATE 
                -Wno-error=missing-field-initializers
                -Wno-error=sign-compare
                -Wno-error=pedantic
            )
        endif()
    endif()
    
    list(APPEND VRHI_EXTERNAL_TARGETS glfw)
endif()

# ============================================================================
# Shader Compilation Libraries (glslang + SPIRV-Cross)
# ============================================================================
message(STATUS "  Adding Shader Compilation Libraries")

# ----------------------------------------------------------------------------
# glslang (GLSL → SPIR-V Compiler)
# ----------------------------------------------------------------------------
message(STATUS "    Adding glslang")

# Disable glslang tests and binaries
set(ENABLE_GLSLANG_BINARIES OFF CACHE BOOL "" FORCE)
set(BUILD_EXTERNAL OFF CACHE BOOL "" FORCE)
set(ENABLE_SPVREMAPPER OFF CACHE BOOL "" FORCE)
set(ENABLE_CTEST OFF CACHE BOOL "" FORCE)

# Enable SPIRV generation (required)
set(ENABLE_SPIRV ON CACHE BOOL "" FORCE)

# Disable SPIRV-Tools optimization (we don't need it for v1.0)
set(ENABLE_OPT OFF CACHE BOOL "" FORCE)

# Always skip glslang's own installation - we'll handle it ourselves
set(ENABLE_GLSLANG_INSTALL OFF CACHE BOOL "" FORCE)

# Add glslang
add_subdirectory(glslang-16.1.0)

# Disable warnings-as-errors for glslang (it has some format warnings)
if(TARGET glslang)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(glslang PRIVATE -Wno-error)
    elseif(MSVC)
        target_compile_options(glslang PRIVATE /WX-)
    endif()
endif()
if(TARGET SPIRV)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(SPIRV PRIVATE -Wno-error)
    elseif(MSVC)
        target_compile_options(SPIRV PRIVATE /WX-)
    endif()
endif()

# Add to external targets
list(APPEND VRHI_EXTERNAL_TARGETS glslang SPIRV)

# ----------------------------------------------------------------------------
# SPIRV-Cross (SPIR-V → GLSL/HLSL/MSL Converter)
# ----------------------------------------------------------------------------
message(STATUS "    Adding SPIRV-Cross")

# Disable SPIRV-Cross tests and CLI
set(SPIRV_CROSS_CLI OFF CACHE BOOL "" FORCE)
set(SPIRV_CROSS_ENABLE_TESTS OFF CACHE BOOL "" FORCE)

# Always skip SPIRV-Cross's own installation - we'll handle it ourselves
set(SPIRV_CROSS_SKIP_INSTALL ON CACHE BOOL "" FORCE)

# Enable only the backends we need for v1.0
set(SPIRV_CROSS_ENABLE_GLSL ON CACHE BOOL "" FORCE)  # For OpenGL
set(SPIRV_CROSS_ENABLE_HLSL OFF CACHE BOOL "" FORCE) # For D3D12 (v2.0)
set(SPIRV_CROSS_ENABLE_MSL OFF CACHE BOOL "" FORCE)  # For Metal (v2.0)
set(SPIRV_CROSS_ENABLE_CPP OFF CACHE BOOL "" FORCE)
set(SPIRV_CROSS_ENABLE_REFLECT ON CACHE BOOL "" FORCE) # For shader reflection
set(SPIRV_CROSS_ENABLE_C_API OFF CACHE BOOL "" FORCE)
set(SPIRV_CROSS_ENABLE_UTIL OFF CACHE BOOL "" FORCE)

# Add SPIRV-Cross
add_subdirectory(SPIRV-Cross-vulkan-sdk-1.4.335)

# Disable warnings-as-errors for SPIRV-Cross (it has some C++14 extension warnings)
if(TARGET spirv-cross-core)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(spirv-cross-core PRIVATE -Wno-error)
    elseif(MSVC)
        target_compile_options(spirv-cross-core PRIVATE /WX-)
    endif()
endif()
if(TARGET spirv-cross-glsl)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(spirv-cross-glsl PRIVATE -Wno-error)
    elseif(MSVC)
        target_compile_options(spirv-cross-glsl PRIVATE /WX-)
    endif()
endif()

# Add to external targets
list(APPEND VRHI_EXTERNAL_TARGETS spirv-cross-core spirv-cross-glsl)

# ----------------------------------------------------------------------------
# Create interface library for shader compilation
# ----------------------------------------------------------------------------
add_library(VRHI_ShaderCompiler INTERFACE)
target_link_libraries(VRHI_ShaderCompiler INTERFACE
    glslang
    SPIRV
    spirv-cross-core
    spirv-cross-glsl
)

# Add include directories for shader compilation
target_include_directories(VRHI_ShaderCompiler INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/glslang-16.1.0
    ${CMAKE_CURRENT_SOURCE_DIR}/SPIRV-Cross-vulkan-sdk-1.4.335
)

# Note: VRHI_ShaderCompiler is an internal interface library
# It should NOT be added to VRHI_EXTERNAL_TARGETS as it's not exported

# Export list of external targets
set(VRHI_EXTERNAL_TARGETS "${VRHI_EXTERNAL_TARGETS}" CACHE INTERNAL "List of external dependency targets")
