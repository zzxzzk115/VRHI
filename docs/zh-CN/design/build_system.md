# 构建系统设计

## 概述

VRHI 采用 **CMake** 作为构建系统，强调简洁性和可移植性。所有第三方依赖通过 `external/` 目录直接包含源代码，不使用 git submodules 或 CPM.cmake 等依赖管理工具。这种设计确保：

- **完全控制**：所有依赖源码在仓库内，构建过程完全确定
- **离线构建**：无需网络连接即可完成构建
- **版本锁定**：依赖版本明确，不会因外部更新而破坏构建
- **简化维护**：避免 submodule 同步问题和 CPM 网络依赖

## 目录结构

```
VRHI/
├── CMakeLists.txt              # 根 CMake 配置
├── cmake/
│   ├── VRHIConfig.cmake.in     # 导出配置模板
│   ├── CompilerOptions.cmake   # 编译器选项和警告
│   ├── PlatformDetection.cmake # 平台检测宏
│   └── Dependencies.cmake      # 依赖查找逻辑
├── external/
│   ├── CMakeLists.txt          # 第三方库构建配置
│   ├── vulkan/                 # Vulkan SDK 头文件
│   │   ├── include/
│   │   └── CMakeLists.txt
│   ├── glad/                   # OpenGL Loader (自生成)
│   │   ├── include/
│   │   ├── src/
│   │   └── CMakeLists.txt
│   └── vk-bootstrap/           # Vulkan 初始化辅助库
│       ├── src/
│       └── CMakeLists.txt
├── include/
│   └── VRHI/
│       ├── VRHI.hpp
│       ├── Resources.hpp
│       └── ...
├── src/
│   ├── CMakeLists.txt
│   ├── Core/
│   ├── Backends/
│   │   ├── Vulkan/
│   │   ├── OpenGL/
│   │   ├── D3D12/              # 未来
│   │   └── Metal/              # 未来
│   └── WindowSystem/           # 窗口系统抽象
└── examples/
    ├── CMakeLists.txt
    └── HelloTriangle/
```

## CMake 设计原则

### 1. 模块化配置

每个子系统拥有独立的 `CMakeLists.txt`：

```cmake
# 根 CMakeLists.txt
cmake_minimum_required(VERSION 3.21)
project(VRHI VERSION 1.0.0 LANGUAGES CXX)

# C++23 标准
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 导入辅助模块
include(cmake/PlatformDetection.cmake)
include(cmake/CompilerOptions.cmake)
include(cmake/Dependencies.cmake)

# 添加子目录
add_subdirectory(external)
add_subdirectory(src)
add_subdirectory(examples EXCLUDE_FROM_ALL)
```

### 2. 编译器选项管理

`cmake/CompilerOptions.cmake` 统一管理编译选项：

```cmake
# 高警告级别
if(MSVC)
    add_compile_options(/W4 /WX)
    # D3D12 后端专用标志
    add_compile_definitions(_WIN32_WINNT=0x0A00)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# 优化选项
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(MSVC)
        add_compile_options(/O2 /GL)
    else()
        add_compile_options(-O3 -flto)
    endif()
endif()

# 调试选项
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(VRHI_DEBUG=1)
    if(NOT MSVC)
        add_compile_options(-g3 -fsanitize=address)
        add_link_options(-fsanitize=address)
    endif()
endif()
```

### 3. 平台检测

`cmake/PlatformDetection.cmake` 提供跨平台宏：

```cmake
# 检测操作系统
if(WIN32)
    set(VRHI_PLATFORM_WINDOWS ON)
    set(VRHI_BACKEND_D3D12_AVAILABLE ON)
elseif(APPLE)
    set(VRHI_PLATFORM_MACOS ON)
    set(VRHI_BACKEND_METAL_AVAILABLE ON)
elseif(UNIX)
    set(VRHI_PLATFORM_LINUX ON)
endif()

# 检测 Android
if(ANDROID)
    set(VRHI_PLATFORM_ANDROID ON)
    set(VRHI_BACKEND_GLES_AVAILABLE ON)
endif()

# 检测 Raspberry Pi (通过环境变量或 CMAKE_SYSTEM_PROCESSOR)
if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm" OR CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
    set(VRHI_PLATFORM_ARM ON)
endif()

# 配置可用后端
set(VRHI_BACKEND_VULKAN_AVAILABLE ON)  # 所有平台尝试 Vulkan
set(VRHI_BACKEND_OPENGL_AVAILABLE ON)  # 桌面 OpenGL
```

