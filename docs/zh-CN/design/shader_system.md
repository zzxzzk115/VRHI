# 着色器系统设计

## 概述

VRHI 的着色器系统需要支持多种图形后端，每个后端对着色器格式有不同要求。本文档详细说明着色器编译、转换和管理的设计决策。

## 核心问题

1. **着色器源语言**：应该使用哪种着色器语言作为主要开发语言？
2. **跨平台编译**：如何将着色器编译到不同后端的原生格式？
3. **运行时支持**：是否需要运行时着色器编译？

## 设计方案

### 方案概述

VRHI 采用 **SPIR-V 为中心的着色器系统**：

```
GLSL/HLSL/Slang (源码)
    ↓
SPIR-V (中间表示)
    ↓
SPIRV-Cross (转换工具)
    ↓
各后端原生格式 (GLSL, MSL, HLSL)
```

### 推荐的着色器工作流

**v1.0 方案（简单可靠）**：
```
开发者编写 GLSL 4.5+
    ↓
glslangValidator 编译为 SPIR-V
    ↓
SPIRV-Cross 转换为目标格式：
    - Vulkan: 直接使用 SPIR-V
    - OpenGL: 转换回 GLSL (特定版本)
    - Metal (v2.0): 转换为 MSL
    - D3D12 (v2.0): 转换为 HLSL
```

**v2.0+ 方案（高级）**：
```
开发者编写 Slang
    ↓
Slang 编译器生成 SPIR-V
    ↓
SPIRV-Cross 转换为目标格式
```

## 技术选型分析

### 1. GLAD vs 手动加载器

**GLAD 版本策略**：

为每个 OpenGL 版本生成独立的 GLAD 加载器：

```
external/glad/
├── glad_gl33/     # OpenGL 3.3 Core
│   ├── include/
│   └── src/
├── glad_gl41/     # OpenGL 4.1 Core
│   ├── include/
│   └── src/
└── glad_gl46/     # OpenGL 4.6 Core
    ├── include/
    └── src/
```

**配置参数**：
- API: `gl=3.3/4.1/4.6`
- Profile: `core`
- Extensions: 根据需要选择
- Loader: `on`

**运行时选择**：
```cpp
// 根据选择的 OpenGL 后端版本加载对应的函数
switch (backendType) {
    case BackendType::OpenGL33:
        gladLoadGL33();  // 加载 GL 3.3 函数
        break;
    case BackendType::OpenGL41:
        gladLoadGL41();  // 加载 GL 4.1 函数
        break;
    case BackendType::OpenGL46:
        gladLoadGL46();  // 加载 GL 4.6 函数
        break;
}
```

**优势**：
- ✅ 精确控制每个版本的函数集
- ✅ 避免不必要的扩展检测
- ✅ 更小的二进制体积（每个后端只链接所需版本）
- ✅ 编译时类型安全

### 2. 着色器语言选择

#### 选项 A: GLSL（推荐 v1.0）

**优势**：
- ✅ 成熟稳定，文档完善
- ✅ glslangValidator 官方支持
- ✅ 与 OpenGL 后端原生兼容
- ✅ 大量现有资源和教程
- ✅ Vulkan SPIR-V 官方路径

**劣势**：
- ❌ 需要为不同后端编写不同版本
- ❌ 语法相对老旧（相比现代语言）

**示例工作流**：
```glsl
// vertex.vert (GLSL 4.5)
#version 450

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

layout(location = 0) out vec2 v_TexCoord;

void main() {
    gl_Position = vec4(a_Position, 1.0);
    v_TexCoord = a_TexCoord;
}
```

编译：
```bash
glslangValidator -V vertex.vert -o vertex.spv
```

#### 选项 B: Slang（推荐 v2.0+）

**优势**：
- ✅ 现代语法，类似 HLSL/C++
- ✅ 一次编写，多平台支持
- ✅ 强大的模块系统和泛型
- ✅ 自动生成 SPIR-V
- ✅ 更好的编译时优化

**劣势**：
- ❌ 相对较新，生态系统较小
- ❌ 学习曲线（对 GLSL 用户）
- ❌ 需要额外的编译器依赖

**平台覆盖分析**：

| 后端 | Slang 支持 | 输出格式 |
|------|-----------|---------|
| Vulkan | ✅ | SPIR-V |
| OpenGL 3.3+ | ✅ | GLSL (via SPIRV-Cross) |
| OpenGL ES 3.0+ | ✅ | GLSL ES (via SPIRV-Cross) |
| Direct3D 12 | ✅ | DXIL/HLSL |
| Metal | ✅ | MSL (via SPIRV-Cross) |
| WebGPU | ✅ | WGSL/SPIR-V |

