# VRHI 架构设计

## 概述

VRHI (Versatile Render Hardware Interface) 采用分层架构设计，提供统一的渲染 API 抽象，同时支持多个图形后端。

## 架构层次

```
┌─────────────────────────────────────────┐
│         应用程序层 (Application)          │
└─────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────┐
│      VRHI 核心接口层 (Core Interface)     │
│  - Device                                │
│  - CommandBuffer                         │
│  - Resources (Buffer, Texture, etc.)     │
│  - Pipeline                              │
└─────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────┐
│     后端抽象层 (Backend Abstraction)      │
│  - Backend Factory                       │
│  - Feature Detection                     │
│  - Scoring System                        │
└─────────────────────────────────────────┘
                    ↓
┌──────────┬──────────┬──────────┬─────────┐
│ Vulkan   │ OpenGL   │ OpenGL   │ OpenGL  │
│ Backend  │ 4.x      │ 3.3      │ ES      │
│          │ Backend  │ Backend  │ Backend │
└──────────┴──────────┴──────────┴─────────┘
                    ↓
┌─────────────────────────────────────────┐
│            硬件驱动层 (Drivers)           │
└─────────────────────────────────────────┘
```

## 核心组件

### 1. Device (设备)

设备是 VRHI 的核心组件，代表一个图形硬件设备。

**主要职责:**
- 资源创建和管理
- 命令队列管理
- 特性查询
- 后端生命周期管理

**设计原则:**
- 使用 RAII 管理设备生命周期
- 线程安全的资源创建
- 惰性初始化支持

### 2. CommandBuffer (命令缓冲)

命令缓冲用于记录和提交渲染命令。

**主要职责:**
- 记录渲染命令
- 管理渲染状态
- 提交到 GPU 执行

**设计原则:**
- 支持命令记录和重放
- 最小化状态切换开销
- 后端无关的命令抽象

### 3. Resources (资源)

包括各类 GPU 资源：缓冲区、纹理、采样器等。

**资源类型:**
- **Buffer**: 顶点缓冲、索引缓冲、Uniform 缓冲等
- **Texture**: 1D/2D/3D 纹理、立方体贴图
- **Sampler**: 纹理采样器
- **RenderTarget**: 渲染目标
- **Shader**: 着色器程序

**设计原则:**
- RAII 自动释放资源
- 智能指针管理生命周期
- 支持资源别名和共享

### 4. Pipeline (管线)

图形和计算管线配置。

**主要职责:**
- 着色器绑定
- 渲染状态配置
- 输入布局定义

**设计原则:**
- 不可变管线对象
- 预编译和缓存
- 最小化运行时开销

## 后端系统

### 后端抽象设计原则

VRHI 的后端抽象层设计遵循以下原则，以支持轻松扩展到新的图形 API：

**1. 接口统一性**
- 所有后端实现相同的接口契约
- 核心接口与具体后端解耦
- 使用抽象工厂模式创建后端

**2. 最小公共功能集**
- 核心 API 仅包含所有后端都能支持的功能
- 高级特性通过特性查询和可选接口暴露
- 避免为单一后端设计专有 API

**3. 后端能力查询**
- 每个后端报告其支持的特性集合
- 应用程序根据特性调整渲染路径
- 运行时动态查询，不依赖编译期配置

**4. 零成本抽象**
- 虚函数调用最小化，关键路径使用模板
- 编译期多态优先于运行期多态
- 内联友好的接口设计

**5. 平台感知但不依赖**
- 后端可以利用平台特性优化
- 核心接口不包含平台特定代码
- 通过适配器模式隔离平台差异

### 后端接口抽象

所有后端实现统一的接口，通过工厂模式创建：

