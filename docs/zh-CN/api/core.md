# 核心接口

## 概述

VRHI 核心接口提供了设备初始化、配置和基本功能的访问。

## 命名空间

所有 VRHI 接口都在 `VRHI` 命名空间下：

```cpp
namespace VRHI {
    // 所有接口和类型
}
```

## 核心类型

### BackendType

表示图形后端类型：

```cpp
enum class BackendType {
    Vulkan,         // Vulkan 后端
    OpenGL46,       // OpenGL 4.6
    OpenGL41,       // OpenGL 4.1
    OpenGL33,       // OpenGL 3.3
    OpenGL20,       // OpenGL 2.0
    OpenGLES31,     // OpenGL ES 3.1
    OpenGLES30,     // OpenGL ES 3.0
    OpenGLES20,     // OpenGL ES 2.0
    Auto,           // 自动选择最佳后端
};
```

### DeviceConfig

设备创建配置：

```cpp
struct DeviceConfig {
    // 后端选择
    BackendType preferredBackend = BackendType::Auto;
    
    // 特性需求
    FeatureRequirements features;
    
    // 调试选项
    bool enableValidation = false;
    bool enableDebugMarkers = false;
    
    // 窗口系统集成
    void* windowHandle = nullptr;  // 平台相关窗口句柄
    uint32_t width = 1280;
    uint32_t height = 720;
    
    // 缓冲配置
    bool vsync = true;
    uint32_t backBufferCount = 2;
    
    // 日志级别
    LogLevel logLevel = LogLevel::Info;
};
```

### Error

错误类型：

```cpp
struct Error {
    enum class Code {
        Success = 0,
        InvalidConfig,           // 配置无效
        NoCompatibleBackend,     // 没有兼容的后端（常见于必需特性不满足）
        InitializationFailed,    // 初始化失败
        DeviceLost,             // 设备丢失
        OutOfMemory,            // 内存不足
        ValidationError,        // 验证错误
        CompilationError,       // 编译错误
        UnsupportedFeature,     // 不支持的特性
    };
    
    Code code = Code::Success;
    std::string message;
    
    operator bool() const { return code == Code::Success; }
};
```

**错误码说明**:

- **`Success`**: 操作成功
- **`InvalidConfig`**: 提供的配置无效（如窗口句柄为空）
- **`NoCompatibleBackend`**: 没有后端能满足所有必需特性
  - 发生于应用指定了 `FeatureRequirements.required`，但没有任何可用后端支持所有这些特性
  - 错误消息会详细说明哪些必需特性不被支持
  - 应用应该通知用户硬件不满足要求，并考虑降级功能或退出
- **`InitializationFailed`**: 后端初始化失败（如驱动问题）
- **`DeviceLost`**: 设备丢失（如驱动崩溃、GPU 超时）
- **`OutOfMemory`**: 内存分配失败
- **`ValidationError`**: 验证层检测到错误（调试模式）
- **`CompilationError`**: 着色器编译失败
- **`UnsupportedFeature`**: 运行时尝试使用不支持的特性

## 全局函数

### CreateDevice

创建渲染设备：

```cpp
std::expected<std::unique_ptr<Device>, Error> 
CreateDevice(const DeviceConfig& config = {});
```

**参数**:
- `config`: 设备配置

**返回值**:
- 成功: 包含 `Device` 智能指针的 `expected`
- 失败: 包含错误信息的 `expected`

**示例**:
```cpp
// 示例 1: 自动选择最佳后端
auto result = VRHI::CreateDevice();
if (result) {
    auto device = std::move(*result);
    // 使用 device
} else {
    std::cerr << "Failed: " << result.error().message << "\n";
}

// 示例 2: 指定后端和特性
DeviceConfig config;
config.preferredBackend = BackendType::Vulkan;
config.enableValidation = true;
config.features.required = {Feature::Compute};

auto device = VRHI::CreateDevice(config);

// 示例 3: 严格的必需特性验证
DeviceConfig strictConfig;
strictConfig.features.required = {
    Feature::Compute,
    Feature::Texture3D,
    Feature::MultiDrawIndirect,
};

auto strictResult = VRHI::CreateDevice(strictConfig);

if (!strictResult) {
    // 处理必需特性不满足
    if (strictResult.error().code == Error::Code::NoCompatibleBackend) {
        std::cerr << "硬件不支持所需的功能:\n";
        std::cerr << strictResult.error().message << "\n";
        
        // 应用应该:
        // 1. 显示友好的错误消息给用户
        // 2. 提供系统需求信息
        // 3. 考虑降级到基础功能或退出
        
        ShowUserMessage(
            "硬件不满足要求",
            "您的显卡不支持本程序需要的高级功能。\n"
            "需要支持：计算着色器、3D 纹理、间接绘制"
        );
        
        return EXIT_FAILURE;
    }
}

// 成功后，保证支持所有必需特性
auto device = std::move(*strictResult);
// 可以安全地使用计算着色器等功能

// 示例 4: 降级策略
auto highEndResult = VRHI::CreateDevice(highEndConfig);

if (!highEndResult) {
    std::cout << "高级功能不可用，尝试基础模式...\n";
    
    // 尝试降级配置
    DeviceConfig basicConfig;
    basicConfig.features.required = {
        Feature::Texture2D,
        Feature::VertexBuffers,
    };
    
    auto basicResult = VRHI::CreateDevice(basicConfig);
    if (basicResult) {
        useBasicRenderPath = true;
        device = std::move(*basicResult);
    } else {
        std::cerr << "硬件完全不支持\n";
        return EXIT_FAILURE;
    }
}
```

