# 后端接口

## 概述

VRHI 支持多个图形后端，每个后端都实现统一的接口。本文档描述后端系统的设计和各个后端的特性。

## 后端架构

### IBackend 接口

所有后端都实现 `IBackend` 接口：

```cpp
class IBackend {
public:
    virtual ~IBackend() = default;
    
    // 基本信息
    virtual BackendType GetType() const = 0;
    virtual std::string GetName() const = 0;
    virtual std::string GetVersion() const = 0;
    
    // 特性检测
    virtual FeatureSet GetSupportedFeatures() const = 0;
    virtual bool IsFeatureSupported(Feature feature) const = 0;
    
    // 评分
    virtual float CalculateScore(const FeatureRequirements& requirements) const = 0;
    
    // 初始化
    virtual bool Initialize(const DeviceConfig& config) = 0;
    virtual void Shutdown() = 0;
    
protected:
    IBackend() = default;
};
```

## 支持的后端

### 1. Vulkan Backend

**目标平台**: Windows, Linux, Android

**特点**:
- ✅ 最佳性能
- ✅ 完整的现代特性支持
- ✅ 细粒度控制
- ✅ 异步计算
- ✅ 光线追踪支持（扩展）
- ⚠️ 复杂度高
- ⚠️ 需要较新的驱动

**最低要求**: Vulkan 1.0

**推荐场景**:
- 高性能 PC 游戏
- 需要最新特性的应用
- 移动设备（Android 7.0+）

```cpp
// Vulkan 特定配置
DeviceConfig config;
config.preferredBackend = BackendType::Vulkan;
config.features.required = {
    Feature::Compute,
    Feature::AsyncCompute,
};

auto device = VRHI::CreateDevice(config);
```

### 2. OpenGL 4.6 Backend

**目标平台**: Windows, Linux

**特点**:
- ✅ 现代 OpenGL 特性
- ✅ 直接状态访问 (DSA)
- ✅ SPIR-V 着色器支持
- ✅ 良好的性能
- ✅ 广泛的工具支持
- ⚠️ 驱动质量差异大

**最低要求**: OpenGL 4.6

**推荐场景**:
- 高端 PC
- 需要 OpenGL 工具链的开发

### 3. OpenGL 4.1 Backend

**目标平台**: Windows, Linux, macOS

**特点**:
- ✅ 完整的现代 OpenGL 特性
- ✅ macOS 最大支持版本
- ✅ 良好的性能
- ✅ 广泛的工具支持
- ⚠️ 部分高级特性不可用（相比 4.6）

**最低要求**: OpenGL 4.1

**推荐场景**:
- 中高端 PC
- macOS 应用开发
- 需要跨平台兼容性

### 4. OpenGL 3.3 Backend

**目标平台**: Windows, Linux, macOS

**特点**:
- ✅ 核心渲染功能完整
- ✅ 极广的硬件兼容性
- ✅ 成熟稳定
- ❌ 无计算着色器
- ❌ 无多重间接绘制

**最低要求**: OpenGL 3.3 Core Profile

**推荐场景**:
- 通用 PC 游戏
- macOS 应用
- 最低桌面 GL 版本要求

### 5. OpenGL ES 3.1 Backend

**目标平台**: Android, iOS

**特点**:
- ✅ 计算着色器支持
- ✅ 现代移动特性
- ✅ 良好的性能
- ✅ 着色器存储缓冲
- ⚠️ 需要较新的设备

**最低要求**: OpenGL ES 3.1

**推荐场景**:
- 现代移动游戏
- 需要计算着色器的移动应用

### 6. OpenGL ES 3.0 Backend

**目标平台**: Android, iOS, 树莓派

**特点**:
- ✅ 主流移动设备支持
- ✅ 多重渲染目标
- ✅ 3D 纹理
- ✅ 实例化渲染
- ✅ 最低 GLES 版本要求
- ❌ 无计算着色器

**最低要求**: OpenGL ES 3.0

**推荐场景**:
- 主流移动游戏
- 树莓派应用
- 嵌入式设备

## 后端选择策略

### 自动选择算法

```
1. 枚举所有可用后端
2. 对每个后端:
   a. 检查是否满足必需特性
   b. 计算评分:
      - 特性支持度: 40%
      - 性能: 30%
      - 稳定性: 20%
      - 兼容性: 10%
3. 选择得分最高的后端
```

### 平台特定策略

#### Windows
```
优先级:
1. Vulkan (如果可用)
2. OpenGL 4.6
3. OpenGL 4.1
4. OpenGL 3.3
```

#### Linux
```
优先级:
1. Vulkan (如果可用)
2. OpenGL 4.6
3. OpenGL 4.1
4. OpenGL 3.3
```

#### macOS
```
优先级:
1. OpenGL 4.1 (macOS 最大支持版本)
2. OpenGL 3.3 Core (兼容模式)
注: 未来可能添加 Metal 后端
```

#### Android
```
优先级:
1. Vulkan (Android 7.0+)
2. OpenGL ES 3.1
3. OpenGL ES 3.0
```

