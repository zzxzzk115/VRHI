# VRHI v1.0 下一步计划和建议

**最后更新**: 2026-01-18

## 执行摘要

基于对所有文档和 KANBAN 看板的全面审查，本文档为 VRHI 开发的下一阶段提供建议。项目已成功完成阶段 1-2（基础设施与核心接口），并在后端实现上取得了重大进展。

## 当前状态审查

### ✅ 已完成（阶段 1-2）
阶段 1-2 的所有 27 个任务都标记为 **Done**：

#### 构建系统与项目结构 (9/9 完成)
- ✅ 使用 C++23 配置的根 CMakeLists.txt
- ✅ `external/` 目录结构已建立
- ✅ Vulkan 和 OpenGL 检测已配置
- ✅ 子目录的模块化 CMakeLists
- ✅ 构建选项和平台检测
- ✅ 安装目标已配置

#### 核心类型与错误处理 (8/8 完成)
- ✅ 带错误码的 Error 类
- ✅ BackendType 和 Feature 枚举
- ✅ DeviceConfig 和 FeatureRequirements 结构体
- ✅ 基础类型和 std::expected 别名
- ✅ 错误处理单元测试

#### 设备抽象 (8/8 完成)
- ✅ IBackend、IDevice、ICommandBuffer 接口已定义
- ✅ IBuffer、ITexture、IPipeline、IShader 接口已定义
- ✅ 用于测试的 Mock 后端已实现

#### 核心 API 函数 (5/5 完成)
- ✅ EnumerateBackends() 和 CreateDevice() 签名
- ✅ 后端注册表和工厂模式
- ✅ 初始化/关闭函数

#### 后端评分与选择 (8/8 完成)
- ✅ 后端评分算法（40% 特性，30% 性能，20% 稳定性，10% 兼容性）
- ✅ 平台原生 API 加分逻辑
- ✅ 所有评分组件已实现
- ✅ 评分系统单元测试

#### 测试基础设施 (3/3 完成)
- ✅ Google Test 框架已设置
- ✅ 核心类型和错误处理测试
- ✅ CI/CD 流水线正常运行

### 🚧 进行中
- **OpenGL 3.3 后端**: 大部分已实现（设备、资源、命令缓冲区）
- **GLFW 窗口系统**: 已完全实现并带有示例
- **HelloTriangle 示例**: 已实现并可编译

### ❌ 未开始（高优先级）
以下关键组件仍处于 **Backlog** 状态：

1. **着色器编译系统**（任务 R4.1-R4.7）- **关键**
2. **特性检测**（任务 B2.1-B2.7）
3. **Vulkan 后端**（任务 R6.1-R6.8）
4. **完整资源管理**（任务 R1.1-R1.8、R2.1-R2.7、R3.1-R3.7）

## 关键建议：添加着色器编译库

### 优先级：**P0（关键）**

着色器编译系统对 VRHI v1.0 至关重要。根据[着色器系统设计](design/shader_system.md)，需要以下库：

### 1. glslang（GLSL → SPIR-V 编译器）

**仓库**: https://github.com/KhronosGroup/glslang

**版本**: 最新稳定版（推荐 v14.x 或更高）

**许可证**: BSD 3-Clause（与 MIT 兼容）

**用途**: 
- 将 GLSL 4.5+ 着色器编译为 SPIR-V 中间表示
- 官方 Khronos 参考编译器
- Vulkan 和 OpenGL 后端都需要

**集成步骤**:
```bash
# 添加到 external/
cd external/
git clone --depth 1 --branch 14.0.0 https://github.com/KhronosGroup/glslang.git
```

**CMake 集成**:
```cmake
# 在 external/CMakeLists.txt 中
add_subdirectory(glslang)
# 链接目标: glslang, SPIRV
```

**大小**: ~15 MB（源码），~5 MB（编译后）

### 2. SPIRV-Cross（SPIR-V → GLSL/MSL/HLSL 转换器）

**仓库**: https://github.com/KhronosGroup/SPIRV-Cross

