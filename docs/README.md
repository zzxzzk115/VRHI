# VRHI Documentation

Welcome to the VRHI (Versatile Render Hardware Interface) documentation.

[中文文档](zh-CN/README.md)

## Table of Contents

### Design Documents
- [Architecture Design](design/architecture.md) - VRHI overall architecture
- [Backend Scoring System](design/backend_scoring.md) - Backend selection and scoring mechanism
- [Feature Detection](design/feature_detection.md) - Hardware feature detection system
- [RAII Principles](design/raii_principles.md) - Resource management and RAII implementation
- [Build System Design](design/build_system.md) - CMake build system and dependency management
- [Window System Abstraction](design/window_system.md) - Multi-window-library support and event handling

### API Reference
- [Core Interface](api/core.md) - Core RHI interface
- [Device Management](api/device.md) - Device and context management
- [Resource Management](api/resources.md) - Buffers, textures, and other resources
- [Command Recording](api/commands.md) - Command buffers and submission
- [Backend Interfaces](api/backends.md) - Vulkan and OpenGL backends

### Examples and Best Practices
- [Quick Start](examples/quick_start.md) - Getting started guide
- [Best Practices](examples/best_practices.md) - Usage recommendations and best practices
- [Cross-Platform Development](examples/cross_platform.md) - Cross-platform development guide

## About VRHI

VRHI is a modern rendering hardware abstraction layer designed to:

- **Multi-Backend Support**: Supports Vulkan and multiple OpenGL versions (3.3, 2.0, 4.1, 4.6, ES 2.0, ES 3.0, ES 3.1)
- **Smart Backend Selection**: Automatically detects and scores backends based on hardware features
- **Modern C++**: Uses C++23 standard features
- **RAII Resource Management**: Automatic resource lifecycle management prevents memory leaks
- **Wide Compatibility**: From high-performance PCs to Raspberry Pi and Android phones
- **Performance First**: Pursues ultimate performance while ensuring compatibility

## Quick Links

- [Project Homepage](https://github.com/zzxzzk115/VRHI)
- [Issue Tracker](https://github.com/zzxzzk115/VRHI/issues)
- [Contributing Guide](../CONTRIBUTING.md)
