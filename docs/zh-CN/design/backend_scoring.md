# 后端评分系统设计

## 概述

VRHI 的核心创新之一是其智能后端选择系统。在初始化时，VRHI 会检测所有可用后端，查询硬件支持的特性，然后根据应用需求对每个后端进行评分，最终选择得分最高的后端。

## 设计目标

1. **自动化**: 应用无需手动选择后端，系统自动选择最优方案
2. **智能**: 综合考虑功能支持、性能、稳定性等因素
3. **可定制**: 允许应用指定必需特性和偏好
4. **透明**: 提供评分细节，便于调试和优化

## 评分维度

### 1. 特性支持度 (Feature Support Score)

根据后端支持的特性数量和质量评分。

**权重**: 40%

**计算方法**:
```
特性支持分 = (必需特性满足度 × 100) + (可选特性满足度 × 50)
```

**必需特性 (Must-Have)**:
- 如果不满足任何一个必需特性，后端得分为 0，直接排除
- 如果满足所有必需特性，得到满分基础分

**可选特性 (Nice-to-Have)**:
- 每支持一个可选特性，增加额外分数
- 支持的可选特性越多，得分越高

**示例特性列表**:
```cpp
enum class Feature {
    // 核心特性
    Compute,                    // 计算着色器
    Geometry,                   // 几何着色器
    Tessellation,               // 细分曲面
    
    // 纹理特性
    Texture3D,                  // 3D 纹理
    TextureCompression,         // 纹理压缩
    AnisotropicFiltering,       // 各向异性过滤
    
    // 高级特性
    MultiDrawIndirect,          // 间接绘制
    RayTracing,                 // 光线追踪
    MeshShading,                // 网格着色器
    
    // 性能特性
    AsyncCompute,               // 异步计算
    BindlessResources,          // 无绑定资源
    
    // 调试特性
    DebugMarkers,               // 调试标记
    GPUValidation,              // GPU 验证层
};
```

### 2. 性能分数 (Performance Score)

基于后端的理论性能和已知基准测试结果。

**权重**: 30%

**考虑因素**:
- **API 开销**: Vulkan ≈ D3D12 ≈ Metal < OpenGL 4.x < OpenGL 3.x < OpenGL 2.x
- **驱动质量**: 根据平台和供应商的已知表现
- **硬件能力**: GPU 计算能力、内存带宽等

**性能排序** (从高到低):

**现代低开销 API** (相似性能，平台原生优势):
1. Vulkan (跨平台，最低开销，最高性能)
2. Direct3D 12 (Windows 原生，驱动优化最好) 🚧 计划
3. Metal (Apple 平台原生，统一内存优势) 🚧 计划

**传统 API**:
4. OpenGL 4.6 (现代 OpenGL，DSA 支持)
5. OpenGL 4.1 (完整现代特性，macOS 最大支持)
6. OpenGL ES 3.1 (移动端高性能)
7. OpenGL 3.3 (传统桌面 GL)
8. OpenGL ES 3.0 (主流移动端)
9. OpenGL ES 2.0 (基础移动端)
10. OpenGL 2.0 (兼容模式)

**平台优先级**:
- **Windows**: D3D12 (计划) > Vulkan > OpenGL 4.6
- **macOS**: Metal (计划) > OpenGL 4.1
- **iOS**: Metal (计划) > OpenGL ES 3.0
- **Linux**: Vulkan > OpenGL 4.6
- **Android**: Vulkan > OpenGL ES 3.1

**性能分数公式**:
```
性能分 = 基础性能分 + 平台原生加成 + 硬件加成分 - 已知问题惩罚分
```

其中，平台原生加成：
- 后端为平台原生 API（如 Windows 上的 D3D12，macOS 上的 Metal）：+10 分
- 后端广泛支持但非原生（如 Vulkan）：+5 分
- 后端为兼容层（如 macOS 上的 OpenGL）：0 分