**版本**: 最新稳定版（推荐 v2024-01-04 或更高）

**许可证**: Apache 2.0（与 MIT 兼容）

**用途**:
- 将 SPIR-V 转换为后端特定的着色器语言
- Vulkan → 直接使用 SPIR-V
- OpenGL → 转换为 GLSL 3.30/4.10/4.60
- Metal (v2.0) → 转换为 MSL
- D3D12 (v2.0) → 转换为 HLSL
- 着色器反射（uniforms、attributes、bindings）

**集成步骤**:
```bash
# 添加到 external/
cd external/
git clone --depth 1 --branch vulkan-sdk-1.3.275.0 https://github.com/KhronosGroup/SPIRV-Cross.git
```

**CMake 集成**:
```cmake
# 在 external/CMakeLists.txt 中
add_subdirectory(SPIRV-Cross)
# 链接目标: spirv-cross-core, spirv-cross-glsl, spirv-cross-msl, spirv-cross-hlsl
```

**大小**: ~8 MB（源码），~3 MB（编译后）

### 3. SPIRV-Tools（SPIR-V 优化与验证）

**仓库**: https://github.com/KhronosGroup/SPIRV-Tools

**版本**: 与 glslang 版本匹配的最新稳定版

**许可证**: Apache 2.0（与 MIT 兼容）

**用途**:
- 验证 SPIR-V 字节码的正确性
- 优化 SPIR-V 以提高性能
- glslang 所需

**集成步骤**:
```bash
# 添加到 external/
cd external/
git clone --depth 1 --branch vulkan-sdk-1.3.275.0 https://github.com/KhronosGroup/SPIRV-Tools.git
```

**CMake 集成**:
```cmake
# 在 external/CMakeLists.txt 中
# 注意: SPIRV-Tools 需要 SPIRV-Headers 作为依赖
add_subdirectory(SPIRV-Tools)
```

**大小**: ~12 MB（源码），~4 MB（编译后）

### 4. SPIRV-Headers（SPIR-V 定义）

**仓库**: https://github.com/KhronosGroup/SPIRV-Headers

**版本**: 与 SPIRV-Tools 匹配的最新稳定版

**许可证**: MIT（与 VRHI 相同）

**用途**:
- 仅头文件库，包含 SPIR-V 定义
- SPIRV-Tools 和 glslang 所需

**集成步骤**:
```bash
# 添加到 external/
cd external/
git clone --depth 1 --branch vulkan-sdk-1.3.275.0 https://github.com/KhronosGroup/SPIRV-Headers.git
```

**CMake 集成**:
```cmake
# 在 external/CMakeLists.txt 中
add_subdirectory(SPIRV-Headers)
```

**大小**: ~500 KB（仅头文件）

## 推荐实施顺序

### 阶段 1：着色器编译基础设施（第 1-2 周）

**任务**（目前在 Backlog 中的 R4.1-R4.7）：

1. **添加 SPIRV-Headers**（任务 R4.1 - 工作量: S）
   - 克隆到 `external/SPIRV-Headers/`
   - 添加到 `external/CMakeLists.txt`
   - 验证编译

2. **添加 SPIRV-Tools**（任务 R4.2 - 工作量: M）
   - 克隆到 `external/SPIRV-Tools/`
   - 配置 CMake 选项（禁用测试）
   - 链接 SPIRV-Headers
   - 验证编译

3. **添加 glslang**（任务 R4.3 - 工作量: M）
   - 克隆到 `external/glslang/`
   - 链接 SPIRV-Tools 和 SPIRV-Headers
   - 验证 GLSL → SPIR-V 编译工作正常

4. **添加 SPIRV-Cross**（任务 R4.4 - 工作量: M）
   - 克隆到 `external/SPIRV-Cross/`
   - 配置 CMake（启用 GLSL 后端，暂时禁用其他）
   - 验证 SPIR-V → GLSL 转换工作正常

