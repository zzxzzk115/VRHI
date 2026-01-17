# 设备管理 API

## 概述

设备 (Device) 是 VRHI 的核心对象，代表一个图形硬件设备。本文档详细描述设备的创建、管理和使用。

## 设备创建

### CreateDevice

```cpp
std::expected<std::unique_ptr<Device>, Error> 
CreateDevice(const DeviceConfig& config = {});
```

创建渲染设备实例。

**参数**:
- `config`: 设备配置（可选，使用默认配置）

**返回值**:
- 成功: `std::expected` 包含 `Device` 的 `unique_ptr`
- 失败: `std::expected` 包含 `Error` 对象

**示例**:
```cpp
// 使用默认配置（自动选择最佳后端）
auto device = VRHI::CreateDevice().value();

// 自定义配置
VRHI::DeviceConfig config;
config.preferredBackend = VRHI::BackendType::Vulkan;
config.enableValidation = true;
config.width = 1920;
config.height = 1080;

auto result = VRHI::CreateDevice(config);
if (result) {
    auto device = std::move(*result);
} else {
    std::cerr << result.error().message << "\n";
}
```

## DeviceConfig 结构

### 配置选项

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
    void* windowHandle = nullptr;
    uint32_t width = 1280;
    uint32_t height = 720;
    
    // 交换链配置
    bool vsync = true;
    uint32_t backBufferCount = 2;
    
    // 日志级别
    LogLevel logLevel = LogLevel::Info;
};
```

### 配置示例

#### 游戏应用配置
```cpp
VRHI::DeviceConfig config;
config.preferredBackend = VRHI::BackendType::Auto;
config.features.required = {
    VRHI::Feature::Texture2D,
    VRHI::Feature::MultiDrawIndirect,
};
config.features.optional = {
    VRHI::Feature::Compute,
    VRHI::Feature::RayTracing,
};
config.enableValidation = false;  // 生产环境关闭
config.vsync = true;               // 游戏通常启用垂直同步
config.backBufferCount = 3;        // 三重缓冲
```

#### 工具应用配置
```cpp
VRHI::DeviceConfig config;
config.preferredBackend = VRHI::BackendType::Auto;
config.enableValidation = true;    // 工具开发时启用
config.enableDebugMarkers = true;
config.logLevel = VRHI::LogLevel::Debug;
config.vsync = false;              // 工具可能不需要
```

#### 移动应用配置
```cpp
VRHI::DeviceConfig config;
config.preferredBackend = VRHI::BackendType::Auto;
config.features.required = {
    VRHI::Feature::Texture2D,
};
// 移动平台降低要求
config.features.optional = {
    VRHI::Feature::Compute,
};
config.vsync = true;
config.backBufferCount = 2;  // 移动平台通常使用双缓冲
```

## Device 接口详解

### 信息查询

#### GetBackendType
```cpp
BackendType GetBackendType() const noexcept;
```

获取当前使用的后端类型。

```cpp
auto backend = device->GetBackendType();
switch (backend) {
case VRHI::BackendType::Vulkan:
    std::cout << "Using Vulkan\n";
    break;
case VRHI::BackendType::OpenGL46:
    std::cout << "Using OpenGL 4.6\n";
    break;
// ...
}
```

#### GetBackendInfo
```cpp
BackendInfo GetBackendInfo() const;
```

获取详细的后端信息，包括评分细节。

```cpp
auto info = device->GetBackendInfo();
std::cout << "Backend: " << info.name << "\n";
std::cout << "Version: " << info.version << "\n";
std::cout << "Device: " << info.deviceName << "\n";
std::cout << "Score: " << info.score << "\n";

// 查看评分细节
for (const auto& detail : info.scoreDetails) {
    std::cout << "  " << detail.category << ": " 
              << detail.score << " × " << detail.weight << "\n";
}
```

#### GetProperties
```cpp
const DeviceProperties& GetProperties() const noexcept;
```

获取设备属性。

```cpp
auto props = device->GetProperties();
std::cout << "Device: " << props.deviceName << "\n";
std::cout << "Vendor: " << props.vendorName << "\n";
std::cout << "Memory: " << props.totalMemory / (1024*1024) << " MB\n";
std::cout << "Compute Units: " << props.computeUnits << "\n";
```

### 特性查询

#### GetFeatures
```cpp
const FeatureSet& GetFeatures() const noexcept;
```

获取完整的特性集。

```cpp
const auto& features = device->GetFeatures();

// 查询纹理特性
std::cout << "Max Texture Size: " 
          << features.texture.maxTextureSize << "\n";
std::cout << "Max Anisotropy: " 
          << features.texture.maxAnisotropy << "\n";

// 查询计算特性
if (features.compute.computeShader) {
    std::cout << "Max Work Group Size: "
              << features.compute.maxWorkGroupSizeX << " × "
              << features.compute.maxWorkGroupSizeY << " × "
              << features.compute.maxWorkGroupSizeZ << "\n";
}
```

#### IsFeatureSupported
```cpp
bool IsFeatureSupported(Feature feature) const noexcept;
```

检查单个特性是否支持。

```cpp
if (device->IsFeatureSupported(VRHI::Feature::Compute)) {
    // 使用计算着色器路径
    InitComputePipeline();
} else {
    // 使用传统渲染路径
    InitGraphicsPipeline();
}

