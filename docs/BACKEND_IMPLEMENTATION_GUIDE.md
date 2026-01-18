# 后端实现指南 (Backend Implementation Guide)

本文档说明如何为 VRHI 添加新的渲染后端。通过遵循这个指南，你可以轻松地添加对新图形 API 的支持。

## 概述

VRHI 的多后端架构设计为绝对可扩展。每个后端都是一个独立的模块，通过统一的接口与核心系统交互。

## 后端组件

实现一个完整的后端需要以下组件：

1. **Backend 类** - 实现 `IBackend` 接口
2. **Device 类** - 实现 `Device` 接口
3. **资源类** - 实现 `Buffer`, `Texture`, `Sampler` 等接口
4. **其他类** - CommandBuffer, Pipeline, Shader 等

## 步骤 1: 创建后端目录结构

```
src/Backends/YourBackend/
├── YourBackend.hpp          # Backend 实现
├── YourBackend.cpp
├── YourBackendDevice.hpp    # Device 实现
├── YourBackendDevice.cpp
├── YourBackendBuffer.hpp    # Buffer 实现
├── YourBackendBuffer.cpp
├── YourBackendTexture.hpp   # Texture 实现
├── YourBackendTexture.cpp
├── YourBackendSampler.hpp   # Sampler 实现
├── YourBackendSampler.cpp
└── ...                      # 其他资源
```

## 步骤 2: 实现 Backend 类

```cpp
// YourBackend.hpp
#pragma once
#include <VRHI/Backend.hpp>

namespace VRHI {

class YourBackend : public IBackend {
public:
    YourBackend() = default;
    ~YourBackend() override = default;
    
    // 基本信息
    BackendType GetType() const noexcept override;
    std::string_view GetName() const noexcept override;
    Version GetVersion() const noexcept override;
    
    // 特性检测
    std::expected<FeatureSet, Error> GetSupportedFeatures() const override;
    bool IsFeatureSupported(Feature feature) const noexcept override;
    
    // 后端评分
    float CalculateScore(const FeatureRequirements& requirements) const override;
    
    // 设备创建
    std::expected<std::unique_ptr<Device>, Error>
    CreateDevice(const DeviceConfig& config) override;
};

} // namespace VRHI
```

## 步骤 3: 实现 Device 类

```cpp
// YourBackendDevice.hpp
#pragma once
#include <VRHI/VRHI.hpp>

namespace VRHI {

class YourBackendDevice : public Device {
public:
    YourBackendDevice(const DeviceConfig& config);
    ~YourBackendDevice() override;
    
    // 设备信息
    BackendType GetBackendType() const noexcept override;
    BackendInfo GetBackendInfo() const override;
    const FeatureSet& GetFeatures() const noexcept override;
    bool IsFeatureSupported(Feature feature) const noexcept override;
    const DeviceProperties& GetProperties() const noexcept override;
    
    // 资源创建
    std::expected<std::unique_ptr<Buffer>, Error>
    CreateBuffer(const BufferDesc& desc) override;
    
    std::expected<std::unique_ptr<Texture>, Error>
    CreateTexture(const TextureDesc& desc) override;
    
    std::expected<std::unique_ptr<Sampler>, Error>
    CreateSampler(const SamplerDesc& desc) override;
    
    // ... 其他资源创建方法
    
private:
    DeviceConfig m_config;
    FeatureSet m_features;
    DeviceProperties m_properties;
    
    // 后端特定的上下文/设备句柄
    void* m_nativeDevice = nullptr;
};

} // namespace VRHI
```

## 步骤 4: 实现资源类 (以 Buffer 为例)

```cpp
// YourBackendBuffer.hpp
#pragma once
#include <VRHI/Resources.hpp>

namespace VRHI {

class YourBackendBuffer : public Buffer {
public:
    ~YourBackendBuffer() override;
    
    static std::expected<std::unique_ptr<Buffer>, Error>
    Create(const BufferDesc& desc);
    
    // Buffer 接口实现
    size_t GetSize() const noexcept override;
    BufferUsage GetUsage() const noexcept override;
    
    void* Map() override;
    void* Map(size_t offset, size_t size) override;
    void Unmap() override;
    
    void Update(const void* data, size_t size, size_t offset = 0) override;
    void Read(void* data, size_t size, size_t offset = 0) override;
    
private:
    YourBackendBuffer(const BufferDesc& desc, /* 原生句柄 */);
    
    BufferDesc m_desc;
    void* m_nativeBuffer = nullptr;  // 后端特定的缓冲区句柄
    void* m_mappedPtr = nullptr;
};

} // namespace VRHI
```

