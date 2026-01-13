# 特性检测系统

## 概述

VRHI 的特性检测系统负责在初始化阶段查询硬件和驱动对各种渲染特性的支持情况。这是后端评分和选择的基础。

## 设计原则

1. **全面性**: 检测所有可能影响渲染的特性
2. **准确性**: 不仅查询声称支持，还验证实际工作
3. **高效性**: 检测过程快速，不影响启动时间
4. **可扩展**: 易于添加新特性检测
5. **跨平台**: 统一的检测接口，平台相关细节封装在后端

## 特性分类

### 核心渲染特性

```cpp
struct CoreFeatures {
    // 着色器阶段
    bool vertexShader;           // 顶点着色器
    bool fragmentShader;         // 片段着色器
    bool geometryShader;         // 几何着色器
    bool tessellationShader;     // 细分着色器
    bool computeShader;          // 计算着色器
    bool meshShader;             // 网格着色器 (Vulkan/DX12)
    bool taskShader;             // 任务着色器
    
    // 缓冲区
    bool uniformBuffers;         // Uniform 缓冲区
    bool storageBuffers;         // 存储缓冲区 (SSBO)
    bool vertexBuffers;          // 顶点缓冲区
    bool indexBuffers;           // 索引缓冲区
    bool indirectBuffers;        // 间接绘制缓冲区
    
    // 基础功能
    bool instancing;             // 实例化渲染
    bool multiDrawIndirect;      // 多重间接绘制
    bool drawBaseVertex;         // 基础顶点绘制
};
```

### 纹理特性

```cpp
struct TextureFeatures {
    // 纹理类型
    bool texture1D;              // 1D 纹理
    bool texture2D;              // 2D 纹理
    bool texture3D;              // 3D 纹理
    bool textureCube;            // 立方体贴图
    bool texture2DArray;         // 2D 纹理数组
    bool textureCubeArray;       // 立方体贴图数组
    
    // 纹理格式
    bool floatTextures;          // 浮点纹理
    bool depthTextures;          // 深度纹理
    bool stencilTextures;        // 模板纹理
    bool compressedTextures;     // 压缩纹理
    
    // 压缩格式支持
    bool dxt;                    // DXT/BC 压缩
    bool etc2;                   // ETC2 压缩
    bool astc;                   // ASTC 压缩
    bool pvrtc;                  // PVRTC 压缩
    
    // 采样特性
    bool anisotropicFiltering;   // 各向异性过滤
    bool shadowSamplers;         // 阴影采样器
    bool seamlessCubemap;        // 无缝立方体贴图
    
    // 高级特性
    uint32_t maxTextureSize;     // 最大纹理尺寸
    uint32_t max3DTextureSize;   // 最大 3D 纹理尺寸
    uint32_t maxArrayLayers;     // 最大数组层数
    float maxAnisotropy;         // 最大各向异性级别
};
```

### 渲染特性

```cpp
struct RenderingFeatures {
    // 渲染目标
    bool multipleRenderTargets;  // 多渲染目标 (MRT)
    uint32_t maxColorAttachments; // 最大颜色附件数
    bool independentBlend;       // 独立混合
    
    // 混合模式
    bool advancedBlending;       // 高级混合模式
    bool logicOp;                // 逻辑操作
    bool dualSourceBlending;     // 双源混合
    
    // 深度/模板
    bool depthClamp;             // 深度钳制
    bool depthBoundsTest;        // 深度边界测试
    bool stencilExport;          // 模板导出
    
    // 光栅化
    bool wireframeMode;          // 线框模式
    bool pointSize;              // 可编程点大小
    bool lineWidth;              // 可变线宽
    bool polygonOffsetClamp;     // 多边形偏移钳制
    
    // 多重采样
    bool multisample;            // 多重采样
    uint32_t maxSamples;         // 最大采样数
    bool sampleShading;          // 采样着色
    bool alphaToCoverage;        // Alpha to Coverage
};
```

### 计算特性

```cpp
struct ComputeFeatures {
    bool computeShader;          // 计算着色器支持
    
    // 工作组限制
    uint32_t maxWorkGroupSizeX;  // X 维度最大工作组大小
    uint32_t maxWorkGroupSizeY;  // Y 维度最大工作组大小
    uint32_t maxWorkGroupSizeZ;  // Z 维度最大工作组大小
    uint32_t maxWorkGroupInvocations; // 最大调用数
    
    // 调度限制
    uint32_t maxComputeWorkGroupCountX;
    uint32_t maxComputeWorkGroupCountY;
    uint32_t maxComputeWorkGroupCountZ;
    
    // 共享内存
    uint32_t maxComputeSharedMemorySize;
    
    // 高级特性
    bool subgroupOperations;     // 子组操作
    uint32_t subgroupSize;       // 子组大小
};
```

