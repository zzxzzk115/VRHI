# Quick Start

> **Note**: For the complete Chinese documentation, see [中文版](../zh-CN/examples/quick_start.md)

## Installation

VRHI is a header-only library using the C++23 standard.

### Requirements

- C++23 compatible compiler (GCC 13+, Clang 16+, MSVC 2022+)
- CMake 3.20+
- Platform-specific graphics API SDKs:
  - Vulkan SDK (optional)
  - OpenGL drivers

### Integration

**Using CMake**:

```cmake
# CMakeLists.txt
cmake_minimum_required(VERSION 3.20)
project(MyProject)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add VRHI
add_subdirectory(external/VRHI)

add_executable(myapp main.cpp)
target_link_libraries(myapp PRIVATE VRHI::VRHI)
```

## Hello Triangle Example

A complete Hello Triangle example demonstrating basic VRHI usage:

```cpp
#include <VRHI/VRHI.hpp>
#include <VRHI/Resources.hpp>
#include <iostream>

int main() {
    // 1. Create device
    VRHI::DeviceConfig config;
    config.width = 800;
    config.height = 600;
    config.enableValidation = true;
    
    auto deviceResult = VRHI::CreateDevice(config);
    if (!deviceResult) {
        std::cerr << "Failed to create device: " 
                  << deviceResult.error().message << "\n";
        return 1;
    }
    
    auto device = std::move(*deviceResult);
    
    // 2. Create vertex buffer
    // 3. Create shaders
    // 4. Create graphics pipeline
    // 5. Render loop
    // 6. Cleanup
    
    return 0;
}
```

For the complete tutorial including vertex data, shaders, pipeline setup, and render loop, please refer to the [Chinese version](../zh-CN/examples/quick_start.md).