```cpp
// 后端接口基类
class IBackend {
public:
    virtual ~IBackend() = default;
    
    // 基本信息
    virtual BackendType GetType() const = 0;
    virtual std::string_view GetName() const = 0;
    virtual Version GetVersion() const = 0;
    
    // 特性查询
    virtual FeatureSet GetSupportedFeatures() const = 0;
    virtual bool IsFeatureSupported(Feature feature) const = 0;
    virtual Limits GetLimits() const = 0;
    
    // 评分系统
    virtual int CalculateScore(const FeatureRequirements& req) const = 0;
    
    // 设备管理
    virtual std::expected<std::unique_ptr<IDevice>, Error> 
        CreateDevice(const DeviceConfig& config) = 0;
    
    // 后端特定能力（可选）
    virtual void* GetNativeHandle() const { return nullptr; }
};

// 设备接口基类
class IDevice {
public:
    virtual ~IDevice() = default;
    
    // 资源创建（所有后端必须实现）
    virtual std::expected<std::unique_ptr<Buffer>, Error>
        CreateBuffer(const BufferDesc& desc) = 0;
    virtual std::expected<std::unique_ptr<Texture>, Error>
        CreateTexture(const TextureDesc& desc) = 0;
    virtual std::expected<std::unique_ptr<Shader>, Error>
        CreateShader(const ShaderDesc& desc) = 0;
    virtual std::expected<std::unique_ptr<Pipeline>, Error>
        CreatePipeline(const PipelineDesc& desc) = 0;
    
    // 命令提交
    virtual std::expected<std::unique_ptr<CommandBuffer>, Error>
        CreateCommandBuffer() = 0;
    virtual Error Submit(CommandBuffer* cmd, 
                        std::span<const Semaphore*> wait = {},
                        std::span<const Semaphore*> signal = {}) = 0;
    
    // 同步原语
    virtual std::expected<std::unique_ptr<Fence>, Error>
        CreateFence(bool signaled = false) = 0;
    virtual std::expected<std::unique_ptr<Semaphore>, Error>
        CreateSemaphore() = 0;
    
    // 交换链（可选，无头渲染不需要）
    virtual std::expected<std::unique_ptr<SwapChain>, Error>
        CreateSwapChain(const SwapChainDesc& desc) = 0;
    
    // 设备查询
    virtual BackendType GetBackendType() const = 0;
    virtual const FeatureSet& GetFeatures() const = 0;
    virtual const Limits& GetLimits() const = 0;
    
    // 本地句柄访问（用于互操作）
    virtual void* GetNativeDeviceHandle() const { return nullptr; }
};
```

### 后端工厂

使用工厂模式注册和创建后端：

```cpp
// 后端工厂
class BackendFactory {
public:
    // 注册后端（由各后端实现自动注册）
    using BackendCreator = std::function<std::unique_ptr<IBackend>()>;
    static void RegisterBackend(BackendType type, BackendCreator creator);
    
    // 枚举可用后端
    static std::vector<BackendType> EnumerateBackends();
    
    // 创建后端实例
    static std::expected<std::unique_ptr<IBackend>, Error>
        CreateBackend(BackendType type);
    
    // 自动选择最佳后端
    static std::expected<std::unique_ptr<IBackend>, Error>
        CreateBestBackend(const FeatureRequirements& requirements);
};

// 后端自动注册宏（每个后端实现使用）
#define VRHI_REGISTER_BACKEND(Type, Creator) \
    namespace { \
        struct BackendRegistrar_##Type { \
            BackendRegistrar_##Type() { \
                BackendFactory::RegisterBackend(BackendType::Type, Creator); \
            } \
        }; \
        static BackendRegistrar_##Type registrar_##Type; \
    }

// 示例：Vulkan 后端注册
// VRHI_REGISTER_BACKEND(Vulkan, []() -> std::unique_ptr<IBackend> {
//     return std::make_unique<VulkanBackend>();
// });
```

### 跨后端抽象示例

不同后端如何映射到统一接口：

| VRHI 概念 | Vulkan | Direct3D 12 | Metal | OpenGL |
|-----------|--------|-------------|-------|--------|
| **Device** | VkDevice | ID3D12Device | MTLDevice | GL Context |
| **Buffer** | VkBuffer | ID3D12Resource | MTLBuffer | GLuint (VBO) |
| **Texture** | VkImage | ID3D12Resource | MTLTexture | GLuint (Texture) |
| **Shader** | VkShaderModule | ID3D12PipelineState | MTLLibrary | GLuint (Program) |
| **CommandBuffer** | VkCommandBuffer | ID3D12CommandList | MTLCommandBuffer | Display List |
| **Fence** | VkFence | ID3D12Fence | MTLFence | glFenceSync |
| **Pipeline** | VkPipeline | ID3D12PipelineState | MTLRenderPipelineState | GL State |

