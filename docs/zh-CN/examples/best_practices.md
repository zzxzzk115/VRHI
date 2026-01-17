# VRHI 最佳实践

## 资源管理

### 1. 使用 RAII 和智能指针

✅ **推荐**:
```cpp
{
    auto buffer = device->CreateBuffer(desc).value();
    // 使用 buffer
} // 自动释放
```

❌ **避免**:
```cpp
Buffer* buffer = CreateBufferRaw(desc);
// 需要手动释放，容易忘记
delete buffer;
```

### 2. 批量创建资源

✅ **推荐**:
```cpp
std::vector<std::unique_ptr<Buffer>> buffers;
for (const auto& desc : bufferDescs) {
    buffers.push_back(device->CreateBuffer(desc).value());
}
```

### 3. 重用资源

```cpp
// 创建资源池
class BufferPool {
    std::vector<std::unique_ptr<Buffer>> buffers;
    size_t nextIndex = 0;
    
public:
    Buffer* GetBuffer() {
        if (nextIndex < buffers.size()) {
            return buffers[nextIndex++].get();
        }
        buffers.push_back(device->CreateBuffer(desc).value());
        return buffers.back().get();
    }
    
    void Reset() { nextIndex = 0; }
};

BufferPool pool;
// 每帧重置
pool.Reset();
```

## 性能优化

### 1. 最小化状态切换

✅ **推荐**:
```cpp
// 按管线分组绘制调用
cmd->BindPipeline(pipelineA);
for (auto& obj : objectsWithPipelineA) {
    cmd->BindVertexBuffer(obj.buffer);
    cmd->Draw(obj.vertexCount);
}

cmd->BindPipeline(pipelineB);
for (auto& obj : objectsWithPipelineB) {
    cmd->BindVertexBuffer(obj.buffer);
    cmd->Draw(obj.vertexCount);
}
```

❌ **避免**:
```cpp
// 频繁切换管线
for (auto& obj : allObjects) {
    cmd->BindPipeline(obj.pipeline);  // 可能频繁切换
    cmd->BindVertexBuffer(obj.buffer);
    cmd->Draw(obj.vertexCount);
}
```

### 2. 使用正确的内存访问模式

```cpp
// 静态数据 - GpuOnly，使用临时缓冲上传
BufferDesc desc;
desc.memoryAccess = MemoryAccess::GpuOnly;
auto staticBuffer = device->CreateBuffer(desc).value();

// 临时缓冲
BufferDesc stagingDesc;
stagingDesc.memoryAccess = MemoryAccess::CpuToGpu;
stagingDesc.usage = BufferUsage::TransferSrc;
auto staging = device->CreateBuffer(stagingDesc).value();

staging->Update(data, size);
CopyBuffer(cmd, staging.get(), staticBuffer.get(), size);

// 动态数据 - CpuToGpu，每帧更新
BufferDesc dynamicDesc;
dynamicDesc.memoryAccess = MemoryAccess::CpuToGpu;
auto dynamicBuffer = device->CreateBuffer(dynamicDesc).value();
dynamicBuffer->Update(frameData, frameDataSize);
```

### 3. 批处理绘制调用

```cpp
// 使用实例化渲染
cmd->BindPipeline(pipeline);
cmd->BindVertexBuffer(0, vertexBuffer);
cmd->BindVertexBuffer(1, instanceBuffer);  // 实例数据
cmd->DrawInstanced(vertexCount, instanceCount);

// 使用间接绘制
if (device->IsFeatureSupported(Feature::MultiDrawIndirect)) {
    cmd->MultiDrawIndirect(indirectBuffer, drawCount);
}
```

### 4. 异步资源加载

```cpp
// 在后台线程加载资源
std::future<std::unique_ptr<Texture>> LoadTextureAsync(
    Device* device, const std::string& path) {
    
    return std::async(std::launch::async, [device, path]() {
        auto imageData = ReadImageFile(path);
        
        TextureDesc desc;
        desc.width = imageData.width;
        desc.height = imageData.height;
        desc.format = TextureFormat::RGBA8_SRGB;
        
        return device->CreateTexture(desc).value();
    });
}

// 使用
auto textureFuture = LoadTextureAsync(device.get(), "texture.png");
// 做其他工作...
auto texture = textureFuture.get();
```

## 错误处理

### 1. 始终检查返回值

✅ **推荐**:
```cpp
auto result = device->CreateBuffer(desc);
if (!result) {
    std::cerr << "Failed to create buffer: " 
              << result.error().message << "\n";
    return;
}
auto buffer = std::move(*result);
```

❌ **避免**:
```cpp
auto buffer = device->CreateBuffer(desc).value();  // 可能崩溃
```

### 2. 优雅降级

```cpp
// 尝试使用高级特性，失败则降级
if (device->IsFeatureSupported(Feature::Compute)) {
    // 使用计算着色器加速
    RunComputeShader();
} else {
    // 降级到 CPU 计算或简化算法
    RunCPUFallback();
}
```

### 3. 验证层

```cpp
// 开发时启用
#ifdef DEBUG
config.enableValidation = true;
config.logLevel = LogLevel::Debug;
#else
config.enableValidation = false;
config.logLevel = LogLevel::Warning;
#endif
```

## 多线程

### 1. 每线程命令缓冲

