# VRHI

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![C++23](https://img.shields.io/badge/C%2B%2B-23-blue.svg)](https://en.cppreference.com/w/cpp/23)

一个现代化的渲染硬件抽象层 (Render Hardware Interface)，支持多图形后端，基于 C++23 设计。

## ✨ 特性

- 🎯 **多后端支持**: Vulkan、OpenGL 3.3/4.3/4.6、OpenGL ES 2.0/3.0/3.1
- 🤖 **智能后端选择**: 自动检测硬件特性并评分，选择最优后端
- 🔒 **现代 C++23**: 充分利用最新 C++ 标准特性
- 🛡️ **RAII 资源管理**: 自动化资源生命周期，避免泄漏
- 🌐 **跨平台**: Windows、Linux、macOS、Android、iOS、树莓派
- ⚡ **高性能**: 零开销抽象，从高端 PC 到低端移动设备
- 📦 **Header-Only**: 仅头文件库，易于集成

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

完整文档请查看 [docs/](docs/) 目录：

- [架构设计](docs/design/architecture.md) - VRHI 整体架构
- [后端评分系统](docs/design/backend_scoring.md) - 智能后端选择机制
- [特性检测](docs/design/feature_detection.md) - 硬件特性检测
- [RAII 原则](docs/design/raii_principles.md) - 资源管理设计
- [API 参考](docs/api/core.md) - 核心 API 文档
- [快速入门](docs/examples/quick_start.md) - 入门教程
- [最佳实践](docs/examples/best_practices.md) - 使用建议

## 🎯 核心设计

### 智能后端选择

VRHI 的创新之处在于初始化时会：

1. **检测所有可用后端** (Vulkan, OpenGL, OpenGL ES)
2. **查询硬件特性支持** (计算着色器、光线追踪等)
3. **综合评分** (特性支持度 40% + 性能 30% + 稳定性 20% + 兼容性 10%)
4. **自动选择最优后端** 或由用户手动指定

```cpp
VRHI::DeviceConfig config;
config.features.required = {VRHI::Feature::Compute};  // 必需特性
config.features.optional = {VRHI::Feature::RayTracing};  // 可选特性

auto device = VRHI::CreateDevice(config).value();
// 自动选择支持计算着色器且评分最高的后端
```

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

## 🔧 构建要求

- **编译器**: GCC 13+, Clang 16+, MSVC 2022+
- **C++ 标准**: C++23
- **CMake**: 3.20+
- **平台**: Windows, Linux, macOS, Android, iOS, 树莓派

### 依赖项

- Vulkan SDK (可选，用于 Vulkan 后端)
- OpenGL 驱动 (系统提供)

## 📦 集成到项目

### CMake

```cmake
add_subdirectory(external/VRHI)
target_link_libraries(your_app PRIVATE VRHI::VRHI)
```

## 🎨 支持的后端

| 后端 | 平台 | 性能 | 兼容性 | 用途 |
|------|------|------|--------|------|
| **Vulkan** | Windows, Linux, Android | ⭐⭐⭐⭐⭐ | ⭐⭐⭐ | 高性能 PC、现代移动设备 |
| **OpenGL 4.6** | Windows, Linux | ⭐⭐⭐⭐ | ⭐⭐⭐⭐ | 高端 PC |
| **OpenGL 4.3** | Windows, Linux | ⭐⭐⭐⭐ | ⭐⭐⭐⭐ | 中高端 PC |
| **OpenGL 3.3** | Windows, Linux, macOS | ⭐⭐⭐ | ⭐⭐⭐⭐⭐ | 通用 PC、老旧硬件 |
| **OpenGL ES 3.1** | Android, iOS | ⭐⭐⭐⭐ | ⭐⭐⭐ | 现代移动设备 |
| **OpenGL ES 3.0** | Android, iOS | ⭐⭐⭐ | ⭐⭐⭐⭐ | 主流移动设备 |
| **OpenGL ES 2.0** | Android, 树莓派 | ⭐⭐ | ⭐⭐⭐⭐⭐ | 低端设备、嵌入式 |

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