### 支持的后端

#### 已实现后端

1. **Vulkan Backend**
   - 目标: 高性能 PC 和现代移动设备
   - 版本: Vulkan 1.0+
   - 优势: 最佳性能、最全特性支持
   - 平台: Windows, Linux, Android

2. **OpenGL 4.6 Backend**
   - 目标: 高端 PC
   - 优势: 完整的现代 OpenGL 特性
   - 平台: Windows, Linux

3. **OpenGL 4.1 Backend**
   - 目标: 中高端 PC、macOS
   - 优势: 完整的现代 OpenGL 特性、macOS 最大支持版本
   - 平台: Windows, Linux, macOS

4. **OpenGL 3.3 Backend**
   - 目标: PC 和笔记本通用支持
   - 优势: 广泛兼容性，最低桌面 GL 版本
   - 平台: Windows, Linux, macOS

5. **OpenGL ES 3.1 Backend**
   - 目标: 现代移动设备
   - 优势: 计算着色器支持
   - 平台: Android, iOS

6. **OpenGL ES 3.0 Backend**
   - 目标: 主流移动设备和嵌入式
   - 优势: 平衡性能和兼容性，最低 GLES 版本
   - 平台: Android, iOS, Raspberry Pi

#### 计划中后端

7. **Direct3D 12 Backend** 🚧
   - 目标: Windows 高性能渲染
   - 版本: D3D12 (Windows 10+)
   - 优势: Windows 原生 API，驱动优化最好
   - 平台: Windows 10/11
   - 特性: 与 Vulkan 相似的现代 API 设计
   - 预期实现: v2.0
   
   **抽象映射**:
   - Device → ID3D12Device
   - CommandBuffer → ID3D12CommandList
   - Buffer/Texture → ID3D12Resource
   - Pipeline → ID3D12PipelineState
   - Fence → ID3D12Fence
   - Heap → ID3D12Heap (显式内存管理)

8. **Metal Backend** 🚧
    - 目标: macOS/iOS 高性能渲染
    - 版本: Metal 2.0+
    - 优势: Apple 平台原生 API，最佳性能
    - 平台: macOS 10.13+, iOS 11+
    - 特性: 统一的内存模型，优秀的调试工具
    - 预期实现: v2.0
    
    **抽象映射**:
    - Device → MTLDevice
    - CommandBuffer → MTLCommandBuffer
    - Buffer → MTLBuffer
    - Texture → MTLTexture
    - Pipeline → MTLRenderPipelineState
    - Fence → MTLFence/MTLEvent
    - 统一内存 → 自动处理 CPU/GPU 共享

9. **WebGPU Backend** 🚧
    - 目标: Web 浏览器和跨平台
    - 版本: WebGPU 1.0
    - 优势: 现代浏览器支持，跨平台一致性
    - 平台: Chrome, Firefox, Safari (通过 WASM)
    - 预期实现: v3.0

### 后端扩展指南

添加新后端的步骤：

1. **创建后端实现类**
   ```cpp
   class NewBackend : public IBackend {
   public:
       BackendType GetType() const override { 
           return BackendType::NewBackend; 
       }
       // 实现所有必需接口...
   };
   ```

2. **实现设备类**
   ```cpp
   class NewBackendDevice : public IDevice {
   public:
       // 实现所有资源创建和管理接口...
   };
   ```

3. **注册后端**
   ```cpp
   VRHI_REGISTER_BACKEND(NewBackend, []() {
       return std::make_unique<NewBackend>();
   });
   ```

4. **特性映射**
   - 将 VRHI 特性映射到后端能力
   - 实现特性查询接口
   - 处理后端特有限制

5. **测试验证**
   - 运行 VRHI 兼容性测试套件
   - 验证资源创建和管理
   - 性能基准测试

## 初始化流程

```
1. 创建 VRHI 实例
   ↓
2. 枚举可用后端
   ↓
3. 查询每个后端的硬件特性支持
   ↓
4. 根据应用需求对后端评分
   ↓
5. 选择得分最高的后端
   ↓
6. 创建 Device 实例
   ↓
7. 应用开始使用 VRHI API
```

