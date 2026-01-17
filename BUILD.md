# Building VRHI

This document describes how to build VRHI from source.

## Prerequisites

### Compiler Requirements

VRHI requires a modern C++23-compliant compiler:

- **GCC**: 13.0 or later
- **Clang**: 18.0 or later (required for std::expected support)
- **MSVC**: 2022 (19.30) or later

### Build Tools

- **CMake**: 3.21 or later
- **Build System**: Make, Ninja, or Visual Studio

## Quick Start

### Linux / macOS

```bash
# Configure
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build --config Release

# Install (optional)
sudo cmake --install build
```

### Windows

```bash
# Configure (Visual Studio)
cmake -B build -G "Visual Studio 17 2022"

# Build
cmake --build build --config Release

# Install (optional)
cmake --install build
```

## Build Options

VRHI provides various CMake options to customize the build:

### Backend Options

```cmake
option(VRHI_ENABLE_VULKAN "Enable Vulkan backend" ON)
option(VRHI_ENABLE_OPENGL "Enable OpenGL backend" ON)
option(VRHI_ENABLE_D3D12 "Enable Direct3D 12 backend (Windows only)" OFF)
option(VRHI_ENABLE_METAL "Enable Metal backend (macOS/iOS only)" OFF)
```

### Window System Options

```cmake
option(VRHI_WINDOW_SDL2 "Enable SDL2 window system support" OFF)
option(VRHI_WINDOW_SDL3 "Enable SDL3 window system support" OFF)
option(VRHI_WINDOW_GLFW "Enable GLFW window system support" OFF)
option(VRHI_WINDOW_EGL "Enable EGL window system support" OFF)
```

### Build Options

```cmake
option(VRHI_BUILD_EXAMPLES "Build example applications" ON)
option(VRHI_BUILD_TESTS "Build unit tests" OFF)
option(VRHI_BUILD_SHARED_LIBS "Build shared library instead of static" OFF)
option(VRHI_ENABLE_VALIDATION "Enable API validation layers" ON)
option(VRHI_ENABLE_PROFILING "Enable built-in profiling" OFF)
```

### Example Configuration

```bash
# Windows with D3D12 and Vulkan
cmake -B build -G "Visual Studio 17 2022" \
    -DVRHI_ENABLE_D3D12=ON \
    -DVRHI_ENABLE_VULKAN=ON \
    -DVRHI_WINDOW_SDL2=ON

# Linux with Vulkan only
cmake -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DVRHI_ENABLE_VULKAN=ON \
    -DVRHI_ENABLE_OPENGL=OFF

# macOS with Metal (when implemented)
cmake -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DVRHI_ENABLE_METAL=ON \
    -DVRHI_WINDOW_GLFW=ON
```

## Build Types

CMake supports multiple build types:

- **Debug**: No optimization, debug symbols, validation enabled
- **Release**: Full optimization, no debug symbols
- **RelWithDebInfo**: Optimized with debug symbols
- **MinSizeRel**: Optimized for minimal size

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo
```

## Advanced Build Options

### Compiler Sanitizers (Linux/macOS Debug builds)

```bash
cmake -B build \
    -DCMAKE_BUILD_TYPE=Debug \
    -DVRHI_ENABLE_ASAN=ON \
    -DVRHI_ENABLE_UBSAN=ON
```

### Custom Installation Prefix

```bash
cmake -B build -DCMAKE_INSTALL_PREFIX=/usr/local
cmake --install build
```

### Verbose Build Output

```bash
cmake --build build --verbose
```

## Project Structure

After building, the directory structure will be:

```
build/
├── lib/
│   └── libVRHI.a         # Static library
├── bin/                   # Example executables (if enabled)
├── CMakeFiles/
└── compile_commands.json  # For IDE integration
```

## Integration with Other Projects

### Using CMake

```cmake
# Find VRHI
find_package(VRHI 1.0 REQUIRED)

# Link to your target
add_executable(MyApp main.cpp)
target_link_libraries(MyApp PRIVATE VRHI::VRHI)
```

### As a Subdirectory

```cmake
# Add VRHI as a subdirectory
add_subdirectory(external/VRHI)

# Link to your target
add_executable(MyApp main.cpp)
target_link_libraries(MyApp PRIVATE VRHI::VRHI)
```

## Troubleshooting

### Compiler Version Too Old

If you get an error about C++23 support:

```
CMake Error: MSVC 2022 (19.30) or later is required for C++23 support
```

Solution: Upgrade your compiler to a supported version.

### Vulkan SDK Not Found

If you want to use the Vulkan backend but don't have the SDK installed:

1. Download and install the [Vulkan SDK](https://vulkan.lunarg.com/)
2. Set the `VULKAN_SDK` environment variable
3. Reconfigure CMake

### Missing Dependencies

VRHI uses a self-contained dependency model. All required dependencies will be included in the `external/` directory during development. No external dependencies need to be installed separately.

## Current Status

🚧 **VRHI is currently in the design and initial setup phase**

- ✅ Build system configured
- ✅ Project structure established
- ✅ Header-only API defined
- 🚧 Backend implementations in progress
- 🚧 Examples to be added

For more information, see the [documentation](docs/README.md).
