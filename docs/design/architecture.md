# VRHI Architecture Design

> **Note**: For the complete Chinese documentation, see [中文版](../zh-CN/design/architecture.md)

## Overview

VRHI (Versatile Render Hardware Interface) uses a layered architecture design, providing a unified rendering API abstraction while supporting multiple graphics backends.

## Architecture Layers

```
┌─────────────────────────────────────────┐
│         Application Layer                │
└─────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────┐
│      VRHI Core Interface Layer           │
│  - Device                                │
│  - CommandBuffer                         │
│  - Resources (Buffer, Texture, etc.)     │
│  - Pipeline                              │
└─────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────┐
│     Backend Abstraction Layer            │
│  - Backend Factory                       │
│  - Feature Detection                     │
│  - Scoring System                        │
└─────────────────────────────────────────┘
                    ↓
┌──────────┬──────────┬──────────┬─────────┐
│ Vulkan   │ D3D12    │  Metal   │ OpenGL  │
│ Backend  │ Backend  │ Backend  │ Backends│
│          │ (planned)│ (planned)│         │
└──────────┴──────────┴──────────┴─────────┘
                    ↓
┌─────────────────────────────────────────┐
│            Hardware Drivers              │
└─────────────────────────────────────────┘
```

## Backend Abstraction Principles

VRHI's backend abstraction layer follows these principles to support easy extension to new graphics APIs:

**1. Interface Uniformity**
- All backends implement the same interface contract
- Core interfaces are decoupled from specific backends
- Abstract factory pattern for backend creation

**2. Minimum Common Feature Set**
- Core API only includes features all backends can support
- Advanced features exposed through feature queries and optional interfaces
- Avoid designing proprietary APIs for single backends

**3. Backend Capability Queries**
- Each backend reports its supported feature set
- Applications adjust rendering paths based on features
- Runtime dynamic queries, no compile-time configuration dependencies

**4. Zero-Cost Abstraction**
- Minimize virtual function calls, use templates for critical paths
- Compile-time polymorphism over runtime polymorphism
- Inline-friendly interface design

**5. Platform-Aware but Not Dependent**
- Backends can leverage platform features for optimization
- Core interfaces contain no platform-specific code
- Adapter pattern to isolate platform differences

## Core Components

### 1. Device

The Device is the core component of VRHI, representing a graphics hardware device.

**Main Responsibilities:**
- Resource creation and management
- Command queue management
- Feature queries
- Backend lifecycle management

### 2. CommandBuffer

Command buffers are used to record and submit rendering commands.

### 3. Resources

Includes various GPU resources: buffers, textures, samplers, etc.

### 4. Pipeline

Graphics and compute pipeline configuration.

## Backend System

### Current Backends

1. **Vulkan Backend** - High-performance PC and modern mobile devices (Windows, Linux, Android)
2. **OpenGL 4.6 Backend** - High-end PC (Windows, Linux)
3. **OpenGL 4.1 Backend** - Mid to high-end PC, macOS (maximum supported version)
4. **OpenGL 3.3 Backend** - General PC support, minimum desktop GL version
5. **OpenGL ES 3.1 Backend** - Modern mobile devices
6. **OpenGL ES 3.0 Backend** - Mainstream mobile devices, minimum GLES version, embedded devices

### Planned Backends (v2.0)

7. **Direct3D 12 Backend** 🚧 - Windows high-performance native API
8. **Metal Backend** 🚧 - macOS/iOS high-performance native API
9. **WebGPU Backend** 🚧 - Web browsers and cross-platform (v3.0)

### Cross-Backend Abstraction Mapping

| VRHI Concept | Vulkan | Direct3D 12 | Metal | OpenGL |
|--------------|--------|-------------|-------|--------|
| **Device** | VkDevice | ID3D12Device | MTLDevice | GL Context |
| **Buffer** | VkBuffer | ID3D12Resource | MTLBuffer | GLuint (VBO) |
| **Texture** | VkImage | ID3D12Resource | MTLTexture | GLuint (Texture) |
| **Shader** | VkShaderModule | ID3D12PipelineState | MTLLibrary | GLuint (Program) |
| **CommandBuffer** | VkCommandBuffer | ID3D12CommandList | MTLCommandBuffer | Display List |
| **Fence** | VkFence | ID3D12Fence | MTLFence | glFenceSync |
| **Pipeline** | VkPipeline | ID3D12PipelineState | MTLRenderPipelineState | GL State |

## Platform Support

### Desktop
- **Windows**: Vulkan, OpenGL 4.6/4.1/3.3, Direct3D 12 (planned v2.0)
- **Linux**: Vulkan, OpenGL 4.6/4.1/3.3
- **macOS**: OpenGL 4.1/3.3, Metal (planned v2.0, recommended for performance)

### Mobile
- **Android**: Vulkan, OpenGL ES 3.1/3.0
- **iOS**: OpenGL ES 3.1/3.0, Metal (planned v2.0, recommended for performance)

### Embedded
- Raspberry Pi (OpenGL ES 3.0+, Vulkan experimental on Pi 4+)
- Other ARM Linux devices

### Web
- WebGPU (planned v3.0)

## Roadmap

### v2.0 - Modern API Support
- Direct3D 12 backend (Windows)
- Metal backend (macOS/iOS)
- Ray tracing extensions
- Mesh shaders

### v3.0 - Web and Advanced Features
- WebGPU backend
- Variable rate shading
- Advanced ray tracing features

## Design Philosophy

VRHI's abstraction design follows these core principles:

1. **Backend Agnostic**: Core API contains no backend-specific concepts
2. **Progressive Enhancement**: Start from minimum feature set, enable advanced features via queries
3. **Explicit over Implicit**: Clear interface contracts, no hidden state transitions
4. **Extension Friendly**: New backends only need to implement unified interface
5. **Performance First**: Minimize abstraction overhead, near-native API performance

This design ensures VRHI can easily extend to any new graphics API, whether Direct3D 12, Metal, WebGPU, or future APIs yet to emerge.

For detailed documentation including complete interface definitions, factory patterns, and extension guides, please refer to the [Chinese version](../zh-CN/design/architecture.md).