## 线程模型

VRHI 采用以下线程安全策略：

- **Device 创建**: 线程安全，支持多线程创建资源
- **CommandBuffer**: 非线程安全，每个线程应使用独立的 CommandBuffer
- **资源访问**: 读操作线程安全，写操作需外部同步
- **后端选择**: 初始化阶段单线程

## 内存管理

### RAII 原则

所有资源使用 RAII 模式管理：

```cpp
{
    auto device = VRHI::CreateDevice(config);
    auto buffer = device->CreateBuffer(bufferDesc);
    // 使用 buffer
} // buffer 和 device 自动释放
```

### 智能指针

- 使用 `std::unique_ptr` 管理独占资源
- 使用 `std::shared_ptr` 管理共享资源
- 避免裸指针和手动内存管理

### 资源池

- 内部使用对象池减少分配开销
- 自动回收和重用资源
- 对用户透明

## 错误处理

VRHI 使用 C++23 的 `std::expected` 进行错误处理：

```cpp
auto result = device->CreateBuffer(desc);
if (result) {
    auto buffer = std::move(*result);
    // 使用 buffer
} else {
    auto error = result.error();
    // 处理错误
}
```

## 平台支持

### 桌面平台
- **Windows**
  - 当前: Vulkan, OpenGL 4.6/4.1/3.3
  - 计划: Direct3D 12 (v2.0)
- **Linux**
  - 当前: Vulkan, OpenGL 4.6/4.1/3.3
- **macOS**
  - 当前: OpenGL 4.1/3.3 (最高支持 4.1)
  - 计划: Metal (v2.0, 推荐用于高性能)

### 移动平台
- **Android**
  - 当前: Vulkan, OpenGL ES 3.1/3.0
- **iOS**
  - 当前: OpenGL ES 3.1/3.0
  - 计划: Metal (v2.0, 推荐用于高性能)

### 嵌入式平台
- 树莓派 (OpenGL ES 3.0+, Vulkan 实验性)
- 其他 ARM Linux 设备

### Web 平台
- 计划: WebGPU (v3.0)

## 性能考虑

1. **零开销抽象**: 尽量使用编译期多态避免运行时开销
2. **批处理**: 支持命令批处理减少 API 调用
3. **状态缓存**: 缓存渲染状态避免重复设置
4. **异步操作**: 支持异步资源加载和编译
5. **内存对齐**: 保证缓冲区数据正确对齐

## 未来扩展路线图

### v2.0 - 现代 API 支持
- ✅ Direct3D 12 后端 (Windows)
  - 完整的 D3D12 特性支持
  - 光线追踪 (DXR) 扩展
  - 变速着色 (VRS)
  - 网格着色器
  
- ✅ Metal 后端 (macOS/iOS)
  - Metal 2.0+ 特性
  - MetalFX 上采样
  - 光线追踪支持
  - 统一内存优化

### v3.0 - Web 和高级特性
- ✅ WebGPU 后端
  - 浏览器原生支持
  - WASM 集成
  
- ✅ 高级渲染特性
  - 光线追踪统一抽象 (RT Cores)
  - 网格着色器管线
  - 可变速率着色
  - 采样器反馈

### v4.0 - AI 和专用硬件
- ✅ AI 加速器集成
  - Tensor Cores 支持
  - 神经网络推理
  
- ✅ 其他后端
  - 游戏主机平台 (需授权)
  - 专有硬件支持

## 设计哲学总结

VRHI 的抽象设计遵循以下核心原则：

1. **后端无关性**: 核心 API 不包含任何后端特定概念
2. **渐进增强**: 从最小功能集开始，通过特性查询启用高级功能
3. **显式优于隐式**: 清晰的接口契约，无隐藏的状态转换
4. **扩展友好**: 新后端只需实现统一接口即可集成
5. **性能第一**: 抽象层开销最小化，接近原生 API 性能

这种设计确保 VRHI 可以轻松扩展到任何新的图形 API，无论是 Direct3D 12、Metal、WebGPU，还是未来可能出现的新 API。
