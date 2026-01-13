# 命令记录 API

## 概述

CommandBuffer 用于记录渲染和计算命令，然后提交到 GPU 执行。VRHI 的命令缓冲设计简洁高效，支持命令记录和重放。

## CommandBuffer 类

```cpp
class CommandBuffer {
public:
    virtual ~CommandBuffer() = default;
    
    // CommandBuffer 不可拷贝，只能移动
    CommandBuffer(const CommandBuffer&) = delete;
    CommandBuffer& operator=(const CommandBuffer&) = delete;
    CommandBuffer(CommandBuffer&&) noexcept = default;
    CommandBuffer& operator=(CommandBuffer&&) noexcept = default;
    
    // 渲染通道
    virtual void BeginRenderPass(RenderPass* renderPass, 
                                Framebuffer* framebuffer = nullptr) = 0;
    virtual void EndRenderPass() = 0;
    
    // 管线绑定
    virtual void BindPipeline(Pipeline* pipeline) = 0;
    
    // 资源绑定
    virtual void BindVertexBuffer(uint32_t binding, Buffer* buffer, 
                                  size_t offset = 0) = 0;
    virtual void BindIndexBuffer(Buffer* buffer, IndexType type,
                                size_t offset = 0) = 0;
    virtual void BindUniformBuffer(uint32_t set, uint32_t binding,
                                   Buffer* buffer, size_t offset = 0,
                                   size_t range = 0) = 0;
    
    // 纹理绑定
    virtual void BindTexture(uint32_t set, uint32_t binding,
                            Texture* texture, Sampler* sampler = nullptr) = 0;
    
    // 视口和裁剪
    virtual void SetViewport(float x, float y, float width, float height,
                            float minDepth = 0.0f, float maxDepth = 1.0f) = 0;
    virtual void SetScissor(int32_t x, int32_t y,
                           uint32_t width, uint32_t height) = 0;
    
    // 绘制命令
    virtual void Draw(uint32_t vertexCount, uint32_t instanceCount = 1,
                     uint32_t firstVertex = 0, uint32_t firstInstance = 0) = 0;
    virtual void DrawIndexed(uint32_t indexCount, uint32_t instanceCount = 1,
                            uint32_t firstIndex = 0, int32_t vertexOffset = 0,
                            uint32_t firstInstance = 0) = 0;
    virtual void DrawIndirect(Buffer* buffer, size_t offset,
                             uint32_t drawCount, uint32_t stride) = 0;
    
    // 计算命令
    virtual void Dispatch(uint32_t groupCountX, uint32_t groupCountY,
                         uint32_t groupCountZ) = 0;
    virtual void DispatchIndirect(Buffer* buffer, size_t offset) = 0;
    
    // 资源操作
    virtual void CopyBuffer(Buffer* src, Buffer* dst,
                           size_t srcOffset, size_t dstOffset,
                           size_t size) = 0;
    virtual void CopyTexture(Texture* src, Texture* dst) = 0;
    virtual void CopyBufferToTexture(Buffer* src, Texture* dst) = 0;
    virtual void CopyTextureToBuffer(Texture* src, Buffer* dst) = 0;
    
    // 清除操作
    virtual void ClearColor(float r, float g, float b, float a) = 0;
    virtual void ClearDepth(float depth) = 0;
    virtual void ClearStencil(uint32_t stencil) = 0;
    
    // 同步
    virtual void Barrier(PipelineStage srcStage, PipelineStage dstStage,
                        std::span<MemoryBarrier> barriers) = 0;
    
    // 调试标记
    virtual void BeginDebugMarker(const char* name,
                                 float r = 1.0f, float g = 1.0f,
                                 float b = 1.0f, float a = 1.0f) = 0;
    virtual void EndDebugMarker() = 0;
    virtual void InsertDebugMarker(const char* name,
                                  float r = 1.0f, float g = 1.0f,
                                  float b = 1.0f, float a = 1.0f) = 0;
    
protected:
    CommandBuffer() = default;
};
```

## 枚举类型

### IndexType

```cpp
enum class IndexType {
    UInt16,
    UInt32,
};
```

### PipelineStage

```cpp
enum class PipelineStage : uint32_t {
    Top             = 1 << 0,
    DrawIndirect    = 1 << 1,
    VertexInput     = 1 << 2,
    VertexShader    = 1 << 3,
    FragmentShader  = 1 << 4,
    ColorOutput     = 1 << 5,
    ComputeShader   = 1 << 6,
    Transfer        = 1 << 7,
    Bottom          = 1 << 8,
    AllGraphics     = 1 << 9,
    AllCommands     = 1 << 10,
};
```

