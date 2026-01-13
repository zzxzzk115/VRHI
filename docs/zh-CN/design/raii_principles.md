# RAII 设计原则

## 概述

VRHI 严格遵循 RAII (Resource Acquisition Is Initialization) 原则，确保资源的正确管理和自动释放。这是现代 C++ 的核心设计模式，可以有效避免内存泄漏和资源泄漏。

## RAII 核心概念

**RAII** 的核心思想：
- **获取即初始化**: 资源在对象构造时获取
- **释放即销毁**: 资源在对象析构时释放
- **作用域绑定**: 资源生命周期与对象作用域绑定
- **异常安全**: 即使发生异常也能正确释放资源

## VRHI 中的 RAII 实践

### 1. 智能指针管理

VRHI 使用智能指针自动管理对象生命周期：

```cpp
// 使用 unique_ptr 管理独占资源
std::unique_ptr<Device> device = VRHI::CreateDevice();
// device 离开作用域时自动销毁

// 使用 shared_ptr 管理共享资源
std::shared_ptr<Texture> texture = device->CreateTexture(desc);
// 最后一个引用消失时自动销毁
```

### 2. 资源包装类

所有 GPU 资源都封装在 RAII 类中：

```cpp
class Buffer {
public:
    // 构造时创建资源
    Buffer(Device* device, const BufferDesc& desc) {
        // 创建底层缓冲区
        CreateBackendBuffer(desc);
    }
    
    // 析构时释放资源
    ~Buffer() {
        // 自动释放底层缓冲区
        DestroyBackendBuffer();
    }
    
    // 禁止拷贝，允许移动
    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;
    Buffer(Buffer&&) noexcept = default;
    Buffer& operator=(Buffer&&) noexcept = default;
    
private:
    void* backendHandle = nullptr;
};
```

### 3. 命令缓冲区作用域

命令记录使用作用域保护：

```cpp
void RenderFrame() {
    auto cmd = device->BeginCommandBuffer();
    
    {
        // 开始渲染通道
        auto renderPass = cmd->BeginRenderPass(renderTarget);
        
        // 记录命令
        cmd->BindPipeline(pipeline);
        cmd->Draw(vertexCount);
        
        // renderPass 析构时自动结束渲染通道
    }
    
    // 提交命令
    device->Submit(std::move(cmd));
}
```

### 4. 映射内存保护

内存映射使用 RAII 保证正确解除映射：

```cpp
class MappedBuffer {
public:
    MappedBuffer(Buffer* buffer) : buffer_(buffer) {
        data_ = buffer->Map();
    }
    
    ~MappedBuffer() {
        buffer_->Unmap();
    }
    
    void* GetData() { return data_; }
    
private:
    Buffer* buffer_;
    void* data_;
};

// 使用
{
    MappedBuffer mapped(buffer.get());
    memcpy(mapped.GetData(), data, size);
} // 自动解除映射
```

## C++23 特性应用

### 1. std::expected 错误处理

使用 `std::expected` 替代异常，实现 RAII 友好的错误处理：

```cpp
#include <expected>

// 返回结果或错误
std::expected<std::unique_ptr<Device>, Error> CreateDevice(const Config& config) {
    if (!ValidateConfig(config)) {
        return std::unexpected(Error::InvalidConfig);
    }
    
    auto device = std::make_unique<Device>(config);
    if (!device->Initialize()) {
        return std::unexpected(Error::InitializationFailed);
    }
    
    return device;
}

// 使用
auto result = CreateDevice(config);
if (result) {
    auto device = std::move(*result);
    // 使用 device
} else {
    // 处理错误
    std::cerr << "Error: " << result.error() << "\n";
}
```

### 2. 移动语义和完美转发

充分利用 C++23 的移动语义：

