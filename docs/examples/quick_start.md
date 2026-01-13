# 快速入门

## 安装

VRHI 是一个仅头文件库（header-only），使用 C++23 标准。

### 要求

- C++23 兼容编译器 (GCC 13+, Clang 16+, MSVC 2022+)
- CMake 3.20+
- 平台相关的图形 API SDK:
  - Vulkan SDK (可选)
  - OpenGL 驱动

### 集成到项目

**使用 CMake**:

```cmake
# CMakeLists.txt
cmake_minimum_required(VERSION 3.20)
project(MyProject)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加 VRHI
add_subdirectory(external/VRHI)

add_executable(myapp main.cpp)
target_link_libraries(myapp PRIVATE VRHI::VRHI)
```

**手动集成**:

```cpp
// 包含头文件
#include <VRHI/VRHI.hpp>
#include <VRHI/Resources.hpp>
```

## Hello Triangle 示例

这是一个完整的 Hello Triangle 示例，展示了 VRHI 的基本用法。

```cpp
#include <VRHI/VRHI.hpp>
#include <VRHI/Resources.hpp>
#include <iostream>

// 顶点数据
struct Vertex {
    float position[3];
    float color[3];
};

const Vertex vertices[] = {
    {{  0.0f,  0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f }},  // 顶部 - 红色
    {{ -0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f }},  // 左下 - 绿色
    {{  0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }},  // 右下 - 蓝色
};

// 顶点着色器
const char* vertexShaderSource = R"(
    #version 450
    
    layout(location = 0) in vec3 inPosition;
    layout(location = 1) in vec3 inColor;
    
    layout(location = 0) out vec3 fragColor;
    
    void main() {
        gl_Position = vec4(inPosition, 1.0);
        fragColor = inColor;
    }
)";

// 片段着色器
const char* fragmentShaderSource = R"(
    #version 450
    
    layout(location = 0) in vec3 fragColor;
    layout(location = 0) out vec4 outColor;
    
    void main() {
        outColor = vec4(fragColor, 1.0);
    }
)";

int main() {
    // 1. 创建设备
    VRHI::DeviceConfig config;
    config.width = 800;
    config.height = 600;
    config.enableValidation = true;  // 启用验证层用于开发
    
    auto deviceResult = VRHI::CreateDevice(config);
    if (!deviceResult) {
        std::cerr << "Failed to create device: " 
                  << deviceResult.error().message << "\n";
        return 1;
    }
    
    auto device = std::move(*deviceResult);
    
    // 打印使用的后端
    auto info = device->GetBackendInfo();
    std::cout << "Using backend: " << info.name << "\n";
    std::cout << "Device: " << info.deviceName << "\n";
    
    // 2. 创建顶点缓冲
    VRHI::BufferDesc bufferDesc;
    bufferDesc.size = sizeof(vertices);
    bufferDesc.usage = VRHI::BufferUsage::Vertex;
    bufferDesc.memoryAccess = VRHI::MemoryAccess::CpuToGpu;
    bufferDesc.initialData = vertices;
    bufferDesc.debugName = "Triangle Vertex Buffer";
    
    auto vertexBufferResult = device->CreateBuffer(bufferDesc);
    if (!vertexBufferResult) {
        std::cerr << "Failed to create vertex buffer\n";
        return 1;
    }
    
    auto vertexBuffer = std::move(*vertexBufferResult);
    
    // 3. 创建着色器
    VRHI::ShaderDesc vertexShaderDesc;
    vertexShaderDesc.stage = VRHI::ShaderStage::Vertex;
    vertexShaderDesc.code = vertexShaderSource;
    vertexShaderDesc.codeSize = strlen(vertexShaderSource);
    vertexShaderDesc.debugName = "Vertex Shader";
    
    auto vertexShaderResult = device->CreateShader(vertexShaderDesc);
    if (!vertexShaderResult) {
        std::cerr << "Failed to create vertex shader\n";
        return 1;
    }
    auto vertexShader = std::move(*vertexShaderResult);
    
    VRHI::ShaderDesc fragmentShaderDesc;
    fragmentShaderDesc.stage = VRHI::ShaderStage::Fragment;
    fragmentShaderDesc.code = fragmentShaderSource;
    fragmentShaderDesc.codeSize = strlen(fragmentShaderSource);
    fragmentShaderDesc.debugName = "Fragment Shader";
    
    auto fragmentShaderResult = device->CreateShader(fragmentShaderDesc);
    if (!fragmentShaderResult) {
        std::cerr << "Failed to create fragment shader\n";
        return 1;
    }
    auto fragmentShader = std::move(*fragmentShaderResult);
    
    // 4. 创建图形管线
    VRHI::PipelineDesc pipelineDesc;
    pipelineDesc.vertexShader = vertexShader.get();
    pipelineDesc.fragmentShader = fragmentShader.get();
    
    // 设置顶点输入布局
    pipelineDesc.vertexInputs = {
        { 0, 0, VRHI::VertexFormat::Float3, 0 },              // position
        { 1, 0, VRHI::VertexFormat::Float3, sizeof(float) * 3 }, // color
    };
    
    pipelineDesc.vertexBindings = {
        { 0, sizeof(Vertex), VRHI::VertexInputRate::Vertex },
    };
    
    auto pipelineResult = device->CreatePipeline(pipelineDesc);
    if (!pipelineResult) {
        std::cerr << "Failed to create pipeline\n";
        return 1;
    }
    auto pipeline = std::move(*pipelineResult);
    
    // 5. 渲染循环
    // 注意: WindowShouldClose() 和 PollEvents() 是窗口系统函数的示例
    // 实际使用时需要根据平台使用 GLFW、SDL 或原生窗口 API
    bool running = true;
    while (running) {
        // 创建命令缓冲
        auto cmd = device->CreateCommandBuffer();
        
        // 开始渲染
        cmd->BeginRenderPass(device->GetSwapChain()->GetCurrentRenderPass());
        
        // 设置视口和裁剪
        cmd->SetViewport(0, 0, 800, 600);
        cmd->SetScissor(0, 0, 800, 600);
        
        // 绑定管线
        cmd->BindPipeline(pipeline.get());
        
        // 绑定顶点缓冲
        cmd->BindVertexBuffer(0, vertexBuffer.get());
        
        // 绘制三角形
        cmd->Draw(3, 1, 0, 0);
        
        // 结束渲染
        cmd->EndRenderPass();
        
        // 提交命令
        device->Submit(std::move(cmd));
        
        // 呈现
        device->Present();
        
        // 处理窗口事件 (使用 GLFW/SDL 或平台特定 API)
        // 例如使用 GLFW:
        // glfwPollEvents();
        // running = !glfwWindowShouldClose(window);
    }
    
    // 6. 清理
    device->WaitIdle();
    
    std::cout << "Application closed successfully\n";
    return 0;
}
```