## 依赖管理策略

### external/ 目录结构

所有第三方依赖直接包含源代码，无外部依赖：

```cmake
# external/CMakeLists.txt

# Vulkan 头文件 (仅头文件，运行时动态加载)
add_library(vulkan-headers INTERFACE)
target_include_directories(vulkan-headers INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/vulkan/include
)

# GLAD (OpenGL Loader)
add_library(glad STATIC
    glad/src/glad.c
)
target_include_directories(glad PUBLIC
    glad/include
)

# vk-bootstrap (Vulkan 初始化辅助)
add_library(vk-bootstrap STATIC
    vk-bootstrap/src/VkBootstrap.cpp
)
target_link_libraries(vk-bootstrap PUBLIC vulkan-headers)

# 导出给主项目
set(VRHI_EXTERNAL_TARGETS
    vulkan-headers
    glad
    vk-bootstrap
    PARENT_SCOPE
)
```

### 依赖版本管理

在 `external/README.md` 中记录每个依赖的版本和来源：

```markdown
# External Dependencies

## Vulkan Headers
- **Version**: 1.3.275
- **Source**: https://github.com/KhronosGroup/Vulkan-Headers
- **License**: Apache 2.0
- **Date**: 2024-01-15
- **Files**: include/vulkan/*

## GLAD (OpenGL Loader)
- **Version**: 2.0.5
- **Generated**: https://glad.dav1d.de/
- **Profile**: OpenGL 4.6 Core + Extensions
- **Date**: 2024-01-15
- **Files**: include/glad/*, src/glad.c

## vk-bootstrap
- **Version**: 1.3.275
- **Source**: https://github.com/charles-lunarg/vk-bootstrap
- **License**: MIT
- **Date**: 2024-01-15
- **Files**: src/VkBootstrap.{h,cpp}
```

## 后端构建配置

### 条件编译后端

根据平台自动启用/禁用后端：

```cmake
# src/CMakeLists.txt

# 核心库源文件
set(VRHI_CORE_SOURCES
    Core/Device.cpp
    Core/Error.cpp
    Core/Features.cpp
)

# 后端源文件（条件添加）
set(VRHI_BACKEND_SOURCES)

# Vulkan 后端
if(VRHI_BACKEND_VULKAN_AVAILABLE)
    list(APPEND VRHI_BACKEND_SOURCES
        Backends/Vulkan/VulkanBackend.cpp
        Backends/Vulkan/VulkanDevice.cpp
        Backends/Vulkan/VulkanResources.cpp
    )
endif()

# OpenGL 后端
if(VRHI_BACKEND_OPENGL_AVAILABLE)
    list(APPEND VRHI_BACKEND_SOURCES
        Backends/OpenGL/OpenGLBackend.cpp
        Backends/OpenGL/OpenGLDevice.cpp
        Backends/OpenGL/OpenGLResources.cpp
    )
endif()

# D3D12 后端 (仅 Windows)
if(VRHI_BACKEND_D3D12_AVAILABLE)
    list(APPEND VRHI_BACKEND_SOURCES
        Backends/D3D12/D3D12Backend.cpp
        Backends/D3D12/D3D12Device.cpp
        Backends/D3D12/D3D12Resources.cpp
    )
endif()

# Metal 后端 (仅 macOS/iOS)
if(VRHI_BACKEND_METAL_AVAILABLE)
    list(APPEND VRHI_BACKEND_SOURCES
        Backends/Metal/MetalBackend.mm  # Objective-C++
        Backends/Metal/MetalDevice.mm
        Backends/Metal/MetalResources.mm
    )
endif()

# 窗口系统抽象
set(VRHI_WINDOW_SOURCES
    WindowSystem/WindowAbstraction.cpp
    WindowSystem/KeyCodeMapper.cpp
    WindowSystem/EventDispatcher.cpp
)

# 创建主库
add_library(VRHI STATIC
    ${VRHI_CORE_SOURCES}
    ${VRHI_BACKEND_SOURCES}
    ${VRHI_WINDOW_SOURCES}
)

target_include_directories(VRHI
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# 链接依赖
target_link_libraries(VRHI
    PRIVATE
        vulkan-headers
        glad
        vk-bootstrap
)

# 平台特定链接
if(WIN32)
    target_link_libraries(VRHI PRIVATE d3d12 dxgi dxguid)
elseif(APPLE)
    target_link_libraries(VRHI PRIVATE "-framework Metal" "-framework QuartzCore")
endif()
```

