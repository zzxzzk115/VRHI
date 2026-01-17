# Build System Design

## Overview

VRHI uses **CMake** as its build system, emphasizing simplicity and portability. All third-party dependencies are directly included via the `external/` directory, without using git submodules or dependency managers like CPM.cmake. This design ensures:

- **Complete Control**: All dependency source code is in the repository
- **Offline Builds**: No network connection required
- **Version Locking**: Dependency versions are explicit and won't break builds
- **Simplified Maintenance**: Avoids submodule synchronization issues and CPM network dependencies

## Key Features

- **CMake 3.21+**: Modern CMake with target-based configuration
- **C++23 Standard**: Leveraging latest language features
- **Modular Structure**: Independent CMakeLists.txt for each subsystem
- **Platform Detection**: Automatic backend enablement based on OS
- **Flexible Options**: Rich build options for different backend and window system combinations

## Directory Structure

```
VRHI/
├── CMakeLists.txt              # Root CMake configuration
├── cmake/
│   ├── VRHIConfig.cmake.in     # Export configuration template
│   ├── CompilerOptions.cmake   # Compiler options and warnings
│   ├── PlatformDetection.cmake # Platform detection macros
│   └── Dependencies.cmake      # Dependency finding logic
├── external/
│   ├── CMakeLists.txt          # Third-party library build config
│   ├── vulkan/                 # Vulkan SDK headers
│   ├── glad/                   # OpenGL Loader
│   └── vk-bootstrap/           # Vulkan initialization helper
├── include/
│   └── VRHI/
├── src/
│   ├── Core/
│   ├── Backends/
│   │   ├── Vulkan/
│   │   ├── OpenGL/
│   │   ├── D3D12/              # Future
│   │   └── Metal/              # Future
│   └── WindowSystem/
└── examples/
```

For detailed documentation, see [Chinese version](../zh-CN/design/build_system.md).

## Quick Start

### Configure

```bash
# Windows - D3D12 + Vulkan
cmake -B build -G "Visual Studio 17 2022"

# Linux - Vulkan + OpenGL
cmake -B build -DCMAKE_BUILD_TYPE=Release

# macOS - Metal + OpenGL 4.1
cmake -B build -G Xcode
```

### Build

```bash
cmake --build build --config Release
```

### Build Options

```cmake
# Backend selection
option(VRHI_ENABLE_VULKAN "Enable Vulkan backend" ON)
option(VRHI_ENABLE_OPENGL "Enable OpenGL backend" ON)
option(VRHI_ENABLE_D3D12 "Enable Direct3D 12 backend" ${WIN32})
option(VRHI_ENABLE_METAL "Enable Metal backend" ${APPLE})

# Window system support
option(VRHI_WINDOW_SDL2 "Enable SDL2 support" ON)
option(VRHI_WINDOW_SDL3 "Enable SDL3 support" OFF)
option(VRHI_WINDOW_GLFW "Enable GLFW support" ON)
option(VRHI_WINDOW_EGL "Enable EGL support" OFF)
```

## External Dependencies

All dependencies included in `external/` directory:

- **Vulkan Headers** (1.3.275): Vulkan API headers
- **GLAD** (2.0.5): OpenGL loader
- **vk-bootstrap** (1.3.275): Vulkan initialization helper

Versions documented in `external/README.md`.

## Integration

Install VRHI and use in your project:

```cmake
find_package(VRHI 1.0 REQUIRED)

add_executable(MyApp main.cpp)
target_link_libraries(MyApp PRIVATE VRHI::VRHI)
```

For comprehensive documentation including CMake examples, platform-specific configurations, and CI setup, see the [detailed Chinese documentation](../zh-CN/design/build_system.md).