### 3. 稳定性分数 (Stability Score)

基于后端的成熟度和已知问题。

**权重**: 20%

**考虑因素**:
- **驱动成熟度**: 该平台上驱动的稳定性
- **已知问题**: 是否存在已知的崩溃或渲染错误
- **测试覆盖**: 该后端的测试覆盖程度

**稳定性评级**:
```cpp
enum class StabilityLevel {
    Experimental = 0,   // 实验性，可能不稳定
    Beta = 50,          // 测试阶段，基本稳定
    Stable = 80,        // 稳定版本
    Production = 100,   // 生产就绪，经过充分测试
};
```

### 4. 兼容性分数 (Compatibility Score)

设备和驱动对该后端的支持程度。

**权重**: 10%

**考虑因素**:
- **驱动版本**: 是否为最新或推荐版本
- **扩展支持**: 必需扩展是否都可用
- **平台支持**: 该平台对后端的原生支持程度

## 评分算法

### 总分计算

```cpp
总分 = (特性支持分 × 0.4) + 
       (性能分 × 0.3) + 
       (稳定性分 × 0.2) + 
       (兼容性分 × 0.1)
```

### 评分流程

```
1. 枚举所有可用后端
   ↓
2. 对每个后端:
   a. 检查必需特性 → 不满足则跳过
   b. 计算特性支持分
   c. 计算性能分
   d. 查询稳定性评级
   e. 检查兼容性
   f. 计算总分
   ↓
3. 按总分降序排序
   ↓
4. 选择得分最高的后端
   ↓
5. 如果没有后端满足所有必需特性 → 返回错误
   ↓
6. 返回选中的后端及评分详情
```

## 必需特性验证机制

### 设计原则

对于明确需要特定功能的应用程序，VRHI 提供严格的必需特性验证机制：

1. **严格验证**: 所有在 `required` 列表中的特性都必须被选中的后端支持
2. **早期失败**: 如果没有后端能满足所有必需特性，立即返回错误，不进行初始化
3. **明确错误信息**: 清楚地指出哪些必需特性不被任何后端支持
4. **应用控制**: 由应用开发者决定哪些特性是必需的，而非 VRHI

### 验证流程

```
1. 应用指定必需特性列表
   ↓
2. VRHI 枚举所有可用后端
   ↓
3. 对每个后端检查:
   是否支持所有必需特性?
   ├─ 是 → 保留此后端，继续评分
   └─ 否 → 排除此后端
   ↓
4. 检查候选后端列表:
   ├─ 为空 → 返回 Error::NoCompatibleBackend
   │          包含缺失特性的详细信息
   └─ 非空 → 继续后端评分和选择
```

### 错误处理

当没有后端满足所有必需特性时，`CreateDevice` 返回错误：

**错误码**: `Error::Code::NoCompatibleBackend`

**错误信息包含**:
- 缺失的必需特性列表
- 每个后端不支持的特性详情
- 建议的替代方案（如降级某些特性需求）

### 代码示例

```cpp
// 定义严格的特性需求
FeatureRequirements requirements;
requirements.required = {
    Feature::Compute,           // 必需：计算着色器
    Feature::Texture3D,         // 必需：3D 纹理
    Feature::MultiDrawIndirect, // 必需：间接多重绘制
};

auto deviceResult = VRHI::CreateDevice(requirements);

if (!deviceResult) {
    // 处理必需特性不满足的情况
    const auto& error = deviceResult.error();
    
    if (error.code == Error::Code::NoCompatibleBackend) {
        std::cerr << "错误：没有后端支持所有必需特性\n";
        std::cerr << error.message << "\n";
        
        // 应用应该:
        // 1. 通知用户硬件不满足最低要求
        // 2. 提供替代方案（如禁用某些功能）
        // 3. 或直接退出
        
        return EXIT_FAILURE;
    }
}

auto device = std::move(*deviceResult);
// 此时保证设备支持所有必需特性
```

### 验证保证