### EnumerateBackends

枚举所有可用后端：

```cpp
std::vector<BackendInfo> EnumerateBackends();
```

**返回值**:
- 可用后端信息列表

**示例**:
```cpp
auto backends = VRHI::EnumerateBackends();
for (const auto& backend : backends) {
    std::cout << backend.name << " - " 
              << backend.version << "\n";
}
```

### GetVersion

获取 VRHI 版本：

```cpp
struct Version {
    uint32_t major;
    uint32_t minor;
    uint32_t patch;
    std::string_view string;
};

Version GetVersion();
```

**示例**:
```cpp
auto version = VRHI::GetVersion();
std::cout << "VRHI " << version.string << "\n";
```

## Device 接口

### 基本信息

```cpp
class Device {
public:
    // 获取后端类型
    BackendType GetBackendType() const;
    
    // 获取后端信息
    BackendInfo GetBackendInfo() const;
    
    // 获取支持的特性
    const FeatureSet& GetFeatures() const;
    
    // 检查特性支持
    bool IsFeatureSupported(Feature feature) const;
    
    // 获取限制值
    uint32_t GetLimit(Limit limit) const;
    
    // 获取设备属性
    const DeviceProperties& GetProperties() const;
};
```

### 资源创建

```cpp
class Device {
public:
    // 缓冲区
    std::expected<std::unique_ptr<Buffer>, Error>
    CreateBuffer(const BufferDesc& desc);
    
    // 纹理
    std::expected<std::unique_ptr<Texture>, Error>
    CreateTexture(const TextureDesc& desc);
    
    // 采样器
    std::expected<std::unique_ptr<Sampler>, Error>
    CreateSampler(const SamplerDesc& desc);
    
    // 着色器
    std::expected<std::unique_ptr<Shader>, Error>
    CreateShader(const ShaderDesc& desc);
    
    // 管线
    std::expected<std::unique_ptr<Pipeline>, Error>
    CreatePipeline(const PipelineDesc& desc);
    
    // 渲染通道
    std::expected<std::unique_ptr<RenderPass>, Error>
    CreateRenderPass(const RenderPassDesc& desc);
    
    // 帧缓冲
    std::expected<std::unique_ptr<Framebuffer>, Error>
    CreateFramebuffer(const FramebufferDesc& desc);
};
```

### 命令执行

```cpp
class Device {
public:
    // 创建命令缓冲
    std::unique_ptr<CommandBuffer> CreateCommandBuffer();
    
    // 提交命令
    void Submit(std::unique_ptr<CommandBuffer> cmd);
    
    // 提交多个命令缓冲
    void Submit(std::span<std::unique_ptr<CommandBuffer>> cmds);
    
    // 等待设备空闲
    void WaitIdle();
    
    // 等待特定命令完成
    void Wait(const Fence& fence);
};
```

### 同步

```cpp
class Device {
public:
    // 创建围栏
    std::unique_ptr<Fence> CreateFence(bool signaled = false);
    
    // 创建信号量
    std::unique_ptr<Semaphore> CreateSemaphore();
    
    // 刷新命令
    void Flush();
};
```

### 交换链

```cpp
class Device {
public:
    // 获取交换链
    SwapChain* GetSwapChain();
    
    // 呈现
    void Present();
    
    // 调整大小
    void Resize(uint32_t width, uint32_t height);
};
```

## BackendInfo 结构

