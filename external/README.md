# External Dependencies

This directory contains third-party dependencies for VRHI. All dependencies are self-contained within this directory to ensure offline builds and version control.

## Philosophy

VRHI uses a **self-contained dependency model**:
- ✅ No git submodules
- ✅ No CPM.cmake or other network-based dependency managers
- ✅ All source code included directly in the repository
- ✅ Full control over dependency versions
- ✅ Offline builds supported

## Dependencies to be Added

The following dependencies will be added during development:

### Graphics API Support

- **Vulkan Headers** (Header-only)
  - Version: 1.3.x
  - Source: https://github.com/KhronosGroup/Vulkan-Headers
  - License: Apache 2.0
  - Purpose: Vulkan API definitions

- **GLAD** (OpenGL Loader)
  - Version: 2.0.x
  - Generated from: https://glad.dav1d.de/
  - License: Public Domain / MIT
  - Purpose: OpenGL function loading

- **vk-bootstrap** (Optional)
  - Version: 1.3.x
  - Source: https://github.com/charles-lunarg/vk-bootstrap
  - License: MIT
  - Purpose: Vulkan initialization helper

### Window System (Optional)

- **SDL2** (Optional)
  - Version: 2.x
  - Source: https://github.com/libsdl-org/SDL
  - License: Zlib
  - Purpose: Cross-platform window and input handling

- **SDL3** (Optional)
  - Version: 3.x
  - Source: https://github.com/libsdl-org/SDL
  - License: Zlib
  - Purpose: Next-generation SDL

- **GLFW** (Optional)
  - Version: 3.x
  - Source: https://github.com/glfw/glfw
  - License: Zlib
  - Purpose: Lightweight window management

## How to Add Dependencies

When adding a new dependency:

1. Create a subdirectory with the dependency name
2. Copy the required source files
3. Add a CMakeLists.txt for the dependency
4. Update this README with version and license information
5. Update `external/CMakeLists.txt` to include the dependency

## Current Status

🚧 **Directory is currently empty** - Dependencies will be added during implementation phase.

Refer to the [Build System Design](../docs/zh-CN/design/build_system.md) for the complete dependency management strategy.
