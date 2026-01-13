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

### 后端抽象

所有后端实现统一的接口，通过工厂模式创建：

```cpp
class IBackend {
public:
    virtual ~IBackend() = default;
    virtual BackendType GetType() const = 0;
    virtual FeatureSet GetSupportedFeatures() const = 0;
    virtual int CalculateScore(const FeatureRequirements& req) const = 0;
    // ... 其他接口
};
```

### 支持的后端

1. **Vulkan Backend**
   - 目标: 高性能 PC 和现代移动设备
   - 版本: Vulkan 1.0+
   - 优势: 最佳性能、最全特性支持

2. **OpenGL 4.6 Backend**
   - 目标: 高端 PC
   - 优势: 完整的现代 OpenGL 特性

3. **OpenGL 4.3 Backend**
   - 目标: 中高端 PC
   - 优势: 计算着色器支持

4. **OpenGL 3.3 Backend**
   - 目标: 较老的 PC 和笔记本
   - 优势: 广泛兼容性

5. **OpenGL 2.0 Backend**
   - 目标: 老旧硬件
   - 优势: 最大兼容性

6. **OpenGL ES 3.1 Backend**
   - 目标: 现代移动设备
   - 优势: 计算着色器支持

7. **OpenGL ES 3.0 Backend**
   - 目标: 主流移动设备
   - 优势: 平衡性能和兼容性

8. **OpenGL ES 2.0 Backend**
   - 目标: 老旧移动设备、树莓派
   - 优势: 最大移动兼容性

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
- Windows (Vulkan, OpenGL)
- Linux (Vulkan, OpenGL)
- macOS (OpenGL, 未来可能支持 Metal)

### 移动平台
- Android (Vulkan, OpenGL ES)
- iOS (OpenGL ES, 未来可能支持 Metal)

### 嵌入式平台
- 树莓派 (OpenGL ES)
- 其他 ARM Linux 设备

## 性能考虑

1. **零开销抽象**: 尽量使用编译期多态避免运行时开销
2. **批处理**: 支持命令批处理减少 API 调用
3. **状态缓存**: 缓存渲染状态避免重复设置
4. **异步操作**: 支持异步资源加载和编译
5. **内存对齐**: 保证缓冲区数据正确对齐

## 未来扩展

- Direct3D 12 后端支持
- Metal 后端支持
- WebGPU 后端支持
- 光线追踪扩展
- 网格着色器支持
