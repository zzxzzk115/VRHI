# VRHI 文档

欢迎使用 VRHI (Versatile Render Hardware Interface) 文档。

## 目录

### 编码规范
- [编码规范](CODING_STANDARDS.md) - VRHI 的代码风格和约定

### 设计文档
- [架构设计](design/architecture.md) - VRHI 的整体架构设计
- [后端评分系统](design/backend_scoring.md) - 后端选择和评分机制
- [特性检测](design/feature_detection.md) - 硬件特性检测系统
- [RAII 设计原则](design/raii_principles.md) - 资源管理和 RAII 实现
- [构建系统设计](design/build_system.md) - CMake 构建系统和依赖管理
- [窗口系统抽象](design/window_system.md) - 多窗口库支持和事件处理

### API 参考
- [核心接口](api/core.md) - 核心 RHI 接口
- [设备管理](api/device.md) - 设备和上下文管理
- [资源管理](api/resources.md) - 缓冲区、纹理等资源
- [命令记录](api/commands.md) - 命令缓冲区和提交
- [后端接口](api/backends.md) - Vulkan 和 OpenGL 后端

### 示例和最佳实践
- [快速入门](examples/quick_start.md) - 快速上手指南
- [最佳实践](examples/best_practices.md) - 使用建议和最佳实践
- [跨平台开发](examples/cross_platform.md) - 跨平台开发指南

## 关于 VRHI

VRHI 是一个现代化的渲染硬件抽象层，旨在：

- **多后端支持**: 支持 Vulkan 和多个 OpenGL 版本 (4.6, 4.1, 3.3, ES 3.1, ES 3.0)
- **智能后端选择**: 基于硬件特性自动评分并选择最优后端
- **现代 C++**: 使用 C++23 标准，充分利用现代 C++ 特性
- **RAII 资源管理**: 自动化资源生命周期管理，避免内存泄漏
- **广泛兼容性**: 从高性能 PC 到树莓派、安卓手机的全平台支持
- **性能优先**: 在保证兼容性的同时追求极致性能

## 快速链接

- [项目主页](https://github.com/zzxzzk115/VRHI)
- [问题追踪](https://github.com/zzxzzk115/VRHI/issues)
- [贡献指南](../CONTRIBUTING.md)