5. **与 Shader API 集成**（任务 R4.5 - 工作量: M）
   - 实现支持 SPIR-V 输入的 `CreateShader()`
   - 为 OpenGL 后端实现运行时 SPIR-V → GLSL 转换
   - 添加着色器编译示例

6. **实现着色器反射**（任务 R4.6 - 工作量: L）
   - 使用 SPIRV-Cross 反射 API
   - 提取 uniform buffers、textures、attributes
   - 自动填充管线布局

7. **添加着色器缓存**（任务 R4.7 - 工作量: M）
   - 缓存编译后的 SPIR-V 字节码
   - 缓存转换后的 GLSL 源码
   - 实现缓存失效

**验收标准**:
- ✅ 所有库在 Windows、Linux、macOS 上编译
- ✅ 可以编译 GLSL 4.5 → SPIR-V
- ✅ 可以转换 SPIR-V → GLSL 3.30（用于 OpenGL 3.3）
- ✅ 着色器反射提取 uniform/texture 绑定
- ✅ HelloTriangle 示例使用编译的着色器

### 阶段 2：特性检测（第 3 周）

**任务**（目前在 Backlog 中的 B2.1-B2.7）：

1. 为 OpenGL 后端实现 `IsFeatureSupported()`
2. 查询 OpenGL 扩展和能力
3. 将 OpenGL 特性映射到 VRHI 特性枚举
4. 在设备创建前添加特性验证
5. 编写特性检测测试

### 阶段 3：完成资源管理（第 4-5 周）

**任务**（目前在 Backlog 中的 R1.1-R3.7）：

1. 完成 Buffer 实现，包含所有用途标志
2. 完成 Texture 实现，包含所有格式
3. 完成 Shader 和 Pipeline 管理
4. 添加资源状态跟踪
5. 实现正确的销毁顺序
6. 使用 ASAN/Valgrind 测试资源生命周期

### 阶段 4：Vulkan 后端骨架（第 6-7 周）

**任务**（目前在 Backlog 中的 R6.1-R6.8）：

1. 链接 Vulkan SDK
2. 实现 VulkanBackend 和 VulkanDevice 类
3. 创建 Vulkan 实例和逻辑设备
4. 实现基本资源创建（buffer、texture、shader）
5. 验证使用 Vulkan 渲染三角形
6. 测试多后端切换

## 其他依赖（较低优先级）

### v1.x 可选

#### 1. Vulkan Memory Allocator (VMA)

**仓库**: https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator

**用途**: 高效的 Vulkan 内存分配和管理

**优先级**: P1（实现 Vulkan 后端时）

#### 2. vk-bootstrap

**仓库**: https://github.com/charles-lunarg/vk-bootstrap

**用途**: 简化 Vulkan 初始化

**优先级**: P2（锦上添花，减少样板代码）

### v2.0（未来）

#### 1. Slang 着色器编译器

**仓库**: https://github.com/shader-slang/slang

**用途**: 现代着色器语言，编译到所有后端

**优先级**: P3（计划在 v2.0）

## 添加依赖后的目录结构

```
external/
├── CMakeLists.txt
├── README.md
├── glad_gl33/              # ✅ 已存在
├── glad_gl46/              # ✅ 已存在
├── glfw-3.4/               # ✅ 已存在
├── googletest-1.17.0/      # ✅ 已存在
├── SPIRV-Headers/          # ⬅️ 添加这个（P0）
├── SPIRV-Tools/            # ⬅️ 添加这个（P0）
├── glslang/                # ⬅️ 添加这个（P0）
├── SPIRV-Cross/            # ⬅️ 添加这个（P0）
└── VulkanMemoryAllocator/  # ⬅️ 稍后添加（P1，用于 Vulkan 后端）
```

## CMake 配置示例