```cpp
class Texture {
public:
    // 移动构造
    Texture(Texture&& other) noexcept
        : handle_(std::exchange(other.handle_, nullptr))
        , size_(std::exchange(other.size_, 0))
    {}
    
    // 移动赋值
    Texture& operator=(Texture&& other) noexcept {
        if (this != &other) {
            // 释放当前资源
            Release();
            
            // 移动资源
            handle_ = std::exchange(other.handle_, nullptr);
            size_ = std::exchange(other.size_, 0);
        }
        return *this;
    }
    
private:
    void Release() {
        if (handle_) {
            DestroyTexture(handle_);
            handle_ = nullptr;
        }
    }
    
    void* handle_ = nullptr;
    size_t size_ = 0;
};
```

### 3. 概念和约束

使用 concepts 确保类型安全：

```cpp
#include <concepts>

// 定义资源概念
template<typename T>
concept Resource = requires(T t) {
    { t.IsValid() } -> std::same_as<bool>;
    { t.Release() } -> std::same_as<void>;
};

// 通用资源管理器
template<Resource T>
class ResourceHandle {
public:
    explicit ResourceHandle(T resource) : resource_(std::move(resource)) {}
    
    ~ResourceHandle() {
        if (resource_.IsValid()) {
            resource_.Release();
        }
    }
    
    ResourceHandle(const ResourceHandle&) = delete;
    ResourceHandle& operator=(const ResourceHandle&) = delete;
    ResourceHandle(ResourceHandle&&) noexcept = default;
    ResourceHandle& operator=(ResourceHandle&&) noexcept = default;
    
    T* operator->() { return &resource_; }
    const T* operator->() const { return &resource_; }
    
private:
    T resource_;
};
```

### 4. 范围 (Ranges) 和视图

使用 C++23 ranges 简化资源管理：

```cpp
#include <ranges>

// 管理资源集合
class ResourcePool {
public:
    // 使用 ranges 过滤活跃资源
    auto GetActiveResources() {
        return resources_ 
            | std::views::filter([](const auto& res) { 
                return res->IsActive(); 
              });
    }
    
    // 清理未使用资源
    void CleanupUnused() {
        std::erase_if(resources_, [](const auto& res) {
            return !res->IsActive();
        });
    }
    
private:
    std::vector<std::unique_ptr<Resource>> resources_;
};
```

### 5. 协程支持

使用 C++23 协程实现异步资源加载：

```cpp
#include <coroutine>

// 异步加载纹理
std::future<std::unique_ptr<Texture>> LoadTextureAsync(const std::string& path) {
    co_return co_await AsyncLoad([path](Device* device) {
        auto data = ReadFile(path);
        return device->CreateTexture(data);
    });
}

// 使用
auto textureFuture = LoadTextureAsync("texture.png");
// ... 做其他工作 ...
auto texture = textureFuture.get(); // 自动管理生命周期
```

## 作用域管理模式

### 1. 作用域守卫

```cpp
class ScopeGuard {
public:
    template<typename Func>
    explicit ScopeGuard(Func&& func) : func_(std::forward<Func>(func)) {}
    
    ~ScopeGuard() {
        if (!dismissed_) {
            func_();
        }
    }
    
    void Dismiss() { dismissed_ = true; }
    
    ScopeGuard(const ScopeGuard&) = delete;
    ScopeGuard& operator=(const ScopeGuard&) = delete;
    
private:
    std::function<void()> func_;
    bool dismissed_ = false;
};

// 使用
void RenderWithCleanup() {
    auto texture = device->CreateTempTexture();
    ScopeGuard cleanup([&] { device->DestroyTexture(texture); });
    
    // 使用 texture
    Render(texture);
    
    // texture 自动清理
}
```

### 2. 栈分配器

