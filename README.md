# VRHI

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![C++23](https://img.shields.io/badge/C%2B%2B-23-blue.svg)](https://en.cppreference.com/w/cpp/23)

[中文文档](README_zh-CN.md)

A modern Render Hardware Interface (RHI) supporting multiple graphics backends, designed with C++23.

## ✨ Features

- 🎯 **Multi-Backend Support**: Vulkan, OpenGL 3.3/4.3/4.6, OpenGL ES 2.0/3.0/3.1
- 🤖 **Smart Backend Selection**: Automatically detects hardware features and scores backends to select the optimal one
- 🔒 **Modern C++23**: Leverages the latest C++ standard features
- 🛡️ **RAII Resource Management**: Automated resource lifecycle management to prevent leaks
- 🌐 **Cross-Platform**: Windows, Linux, macOS, Android, iOS, Raspberry Pi
- ⚡ **High Performance**: Zero-overhead abstraction, from high-end PCs to low-end mobile devices
- 📦 **Header-Only**: Header-only library for easy integration

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

- [Architecture Design](docs/design/architecture.md) - VRHI overall architecture
- [Backend Scoring System](docs/design/backend_scoring.md) - Smart backend selection mechanism
- [Feature Detection](docs/design/feature_detection.md) - Hardware feature detection
- [RAII Principles](docs/design/raii_principles.md) - Resource management design
- [API Reference](docs/api/core.md) - Core API documentation
- [Quick Start](docs/examples/quick_start.md) - Getting started guide
- [Best Practices](docs/examples/best_practices.md) - Usage recommendations and best practices

[中文文档](docs/zh-CN/README.md)

## 🎯 Core Design

### Smart Backend Selection

VRHI's innovation lies in its initialization process:

1. **Detect all available backends** (Vulkan, OpenGL, OpenGL ES)
2. **Query hardware feature support** (compute shaders, ray tracing, etc.)
3. **Comprehensive scoring** (Feature support 40% + Performance 30% + Stability 20% + Compatibility 10%)
4. **Automatically select optimal backend** or allow manual specification

```cpp
VRHI::DeviceConfig config;
config.features.required = {VRHI::Feature::Compute};  // Required features
config.features.optional = {VRHI::Feature::RayTracing};  // Optional features

auto device = VRHI::CreateDevice(config).value();
// Automatically selects backend that supports compute shaders with highest score
```

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

## 🔧 Build Requirements

- **Compiler**: GCC 13+, Clang 16+, MSVC 2022+
- **C++ Standard**: C++23
- **CMake**: 3.20+
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

| Backend | Platform | Performance | Compatibility | Use Case |
|---------|----------|-------------|---------------|----------|
| **Vulkan** | Windows, Linux, Android | ⭐⭐⭐⭐⭐ | ⭐⭐⭐ | High-performance PC, modern mobile devices |
| **OpenGL 4.6** | Windows, Linux | ⭐⭐⭐⭐ | ⭐⭐⭐⭐ | High-end PC |
| **OpenGL 4.3** | Windows, Linux | ⭐⭐⭐⭐ | ⭐⭐⭐⭐ | Mid to high-end PC |
| **OpenGL 3.3** | Windows, Linux, macOS | ⭐⭐⭐ | ⭐⭐⭐⭐⭐ | General PC, older hardware |
| **OpenGL ES 3.1** | Android, iOS | ⭐⭐⭐⭐ | ⭐⭐⭐ | Modern mobile devices |
| **OpenGL ES 3.0** | Android, iOS | ⭐⭐⭐ | ⭐⭐⭐⭐ | Mainstream mobile devices |
| **OpenGL ES 2.0** | Android, Raspberry Pi | ⭐⭐ | ⭐⭐⭐⭐⭐ | Low-end devices, embedded |

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