// 检查多个特性
std::vector<VRHI::Feature> requiredFeatures = {
    VRHI::Feature::Texture3D,
    VRHI::Feature::MultiDrawIndirect,
};

bool allSupported = std::all_of(
    requiredFeatures.begin(),
    requiredFeatures.end(),
    [&](auto feature) {
        return device->IsFeatureSupported(feature);
    }
);
```

### 资源创建

所有资源创建方法返回 `std::expected`，便于错误处理。

#### CreateBuffer
```cpp
std::expected<std::unique_ptr<Buffer>, Error>
CreateBuffer(const BufferDesc& desc);
```

详见 [资源管理 API](resources.md#buffer-缓冲区)。

#### CreateTexture
```cpp
std::expected<std::unique_ptr<Texture>, Error>
CreateTexture(const TextureDesc& desc);
```

详见 [资源管理 API](resources.md#texture-纹理)。

#### CreateShader
```cpp
std::expected<std::unique_ptr<Shader>, Error>
CreateShader(const ShaderDesc& desc);
```

#### CreatePipeline
```cpp
std::expected<std::unique_ptr<Pipeline>, Error>
CreatePipeline(const PipelineDesc& desc);
```

### 命令执行

#### CreateCommandBuffer
```cpp
std::unique_ptr<CommandBuffer> CreateCommandBuffer();
```

创建命令缓冲区。命令缓冲区用于记录渲染命令。

```cpp
auto cmd = device->CreateCommandBuffer();
cmd->BeginRenderPass(renderPass);
cmd->Draw(3);
cmd->EndRenderPass();
device->Submit(std::move(cmd));
```

#### Submit
```cpp
void Submit(std::unique_ptr<CommandBuffer> cmd);
void Submit(std::span<std::unique_ptr<CommandBuffer>> cmds);
```

提交命令缓冲区到 GPU 执行。

```cpp
// 提交单个命令缓冲
device->Submit(std::move(cmd));

// 提交多个命令缓冲
std::vector<std::unique_ptr<CommandBuffer>> cmds;
cmds.push_back(device->CreateCommandBuffer());
cmds.push_back(device->CreateCommandBuffer());
// 记录命令...
device->Submit(cmds);
```

#### WaitIdle
```cpp
void WaitIdle();
```

等待设备完成所有待处理的工作。在销毁资源前必须调用。

```cpp
// 渲染循环结束后
device->WaitIdle();

// 销毁资源前
device->WaitIdle();
resources.clear();
```

#### Flush
```cpp
void Flush();
```

刷新待处理的命令，但不等待完成。

```cpp
// 确保命令被提交到 GPU
device->Flush();

// 可以继续记录新命令
auto cmd = device->CreateCommandBuffer();
```

### 同步对象

#### CreateFence
```cpp
std::unique_ptr<Fence> CreateFence(bool signaled = false);
```

创建围栏对象，用于 CPU-GPU 同步。

```cpp
auto fence = device->CreateFence();

// 提交命令并关联围栏
device->Submit(std::move(cmd), fence.get());

// 等待命令完成
fence->Wait();

// 检查状态
if (fence->IsSignaled()) {
    std::cout << "Commands completed\n";
}
```

#### CreateSemaphore
```cpp
std::unique_ptr<Semaphore> CreateSemaphore();
```

创建信号量，用于 GPU-GPU 同步。

```cpp
auto semaphore = device->CreateSemaphore();

// 在提交时使用信号量同步
device->Submit(cmd1, nullptr, semaphore.get());  // 信号
device->Submit(cmd2, semaphore.get(), nullptr);  // 等待
```

### 交换链管理

#### GetSwapChain
```cpp
SwapChain* GetSwapChain() noexcept;
```

获取交换链对象。

```cpp
auto* swapChain = device->GetSwapChain();
auto* currentImage = swapChain->GetCurrentImage();
```

#### Present
```cpp
void Present();
```

呈现当前帧到屏幕。

```cpp
// 渲染循环
while (!WindowShouldClose()) {
    auto cmd = device->CreateCommandBuffer();
    // 记录渲染命令...
    device->Submit(std::move(cmd));
    device->Present();  // 显示到屏幕
}
```

#### Resize
```cpp
void Resize(uint32_t width, uint32_t height);
```

调整交换链大小。

```cpp
// 窗口大小改变时
void OnWindowResize(uint32_t newWidth, uint32_t newHeight) {
    device->WaitIdle();  // 等待当前帧完成
    device->Resize(newWidth, newHeight);
}
```

## 设备生命周期管理

### 初始化模式

```cpp
class Renderer {
    std::unique_ptr<VRHI::Device> device;
    
public:
    bool Initialize() {
        auto result = VRHI::CreateDevice();
        if (!result) {
            std::cerr << "Failed: " << result.error().message << "\n";
            return false;
        }
        
        device = std::move(*result);
        
        // 创建资源...
        return true;
    }
    