### Buffer 实现示例

```cpp
// YourBackendBuffer.cpp
#include "YourBackendBuffer.hpp"
#include <VRHI/Logging.hpp>

namespace VRHI {

YourBackendBuffer::YourBackendBuffer(const BufferDesc& desc, /* 原生句柄 */)
    : m_desc(desc)
{
    // 初始化后端特定的缓冲区
}

YourBackendBuffer::~YourBackendBuffer() {
    // 清理后端资源
    if (m_nativeBuffer) {
        // 释放原生缓冲区
    }
}

std::expected<std::unique_ptr<Buffer>, Error>
YourBackendBuffer::Create(const BufferDesc& desc) {
    // 1. 验证描述符
    if (desc.size == 0) {
        return std::unexpected(Error{
            Error::Code::InvalidConfig,
            "Buffer size must be greater than 0"
        });
    }
    
    // 2. 创建后端特定的缓冲区
    // void* nativeBuffer = CreateNativeBuffer(...);
    
    // 3. 返回 VRHI Buffer 对象
    auto buffer = std::unique_ptr<Buffer>(
        new YourBackendBuffer(desc, /* nativeBuffer */)
    );
    
    return buffer;
}

size_t YourBackendBuffer::GetSize() const noexcept {
    return m_desc.size;
}

BufferUsage YourBackendBuffer::GetUsage() const noexcept {
    return m_desc.usage;
}

void* YourBackendBuffer::Map() {
    return Map(0, m_desc.size);
}

void* YourBackendBuffer::Map(size_t offset, size_t size) {
    // 映射后端缓冲区
    // m_mappedPtr = MapNativeBuffer(m_nativeBuffer, offset, size);
    return m_mappedPtr;
}

void YourBackendBuffer::Unmap() {
    if (m_mappedPtr) {
        // UnmapNativeBuffer(m_nativeBuffer);
        m_mappedPtr = nullptr;
    }
}

void YourBackendBuffer::Update(const void* data, size_t size, size_t offset) {
    if (offset + size > m_desc.size) {
        LogError("Buffer update out of bounds");
        return;
    }
    
    // 更新后端缓冲区
    // UpdateNativeBuffer(m_nativeBuffer, data, size, offset);
}

void YourBackendBuffer::Read(void* data, size_t size, size_t offset) {
    if (offset + size > m_desc.size) {
        LogError("Buffer read out of bounds");
        return;
    }
    
    // 从后端缓冲区读取
    // ReadNativeBuffer(m_nativeBuffer, data, size, offset);
}

} // namespace VRHI
```

## 步骤 5: 注册后端

```cpp
// YourBackend.cpp
#include "YourBackend.hpp"
#include "YourBackendDevice.hpp"
#include <VRHI/Backend.hpp>

namespace VRHI {

// 后端注册 - 在库初始化时自动调用
VRHI_REGISTER_BACKEND(YourBackend, []() {
    return std::make_unique<YourBackend>();
});

// ... Backend 实现

} // namespace VRHI
```

## 步骤 6: 更新 CMakeLists.txt

```cmake
# src/CMakeLists.txt

if(VRHI_ENABLE_YOUR_BACKEND)
    list(APPEND VRHI_BACKEND_SOURCES
        Backends/YourBackend/YourBackend.cpp
        Backends/YourBackend/YourBackendDevice.cpp
        Backends/YourBackend/YourBackendBuffer.cpp
        Backends/YourBackend/YourBackendTexture.cpp
        Backends/YourBackend/YourBackendSampler.cpp
        # ... 其他源文件
    )
    message(STATUS "YourBackend enabled")
endif()
```

## 参考实现

VRHI 提供了两个参考实现：

### 1. Null Backend (简单参考)

- **位置**: `src/Core/NullBackend.cpp`, `src/Core/NullResources.cpp`
- **特点**: 使用 CPU 内存模拟 GPU 资源
- **用途**: 
  - 理解资源接口的最小实现
  - 无需图形上下文的测试
  - 后端扩展性的演示