```cpp
class StackAllocator {
public:
    explicit StackAllocator(size_t size)
        : buffer_(std::make_unique<uint8_t[]>(size))
        , capacity_(size)
        , offset_(0)
    {}
    
    // 分配内存
    void* Allocate(size_t size, size_t alignment = alignof(std::max_align_t)) {
        size_t aligned = AlignUp(offset_, alignment);
        if (aligned + size > capacity_) {
            return nullptr;
        }
        
        void* ptr = buffer_.get() + aligned;
        offset_ = aligned + size;
        return ptr;
    }
    
    // 重置分配器（不释放内存）
    void Reset() {
        offset_ = 0;
    }
    
    // RAII 标记
    class Marker {
    public:
        Marker(StackAllocator* allocator)
            : allocator_(allocator)
            , savedOffset_(allocator->offset_)
        {}
        
        ~Marker() {
            allocator_->offset_ = savedOffset_;
        }
        
    private:
        StackAllocator* allocator_;
        size_t savedOffset_;
    };
    
private:
    std::unique_ptr<uint8_t[]> buffer_;
    size_t capacity_;
    size_t offset_;
};

// 使用
{
    StackAllocator::Marker marker(&allocator);
    void* temp = allocator.Allocate(1024);
    // 使用临时内存
} // 自动恢复分配器状态
```

## 资源生命周期管理

### 1. 延迟销毁

```cpp
class DeferredDeleter {
public:
    template<typename T>
    void Delete(std::unique_ptr<T> resource) {
        // 标记当前帧
        uint64_t frameId = GetCurrentFrameId();
        
        // 延迟删除（等待 GPU 完成使用）
        pendingDeletions_.emplace_back(
            frameId + kFramesInFlight,
            [res = std::move(resource)]() mutable {
                res.reset();  // RAII 销毁
            }
        );
    }
    
    void ProcessDeletions() {
        uint64_t currentFrame = GetCurrentFrameId();
        
        // 删除已完成的资源
        std::erase_if(pendingDeletions_, [currentFrame](const auto& item) {
            if (item.first <= currentFrame) {
                item.second();  // 执行删除
                return true;
            }
            return false;
        });
    }
    
private:
    static constexpr int kFramesInFlight = 3;
    std::vector<std::pair<uint64_t, std::function<void()>>> pendingDeletions_;
};
```

### 2. 引用计数

```cpp
template<typename T>
class RefCounted {
public:
    template<typename... Args>
    static std::shared_ptr<T> Create(Args&&... args) {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }
    
protected:
    RefCounted() = default;
    virtual ~RefCounted() = default;
};

// 使用
class Shader : public RefCounted<Shader> {
    // ...
};

auto shader = Shader::Create(source);  // 自动引用计数
```

### 3. 资源池

```cpp
template<typename T>
class ObjectPool {
public:
    class PooledObject {
    public:
        PooledObject(ObjectPool* pool, std::unique_ptr<T> obj)
            : pool_(pool), obj_(std::move(obj))
        {}
        
        ~PooledObject() {
            if (obj_) {
                pool_->Return(std::move(obj_));
            }
        }
        
        T* operator->() { return obj_.get(); }
        T& operator*() { return *obj_; }
        
    private:
        ObjectPool* pool_;
        std::unique_ptr<T> obj_;
    };
    
    PooledObject Acquire() {
        std::unique_lock lock(mutex_);
        
        if (!pool_.empty()) {
            auto obj = std::move(pool_.back());
            pool_.pop_back();
            return PooledObject(this, std::move(obj));
        }
        
        return PooledObject(this, std::make_unique<T>());
    }
    
private:
    void Return(std::unique_ptr<T> obj) {
        std::unique_lock lock(mutex_);
        pool_.push_back(std::move(obj));
    }
    
    std::vector<std::unique_ptr<T>> pool_;
    std::mutex mutex_;
};

// 使用
ObjectPool<CommandBuffer> cmdPool;
{
    auto cmd = cmdPool.Acquire();
    cmd->Begin();
    // 使用命令缓冲
} // 自动归还池中
```

## 异常安全

VRHI 的 RAII 设计保证异常安全：