```cmake
# external/CMakeLists.txt

# 禁用第三方库的测试
set(SPIRV_SKIP_TESTS ON CACHE BOOL "" FORCE)
set(SPIRV_SKIP_EXECUTABLES ON CACHE BOOL "" FORCE)
set(GLSLANG_TESTS OFF CACHE BOOL "" FORCE)
set(ENABLE_GLSLANG_BINARIES OFF CACHE BOOL "" FORCE)

# SPIRV-Headers（仅头文件）
add_subdirectory(SPIRV-Headers)

# SPIRV-Tools
add_subdirectory(SPIRV-Tools)

# glslang
add_subdirectory(glslang)

# SPIRV-Cross
set(SPIRV_CROSS_CLI OFF CACHE BOOL "" FORCE)
set(SPIRV_CROSS_ENABLE_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory(SPIRV-Cross)

# 为着色器编译创建接口库
add_library(VRHI_ShaderCompiler INTERFACE)
target_link_libraries(VRHI_ShaderCompiler INTERFACE
    glslang
    SPIRV
    spirv-cross-core
    spirv-cross-glsl
)
```

## 预期构建大小影响

| 组件 | 源码大小 | 构建大小（Debug） | 构建大小（Release） |
|------|---------|-----------------|-------------------|
| SPIRV-Headers | ~500 KB | 仅头文件 | 仅头文件 |
| SPIRV-Tools | ~12 MB | ~15 MB | ~4 MB |
| glslang | ~15 MB | ~25 MB | ~5 MB |
| SPIRV-Cross | ~8 MB | ~12 MB | ~3 MB |
| **总计** | **~35 MB** | **~52 MB** | **~12 MB** |

**注意**: 这些是增量大小。VRHI 项目总大小会增加，但对于图形 API 抽象层来说是可接受的。

## 构建时间影响

- **初始 CMake 配置**: +30-60 秒
- **完整构建**: +2-4 分钟
- **增量构建**: 影响最小（仅当着色器代码更改时）

## 下一步 KANBAN 更新

添加着色器库后，以下任务应从 **Backlog** 移至 **In Progress**：

1. **R4.1**: 集成 glslang 库 → **Done**
2. **R4.2**: 集成 SPIRV-Cross 库 → **Done**
3. **R4.3**: 实现 GLSL → SPIR-V 编译 → **In Progress**
4. **R4.4**: 实现 SPIR-V → GLSL 转换 → **In Progress**
5. **R4.5**: 实现着色器反射 → **In Progress**
6. **R4.6**: 添加着色器缓存 → **Backlog**
7. **R4.7**: 创建着色器编译示例 → **Backlog**

## 风险评估

### 低风险 ✅
- 所有推荐的库都是官方 Khronos 项目
- 许可证兼容（Apache 2.0、BSD、MIT）
- 在生产环境中广泛使用（Vulkan SDK、主要引擎）
- 积极维护

### 中等风险 ⚠️
- 构建复杂度增加（更多 CMake 依赖）
- 构建时间适度增加
- 需要管理多个库版本

**缓解措施**:
- 固定特定的稳定版本（使用 git 标签）
- 记录版本兼容性矩阵
- 在提交前在所有平台上测试

### 无风险 ❌
- 不依赖外部二进制运行时
- 所有库从源码编译
- 仍支持离线构建

## 结论

**需要立即行动**: 添加四个着色器编译库（SPIRV-Headers、SPIRV-Tools、glslang、SPIRV-Cross），以完成 VRHI v1.0 路线图的阶段 5-6。

**时间线**: 添加这些依赖并实现基本着色器编译应需要 1-2 周。

**影响**: 这将解锁：
- 完整的 OpenGL 3.3 后端实现
- Vulkan 后端实现
- 基于着色器的示例（纹理立方体、高级渲染）
- v1.0 功能完整性

**下一步**:
1. 审查并批准此建议
2. 将着色器编译库添加到 `external/`
3. 更新 KANBAN：将 R4.1-R4.4 标记为"In Progress"
4. 实现着色器编译集成
5. 更新 KANBAN：将 R4.1-R4.4 标记为"Done"
6. 进入阶段 6：Vulkan 后端骨架

---

**有疑问或顾虑？** 在进行集成之前，请在 GitHub Issues 或 Discussions 中讨论。