    void Shutdown() {
        // 等待所有工作完成
        device->WaitIdle();
        
        // 清理资源
        resources.clear();
        
        // device 自动销毁（RAII）
    }
};
```

### 错误恢复

```cpp
void HandleDeviceLost() {
    // 检测设备丢失
    if (device->IsDeviceLost()) {
        std::cout << "Device lost, recreating...\n";
        
        // 保存配置
        auto config = savedConfig;
        
        // 销毁旧设备
        device.reset();
        
        // 重新创建
        auto result = VRHI::CreateDevice(config);
        if (result) {
            device = std::move(*result);
            ReloadResources();
        }
    }
}
```

## 多设备支持

```cpp
// 枚举所有物理设备
auto devices = VRHI::EnumeratePhysicalDevices();

for (const auto& deviceInfo : devices) {
    std::cout << "Device: " << deviceInfo.name << "\n";
    std::cout << "Memory: " << deviceInfo.memory / (1024*1024) << " MB\n";
}

// 创建多个逻辑设备（高级用法）
std::vector<std::unique_ptr<VRHI::Device>> gpuDevices;

for (const auto& deviceInfo : devices) {
    VRHI::DeviceConfig config;
    config.physicalDevice = deviceInfo.id;
    
    auto device = VRHI::CreateDevice(config);
    if (device) {
        gpuDevices.push_back(std::move(*device));
    }
}
```

## 性能监控

```cpp
class DeviceMonitor {
    VRHI::Device* device;
    
public:
    void Update() {
        auto props = device->GetProperties();
        
        // 内存使用
        uint64_t usedMemory = props.totalMemory - props.availableMemory;
        float memoryUsage = (float)usedMemory / props.totalMemory * 100.0f;
        
        std::cout << "Memory Usage: " << memoryUsage << "%\n";
        
        // 检查内存压力
        if (memoryUsage > 90.0f) {
            std::cout << "Warning: High memory usage!\n";
            // 清理缓存、降低质量等
        }
        
        // 查询性能计数器（如果支持）
        if (device->IsFeatureSupported(VRHI::Feature::PerformanceQueries)) {
            auto stats = device->GetPerformanceStats();
            std::cout << "Frame Time: " << stats.frameTime << " ms\n";
            std::cout << "Draw Calls: " << stats.drawCalls << "\n";
        }
    }
};
```

## 最佳实践

### 1. 检查返回值

```cpp
// ✅ Good - always check return values
auto result = VRHI::CreateDevice(config);
if (!result) {
    HandleError(result.error());
    return;
}
auto device = std::move(*result);

// ❌ Avoid - may throw if creation fails
auto device = VRHI::CreateDevice(config).value();
```

### 2. 在销毁前等待

```cpp
// ✅ Good - wait before cleanup
device->WaitIdle();
resources.clear();
device.reset();

// ❌ Avoid - resources may still be in use
resources.clear();
```

### 3. 重用命令缓冲

```cpp
// ✅ Good - reuse command buffers
std::vector<std::unique_ptr<CommandBuffer>> cmdPool;
for (int i = 0; i < kFramesInFlight; ++i) {
    cmdPool.push_back(device->CreateCommandBuffer());
}

// ❌ Avoid - allocating every frame
auto cmd = device->CreateCommandBuffer();
```

### 4. 监控资源

```cpp
// 定期检查
void MonitorResources() {
    auto props = device->GetProperties();
    if (props.availableMemory < threshold) {
        CleanupUnusedResources();
    }
}
```

## 调试技巧

### 启用验证层

```cpp
#ifdef _DEBUG
config.enableValidation = true;
config.enableDebugMarkers = true;
config.logLevel = VRHI::LogLevel::Debug;
#endif
```

### 日志回调

```cpp
VRHI::SetLogCallback([](LogLevel level, const char* message) {
    switch (level) {
    case LogLevel::Error:
        std::cerr << "[ERROR] " << message << "\n";
        break;
    case LogLevel::Warning:
        std::cout << "[WARN] " << message << "\n";
        break;
    case LogLevel::Info:
        std::cout << "[INFO] " << message << "\n";
        break;
    }
});
```

## 线程安全

- **CreateDevice**: 线程安全
- **Device::Create***: 线程安全（资源创建）
- **Device::Submit**: 需要外部同步
- **Device::WaitIdle**: 线程安全
- **Device::Present**: 需要外部同步

```cpp
// 多线程资源创建（安全）
std::vector<std::future<std::unique_ptr<Buffer>>> futures;
for (int i = 0; i < count; ++i) {
    futures.push_back(std::async([&, i]() {
        return device->CreateBuffer(descs[i]).value();
    }));
}

// 提交需要同步
std::mutex submitMutex;
void ThreadSubmit(std::unique_ptr<CommandBuffer> cmd) {
    std::lock_guard lock(submitMutex);
    device->Submit(std::move(cmd));
}
```