```cpp
// Null Backend 的 Buffer 使用 std::vector<uint8_t> 存储数据
class NullBuffer : public Buffer {
    std::vector<uint8_t> m_data;  // CPU 端存储
    // ... 实现所有 Buffer 接口方法
};
```

### 2. OpenGL 3.3 Backend (完整参考)

- **位置**: `src/Backends/OpenGL33/`
- **特点**: 完整的 OpenGL 3.3 实现
- **用途**:
  - 真实后端的完整示例
  - 了解如何与图形 API 交互
  - 性能优化和状态管理的参考

```cpp
// OpenGL Backend 的 Buffer 使用 GLuint 句柄
class OpenGL33Buffer : public Buffer {
    GLuint m_buffer = 0;  // OpenGL 缓冲区对象
    GLenum m_target = GL_ARRAY_BUFFER;
    // ... 实现所有 Buffer 接口方法
};
```

## 关键设计原则

### 1. 接口一致性
所有后端必须实现相同的接口，确保上层代码的可移植性。

### 2. RAII 资源管理
所有资源必须在析构函数中正确清理：

```cpp
YourBackendBuffer::~YourBackendBuffer() {
    if (m_nativeBuffer) {
        // 释放原生资源
        ReleaseNativeBuffer(m_nativeBuffer);
    }
}
```

### 3. 错误处理
使用 `std::expected` 进行错误处理：

```cpp
std::expected<std::unique_ptr<Buffer>, Error>
YourBackendBuffer::Create(const BufferDesc& desc) {
    if (/* 验证失败 */) {
        return std::unexpected(Error{
            Error::Code::InvalidConfig,
            "错误信息"
        });
    }
    // ... 创建资源
    return buffer;
}
```

### 4. 描述符验证
在 Create 函数中验证所有描述符参数：

```cpp
// 检查大小
if (desc.size == 0) {
    return std::unexpected(Error{...});
}

// 检查范围
if (desc.width > maxTextureSize) {
    return std::unexpected(Error{...});
}
```

## 测试

为你的后端编写单元测试和集成测试：

```cpp
TEST(YourBackendTest, BufferCreation) {
    auto backend = std::make_unique<YourBackend>();
    auto device = backend->CreateDevice({});
    
    BufferDesc desc{};
    desc.size = 1024;
    desc.usage = BufferUsage::Vertex;
    
    auto buffer = device->CreateBuffer(desc);
    ASSERT_TRUE(buffer.has_value());
    EXPECT_EQ(buffer->GetSize(), 1024);
}
```

## 构建和调试

1. **启用后端**: 在 CMake 中添加选项
   ```cmake
   option(VRHI_ENABLE_YOUR_BACKEND "Enable YourBackend" ON)
   ```

2. **调试**: 使用 VRHI 的日志系统
   ```cpp
   LogDebug("Buffer created: size={}", desc.size);
   LogError("Failed to create buffer: {}", errorMsg);
   ```

3. **性能**: 使用 VRHI 的性能分析工具（如果启用）

## 完整性检查清单

- [ ] Backend 类实现所有 IBackend 方法
- [ ] Device 类实现所有 Device 方法
- [ ] Buffer 实现 Map/Unmap/Update/Read
- [ ] Texture 实现 Update/UpdateRegion/GenerateMipmaps
- [ ] Sampler 创建和配置
- [ ] Shader 编译和链接
- [ ] Pipeline 状态管理
- [ ] CommandBuffer 命令记录
- [ ] 所有资源正确使用 RAII
- [ ] 错误处理使用 std::expected
- [ ] 添加单元测试
- [ ] 添加集成测试
- [ ] 更新 CMakeLists.txt
- [ ] 更新文档

## 后续步骤

实现基本后端后，你可以：

1. **优化性能**: 添加资源缓存、批处理等
2. **添加高级特性**: 计算着色器、光线追踪等
3. **平台特定优化**: 利用平台原生 API 的优势
4. **调试工具**: 集成后端特定的调试和验证层

## 参考资源

- [VRHI 架构设计](design/architecture.md)
- [RAII 原则](design/raii_principles.md)
- [资源管理 API](api/resources.md)
- [OpenGL33 Backend 源码](../../src/Backends/OpenGL33/)
- [Null Backend 源码](../../src/Core/NullResources.cpp)

---

**注意**: 这是一个不断演进的指南。如果你在实现过程中发现问题或有改进建议，请提交 Issue 或 PR。
