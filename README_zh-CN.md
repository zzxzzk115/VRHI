# VRHI

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![C++23](https://img.shields.io/badge/C%2B%2B-23-blue.svg)](https://en.cppreference.com/w/cpp/23)

一个现代化的渲染硬件抽象层 (Render Hardware Interface)，支持多图形后端，基于 C++23 设计。

## ✨ 特性

- 🎯 **多后端支持**: Vulkan、OpenGL 3.3/4.1/4.6、OpenGL ES 3.0/3.1
  - **计划中 (v2.0)**: Direct3D 12 (Windows)、Metal (macOS/iOS)、WebGPU
- 🤖 **智能后端选择**: 自动检测硬件特性并评分，选择最优后端
- 🔒 **现代 C++23**: 充分利用最新 C++ 标准特性
- 🛡️ **RAII 资源管理**: 自动化资源生命周期，避免泄漏
- 🌐 **跨平台**: Windows、Linux、macOS、Android、iOS、树莓派
- ⚡ **高性能**: 零开销抽象，从高端 PC 到低端移动设备
- 🎨 **后端可扩展性**: 抽象设计允许轻松扩展到新的图形 API (D3D12、Metal 等)
- 🪟 **窗口系统抽象**: 统一接口支持 SDL2、SDL3、GLFW、EGL，自动键码转换
- 🔨 **CMake 构建系统**: 自包含 `external/` 目录，无需 submodule 或 CPM

## 🚀 快速开始

### 最小示例

```cpp
#include <VRHI/VRHI.hpp>

int main() {
    // 创建设备（自动选择最佳后端）
    auto device = VRHI::CreateDevice().value();
    
    // 创建资源
    VRHI::BufferDesc desc;
    desc.size = 1024;
    desc.usage = VRHI::BufferUsage::Vertex;
    auto buffer = device->CreateBuffer(desc).value();
    
    // 渲染循环
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

### 后端选择示例

```cpp
// 查看所有可用后端
auto backends = VRHI::EnumerateBackends();
for (const auto& backend : backends) {
    std::cout << backend.name << " (Score: " << backend.score << ")\n";
}

// 手动指定后端
VRHI::DeviceConfig config;
config.preferredBackend = VRHI::BackendType::Vulkan;
auto device = VRHI::CreateDevice(config).value();
```

## 📚 文档

完整文档请查看 [docs/zh-CN/](docs/zh-CN/) 目录：

### 设计文档
- [架构设计](docs/zh-CN/design/architecture.md) - VRHI 整体架构
- [后端评分系统](docs/zh-CN/design/backend_scoring.md) - 智能后端选择机制
- [特性检测](docs/zh-CN/design/feature_detection.md) - 硬件特性检测
- [RAII 原则](docs/zh-CN/design/raii_principles.md) - 资源管理设计
- [构建系统设计](docs/zh-CN/design/build_system.md) - CMake 构建系统和依赖管理
- [窗口系统抽象](docs/zh-CN/design/window_system.md) - 多窗口库支持

### API 参考与示例
- [API 参考](docs/zh-CN/api/core.md) - 核心 API 文档
- [快速入门](docs/zh-CN/examples/quick_start.md) - 入门教程
- [最佳实践](docs/zh-CN/examples/best_practices.md) - 使用建议

## 🎯 核心设计

### 智能后端选择

VRHI 的创新之处在于初始化时会：

1. **检测所有可用后端** (Vulkan, OpenGL, OpenGL ES, D3D12*, Metal*)
2. **查询硬件特性支持** (计算着色器、光线追踪等)
3. **综合评分** (特性支持度 40% + 性能 30% + 稳定性 20% + 兼容性 10%)
4. **自动选择最优后端** 或由用户手动指定

*计划在 v2.0 实现

```cpp
VRHI::DeviceConfig config;
config.features.required = {VRHI::Feature::Compute};  // 必需特性
config.features.optional = {VRHI::Feature::RayTracing};  // 可选特性

auto device = VRHI::CreateDevice(config).value();
// 自动选择支持计算着色器且评分最高的后端
```

### 后端抽象与可扩展性

VRHI 从设计之初就考虑到轻松扩展到新的图形 API：

- **统一接口**: 所有后端实现相同的接口契约
- **工厂模式**: 新后端可在编译期或运行时注册
- **基于特性**: 核心 API 仅包含跨平台特性
- **平台感知评分**: 原生 API（Windows 上的 D3D12、macOS 上的 Metal）获得优先级

添加新后端只需要：
1. 实现 `IBackend` 和 `IDevice` 接口
2. 向工厂注册后端
3. 将 VRHI 概念映射到原生 API 构造

详见 [架构设计](docs/zh-CN/design/architecture.md) 了解抽象层细节。

### RAII 资源管理

所有资源使用 RAII 和智能指针管理，无需手动释放：

```cpp
{
    auto buffer = device->CreateBuffer(desc).value();
    auto texture = device->CreateTexture(texDesc).value();
    // 使用资源...
} // 自动释放，即使发生异常也安全
```

### 现代错误处理

使用 C++23 的 `std::expected` 进行错误处理，避免异常开销：

```cpp
auto result = device->CreateBuffer(desc);
if (result) {
    auto buffer = std::move(*result);
    // 使用 buffer
} else {
    std::cerr << "Error: " << result.error().message << "\n";
}
```

## 🔧 构建系统

VRHI 使用 **CMake**，所有依赖都自包含在 `external/` 目录中：

- **无 git submodule**: 所有第三方源代码直接包含
- **无 CPM.cmake**: 完全控制依赖版本
- **离线构建**: 无需网络连接
- **模块化配置**: 每个子系统独立 CMakeLists.txt

### 快速构建

```bash
# 配置
cmake -B build -DCMAKE_BUILD_TYPE=Release

# 构建
cmake --build build --config Release
```

### 构建选项

```cmake
# 后端选择
option(VRHI_ENABLE_VULKAN "启用 Vulkan 后端" ON)
option(VRHI_ENABLE_OPENGL "启用 OpenGL 后端" ON)
option(VRHI_ENABLE_D3D12 "启用 D3D12 后端 (Windows)" ${WIN32})
option(VRHI_ENABLE_METAL "启用 Metal 后端 (macOS)" ${APPLE})

# 窗口系统支持
option(VRHI_WINDOW_SDL2 "启用 SDL2 支持" ON)
option(VRHI_WINDOW_SDL3 "启用 SDL3 支持" OFF)
option(VRHI_WINDOW_GLFW "启用 GLFW 支持" ON)
option(VRHI_WINDOW_EGL "启用 EGL 支持" OFF)
```

详见 [构建系统设计](docs/zh-CN/design/build_system.md) 获取完整文档。

### 要求

- **编译器**: GCC 14+, Clang 19+, Apple Clang 15+ (Xcode 15.3+), MSVC 2022+
- **C++ 标准**: C++23
- **CMake**: 3.21+
- **平台**: Windows, Linux, macOS, Android, iOS, 树莓派
- OpenGL 驱动 (系统提供)

## 📦 集成到项目

### CMake

```cmake
add_subdirectory(external/VRHI)
target_link_libraries(your_app PRIVATE VRHI::VRHI)
```

## 🎨 支持的后端

### 当前后端

| 后端 | 平台 | 性能 | 兼容性 | 用途 |
|------|------|------|--------|------|
| **Vulkan** | Windows, Linux, Android | ⭐⭐⭐⭐⭐ | ⭐⭐⭐ | 高性能 PC、现代移动设备 |
| **OpenGL 4.6** | Windows, Linux | ⭐⭐⭐⭐ | ⭐⭐⭐⭐ | 高端 PC |
| **OpenGL 4.1** | Windows, Linux, macOS | ⭐⭐⭐⭐ | ⭐⭐⭐⭐ | 中高端 PC、**macOS**（最高版本） |
| **OpenGL 3.3** | Windows, Linux, macOS | ⭐⭐⭐ | ⭐⭐⭐⭐⭐ | 通用 PC、最低桌面 GL 版本 |
| **OpenGL ES 3.1** | Android, iOS | ⭐⭐⭐⭐ | ⭐⭐⭐ | 现代移动设备 |
| **OpenGL ES 3.0** | Android, iOS, 树莓派 | ⭐⭐⭐ | ⭐⭐⭐⭐ | 主流移动设备、嵌入式 |

### 计划中后端 (v2.0) 🚧

| 后端 | 平台 | 性能 | 兼容性 | 用途 |
|------|------|------|--------|------|
| **Direct3D 12** | Windows 10/11 | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐ | Windows 高性能、原生 API |
| **Metal** | macOS, iOS | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ | Apple 平台、原生 API |
| **WebGPU** | Web 浏览器 | ⭐⭐⭐ | ⭐⭐⭐ | 跨平台 Web 应用 |

**平台优先级**:
- **Windows**: D3D12 (计划) > Vulkan > OpenGL 4.6
- **macOS**: Metal (计划) > OpenGL 4.1
- **iOS**: Metal (计划) > OpenGL ES 3.0
- **Linux**: Vulkan > OpenGL 4.6
- **Android**: Vulkan > OpenGL ES 3.1

## 🤝 贡献

欢迎贡献！请查看 [CONTRIBUTING.md](CONTRIBUTING.md)（待创建）。

## 📄 许可证

本项目采用 MIT 许可证。详见 [LICENSE](LICENSE)。

## 🙏 致谢

VRHI 的设计灵感来自于：
- Vulkan API
- DirectX 12
- Metal
- WebGPU
- bgfx

## 📮 联系

- 问题追踪: [GitHub Issues](https://github.com/zzxzzk115/VRHI/issues)
- 讨论: [GitHub Discussions](https://github.com/zzxzzk115/VRHI/discussions)

---

**注意**: VRHI 目前处于设计阶段，文档和接口设计已完成，实现正在进行中。