```cpp
void RenderScene() {
    auto cmd = device->BeginCommandBuffer();
    
    try {
        // 加载资源
        auto texture = LoadTexture("texture.png");  // 可能抛异常
        auto shader = CompileShader(source);         // 可能抛异常
        
        // 渲染
        cmd->BindTexture(texture);
        cmd->BindShader(shader);
        cmd->Draw();
        
        device->Submit(std::move(cmd));
    } catch (...) {
        // 即使异常，所有 RAII 对象都会正确析构
        // texture, shader, cmd 等都会自动清理
        throw;
    }
}
```

## 最佳实践

### 1. 优先使用智能指针

```cpp
// 好
std::unique_ptr<Buffer> buffer = device->CreateBuffer(desc);

// 避免
Buffer* buffer = new Buffer(device, desc);  // 需要手动 delete
```

### 2. 使用工厂函数

```cpp
// 好
class Texture {
public:
    static std::expected<std::unique_ptr<Texture>, Error> 
    Create(Device* device, const TextureDesc& desc);
    
private:
    Texture() = default;  // 私有构造
};

// 使用
auto result = Texture::Create(device, desc);
```

### 3. 移动而非拷贝

```cpp
// 好 - 移动所有权
auto buffer = std::move(tempBuffer);

// 避免 - 拷贝（如果可能的话）
auto buffer = tempBuffer;  // 如果类型支持拷贝
```

### 4. 使用 auto 和类型推导

```cpp
// 好 - 清晰且不易出错
auto device = VRHI::CreateDevice(config);
auto buffer = device->CreateBuffer(desc);

// 避免 - 冗长
std::unique_ptr<Device> device = VRHI::CreateDevice(config);
```

### 5. 作用域最小化

```cpp
void Render() {
    // 好 - 最小作用域
    {
        auto tempBuffer = device->CreateTempBuffer(size);
        // 使用 tempBuffer
    }  // 立即释放
    
    // 继续其他工作
}
```

## 性能优化

### 1. 延迟初始化

```cpp
class LazyTexture {
public:
    LazyTexture(Device* device, const std::string& path)
        : device_(device), path_(path)
    {}
    
    Texture* Get() {
        if (!texture_) {
            texture_ = device_->LoadTexture(path_);
        }
        return texture_.get();
    }
    
private:
    Device* device_;
    std::string path_;
    std::unique_ptr<Texture> texture_;
};
```

### 2. 对象重用

```cpp
class FrameResources {
public:
    CommandBuffer* GetCommandBuffer() {
        if (currentCmdIndex_ < commandBuffers_.size()) {
            return commandBuffers_[currentCmdIndex_++].get();
        }
        
        auto cmd = device_->CreateCommandBuffer();
        commandBuffers_.push_back(std::move(cmd));
        currentCmdIndex_++;
        return commandBuffers_.back().get();
    }
    
    void Reset() {
        currentCmdIndex_ = 0;
        // 保留 commandBuffers_ 用于下一帧
    }
    
private:
    Device* device_;
    std::vector<std::unique_ptr<CommandBuffer>> commandBuffers_;
    size_t currentCmdIndex_ = 0;
};
```

## 调试支持

```cpp
#ifdef VRHI_DEBUG
class DebugBuffer : public Buffer {
public:
    DebugBuffer(Device* device, const BufferDesc& desc)
        : Buffer(device, desc)
    {
        std::cout << "Buffer created: " << this << "\n";
    }
    
    ~DebugBuffer() override {
        std::cout << "Buffer destroyed: " << this << "\n";
    }
};
#endif
```

## 总结

VRHI 的 RAII 设计原则：

1. **自动化**: 资源管理自动化，无需手动释放
2. **安全**: 异常安全，避免资源泄漏
3. **清晰**: 生命周期明确，易于理解和维护
4. **高效**: 利用移动语义和 C++23 特性优化性能
5. **现代**: 充分利用现代 C++ 特性和最佳实践

通过严格的 RAII 实践，VRHI 提供了安全、高效、易用的资源管理系统。