```cpp
struct BackendInfo {
    // 后端标识
    BackendType type;
    std::string name;
    std::string version;
    
    // 评分信息
    float score;
    struct ScoreDetail {
        std::string category;
        float score;
        float weight;
    };
    std::vector<ScoreDetail> scoreDetails;
    
    // 硬件信息
    std::string deviceName;
    std::string vendorName;
    std::string driverVersion;
    
    // 能力
    FeatureSet features;
};
```

## DeviceProperties 结构

```cpp
struct DeviceProperties {
    // 设备信息
    std::string deviceName;
    std::string vendorName;
    uint32_t vendorId;
    uint32_t deviceId;
    
    // 驱动信息
    std::string driverVersion;
    std::string apiVersion;
    
    // 内存
    uint64_t totalMemory;
    uint64_t availableMemory;
    
    // 计算能力
    uint32_t computeUnits;
    uint32_t maxThreadsPerGroup;
};
```

## 使用示例

### 完整初始化流程

```cpp
#include <VRHI/VRHI.hpp>

int main() {
    // 1. 配置设备
    VRHI::DeviceConfig config;
    config.enableValidation = true;
    config.features.required = {
        VRHI::Feature::Texture2D,
        VRHI::Feature::VertexBuffers,
    };
    config.features.optional = {
        VRHI::Feature::Compute,
    };
    
    // 2. 创建设备
    auto deviceResult = VRHI::CreateDevice(config);
    if (!deviceResult) {
        std::cerr << "Failed to create device: " 
                  << deviceResult.error().message << "\n";
        return 1;
    }
    
    auto device = std::move(*deviceResult);
    
    // 3. 查看设备信息
    auto info = device->GetBackendInfo();
    std::cout << "Using: " << info.name << "\n";
    std::cout << "Score: " << info.score << "\n";
    
    // 4. 检查特性
    if (device->IsFeatureSupported(VRHI::Feature::Compute)) {
        std::cout << "Compute shaders available\n";
    }
    
    // 5. 创建资源
    VRHI::BufferDesc bufferDesc;
    bufferDesc.size = 1024 * 1024;
    bufferDesc.usage = VRHI::BufferUsage::Vertex;
    
    auto bufferResult = device->CreateBuffer(bufferDesc);
    if (bufferResult) {
        auto buffer = std::move(*bufferResult);
        // 使用 buffer
    }
    
    // 6. 渲染循环
    while (!ShouldClose()) {
        auto cmd = device->CreateCommandBuffer();
        
        // 记录命令
        cmd->BeginRenderPass(/*...*/);
        cmd->Draw(/*...*/);
        cmd->EndRenderPass();
        
        // 提交
        device->Submit(std::move(cmd));
        device->Present();
    }
    
    // 7. 清理
    device->WaitIdle();
    
    return 0;
}
```

### 查询和选择后端

```cpp
// 列出所有后端
auto backends = VRHI::EnumerateBackends();

std::cout << "Available backends:\n";
for (const auto& backend : backends) {
    std::cout << "  " << backend.name 
              << " (v" << backend.version << ")\n";
    std::cout << "    Device: " << backend.deviceName << "\n";
    std::cout << "    Score: " << backend.score << "\n";
}

// 手动选择特定后端
VRHI::DeviceConfig config;
config.preferredBackend = VRHI::BackendType::Vulkan;

auto device = VRHI::CreateDevice(config);
```

### 错误处理

```cpp
auto result = VRHI::CreateDevice();

// 方法 1: 使用 if
if (result) {
    auto device = std::move(*result);
} else {
    HandleError(result.error());
}

// 方法 2: 使用 value_or
auto device = std::move(result).value_or(nullptr);
if (!device) {
    // 处理错误
}

// 方法 3: 使用 and_then
result.and_then([](auto&& device) {
    // 使用 device
    return std::expected<void, Error>{};
}).or_else([](const Error& error) {
    // 处理错误
    return std::expected<void, Error>{std::unexpect, error};
});
```

## 线程安全

- `CreateDevice()`: 线程安全
- `Device::Create*()`: 线程安全（资源创建）
- `Device::Submit()`: 需要外部同步
- `CommandBuffer`: 非线程安全，每个线程使用独立实例

## 最佳实践

1. **使用 Auto 后端**: 除非有特殊需求，让 VRHI 自动选择
2. **启用验证**: 开发时启用验证层捕获错误
3. **检查返回值**: 始终检查 `std::expected` 返回值
4. **等待空闲**: 销毁设备前调用 `WaitIdle()`
5. **最小特性需求**: 只要求真正需要的特性