## 构建选项

### 用户可配置选项

```cmake
# 后端选择
option(VRHI_ENABLE_VULKAN "Enable Vulkan backend" ON)
option(VRHI_ENABLE_OPENGL "Enable OpenGL backend" ON)
option(VRHI_ENABLE_D3D12 "Enable Direct3D 12 backend (Windows only)" ${VRHI_PLATFORM_WINDOWS})
option(VRHI_ENABLE_METAL "Enable Metal backend (macOS/iOS only)" ${VRHI_PLATFORM_MACOS})

# 窗口系统支持
option(VRHI_WINDOW_SDL2 "Enable SDL2 window system support" ON)
option(VRHI_WINDOW_SDL3 "Enable SDL3 window system support" OFF)
option(VRHI_WINDOW_GLFW "Enable GLFW window system support" ON)
option(VRHI_WINDOW_EGL "Enable EGL window system support (Linux/Android)" OFF)

# 构建类型
option(VRHI_BUILD_EXAMPLES "Build example applications" ON)
option(VRHI_BUILD_TESTS "Build unit tests" OFF)
option(VRHI_BUILD_SHARED_LIBS "Build shared library instead of static" OFF)

# 调试选项
option(VRHI_ENABLE_VALIDATION "Enable API validation layers (debug builds)" ON)
option(VRHI_ENABLE_PROFILING "Enable built-in profiling" OFF)
```

### 配置示例

```bash
# Windows - D3D12 + Vulkan, SDL2
cmake -B build -G "Visual Studio 17 2022" \
    -DVRHI_ENABLE_D3D12=ON \
    -DVRHI_ENABLE_VULKAN=ON \
    -DVRHI_WINDOW_SDL2=ON \
    -DCMAKE_BUILD_TYPE=Release

# macOS - Metal + OpenGL 4.1, GLFW
cmake -B build -G Xcode \
    -DVRHI_ENABLE_METAL=ON \
    -DVRHI_ENABLE_OPENGL=ON \
    -DVRHI_WINDOW_GLFW=ON

# Linux - Vulkan + OpenGL, SDL3
cmake -B build -G Ninja \
    -DVRHI_ENABLE_VULKAN=ON \
    -DVRHI_ENABLE_OPENGL=ON \
    -DVRHI_WINDOW_SDL3=ON \
    -DCMAKE_BUILD_TYPE=RelWithDebInfo

# Raspberry Pi - OpenGL ES 2.0, EGL
cmake -B build \
    -DVRHI_ENABLE_OPENGL=OFF \
    -DVRHI_BACKEND_GLES_AVAILABLE=ON \
    -DVRHI_WINDOW_EGL=ON \
    -DCMAKE_BUILD_TYPE=Release
```

## 安装和导出

### 安装配置