#### iOS
```
优先级:
1. OpenGL ES 3.1
2. OpenGL ES 3.0
注: 未来可能添加 Metal 后端
```

#### 树莓派
```
树莓派 4+:
1. Vulkan (实验性)
2. OpenGL ES 3.1
3. OpenGL ES 3.0

树莓派 3:
1. OpenGL ES 3.0
```

## 后端特性对照表

| 特性 | Vulkan | GL 4.6 | GL 4.1 | GL 3.3 | GL ES 3.1 | GL ES 3.0 |
|------|--------|--------|--------|--------|-----------|-----------|
| 计算着色器 | ✅ | ✅ | ✅ | ❌ | ✅ | ❌ |
| 几何着色器 | ✅ | ✅ | ✅ | ✅ | ✅ | ❌ |
| 细分着色器 | ✅ | ✅ | ✅ | ✅ | ✅ | ❌ |
| 3D 纹理 | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| 纹理数组 | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| 实例化渲染 | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| 间接绘制 | ✅ | ✅ | ✅ | ❌ | ✅ | ❌ |
| 存储缓冲 | ✅ | ✅ | ✅ | ❌ | ✅ | ❌ |
| 异步计算 | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ |
| 光线追踪 | ✅* | ❌ | ❌ | ❌ | ❌ | ❌ |

*需要扩展支持

## 后端切换

### 运行时切换

VRHI 支持运行时切换后端（高级功能）：

```cpp
auto device = VRHI::CreateDevice().value();

// 获取当前后端
auto currentBackend = device->GetBackendType();
std::cout << "Current: " << (int)currentBackend << "\n";

// 枚举替代后端
auto alternatives = device->GetAlternativeBackends();

// 切换
if (!alternatives.empty()) {
    auto result = device->SwitchBackend(alternatives[0].type);
    if (result) {
        std::cout << "Switched to " << alternatives[0].name << "\n";
    }
}
```

### 回退机制

```cpp
// 尝试创建首选后端，失败则回退
BackendType backends[] = {
    BackendType::Vulkan,
    BackendType::OpenGL46,
    BackendType::OpenGL43,
    BackendType::OpenGL33,
};

std::unique_ptr<Device> device;
for (auto backend : backends) {
    DeviceConfig config;
    config.preferredBackend = backend;
    
    auto result = VRHI::CreateDevice(config);
    if (result) {
        device = std::move(*result);
        break;
    }
}

if (!device) {
    std::cerr << "No compatible backend found!\n";
}
```

## 后端开发指南

### 添加新后端

1. 实现 `IBackend` 接口
2. 实现后端特定的设备、资源、命令缓冲类
3. 添加特性检测代码
4. 实现评分算法
5. 注册后端到工厂

示例框架：

```cpp
class MyBackend : public IBackend {
public:
    BackendType GetType() const override {
        return BackendType::MyBackend;
    }
    
    FeatureSet GetSupportedFeatures() const override {
        FeatureSet features;
        // 检测特性...
        return features;
    }
    
    float CalculateScore(const FeatureRequirements& req) const override {
        // 计算评分...
        return score;
    }
    
    bool Initialize(const DeviceConfig& config) override {
        // 初始化后端...
        return true;
    }
    
    void Shutdown() override {
        // 清理资源...
    }
};
```

## 调试和日志

### 启用后端日志

```cpp
VRHI::SetLogLevel(LogLevel::Debug);

// 日志输出示例:
// [VRHI][Vulkan] Initializing Vulkan backend
// [VRHI][Vulkan] Instance created
// [VRHI][Vulkan] Physical device: NVIDIA GeForce RTX 3080
// [VRHI][Vulkan] Logical device created
```

### 查询后端信息

```cpp
auto info = device->GetBackendInfo();

std::cout << "Backend: " << info.name << "\n";
std::cout << "Version: " << info.version << "\n";
std::cout << "Device: " << info.deviceName << "\n";
std::cout << "Vendor: " << info.vendorName << "\n";
std::cout << "Driver: " << info.driverVersion << "\n";
std::cout << "Score: " << info.score << "\n";

for (const auto& detail : info.scoreDetails) {
    std::cout << "  " << detail.category << ": " 
              << detail.score << " (weight: " 
              << detail.weight << ")\n";
}
```

## 最佳实践

1. **优先使用 Auto**: 让 VRHI 自动选择最佳后端
2. **测试多后端**: 在多个后端上测试应用
3. **提供回退**: 实现回退机制处理不可用的后端
4. **特性检测**: 使用特性检测而非后端类型判断
5. **日志记录**: 记录选择的后端和原因

## 未来计划

### 计划中的后端

- **Direct3D 12**: Windows 高性能
- **Metal**: macOS 和 iOS 原生支持
- **WebGPU**: Web 平台支持

### 实验性特性

- 光线追踪 (Vulkan Ray Tracing)
- 网格着色器 (Vulkan Mesh Shaders)
- 可变速率着色 (Variable Rate Shading)