一旦 `CreateDevice` 成功返回，应用可以确信：

1. **所有必需特性可用**: `required` 列表中的所有特性都被选中的后端支持
2. **运行时安全**: 可以直接使用这些特性，无需额外检查
3. **一致性**: 在设备生命周期内，这些特性始终可用

```cpp
// CreateDevice 成功后
auto device = VRHI::CreateDevice(requirements).value();

// 可以安全地使用必需特性，无需检查
auto computeShader = device->CreateComputeShader(source);  // 保证成功
auto texture3D = device->CreateTexture3D(desc);             // 保证成功
```

### 最佳实践

1. **明确必需特性**: 只将真正必需的特性列入 `required`
   ```cpp
   // ✅ 好 - 只列出真正必需的
   requirements.required = {
       Feature::Texture2D,
       Feature::VertexBuffers,
   };
   
   // ❌ 避免 - 列出所有想要的特性
   requirements.required = {
       Feature::RayTracing,        // 可能过于严格
       Feature::MeshShading,       // 大多数设备不支持
       Feature::BindlessResources,
   };
   ```

2. **使用可选特性**: 对于非关键功能，使用 `optional`
   ```cpp
   requirements.required = {
       Feature::Texture2D,  // 必须有
   };
   requirements.optional = {
       Feature::Compute,    // 有最好，没有也能工作
       Feature::Geometry,
   };
   ```

3. **提供降级路径**: 准备备用实现方案
   ```cpp
   auto device = VRHI::CreateDevice(highEndRequirements);
   
   if (!device) {
       // 尝试降级需求
       auto fallbackDevice = VRHI::CreateDevice(basicRequirements);
       if (fallbackDevice) {
           // 使用基础功能模式
       }
   }
   ```

4. **错误信息本地化**: 向最终用户展示友好的错误信息
   ```cpp
   if (error.code == Error::Code::NoCompatibleBackend) {
       ShowUserDialog(
           "硬件不支持",
           "您的显卡不支持运行此应用所需的功能。\n"
           "最低要求：支持计算着色器的 GPU"
       );
   }
   ```

### 调试支持

启用详细日志以查看特性验证详情：

```cpp
VRHI::SetLogLevel(LogLevel::Debug);

auto device = VRHI::CreateDevice(requirements);

// 日志输出示例:
// [VRHI] Checking required features...
// [VRHI]   Required: Compute - checking...
// [VRHI]     Vulkan: ✅ Supported
// [VRHI]     OpenGL 4.6: ✅ Supported
// [VRHI]     OpenGL 4.1: ✅ Supported
// [VRHI]     OpenGL 3.3: ❌ Not supported
// [VRHI]   Required: MultiDrawIndirect - checking...
// [VRHI]     Vulkan: ✅ Supported
// [VRHI]     OpenGL 4.6: ✅ Supported
// [VRHI]     OpenGL 4.1: ✅ Supported
// [VRHI]     OpenGL 3.3: ❌ Not supported
// [VRHI] Backends passing required features: Vulkan, OpenGL 4.6, OpenGL 4.1
// [VRHI] Selected: Vulkan (score: 95.0)
```

## 代码示例

### 定义特性需求

```cpp
// 应用定义其需求
FeatureRequirements requirements;

// 必需特性
requirements.required = {
    Feature::Texture2D,
    Feature::VertexBuffers,
    Feature::FragmentShaders,
};

// 可选特性 (按优先级排序)
requirements.optional = {
    Feature::Compute,           // 最希望有
    Feature::Geometry,
    Feature::MultiDrawIndirect,
};

// 性能偏好
requirements.performanceLevel = PerformanceLevel::High;

// 稳定性要求
requirements.minStability = StabilityLevel::Stable;
```

### 创建设备