```cmake
# 安装头文件
install(DIRECTORY include/VRHI
    DESTINATION include
    FILES_MATCHING PATTERN "*.hpp"
)

# 安装库
install(TARGETS VRHI ${VRHI_EXTERNAL_TARGETS}
    EXPORT VRHITargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

# 导出 CMake 配置
install(EXPORT VRHITargets
    FILE VRHITargets.cmake
    NAMESPACE VRHI::
    DESTINATION lib/cmake/VRHI
)

# 配置文件生成
include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${CMAKE_SOURCE_DIR}/cmake/VRHIConfig.cmake.in
    ${CMAKE_BINARY_DIR}/VRHIConfig.cmake
    INSTALL_DESTINATION lib/cmake/VRHI
)

write_basic_package_version_file(
    ${CMAKE_BINARY_DIR}/VRHIConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_BINARY_DIR}/VRHIConfig.cmake
    ${CMAKE_BINARY_DIR}/VRHIConfigVersion.cmake
    DESTINATION lib/cmake/VRHI
)
```

### 外部项目集成

用户可通过 `find_package` 使用 VRHI：

```cmake
# 用户项目的 CMakeLists.txt
cmake_minimum_required(VERSION 3.21)
project(MyApp CXX)

# 查找 VRHI
find_package(VRHI 1.0 REQUIRED)

# 创建可执行文件
add_executable(MyApp main.cpp)

# 链接 VRHI
target_link_libraries(MyApp PRIVATE VRHI::VRHI)
```

## 跨平台构建脚本

### build.sh (Linux/macOS)

```bash
#!/bin/bash
set -e

BUILD_TYPE=${1:-Release}
BUILD_DIR="build"

echo "Building VRHI - ${BUILD_TYPE}"

# 配置
cmake -B ${BUILD_DIR} \
    -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
    -DVRHI_BUILD_EXAMPLES=ON

# 构建
cmake --build ${BUILD_DIR} --config ${BUILD_TYPE} -j$(nproc)

# 运行测试 (可选)
if [ "${BUILD_TYPE}" == "Debug" ]; then
    ctest --test-dir ${BUILD_DIR} --output-on-failure
fi

echo "Build complete: ${BUILD_DIR}"
```

### build.bat (Windows)

```batch
@echo off
setlocal

set BUILD_TYPE=%1
if "%BUILD_TYPE%"=="" set BUILD_TYPE=Release

set BUILD_DIR=build

echo Building VRHI - %BUILD_TYPE%

REM 配置
cmake -B %BUILD_DIR% -G "Visual Studio 17 2022" ^
    -DVRHI_BUILD_EXAMPLES=ON

REM 构建
cmake --build %BUILD_DIR% --config %BUILD_TYPE%

REM 运行测试
if "%BUILD_TYPE%"=="Debug" (
    ctest --test-dir %BUILD_DIR% -C %BUILD_TYPE% --output-on-failure
)

echo Build complete: %BUILD_DIR%
```

## 持续集成

### GitHub Actions 工作流

```yaml
# .github/workflows/build.yml
name: Build

on: [push, pull_request]

jobs:
  build-windows:
    runs-on: windows-latest
    steps:
      - uses: actions/checkout@v4
      - name: Configure
        run: cmake -B build -G "Visual Studio 17 2022"
      - name: Build
        run: cmake --build build --config Release
      
  build-linux:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - name: Install dependencies
        run: sudo apt-get install -y libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev
      - name: Configure
        run: cmake -B build -DCMAKE_BUILD_TYPE=Release
      - name: Build
        run: cmake --build build -j$(nproc)
      
  build-macos:
    runs-on: macos-latest
    steps:
      - uses: actions/checkout@v4
      - name: Configure
        run: cmake -B build -DCMAKE_BUILD_TYPE=Release
      - name: Build
        run: cmake --build build -j$(sysctl -n hw.ncpu)
```

## 总结

VRHI 的构建系统设计遵循以下核心原则：

1. **自包含**：所有依赖通过 `external/` 目录直接包含
2. **模块化**：每个子系统独立配置，按需编译
3. **跨平台**：统一的 CMake 配置支持 Windows/Linux/macOS/Android
4. **灵活性**：丰富的构建选项，支持不同后端和窗口系统组合
5. **易集成**：标准 CMake 导出，`find_package` 即可使用

这种设计确保 VRHI 可以在各种环境下稳定构建，同时保持代码库的简洁和可维护性。