### 高级特性

```cpp
struct AdvancedFeatures {
    // 光线追踪
    bool rayTracing;             // 光线追踪支持
    bool rayQuery;               // 光线查询
    bool rayTracingPipeline;     // 光线追踪管线
    
    // 网格着色器
    bool meshShading;            // 网格着色器
    uint32_t maxMeshWorkGroupSize;
    uint32_t maxTaskWorkGroupSize;
    
    // 可变速率着色
    bool variableRateShading;    // VRS
    uint32_t shadingRateTileSize;
    
    // 无绑定资源
    bool bindlessResources;      // 无绑定资源
    bool descriptorIndexing;     // 描述符索引
    bool bufferDeviceAddress;    // 缓冲区设备地址
    
    // 同步
    bool asyncCompute;           // 异步计算
    bool timelineSemaphores;     // 时间线信号量
};
```

### 内存和性能特性

```cpp
struct MemoryFeatures {
    // 内存类型
    uint64_t deviceLocalMemory;  // 设备本地内存大小
    uint64_t hostVisibleMemory;  // 主机可见内存大小
    uint64_t sharedMemory;       // 共享内存大小
    
    // 对齐要求
    uint32_t minUniformBufferAlignment;
    uint32_t minStorageBufferAlignment;
    uint32_t minTexelBufferAlignment;
    uint32_t optimalBufferCopyAlignment;
    
    // 性能特性
    bool unifiedMemory;          // 统一内存架构
    bool dedicatedAllocation;    // 专用分配
    bool memoryBudget;           // 内存预算查询
};
```

## 特性检测流程

```
1. 初始化后端
   ↓
2. 查询基本能力
   a. API 版本
   b. 驱动版本
   c. 硬件信息
   ↓
3. 查询扩展支持
   a. 枚举可用扩展
   b. 检查必需扩展
   ↓
4. 查询特性支持
   a. 核心特性
   b. 纹理特性
   c. 渲染特性
   d. 计算特性
   e. 高级特性
   ↓
5. 验证关键特性
   a. 创建测试资源
   b. 执行测试命令
   c. 验证结果
   ↓
6. 生成特性集
   ↓
7. 返回 FeatureSet 对象
```

## 实现示例

### Vulkan 特性检测

```cpp
FeatureSet VulkanBackend::DetectFeatures() const {
    FeatureSet features;
    
    // 查询物理设备特性
    VkPhysicalDeviceFeatures2 deviceFeatures{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2
    };
    
    // 链接扩展特性结构
    VkPhysicalDeviceVulkan11Features vk11Features{};
    VkPhysicalDeviceVulkan12Features vk12Features{};
    VkPhysicalDeviceVulkan13Features vk13Features{};
    
    deviceFeatures.pNext = &vk11Features;
    vk11Features.pNext = &vk12Features;
    vk12Features.pNext = &vk13Features;
    
    vkGetPhysicalDeviceFeatures2(physicalDevice, &deviceFeatures);
    
    // 填充核心特性
    features.core.geometryShader = deviceFeatures.features.geometryShader;
    features.core.tessellationShader = deviceFeatures.features.tessellationShader;
    features.core.computeShader = true; // Vulkan 始终支持
    
    // 填充纹理特性
    features.texture.anisotropicFiltering = 
        deviceFeatures.features.samplerAnisotropy;
    
    // 查询限制
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(physicalDevice, &properties);
    
    features.texture.maxTextureSize = properties.limits.maxImageDimension2D;
    features.texture.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    
    // 查询内存信息
    VkPhysicalDeviceMemoryProperties memProps;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProps);
    
    for (uint32_t i = 0; i < memProps.memoryHeapCount; ++i) {
        if (memProps.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
            features.memory.deviceLocalMemory = memProps.memoryHeaps[i].size;
        }
    }
    
    return features;
}
```

### OpenGL 特性检测

