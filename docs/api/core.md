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
    OpenGL20,       // OpenGL 2.0
    OpenGLES31,     // OpenGL ES 3.1
    OpenGLES30,     // OpenGL ES 3.0
    OpenGLES20,     // OpenGL ES 2.0
    Auto,           // Automatically select best backend
};
```

## Global Functions

### CreateDevice

Create a rendering device:

```cpp
std::expected<std::unique_ptr<Device>, Error> 
CreateDevice(const DeviceConfig& config = {});
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
