# ============================================================================
# VRHI Library Source
# ============================================================================

# ============================================================================
# Source Files
# ============================================================================

# Core source files
set(VRHI_CORE_SOURCES
    Core/Version.cpp
    Core/Device.cpp
    Core/Logging.cpp
    Core/Backend.cpp
    Core/BackendScoring.cpp
    Core/BackendInit.cpp
    Core/NullBackend.cpp
    Core/NullDevice.cpp
    Core/MockBackend.cpp
    Core/ShaderCompiler.cpp
    # Additional core implementation files will be added here
    # Core/Error.cpp
    # Core/Features.cpp
)

# Backend source files (conditionally added)
set(VRHI_BACKEND_SOURCES)

# Vulkan backend
if(VRHI_ENABLE_VULKAN)
    list(APPEND VRHI_BACKEND_SOURCES
        # Backends/Vulkan/VulkanBackend.cpp
        # Backends/Vulkan/VulkanDevice.cpp
        # Backends/Vulkan/VulkanResources.cpp
        # Backends/Vulkan/VulkanCommands.cpp
    )
    message(STATUS "Vulkan backend enabled")
endif()

# OpenGL backend
if(VRHI_ENABLE_OPENGL)
    list(APPEND VRHI_BACKEND_SOURCES
        Backends/OpenGL33/OpenGL33Backend.cpp
        Backends/OpenGL33/OpenGL33Device.cpp
        Backends/OpenGL33/OpenGL33Buffer.cpp
        Backends/OpenGL33/OpenGL33Texture.cpp
        Backends/OpenGL33/OpenGL33Sampler.cpp
        Backends/OpenGL33/OpenGL33Shader.cpp
        Backends/OpenGL33/OpenGL33Pipeline.cpp
        Backends/OpenGL33/OpenGL33RenderPass.cpp
        Backends/OpenGL33/OpenGL33Framebuffer.cpp
        Backends/OpenGL33/OpenGL33CommandBuffer.cpp
        Backends/OpenGL33/OpenGL33Sync.cpp
        Backends/OpenGL33/GLFormatUtils.cpp
    )
    message(STATUS "OpenGL backend enabled")
endif()

# Direct3D 12 backend (Windows only)
if(VRHI_ENABLE_D3D12 AND VRHI_PLATFORM_WINDOWS)
    list(APPEND VRHI_BACKEND_SOURCES
        # Backends/D3D12/D3D12Backend.cpp
        # Backends/D3D12/D3D12Device.cpp
        # Backends/D3D12/D3D12Resources.cpp
        # Backends/D3D12/D3D12Commands.cpp
    )
    message(STATUS "Direct3D 12 backend enabled")
endif()

# Metal backend (macOS/iOS only)
if(VRHI_ENABLE_METAL AND (VRHI_PLATFORM_MACOS OR VRHI_PLATFORM_IOS))
    list(APPEND VRHI_BACKEND_SOURCES
        # Backends/Metal/MetalBackend.mm
        # Backends/Metal/MetalDevice.mm
        # Backends/Metal/MetalResources.mm
        # Backends/Metal/MetalCommands.mm
    )
    message(STATUS "Metal backend enabled")
endif()

# Window system abstraction
set(VRHI_WINDOW_SOURCES
    WindowSystem/WindowFactory.cpp
)

# GLFW window system
if(VRHI_WINDOW_GLFW)
    list(APPEND VRHI_WINDOW_SOURCES
        WindowSystem/GLFW/GLFWWindow.cpp
    )
    message(STATUS "GLFW window system enabled")
endif()

# SDL2 window system (future)
if(VRHI_WINDOW_SDL2)
    list(APPEND VRHI_WINDOW_SOURCES
        # WindowSystem/SDL2/SDL2Window.cpp
    )
    message(STATUS "SDL2 window system enabled")
endif()

# SDL3 window system (future)
if(VRHI_WINDOW_SDL3)
    list(APPEND VRHI_WINDOW_SOURCES
        # WindowSystem/SDL3/SDL3Window.cpp
    )
    message(STATUS "SDL3 window system enabled")
endif()

# Combine all sources
set(VRHI_ALL_SOURCES
    ${VRHI_CORE_SOURCES}
    ${VRHI_BACKEND_SOURCES}
    ${VRHI_WINDOW_SOURCES}
)

# ============================================================================
# Library Target
# ============================================================================

# For now, create a header-only interface library until source files are added
# This allows the build system to work without implementation files
if(NOT VRHI_ALL_SOURCES)
    # Create interface library (header-only) for now
    add_library(VRHI INTERFACE)
    
    target_include_directories(VRHI
        INTERFACE
            $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )
    
    message(STATUS "VRHI configured as interface library (header-only) - implementation files will be added later")