```cpp
FeatureSet OpenGLBackend::DetectFeatures() const {
    FeatureSet features;
    
    // 查询 GL 版本
    GLint major, minor;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    
    // 基于版本设置特性
    if (major >= 4) {
        features.core.computeShader = (major > 4 || minor >= 3);
        features.core.tessellationShader = true;
        features.core.geometryShader = true;
    } else if (major >= 3) {
        features.core.geometryShader = (major > 3 || minor >= 2);
    }
    
    // 查询扩展
    auto hasExtension = [](const char* ext) {
        GLint numExtensions;
        glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
        for (GLint i = 0; i < numExtensions; ++i) {
            const char* extension = 
                reinterpret_cast<const char*>(glGetStringi(GL_EXTENSIONS, i));
            if (strcmp(extension, ext) == 0) return true;
        }
        return false;
    };
    
    // 纹理压缩
    features.texture.dxt = hasExtension("GL_EXT_texture_compression_s3tc");
    features.texture.etc2 = (major >= 4 || major == 3);
    
    // 查询限制
    GLint maxTextureSize;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
    features.texture.maxTextureSize = maxTextureSize;
    
    if (features.texture.anisotropicFiltering) {
        GLfloat maxAniso;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);
        features.texture.maxAnisotropy = maxAniso;
    }
    
    // 计算着色器限制
    if (features.core.computeShader) {
        GLint maxWorkGroupSize[3];
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &maxWorkGroupSize[0]);
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &maxWorkGroupSize[1]);
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &maxWorkGroupSize[2]);
        
        features.compute.maxWorkGroupSizeX = maxWorkGroupSize[0];
        features.compute.maxWorkGroupSizeY = maxWorkGroupSize[1];
        features.compute.maxWorkGroupSizeZ = maxWorkGroupSize[2];
    }
    
    return features;
}
```

## 特性验证

某些特性声称支持但实际可能不工作，需要验证：

```cpp
bool VerifyComputeShader(const Backend& backend) {
    // 创建简单的计算着色器
    const char* source = R"(
        #version 430
        layout(local_size_x = 1) in;
        layout(std430, binding = 0) buffer Output {
            uint value;
        } output;
        void main() {
            output.value = 42;
        }
    )";
    
    try {
        auto shader = backend.CreateComputeShader(source);
        auto buffer = backend.CreateStorageBuffer(sizeof(uint32_t));
        
        // 执行计算着色器
        auto cmd = backend.CreateCommandBuffer();
        cmd->BindComputeShader(shader);
        cmd->BindStorageBuffer(0, buffer);
        cmd->Dispatch(1, 1, 1);
        backend.Submit(cmd);
        backend.WaitIdle();
        
        // 读取结果
        uint32_t result = 0;
        buffer->Read(&result, sizeof(result));
        
        return result == 42;
    } catch (...) {
        return false;
    }
}
```

## 运行时查询

应用可以在运行时查询特性：

```cpp
auto device = VRHI::CreateDevice();

// 查询完整特性集
FeatureSet features = device->GetFeatures();

// 查询特定特性
if (features.core.computeShader) {
    // 使用计算着色器
}

// 查询特性支持
if (device->IsFeatureSupported(Feature::RayTracing)) {
    // 启用光线追踪代码路径
} else {
    // 使用传统光栅化
}

// 查询限制值
uint32_t maxTexSize = device->GetLimit(Limit::MaxTextureSize);
```

## 特性组

将相关特性分组，便于批量检查：

```cpp
enum class FeatureGroup {
    Basic,              // 基础渲染
    Advanced,           // 高级渲染
    Compute,            // 计算功能
    RayTracing,         // 光线追踪
    MeshShading,        // 网格着色
};

bool SupportsFeatureGroup(FeatureGroup group) const {
    switch (group) {
    case FeatureGroup::Basic:
        return features.core.vertexShader &&
               features.core.fragmentShader &&
               features.texture.texture2D;
               
    case FeatureGroup::Compute:
        return features.core.computeShader &&
               features.core.storageBuffers;
               
    case FeatureGroup::RayTracing:
        return features.advanced.rayTracing &&
               features.advanced.rayTracingPipeline;
               
    // ...
    }
}
```

## 平台差异处理

不同平台的特性检测可能有差异：

```cpp
#ifdef VRHI_PLATFORM_ANDROID
    // Android 特定检测
    features.texture.etc2 = true;  // ETC2 是必需的
    features.texture.astc = CheckASTCSupport();
#endif

#ifdef VRHI_PLATFORM_IOS
    // iOS 特定检测
    features.texture.pvrtc = true;
    features.memory.unifiedMemory = true;
#endif

#ifdef VRHI_PLATFORM_RASPBERRY_PI
    // 树莓派特定检测
    features.texture.etc2 = true;
    // VideoCore 限制
    features.texture.maxTextureSize = 2048;
#endif
```

## 最佳实践

1. **渐进式检测**: 先检测基本特性，再检测高级特性
2. **缓存结果**: 特性检测结果应缓存，避免重复查询
3. **验证关键特性**: 对关键特性进行实际测试验证
4. **日志记录**: 记录检测结果，便于调试
5. **优雅降级**: 缺少特性时提供替代方案

## 性能考虑

- 特性检测应在初始化阶段完成一次
- 避免运行时重复查询
- 使用位域和紧凑数据结构
- 异步检测非关键特性

## 未来扩展

- 运行时特性热更新检测
- 性能基准测试集成
- 云端特性数据库同步
- AI 辅助特性预测
