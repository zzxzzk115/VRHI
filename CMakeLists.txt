# ============================================================================
# VRHI - Versatile Render Hardware Interface
# Copyright (c) 2024 Lazy_V
# SPDX-License-Identifier: MIT
# ============================================================================

cmake_minimum_required(VERSION 3.21)

# Project declaration
project(VRHI
    VERSION 1.0.0
    DESCRIPTION "Modern Render Hardware Interface supporting multiple graphics backends"
    LANGUAGES CXX
)

# ============================================================================
# Project Options
# ============================================================================

# Build options
option(VRHI_BUILD_EXAMPLES "Build example applications" ON)
option(VRHI_BUILD_TESTS "Build unit tests" ON)
option(VRHI_BUILD_SHARED_LIBS "Build shared library instead of static" OFF)

# Backend options
option(VRHI_ENABLE_VULKAN "Enable Vulkan backend" ON)
option(VRHI_ENABLE_OPENGL "Enable OpenGL backend" ON)
option(VRHI_ENABLE_D3D12 "Enable Direct3D 12 backend (Windows only)" OFF)
option(VRHI_ENABLE_METAL "Enable Metal backend (macOS/iOS only)" OFF)

# Window system options
option(VRHI_WINDOW_SDL2 "Enable SDL2 window system support" OFF)
option(VRHI_WINDOW_SDL3 "Enable SDL3 window system support" OFF)
option(VRHI_WINDOW_GLFW "Enable GLFW window system support" OFF)
option(VRHI_WINDOW_EGL "Enable EGL window system support (Linux/Android)" OFF)

# Debug and validation options
option(VRHI_ENABLE_VALIDATION "Enable API validation layers (debug builds)" ON)
option(VRHI_ENABLE_PROFILING "Enable built-in profiling" OFF)

# ============================================================================
# C++ Standard
# ============================================================================

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ============================================================================
# Build Type
# ============================================================================

# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'Release' as none was specified.")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# ============================================================================
# Output Directories
# ============================================================================

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ============================================================================
# Include CMake Modules
# ============================================================================

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(cmake/PlatformDetection.cmake)
include(cmake/CompilerOptions.cmake)
include(cmake/Dependencies.cmake)

# ============================================================================
# Subdirectories
# ============================================================================

# Add external dependencies
add_subdirectory(external)

# Add main library source
add_subdirectory(src)

# Add examples (optional, excluded from 'all' target)
if(VRHI_BUILD_EXAMPLES)
    add_subdirectory(examples EXCLUDE_FROM_ALL)
endif()

# Add tests (optional)
if(VRHI_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# ============================================================================
# Installation
# ============================================================================

include(GNUInstallDirs)

# Install header files
install(DIRECTORY include/VRHI
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.hpp"
)

# Install targets (configured in src/CMakeLists.txt)

# ============================================================================
# Package Configuration
# ============================================================================

include(CMakePackageConfigHelpers)

# Generate the config file that includes the exports
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/VRHIConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/VRHIConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/VRHI
)

# Generate the version file for the config file
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/VRHIConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# Install the configuration files
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/VRHIConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/VRHIConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/VRHI
)

# ============================================================================
# Status Summary
# ============================================================================

message(STATUS "")
message(STATUS "========================================")
message(STATUS "VRHI Configuration Summary")
message(STATUS "========================================")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: C++${CMAKE_CXX_STANDARD}")
message(STATUS "")
message(STATUS "Backends:")
message(STATUS "  Vulkan: ${VRHI_ENABLE_VULKAN}")
message(STATUS "  OpenGL: ${VRHI_ENABLE_OPENGL}")
message(STATUS "  Direct3D 12: ${VRHI_ENABLE_D3D12}")
message(STATUS "  Metal: ${VRHI_ENABLE_METAL}")
message(STATUS "")
message(STATUS "Window Systems:")
message(STATUS "  SDL2: ${VRHI_WINDOW_SDL2}")
message(STATUS "  SDL3: ${VRHI_WINDOW_SDL3}")
message(STATUS "  GLFW: ${VRHI_WINDOW_GLFW}")
message(STATUS "  EGL: ${VRHI_WINDOW_EGL}")
message(STATUS "")
message(STATUS "Build Options:")
message(STATUS "  Examples: ${VRHI_BUILD_EXAMPLES}")
message(STATUS "  Tests: ${VRHI_BUILD_TESTS}")
message(STATUS "  Shared Library: ${VRHI_BUILD_SHARED_LIBS}")
message(STATUS "  Validation: ${VRHI_ENABLE_VALIDATION}")
message(STATUS "  Profiling: ${VRHI_ENABLE_PROFILING}")
message(STATUS "")
message(STATUS "Platform:")
message(STATUS "  System: ${CMAKE_SYSTEM_NAME}")
message(STATUS "  Processor: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "  Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "========================================")
message(STATUS "")
