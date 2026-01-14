# VRHI

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![C++23](https://img.shields.io/badge/C%2B%2B-23-blue.svg)](https://en.cppreference.com/w/cpp/23)

[中文文档](README_zh-CN.md)

A modern Render Hardware Interface (RHI) supporting multiple graphics backends, designed with C++23.

## ✨ Features

- 🎯 **Multi-Backend Support**: Vulkan, OpenGL 3.3/4.1/4.6, OpenGL ES 2.0/3.0/3.1
  - **Planned (v2.0)**: Direct3D 12 (Windows), Metal (macOS/iOS), WebGPU
- 🤖 **Smart Backend Selection**: Automatically detects hardware features and scores backends to select the optimal one
- 🔒 **Modern C++23**: Leverages the latest C++ standard features
- 🛡️ **RAII Resource Management**: Automated resource lifecycle management to prevent leaks
- 🌐 **Cross-Platform**: Windows, Linux, macOS, Android, iOS, Raspberry Pi
- ⚡ **High Performance**: Zero-overhead abstraction, from high-end PCs to low-end mobile devices
- 🎨 **Backend Extensibility**: Abstract design allows easy extension to new graphics APIs (D3D12, Metal, etc.)
- 🪟 **Window System Abstraction**: Unified interface supporting SDL2, SDL3, GLFW, EGL with automatic key code translation
- 🔨 **CMake Build System**: Self-contained `external/` directory, no submodules or CPM required

## 🚀 Quick Start

### Minimal Example

```cpp
#include <VRHI/VRHI.hpp>

int main() {
    // Create device (automatically selects best backend)
    auto device = VRHI::CreateDevice().value();
    
    // Create resources
    VRHI::BufferDesc desc;
    desc.size = 1024;
    desc.usage = VRHI::BufferUsage::Vertex;
    auto buffer = device->CreateBuffer(desc).value();
    
    // Render loop
    while (!ShouldClose()) {
        auto cmd = device->CreateCommandBuffer();
        cmd->Draw(3);
        device->Submit(std::move(cmd));
        device->Present();
    }
    
    device->WaitIdle();
    return 0;
}
```

### Backend Selection Example

```cpp
// View all available backends
auto backends = VRHI::EnumerateBackends();
for (const auto& backend : backends) {
    std::cout << backend.name << " (Score: " << backend.score << ")\n";
}

// Manually specify backend
VRHI::DeviceConfig config;
config.preferredBackend = VRHI::BackendType::Vulkan;
auto device = VRHI::CreateDevice(config).value();
```

## 📚 Documentation

Complete documentation can be found in the [docs/](docs/) directory:

### Design Documents
- [Architecture Design](docs/design/architecture.md) - VRHI overall architecture
- [Backend Scoring System](docs/design/backend_scoring.md) - Smart backend selection mechanism
- [Feature Detection](docs/design/feature_detection.md) - Hardware feature detection
- [RAII Principles](docs/design/raii_principles.md) - Resource management design
- [Build System Design](docs/design/build_system.md) - CMake build system and dependency management
- [Window System Abstraction](docs/design/window_system.md) - Multi-window-library support

### API Reference & Examples
- [API Reference](docs/api/core.md) - Core API documentation
- [Quick Start](docs/examples/quick_start.md) - Getting started guide
- [Best Practices](docs/examples/best_practices.md) - Usage recommendations and best practices

[中文文档](docs/zh-CN/README.md)

## 🎯 Core Design

### Smart Backend Selection

VRHI's innovation lies in its initialization process:

1. **Detect all available backends** (Vulkan, OpenGL, OpenGL ES, D3D12*, Metal*)
2. **Query hardware feature support** (compute shaders, ray tracing, etc.)
3. **Comprehensive scoring** (Feature support 40% + Performance 30% + Stability 20% + Compatibility 10%)
4. **Automatically select optimal backend** or allow manual specification

*Planned for v2.0

```cpp
VRHI::DeviceConfig config;
config.features.required = {VRHI::Feature::Compute};  // Required features
config.features.optional = {VRHI::Feature::RayTracing};  // Optional features

auto device = VRHI::CreateDevice(config).value();
// Automatically selects backend that supports compute shaders with highest score
```

### Backend Abstraction & Extensibility

VRHI is designed from the ground up to support easy extension to new graphics APIs:

- **Unified Interface**: All backends implement the same interface contract
- **Factory Pattern**: New backends can be registered at compile or runtime
- **Feature-Based**: Core API only includes cross-platform features
- **Platform-Aware Scoring**: Native APIs (D3D12 on Windows, Metal on macOS) get priority

Adding a new backend requires only:
1. Implementing the `IBackend` and `IDevice` interfaces
2. Registering the backend with the factory
3. Mapping VRHI concepts to native API constructs

See [Architecture Design](docs/design/architecture.md) for details on the abstraction layer.

### RAII Resource Management

All resources use RAII and smart pointers for management, no manual release needed:

```cpp
{
    auto buffer = device->CreateBuffer(desc).value();
    auto texture = device->CreateTexture(texDesc).value();
    // Use resources...
} // Automatic release, safe even with exceptions
```

### Modern Error Handling

Uses C++23's `std::expected` for error handling, avoiding exception overhead:

```cpp
auto result = device->CreateBuffer(desc);
if (result) {
    auto buffer = std::move(*result);
    // Use buffer
} else {
    std::cerr << "Error: " << result.error().message << "\n";
}
```

## 🔧 Build System

VRHI uses **CMake** with all dependencies self-contained in the `external/` directory:

- **No git submodules**: All third-party source code included directly
- **No CPM.cmake**: Full control over dependency versions
- **Offline builds**: No network connection required
- **Modular configuration**: Independent CMakeLists.txt for each subsystem

### Quick Build

```bash
# Configure
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build --config Release
```

### Build Options

```cmake
# Backend selection
option(VRHI_ENABLE_VULKAN "Enable Vulkan backend" ON)
option(VRHI_ENABLE_OPENGL "Enable OpenGL backend" ON)
option(VRHI_ENABLE_D3D12 "Enable D3D12 backend (Windows)" ${WIN32})
option(VRHI_ENABLE_METAL "Enable Metal backend (macOS)" ${APPLE})

# Window system support
option(VRHI_WINDOW_SDL2 "Enable SDL2 support" ON)
option(VRHI_WINDOW_SDL3 "Enable SDL3 support" OFF)
option(VRHI_WINDOW_GLFW "Enable GLFW support" ON)
option(VRHI_WINDOW_EGL "Enable EGL support" OFF)
```

See [Build System Design](docs/design/build_system.md) for complete documentation.

### Requirements

- **Compiler**: GCC 13+, Clang 16+, MSVC 2022+
- **C++ Standard**: C++23
- **CMake**: 3.21+
- **Platform**: Windows, Linux, macOS, Android, iOS, Raspberry Pi

### Dependencies

- Vulkan SDK (optional, for Vulkan backend)
- OpenGL drivers (system provided)

## 📦 Integration

### CMake

```cmake
add_subdirectory(external/VRHI)
target_link_libraries(your_app PRIVATE VRHI::VRHI)
```

## 🎨 Supported Backends

### Current Backends

| Backend | Platform | Performance | Compatibility | Use Case |
|---------|----------|-------------|---------------|----------|
| **Vulkan** | Windows, Linux, Android | ⭐⭐⭐⭐⭐ | ⭐⭐⭐ | High-performance PC, modern mobile devices |
| **OpenGL 4.6** | Windows, Linux | ⭐⭐⭐⭐ | ⭐⭐⭐⭐ | High-end PC |
| **OpenGL 4.1** | Windows, Linux, macOS | ⭐⭐⭐⭐ | ⭐⭐⭐⭐ | Mid to high-end PC, **macOS** (max version) |
| **OpenGL 3.3** | Windows, Linux, macOS | ⭐⭐⭐ | ⭐⭐⭐⭐⭐ | General PC, older hardware |
| **OpenGL ES 3.1** | Android, iOS | ⭐⭐⭐⭐ | ⭐⭐⭐ | Modern mobile devices |
| **OpenGL ES 3.0** | Android, iOS | ⭐⭐⭐ | ⭐⭐⭐⭐ | Mainstream mobile devices |
| **OpenGL ES 2.0** | Android, Raspberry Pi | ⭐⭐ | ⭐⭐⭐⭐⭐ | Low-end devices, embedded |

### Planned Backends (v2.0) 🚧

| Backend | Platform | Performance | Compatibility | Use Case |
|---------|----------|-------------|---------------|----------|
| **Direct3D 12** | Windows 10/11 | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐ | Windows high-performance, native API |
| **Metal** | macOS, iOS | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ | Apple platforms, native API |
| **WebGPU** | Web browsers | ⭐⭐⭐ | ⭐⭐⭐ | Cross-platform web applications |

**Platform Priorities**:
- **Windows**: D3D12 (planned) > Vulkan > OpenGL 4.6
- **macOS**: Metal (planned) > OpenGL 4.1
- **iOS**: Metal (planned) > OpenGL ES 3.0
- **Linux**: Vulkan > OpenGL 4.6
- **Android**: Vulkan > OpenGL ES 3.1

## 🤝 Contributing

Contributions welcome! Please see [CONTRIBUTING.md](CONTRIBUTING.md) (to be created).

## 📄 License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.

## 🙏 Acknowledgments

VRHI's design is inspired by:
- Vulkan API
- DirectX 12
- Metal
- WebGPU
- bgfx

## 📮 Contact

- Issue Tracker: [GitHub Issues](https://github.com/zzxzzk115/VRHI/issues)
- Discussions: [GitHub Discussions](https://github.com/zzxzzk115/VRHI/discussions)

---

**Note**: VRHI is currently in the design phase. Documentation and interface design are complete, implementation is in progress.
