# VRHI 编码规范

本文档概述了 VRHI 项目的编码标准和约定。所有贡献者都应遵循这些准则，以保持代码的一致性和质量。

## 变量初始化

### 结构体变量初始化

所有局部结构体变量**必须**使用 `{}` （大括号初始化）进行初始化。

✅ **正确**:
```cpp
BufferDesc desc{};
desc.size = 1024;
desc.usage = BufferUsage::Vertex;

BackendInfo info{};
info.type = BackendType::OpenGL33;

ShaderCompilationResult result{};
result.spirv = std::move(spirvData);
```

❌ **错误**:
```cpp
BufferDesc desc;  // 缺少 {}
desc.size = 1024;

BackendInfo info;  // 缺少 {}
info.type = BackendType::OpenGL33;
```

**理由**: 使用 `{}` 确保所有结构体成员都被零初始化或默认初始化，防止未初始化值导致的未定义行为。这对于可能包含没有显式默认值的成员的结构体尤为重要。

### 适用范围

此标准适用于：
- 局部变量（函数内部）
- 临时变量
- 所有结构体/类类型，包括：
  - 描述符结构体（如 `BufferDesc`、`TextureDesc`、`PipelineDesc`）
  - 信息结构体（如 `BackendInfo`、`DeviceInfo`）
  - 结果结构体（如 `ShaderCompilationResult`）
  - 配置结构体（如 `WindowConfig`、`DeviceConfig`）
  - 所有其他自定义结构体/类类型

### 例外情况

- **成员变量**（类字段）：遵循现有的初始化模式（类内初始化器或构造函数初始化）
- **全局变量**：在适当的情况下应使用 `{}` 初始化
- **函数参数**：不需要初始化（值由调用者传递）
- **返回值**：在内联构造返回值时使用 `{}`
- **POD 类型**：虽然不是必需的，但建议使用 `{}` 初始化以保持一致性

## 代码示例

以下示例展示了正确的结构体初始化：

```cpp
std::expected<std::unique_ptr<Buffer>, Error> 
CreateVertexBuffer(Device* device, const void* data, size_t size) {
    // ✅ 正确：使用 {} 初始化
    BufferDesc desc{};
    desc.size = size;
    desc.usage = BufferUsage::Vertex;
    desc.initialData = data;
    desc.debugName = "Vertex Buffer";
    
    auto result = device->CreateBuffer(desc);
    if (!result) {
        return std::unexpected(result.error());
    }
    
    return std::move(*result);
}

void RenderScene(CommandBuffer* cmd) {
    // ✅ 正确：所有结构体变量都使用 {} 初始化
    Viewport viewport{};
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = 1920;
    viewport.height = 1080;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    
    Rect2D scissor{};
    scissor.x = 0;
    scissor.y = 0;
    scissor.width = 1920;
    scissor.height = 1080;
    
    cmd->SetViewport(viewport);
    cmd->SetScissor(scissor);
}
```

## 其他编码规范

### 现代 C++ 特性

- 适当使用 C++23 特性
- 优先使用 `std::expected` 而非异常进行错误处理
- 使用 `std::unique_ptr` 和 `std::shared_ptr` 进行资源管理
- 当类型从上下文中显而易见时，优先使用 `auto`

### 命名约定

- **类型**：大驼峰命名法（如 `BufferDesc`、`DeviceConfig`）
- **变量**：小驼峰命名法（如 `vertexBuffer`、`deviceConfig`）
- **常量**：大驼峰命名法或全大写（如 `MaxFramesInFlight` 或 `MAX_FRAMES_IN_FLIGHT`）
- **私有成员**：`m_` 前缀（如 `m_device`、`m_buffer`）

### 错误处理

- 始终检查 `std::expected` 返回值
- 提供有意义的错误消息
- 使用 `std::unexpected` 返回错误

### 资源管理

- 遵循 RAII 原则
- 使用智能指针实现自动清理
- 避免手动使用 `new`/`delete`

### 注释

- 为不明显的代码添加注释
- 用清晰的描述记录公共 API
- 保持注释与代码更改同步

## 执行

这些标准通过以下方式执行：
- 代码审查
- 静态分析工具（clang-tidy）
- CI/CD 检查

## 参考资料

- [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines)
- [Modern C++ Best Practices](https://github.com/cpp-best-practices/cppbestpractices)
- [VRHI 最佳实践](examples/best_practices.md)