**结论**：Slang 可以覆盖所有规划的平台！

**示例工作流**：
```slang
// shader.slang
struct VertexInput {
    float3 position : POSITION;
    float2 texCoord : TEXCOORD;
};

struct VertexOutput {
    float4 position : SV_Position;
    float2 texCoord : TEXCOORD;
};

VertexOutput vertexMain(VertexInput input) {
    VertexOutput output;
    output.position = float4(input.position, 1.0);
    output.texCoord = input.texCoord;
    return output;
}
```

编译：
```bash
slangc shader.slang -target spirv -o shader.spv
```

#### 选项 C: HLSL

**优势**：
- ✅ Windows/Xbox 原生支持
- ✅ 现代语法
- ✅ DXC 编译器可生成 SPIR-V

**劣势**：
- ❌ 在非微软平台支持较弱
- ❌ OpenGL 转换可能有兼容性问题

### 3. SPIRV-Cross 集成

**用途**：
- 将 SPIR-V 转换为各后端的着色器语言
- 反射着色器元数据（uniform、attribute 等）

**支持的目标**：
- ✅ GLSL (各版本)
- ✅ GLSL ES
- ✅ MSL (Metal Shading Language)
- ✅ HLSL
- ✅ C++ (用于 CPU 侧反射)

**集成方式**：
```cpp
// 使用 SPIRV-Cross 转换
spirv_cross::CompilerGLSL compiler(spirv_binary);
compiler.set_common_options({
    .version = 330,  // GLSL 3.30
    .es = false,
    .enable_420pack_extension = false
});

std::string glsl_source = compiler.compile();
```

**平台覆盖**：SPIRV-Cross 可以覆盖所有规划的平台！

| 后端 | 输入 | 输出 | SPIRV-Cross 角色 |
|------|------|------|------------------|
| Vulkan | SPIR-V | SPIR-V | 不需要（直接使用） |
| OpenGL 4.6 | SPIR-V | GLSL 4.60 | 转换 |
| OpenGL 4.1 | SPIR-V | GLSL 4.10 | 转换 |
| OpenGL 3.3 | SPIR-V | GLSL 3.30 | 转换 |
| OpenGL ES 3.1 | SPIR-V | GLSL ES 3.10 | 转换 |
| OpenGL ES 3.0 | SPIR-V | GLSL ES 3.00 | 转换 |
| Metal | SPIR-V | MSL | 转换 |
| D3D12 | SPIR-V | HLSL | 转换 |

## 推荐方案

### v1.0 实现策略

**着色器语言**：GLSL 4.5+
- 稳定可靠，文档丰富
- 与 Vulkan/OpenGL 无缝集成
- 降低初期开发风险

**OpenGL 函数加载**：多版本 GLAD
- `glad_gl33`、`glad_gl41`、`glad_gl46` 独立加载器
- 编译时生成，运行时按需加载

**编译工具链**：
1. **glslangValidator**: GLSL → SPIR-V
2. **SPIRV-Cross**: SPIR-V → GLSL (各版本)
3. **SPIRV-Tools**: SPIR-V 优化和验证

**工作流**：
```
开发者编写 GLSL 4.5
    ↓
构建时：glslangValidator → SPIR-V
    ↓
运行时：
    - Vulkan: 直接加载 SPIR-V
    - OpenGL: SPIRV-Cross → 对应版本 GLSL → glCompileShader
```

### v2.0 升级路径

**着色器语言**：迁移到 Slang
- 现代语法，更好的开发体验
- 一次编写，多平台部署
- 更强大的类型系统和模块化

**保持兼容**：
- 继续支持 GLSL（通过 glslang）
- 支持 Slang（通过 slangc）
- 用户可选择任一方式

## 依赖库总结

### v1.0 必需依赖