else()
    # Create library target with sources
    if(VRHI_BUILD_SHARED_LIBS)
        add_library(VRHI SHARED ${VRHI_ALL_SOURCES})
        target_compile_definitions(VRHI
            PRIVATE VRHI_EXPORTS
            PUBLIC VRHI_SHARED
        )
    else()
        add_library(VRHI STATIC ${VRHI_ALL_SOURCES})
    endif()
    
    # Include directories for compiled library
    target_include_directories(VRHI
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}
    )
    
    # Compile definitions
    if(VRHI_ENABLE_VULKAN)
        target_compile_definitions(VRHI PRIVATE VRHI_BACKEND_VULKAN=1)
    endif()
    
    if(VRHI_ENABLE_OPENGL)
        target_compile_definitions(VRHI PRIVATE VRHI_BACKEND_OPENGL=1)
    endif()
    
    if(VRHI_ENABLE_D3D12)
        target_compile_definitions(VRHI PRIVATE VRHI_BACKEND_D3D12=1)
    endif()
    
    if(VRHI_ENABLE_METAL)
        target_compile_definitions(VRHI PRIVATE VRHI_BACKEND_METAL=1)
    endif()
    
    if(VRHI_ENABLE_VALIDATION)
        target_compile_definitions(VRHI PRIVATE VRHI_ENABLE_VALIDATION=1)
    endif()
    
    # Window system compile definitions
    if(VRHI_WINDOW_GLFW)
        target_compile_definitions(VRHI PRIVATE VRHI_WINDOW_GLFW=1)
    endif()
    
    if(VRHI_WINDOW_SDL2)
        target_compile_definitions(VRHI PRIVATE VRHI_WINDOW_SDL2=1)
    endif()
    
    if(VRHI_WINDOW_SDL3)
        target_compile_definitions(VRHI PRIVATE VRHI_WINDOW_SDL3=1)
    endif()
    
    if(VRHI_WINDOW_EGL)
        target_compile_definitions(VRHI PRIVATE VRHI_WINDOW_EGL=1)
    endif()
    
    # Link libraries
    target_link_libraries(VRHI PRIVATE Threads::Threads)
    
    # Shader compilation libraries (link directly instead of through interface)
    target_link_libraries(VRHI PRIVATE
        glslang
        SPIRV
        spirv-cross-core
        spirv-cross-glsl
    )
    
    # Add include directories for shader compilation
    target_include_directories(VRHI PRIVATE
        ${CMAKE_SOURCE_DIR}/external/glslang-16.1.0
        ${CMAKE_SOURCE_DIR}/external/SPIRV-Cross-vulkan-sdk-1.4.335
    )
    
    # Backend-specific libraries
    if(VRHI_ENABLE_OPENGL)
        target_link_libraries(VRHI PRIVATE glad::gl33)
    endif()
    
    # Window system libraries
    if(VRHI_WINDOW_GLFW)
        target_link_libraries(VRHI PRIVATE glfw)
    endif()
    
    # Platform-specific libraries
    if(VRHI_PLATFORM_WINDOWS)
        if(VRHI_ENABLE_D3D12)
            target_link_libraries(VRHI PRIVATE d3d12 dxgi dxguid)
        endif()
    elseif(VRHI_PLATFORM_MACOS OR VRHI_PLATFORM_IOS)
        if(VRHI_ENABLE_METAL)
            target_link_libraries(VRHI PRIVATE
                "-framework Metal"
                "-framework QuartzCore"
            )
        endif()
        if(VRHI_PLATFORM_MACOS)
            target_link_libraries(VRHI PRIVATE
                "-framework Cocoa"
            )
        endif()
    elseif(VRHI_PLATFORM_LINUX)
        if(VRHI_HAS_X11)
            target_link_libraries(VRHI PRIVATE X11::X11)
        endif()
        target_link_libraries(VRHI PRIVATE ${CMAKE_DL_LIBS})
    endif()
    
    # Properties
    set_target_properties(VRHI PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
        OUTPUT_NAME "VRHI"
        POSITION_INDEPENDENT_CODE ON
    )
endif()

# Add alias for consistency
add_library(VRHI::VRHI ALIAS VRHI)

# ============================================================================
# Installation
# ============================================================================

include(GNUInstallDirs)

# Install library (only when not in interface mode)
if(TARGET VRHI AND NOT CMAKE_SKIP_INSTALL_RULES)
    # Install VRHI library target
    install(TARGETS VRHI
        EXPORT VRHITargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )
    
    # Install shader compilation dependencies to the same export set
    # This ensures they're available when VRHI is imported
    if(TARGET glslang)
        install(TARGETS glslang SPIRV
            EXPORT VRHITargets
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        )
    endif()
    
    if(TARGET spirv-cross-core)
        install(TARGETS spirv-cross-core spirv-cross-glsl
            EXPORT VRHITargets
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        )
    endif()
    
    # Install export targets
    # This exports VRHI along with its required shader compilation dependencies
    # External dependencies like GLFW and GLAD are not exported
    install(EXPORT VRHITargets
        FILE VRHITargets.cmake
        NAMESPACE VRHI::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/VRHI
    )
endif()

# ============================================================================
# Summary
# ============================================================================

message(STATUS "VRHI library configuration:")
message(STATUS "  Library type: ${VRHI_BUILD_SHARED_LIBS}")
message(STATUS "  Source files: ${VRHI_ALL_SOURCES}")