```cpp
// VRHI 自动选择最佳后端
auto deviceResult = VRHI::CreateDevice(requirements);

if (deviceResult) {
    auto device = std::move(*deviceResult);
    
    // 查看选择的后端
    BackendInfo info = device->GetBackendInfo();
    std::cout << "Selected: " << info.name 
              << " (Score: " << info.score << ")\n";
    
    // 查看评分详细信息
    for (const auto& detail : info.scoreDetails) {
        std::cout << detail.category << ": " 
                  << detail.score << "\n";
    }
} else {
    // 处理错误 - 没有满足需求的后端
    std::cerr << "Failed to create device: " 
              << deviceResult.error().message << "\n";
}
```

### 手动选择后端

```cpp
// 如果需要，也可以手动指定后端
auto backends = VRHI::EnumerateBackends();

// 查看所有后端评分
for (const auto& backend : backends) {
    std::cout << backend.name << ": " 
              << backend.CalculateScore(requirements) << "\n";
}

// 强制使用特定后端
auto device = VRHI::CreateDevice(
    requirements,
    BackendType::OpenGLES30  // 强制使用 OpenGL ES 3.0
);
```

## 平台特定调整

### Windows

```cpp
// Vulkan 优先 (如果可用)
// OpenGL 4.6 次选
// 避免 OpenGL ES (通过模拟器，性能差)
```

### Linux

```cpp
// Vulkan 优先 (Mesa 驱动支持好)
// OpenGL 4.x 次选
// 对于老硬件降级到 OpenGL 3.3
```

### Android

```cpp
// Vulkan 优先 (Android 7.0+)
// OpenGL ES 3.1 次选 (高端设备)
// OpenGL ES 3.0 (主流设备)
// OpenGL ES 2.0 (低端设备)
```

### 树莓派

```cpp
// OpenGL ES 3.0 优先 (树莓派 4)
// OpenGL ES 2.0 备选 (树莓派 3 及更早)
// Vulkan (实验性，树莓派 4)
```

## 调试和日志

VRHI 提供详细的评分日志：

```cpp
// 启用评分日志
VRHI::SetLogLevel(LogLevel::Debug);

// 日志输出示例:
// [VRHI] Enumerating backends...
// [VRHI] Found backend: Vulkan
// [VRHI]   Feature Score: 95/100 (all required + 15 optional)
// [VRHI]   Performance Score: 100/100
// [VRHI]   Stability Score: 80/100 (Stable)
// [VRHI]   Compatibility Score: 90/100
// [VRHI]   Total Score: 93.5
// [VRHI] Found backend: OpenGL 4.6
// [VRHI]   Feature Score: 85/100 (all required + 10 optional)
// [VRHI]   Performance Score: 85/100
// [VRHI]   Stability Score: 100/100 (Production)
// [VRHI]   Compatibility Score: 95/100
// [VRHI]   Total Score: 88.5
// [VRHI] Selected backend: Vulkan (score: 93.5)
```

## 动态后端切换

VRHI 支持运行时切换后端（高级功能）：

```cpp
// 获取当前后端
auto currentBackend = device->GetBackendType();

// 枚举替代后端
auto alternatives = device->GetAlternativeBackends();

// 切换到备选后端
if (!alternatives.empty()) {
    auto result = device->SwitchBackend(alternatives[0]);
    if (result) {
        std::cout << "Switched to " << alternatives[0].name << "\n";
    }
}
```

**使用场景**:
- 检测到当前后端不稳定
- 用户手动选择
- 性能测试和对比
- 回退到更稳定的后端

## 最佳实践

1. **明确需求**: 清晰定义必需和可选特性
2. **合理权重**: 根据应用类型调整评分权重
3. **测试覆盖**: 在多个后端上测试应用
4. **优雅降级**: 为低端后端提供替代方案
5. **性能监控**: 监控实际性能，验证选择的正确性
6. **用户选项**: 允许高级用户手动选择后端

## 未来改进

- 机器学习辅助评分
- 基于历史数据的动态调整
- 用户反馈收集和分析
- A/B 测试不同后端选择策略