**external/glad/** (多版本)
- `glad_gl33/`: OpenGL 3.3 Core
- `glad_gl41/`: OpenGL 4.1 Core
- `glad_gl46/`: OpenGL 4.6 Core
- 来源: https://glad.dav1d.de/
- 许可: MIT/Public Domain

**external/glslang/**
- GLSL 编译器，生成 SPIR-V
- 来源: https://github.com/KhronosGroup/glslang
- 许可: BSD 3-Clause

**external/SPIRV-Cross/**
- SPIR-V 反编译和转换工具
- 来源: https://github.com/KhronosGroup/SPIRV-Cross
- 许可: Apache 2.0

**external/SPIRV-Tools/**
- SPIR-V 优化和验证
- 来源: https://github.com/KhronosGroup/SPIRV-Tools
- 许可: Apache 2.0

### v2.0 可选依赖

**external/slang/**
- Slang 着色器编译器
- 来源: https://github.com/shader-slang/slang
- 许可: MIT

## 着色器 API 设计

### 着色器创建接口

```cpp
// 从 SPIR-V 创建（推荐）
struct ShaderDesc {
    ShaderStage stage;              // Vertex, Fragment, Compute...
    std::span<const uint32_t> spirv; // SPIR-V 二进制
    std::string_view entryPoint = "main";
};

auto shader = device->CreateShader(desc);

// 从源码创建（可选，主要用于调试）
struct ShaderSourceDesc {
    ShaderStage stage;
    std::string_view source;  // GLSL/HLSL/MSL 源码
    ShaderLanguage language;  // 指定语言
    std::string_view entryPoint = "main";
};

auto shader = device->CreateShaderFromSource(desc);
```

### 着色器模块（v2.0+）

```cpp
// 支持多入口点的着色器模块
struct ShaderModuleDesc {
    std::span<const uint32_t> spirv;
    std::vector<std::string> entryPoints;
};

auto module = device->CreateShaderModule(desc);
auto vs = module->GetShader("vertexMain");
auto fs = module->GetShader("fragmentMain");
```

## 构建系统集成

### CMake 配置

```cmake
# GLAD 多版本
add_library(glad_gl33 STATIC
    external/glad/glad_gl33/src/glad.c
)
target_include_directories(glad_gl33 PUBLIC
    external/glad/glad_gl33/include
)

# glslang
add_subdirectory(external/glslang)

# SPIRV-Cross
add_subdirectory(external/SPIRV-Cross)

# 着色器编译工具
add_executable(vrhi-shader-compiler
    tools/shader_compiler/main.cpp
)
target_link_libraries(vrhi-shader-compiler
    glslang
    SPIRV
    spirv-cross-core
    spirv-cross-glsl
)
```

### 着色器编译脚本

```cmake
# 编译 GLSL → SPIR-V
function(compile_shader TARGET SHADER_FILE)
    get_filename_component(FILE_NAME ${SHADER_FILE} NAME)
    set(SPIRV_OUTPUT ${CMAKE_BINARY_DIR}/shaders/${FILE_NAME}.spv)
    
    add_custom_command(
        OUTPUT ${SPIRV_OUTPUT}
        COMMAND glslangValidator -V ${SHADER_FILE} -o ${SPIRV_OUTPUT}
        DEPENDS ${SHADER_FILE}
        COMMENT "Compiling shader: ${SHADER_FILE}"
    )
    
    target_sources(${TARGET} PRIVATE ${SPIRV_OUTPUT})
endfunction()
```

## 运行时着色器反射

使用 SPIRV-Cross 进行反射：

```cpp
// 反射 uniform 信息
spirv_cross::Compiler compiler(spirv);
spirv_cross::ShaderResources resources = compiler.get_shader_resources();

// 遍历 uniform buffers
for (const auto& ubo : resources.uniform_buffers) {
    std::string name = compiler.get_name(ubo.id);
    uint32_t binding = compiler.get_decoration(ubo.id, spv::DecorationBinding);
    // ...
}

// 遍历 textures
for (const auto& image : resources.sampled_images) {
    std::string name = compiler.get_name(image.id);
    uint32_t binding = compiler.get_decoration(image.id, spv::DecorationBinding);
    // ...
}
```

## 总结

### 关键决策

1. **GLAD 版本**: 为每个 OpenGL 版本生成独立的 GLAD 加载器
2. **v1.0 着色器语言**: GLSL 4.5+（稳定、成熟）
3. **v2.0 着色器语言**: Slang（现代、强大）
4. **SPIRV-Cross**: 必需，用于跨平台着色器转换
5. **编译策略**: 构建时 GLSL → SPIR-V，运行时按需转换

### 平台覆盖确认

| 工具 | 所有平台覆盖 | 备注 |
|------|-------------|------|
| GLAD | ✅ | 仅 OpenGL 平台 |
| glslang | ✅ | 生成 SPIR-V |
| SPIRV-Cross | ✅ | 转换到所有后端 |
| Slang | ✅ | 可选，v2.0+ |

### 开发建议

**v1.0 阶段**：
- 使用 GLSL 4.5 编写着色器
- 提供示例着色器和编译脚本
- 文档说明如何添加自定义着色器

**v2.0 阶段**：
- 逐步迁移到 Slang
- 提供 GLSL → Slang 迁移指南
- 同时支持两种方式
