# Core Interface

> **Note**: For the complete Chinese documentation, see [中文版](../zh-CN/api/core.md)

## Overview

The VRHI core interface provides device initialization, configuration, and basic functionality access.

## Namespace

All VRHI interfaces are under the `VRHI` namespace:

```cpp
namespace VRHI {
    // All interfaces and types
}
```

## Core Types

### BackendType

Represents graphics backend type:

```cpp
enum class BackendType {
    Vulkan,         // Vulkan backend
    OpenGL46,       // OpenGL 4.6
    OpenGL41,       // OpenGL 4.1
    OpenGL33,       // OpenGL 3.3
    OpenGLES31,     // OpenGL ES 3.1
    OpenGLES30,     // OpenGL ES 3.0
    Auto,           // Automatically select best backend
};
```

### Error Codes

```cpp
struct Error {
    enum class Code {
        Success = 0,
        InvalidConfig,           // Invalid configuration
        NoCompatibleBackend,     // No backend satisfies all required features
        InitializationFailed,    // Backend initialization failed
        DeviceLost,             // Device lost
        OutOfMemory,            // Out of memory
        ValidationError,        // Validation error
        CompilationError,       // Shader compilation error
        UnsupportedFeature,     // Unsupported feature
    };
    
    Code code = Code::Success;
    std::string message;
    
    operator bool() const { return code == Code::Success; }
};
```

**Important Error Codes**:

- **`NoCompatibleBackend`**: No backend can satisfy all required features
  - Occurs when application specifies `FeatureRequirements.required` but no available backend supports all these features
  - Error message details which required features are unsupported
  - Application should notify user that hardware doesn't meet requirements

## Global Functions

### CreateDevice

Create a rendering device:

```cpp
std::expected<std::unique_ptr<Device>, Error> 
CreateDevice(const DeviceConfig& config = {});
```

**Required Feature Validation Example**:

```cpp
// Define strict feature requirements
DeviceConfig config;
config.features.required = {
    Feature::Compute,
    Feature::Texture3D,
    Feature::MultiDrawIndirect,
};

auto result = VRHI::CreateDevice(config);

if (!result) {
    // Handle required features not satisfied
    if (result.error().code == Error::Code::NoCompatibleBackend) {
        std::cerr << "Hardware doesn't support required features:\n";
        std::cerr << result.error().message << "\n";
        
        // Application should:
        // 1. Show user-friendly error message
        // 2. Provide system requirements info
        // 3. Consider fallback or exit
        
        return EXIT_FAILURE;
    }
}

// Success - guaranteed to support all required features
auto device = std::move(*result);
```

**Fallback Strategy Example**:

```cpp
auto highEndResult = VRHI::CreateDevice(highEndConfig);

if (!highEndResult) {
    std::cout << "Advanced features unavailable, trying basic mode...\n";
    
    // Try downgraded configuration
    DeviceConfig basicConfig;
    basicConfig.features.required = {
        Feature::Texture2D,
        Feature::VertexBuffers,
    };
    
    auto basicResult = VRHI::CreateDevice(basicConfig);
    if (basicResult) {
        useBasicRenderPath = true;
        device = std::move(*basicResult);
    } else {
        std::cerr << "Hardware completely unsupported\n";
        return EXIT_FAILURE;
    }
}
```

### EnumerateBackends

Enumerate all available backends:

```cpp
std::vector<BackendInfo> EnumerateBackends();
```

### GetVersion

Get VRHI version:

```cpp
Version GetVersion();
```

## Device Interface

The Device interface provides methods for resource creation, command execution, synchronization, and swap chain management.

For detailed documentation including complete API reference and usage examples, please refer to the [Chinese version](../zh-CN/api/core.md).