### MemoryBarrier

```cpp
struct MemoryBarrier {
    enum class AccessType : uint32_t {
        None            = 0,
        Read            = 1 << 0,
        Write           = 1 << 1,
        ShaderRead      = 1 << 2,
        ShaderWrite     = 1 << 3,
        TransferRead    = 1 << 4,
        TransferWrite   = 1 << 5,
    };
    
    AccessType srcAccess;
    AccessType dstAccess;
    
    // 可选：特定资源
    Buffer* buffer = nullptr;
    Texture* texture = nullptr;
};
```

## 使用示例

### 基本渲染

```cpp
auto cmd = device->CreateCommandBuffer();

// 开始渲染通道
cmd->BeginRenderPass(renderPass, framebuffer);

// 设置视口和裁剪
cmd->SetViewport(0, 0, 1920, 1080);
cmd->SetScissor(0, 0, 1920, 1080);

// 清除背景
cmd->ClearColor(0.2f, 0.2f, 0.2f, 1.0f);
cmd->ClearDepth(1.0f);

// 绑定管线
cmd->BindPipeline(pipeline.get());

// 绑定顶点缓冲
cmd->BindVertexBuffer(0, vertexBuffer.get());
cmd->BindIndexBuffer(indexBuffer.get(), IndexType::UInt32);

// 绑定纹理
cmd->BindTexture(0, 0, diffuseTexture.get(), sampler.get());

// 绘制
cmd->DrawIndexed(indexCount);

// 结束渲染通道
cmd->EndRenderPass();

// 提交
device->Submit(std::move(cmd));
```

### 计算着色器

```cpp
auto cmd = device->CreateCommandBuffer();

// 绑定计算管线
cmd->BindPipeline(computePipeline.get());

// 绑定存储缓冲
cmd->BindStorageBuffer(0, 0, inputBuffer.get());
cmd->BindStorageBuffer(0, 1, outputBuffer.get());

// 调度计算
uint32_t groupCountX = (dataSize + 255) / 256;
cmd->Dispatch(groupCountX, 1, 1);

// 添加屏障确保计算完成
MemoryBarrier barrier;
barrier.srcAccess = MemoryBarrier::AccessType::ShaderWrite;
barrier.dstAccess = MemoryBarrier::AccessType::ShaderRead;
cmd->Barrier(PipelineStage::ComputeShader, 
             PipelineStage::FragmentShader,
             {&barrier, 1});

device->Submit(std::move(cmd));
```

### 资源更新

```cpp
auto cmd = device->CreateCommandBuffer();

// 拷贝缓冲区
cmd->CopyBuffer(stagingBuffer.get(), gpuBuffer.get(), 
                0, 0, dataSize);

// 拷贝到纹理
cmd->CopyBufferToTexture(textureStaging.get(), texture.get());

// 生成 Mipmaps
texture->GenerateMipmaps(cmd.get());

device->Submit(std::move(cmd));
```

### 多重绘制

```cpp
auto cmd = device->CreateCommandBuffer();

cmd->BeginRenderPass(renderPass, framebuffer);
cmd->BindPipeline(pipeline.get());

// 绘制多个物体
for (const auto& object : objects) {
    // 更新 Uniform
    cmd->BindUniformBuffer(0, 0, object.uniformBuffer);
    
    // 绑定资源
    cmd->BindVertexBuffer(0, object.vertexBuffer);
    cmd->BindIndexBuffer(object.indexBuffer, IndexType::UInt32);
    cmd->BindTexture(0, 1, object.texture, sampler);
    
    // 绘制
    cmd->DrawIndexed(object.indexCount);
}

cmd->EndRenderPass();

device->Submit(std::move(cmd));
```

### 间接绘制