## 关键概念

### 1. 设备 (Device)

设备是 VRHI 的核心对象，代表一个图形设备。通过 `CreateDevice()` 创建。

```cpp
auto device = VRHI::CreateDevice().value();
```

### 2. 资源 (Resources)

资源包括缓冲区、纹理、采样器等。使用 RAII 自动管理生命周期。

```cpp
auto buffer = device->CreateBuffer(desc).value();
// buffer 离开作用域时自动释放
```

### 3. 命令缓冲 (CommandBuffer)

用于记录渲染命令，然后提交到 GPU 执行。

```cpp
auto cmd = device->CreateCommandBuffer();
cmd->Draw(3);
device->Submit(std::move(cmd));
```

### 4. 错误处理

VRHI 使用 `std::expected` 处理错误，避免异常。

```cpp
auto result = device->CreateBuffer(desc);
if (result) {
    auto buffer = std::move(*result);
} else {
    std::cerr << result.error().message << "\n";
}
```

## 后端选择

### 自动选择（推荐）

```cpp
// VRHI 自动选择最佳后端
auto device = VRHI::CreateDevice();
```

### 手动选择

```cpp
VRHI::DeviceConfig config;
config.preferredBackend = VRHI::BackendType::Vulkan;
auto device = VRHI::CreateDevice(config);
```

### 查看可用后端

```cpp
auto backends = VRHI::EnumerateBackends();
for (const auto& backend : backends) {
    std::cout << backend.name << " - Score: " << backend.score << "\n";
}
```

## 特性检测

```cpp
auto device = VRHI::CreateDevice().value();

// 检查单个特性
if (device->IsFeatureSupported(VRHI::Feature::Compute)) {
    std::cout << "Compute shaders supported\n";
}

// 查看所有特性
const auto& features = device->GetFeatures();
std::cout << "Max texture size: " << features.texture.maxTextureSize << "\n";
```

## 常见问题

### Q: 如何选择后端？

A: 默认情况下使用 `BackendType::Auto`，VRHI 会自动选择最佳后端。如果需要特定后端，可以在 `DeviceConfig` 中指定。

### Q: 如何处理不支持的特性？

A: 在创建设备时指定必需特性，如果不满足，`CreateDevice()` 会返回错误。

```cpp
config.features.required = { VRHI::Feature::Compute };
```

### Q: 如何调试？

A: 启用验证层：

```cpp
config.enableValidation = true;
config.logLevel = VRHI::LogLevel::Debug;
```

### Q: 资源何时释放？

A: VRHI 使用 RAII，资源在智能指针销毁时自动释放。确保在销毁设备前等待 GPU 完成：

```cpp
device->WaitIdle();
```

## 下一步

- 查看 [最佳实践](best_practices.md) 了解优化技巧
- 阅读 [API 参考](../api/core.md) 了解完整 API
- 学习 [跨平台开发](cross_platform.md) 适配不同平台

## 更多示例

完整示例代码请查看 GitHub 仓库的 `examples/` 目录。