```cpp
// 每个线程使用独立的命令缓冲
void RenderThread(Device* device, int threadId) {
    auto cmd = device->CreateCommandBuffer();
    
    // 记录命令
    cmd->Draw(/*...*/);
    
    // 提交（需要同步）
    std::lock_guard lock(submitMutex);
    device->Submit(std::move(cmd));
}

// 启动多个渲染线程
std::vector<std::thread> threads;
for (int i = 0; i < threadCount; ++i) {
    threads.emplace_back(RenderThread, device.get(), i);
}

for (auto& thread : threads) {
    thread.join();
}
```

### 2. 资源创建线程安全

```cpp
// 资源创建是线程安全的
std::vector<std::future<std::unique_ptr<Texture>>> futures;

for (const auto& path : texturePaths) {
    futures.push_back(std::async([device, path]() {
        return LoadTexture(device, path);
    }));
}

std::vector<std::unique_ptr<Texture>> textures;
for (auto& future : futures) {
    textures.push_back(future.get());
}
```

## 内存管理

### 1. 监控内存使用

```cpp
auto props = device->GetProperties();
std::cout << "Total memory: " << props.totalMemory / (1024*1024) << " MB\n";
std::cout << "Available: " << props.availableMemory / (1024*1024) << " MB\n";

// 定期检查
if (props.availableMemory < threshold) {
    // 释放缓存资源
    ClearResourceCache();
}
```

### 2. 延迟删除资源

```cpp
class ResourceDeleter {
    struct PendingDelete {
        uint64_t frame;
        std::function<void()> deleter;
    };
    
    std::vector<PendingDelete> pending;
    uint64_t currentFrame = 0;
    
public:
    template<typename T>
    void Delete(std::unique_ptr<T> resource) {
        pending.push_back({
            currentFrame + kFramesInFlight,
            [r = std::move(resource)]() {}  // RAII 删除
        });
    }
    
    void Update() {
        currentFrame++;
        std::erase_if(pending, [this](const auto& item) {
            if (item.frame <= currentFrame) {
                item.deleter();
                return true;
            }
            return false;
        });
    }
};
```

## 调试

### 1. 使用调试名称

```cpp
BufferDesc desc;
desc.debugName = "Vertex Buffer - Model XYZ";
auto buffer = device->CreateBuffer(desc).value();

TextureDesc texDesc;
texDesc.debugName = "Diffuse - Character Texture";
auto texture = device->CreateTexture(texDesc).value();
```

### 2. 使用调试标记

```cpp
if (device->IsFeatureSupported(Feature::DebugMarkers)) {
    cmd->BeginDebugMarker("Render Scene");
    
    cmd->BeginDebugMarker("Opaque Pass");
    RenderOpaqueObjects(cmd);
    cmd->EndDebugMarker();
    
    cmd->BeginDebugMarker("Transparent Pass");
    RenderTransparentObjects(cmd);
    cmd->EndDebugMarker();
    
    cmd->EndDebugMarker();
}
```

### 3. 捕获帧

```cpp
// 使用 RenderDoc 或平台工具
// Vulkan: 使用 RenderDoc
// OpenGL: 使用 apitrace 或 RenderDoc
```

## 跨平台开发

### 1. 测试多个后端

```cpp
// 测试所有可用后端
for (const auto& backendInfo : VRHI::EnumerateBackends()) {
    DeviceConfig config;
    config.preferredBackend = backendInfo.type;
    
    auto device = VRHI::CreateDevice(config);
    if (device) {
        RunTests(device.value().get());
    }
}
```

### 2. 处理平台差异

```cpp
#ifdef VRHI_PLATFORM_MOBILE
    // 移动平台优化
    config.features.optional = {Feature::Compute};  // 可选
#else
    // 桌面平台
    config.features.required = {Feature::Compute};  // 必需
#endif
```

### 3. 纹理格式兼容性

```cpp
// 检查格式支持
TextureFormat preferredFormat = TextureFormat::ASTC_4x4;
if (!IsFormatSupported(device, preferredFormat)) {
    // 降级到通用格式
    preferredFormat = TextureFormat::RGBA8_UNorm;
}
```

## 着色器管理

### 1. 着色器编译缓存

```cpp
class ShaderCache {
    std::unordered_map<std::string, std::unique_ptr<Shader>> cache;
    
public:
    Shader* GetShader(Device* device, const std::string& path) {
        auto it = cache.find(path);
        if (it != cache.end()) {
            return it->second.get();
        }
        
        auto shader = CompileShader(device, path);
        auto* ptr = shader.get();
        cache[path] = std::move(shader);
        return ptr;
    }
};
```

### 2. 着色器变体

```cpp
// 使用宏定义创建着色器变体
ShaderDesc desc;
desc.code = shaderSource;
desc.codeSize = strlen(shaderSource);

// 基础版本
auto basicShader = device->CreateShader(desc).value();

// 高质量版本
desc.defines = {{"HIGH_QUALITY", "1"}};
auto hqShader = device->CreateShader(desc).value();
```

## 总结

1. **使用 RAII**: 自动管理资源生命周期
2. **检查错误**: 始终检查 `std::expected` 返回值
3. **优化性能**: 减少状态切换，批处理绘制
4. **多线程**: 每线程独立命令缓冲
5. **调试**: 使用调试名称和标记
6. **跨平台**: 测试多个后端，处理平台差异
7. **内存管理**: 监控使用，延迟删除
8. **着色器**: 缓存和管理变体

遵循这些最佳实践，可以构建高效、稳定、可维护的 VRHI 应用程序。