```cpp
// 准备间接绘制缓冲
struct DrawCommand {
    uint32_t vertexCount;
    uint32_t instanceCount;
    uint32_t firstVertex;
    uint32_t firstInstance;
};

std::vector<DrawCommand> commands = {
    {100, 1, 0, 0},
    {200, 1, 0, 0},
    {150, 1, 0, 0},
};

BufferDesc indirectDesc;
indirectDesc.size = commands.size() * sizeof(DrawCommand);
indirectDesc.usage = BufferUsage::Indirect;
indirectDesc.memoryAccess = MemoryAccess::CpuToGpu;
indirectDesc.initialData = commands.data();

auto indirectBuffer = device->CreateBuffer(indirectDesc).value();

// 使用间接绘制
auto cmd = device->CreateCommandBuffer();
cmd->BeginRenderPass(renderPass, framebuffer);
cmd->BindPipeline(pipeline.get());
cmd->BindVertexBuffer(0, vertexBuffer.get());

cmd->DrawIndirect(indirectBuffer.get(), 0, 
                  commands.size(), sizeof(DrawCommand));

cmd->EndRenderPass();
device->Submit(std::move(cmd));
```

### 调试标记

```cpp
auto cmd = device->CreateCommandBuffer();

cmd->BeginDebugMarker("Frame Rendering");

cmd->BeginRenderPass(renderPass, framebuffer);

cmd->BeginDebugMarker("Opaque Objects", 1.0f, 0.0f, 0.0f);
RenderOpaqueObjects(cmd.get());
cmd->EndDebugMarker();

cmd->BeginDebugMarker("Transparent Objects", 0.0f, 1.0f, 0.0f);
RenderTransparentObjects(cmd.get());
cmd->EndDebugMarker();

cmd->EndRenderPass();

cmd->EndDebugMarker();

device->Submit(std::move(cmd));
```

## 最佳实践

### 1. 命令缓冲重用

```cpp
// 创建命令缓冲池
std::vector<std::unique_ptr<CommandBuffer>> commandBuffers;

for (int i = 0; i < framesInFlight; ++i) {
    commandBuffers.push_back(device->CreateCommandBuffer());
}

// 每帧循环使用
uint32_t frameIndex = GetCurrentFrameIndex();
auto* cmd = commandBuffers[frameIndex].get();

// 重新记录命令（命令缓冲会自动重置）
cmd->BeginRenderPass(/*...*/);
// ...
```

### 2. 批处理状态更改

```cpp
// 按管线分组
cmd->BindPipeline(pipelineA);
for (auto& obj : objectsWithPipelineA) {
    cmd->BindVertexBuffer(0, obj.buffer);
    cmd->Draw(obj.count);
}

cmd->BindPipeline(pipelineB);
for (auto& obj : objectsWithPipelineB) {
    cmd->BindVertexBuffer(0, obj.buffer);
    cmd->Draw(obj.count);
}
```

### 3. 使用屏障同步

```cpp
// 确保计算完成后再读取
cmd->Dispatch(groupCount, 1, 1);

MemoryBarrier barrier;
barrier.srcAccess = MemoryBarrier::AccessType::ShaderWrite;
barrier.dstAccess = MemoryBarrier::AccessType::ShaderRead;
barrier.buffer = outputBuffer.get();

cmd->Barrier(PipelineStage::ComputeShader,
             PipelineStage::VertexShader,
             {&barrier, 1});
```

### 4. 合理使用调试标记

```cpp
#ifdef VRHI_DEBUG
    cmd->BeginDebugMarker("Complex Operation");
#endif
    
    // 操作...
    
#ifdef VRHI_DEBUG
    cmd->EndDebugMarker();
#endif
```

## 线程安全

- `CommandBuffer` 非线程安全
- 每个线程应使用独立的 `CommandBuffer`
- 提交时需要外部同步

```cpp
// 多线程命令记录
std::vector<std::unique_ptr<CommandBuffer>> cmds;
std::vector<std::thread> threads;

for (int i = 0; i < threadCount; ++i) {
    threads.emplace_back([&, i]() {
        auto cmd = device->CreateCommandBuffer();
        RecordCommands(cmd.get(), i);
        
        std::lock_guard lock(mutex);
        cmds.push_back(std::move(cmd));
    });
}

for (auto& thread : threads) {
    thread.join();
}

// 提交所有命令
device->Submit(cmds);
```

## 性能提示

1. **最小化绘制调用**: 使用实例化和间接绘制
2. **减少状态切换**: 按管线和材质排序
3. **批处理资源更新**: 一次更新多个资源
4. **使用异步计算**: 利用计算队列并行
5. **合理使用屏障**: 只在必要时同步

## 未来扩展

- 多队列支持（图形、计算、传输）
- 二级命令缓冲
- 条件渲染
- 变换反馈
