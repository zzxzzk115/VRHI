# VRHI Coding Standards

> **Note**: For the Chinese version, see [中文版](zh-CN/CODING_STANDARDS.md)

This document outlines the coding standards and conventions for the VRHI project. All contributors should follow these guidelines to maintain code consistency and quality.

## Variable Initialization

### Struct Variable Initialization

All local struct variables **must** be initialized with `{}` (brace initialization).

✅ **Correct**:
```cpp
BufferDesc desc{};
desc.size = 1024;
desc.usage = BufferUsage::Vertex;

BackendInfo info{};
info.type = BackendType::OpenGL33;

ShaderCompilationResult result{};
result.spirv = std::move(spirvData);
```

❌ **Incorrect**:
```cpp
BufferDesc desc;  // Missing {}
desc.size = 1024;

BackendInfo info;  // Missing {}
info.type = BackendType::OpenGL33;
```

**Rationale**: Using `{}` ensures that all struct members are zero-initialized or default-initialized, preventing undefined behavior from uninitialized values. This is particularly important for structs that may have members without explicit default values.

### Applies To

This standard applies to:
- Local variables (inside functions)
- Temporary variables
- All struct/class types including:
  - Descriptor structures (e.g., `BufferDesc`, `TextureDesc`, `PipelineDesc`)
  - Info structures (e.g., `BackendInfo`, `DeviceInfo`)
  - Result structures (e.g., `ShaderCompilationResult`)
  - Configuration structures (e.g., `WindowConfig`, `DeviceConfig`)
  - All other custom struct/class types

### Exceptions

- **Member variables** (class fields): Follow existing initialization patterns (in-class initializers or constructor initialization)
- **Global variables**: Should use `{}` initialization where appropriate
- **Function parameters**: No initialization needed (values are passed by caller)
- **Return values**: Use `{}` when constructing return values inline
- **POD types**: While not required, `{}` initialization is recommended for consistency

## Example Code

Here's an example showing proper struct initialization:

```cpp
std::expected<std::unique_ptr<Buffer>, Error> 
CreateVertexBuffer(Device* device, const void* data, size_t size) {
    // ✅ Correct: Using {} initialization
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
    // ✅ Correct: All struct variables initialized with {}
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

## Additional Coding Standards

### Modern C++ Features

- Use C++23 features appropriately
- Prefer `std::expected` over exceptions for error handling
- Use `std::unique_ptr` and `std::shared_ptr` for resource management
- Prefer `auto` when the type is obvious from context

### Naming Conventions

- **Types**: PascalCase (e.g., `BufferDesc`, `DeviceConfig`)
- **Variables**: camelCase (e.g., `vertexBuffer`, `deviceConfig`)
- **Constants**: PascalCase or UPPER_CASE (e.g., `MaxFramesInFlight` or `MAX_FRAMES_IN_FLIGHT`)
- **Private members**: `m_` prefix (e.g., `m_device`, `m_buffer`)

### Error Handling

- Always check `std::expected` return values
- Provide meaningful error messages
- Use `std::unexpected` to return errors

### Resource Management

- Follow RAII principles
- Use smart pointers for automatic cleanup
- Avoid manual `new`/`delete`

### Comments

- Use comments for non-obvious code
- Document public APIs with clear descriptions
- Keep comments up-to-date with code changes

## Enforcement

These standards are enforced through:
- Code reviews
- Static analysis tools (clang-tidy)
- CI/CD checks

## References

- [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines)
- [Modern C++ Best Practices](https://github.com/cpp-best-practices/cppbestpractices)
- [VRHI Best Practices](examples/best_practices.md)
