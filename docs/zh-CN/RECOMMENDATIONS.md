# VRHI 开发建议

**日期**: 2026-01-18

**状态**: 阶段 1-2 完成，阶段 3-6 进行中

## 快速摘要

✅ **已完成**: 阶段 1-2 的所有 27 个任务（基础设施与核心接口）  
✅ **已完成**: 19 个额外任务（窗口系统 + OpenGL 后端 + 示例）  
⚠️ **关键下一步**: 添加着色器编译库

## 已完成的工作

### 阶段 1-2: 基础设施与核心接口 ✅
- 支持 C++23 的构建系统
- 使用 `std::expected` 的核心类型和错误处理
- 设备抽象接口
- 后端工厂和评分系统
- 用于测试的 Mock 后端
- 全面的单元测试

### 额外成就 ✅
- **OpenGL 3.3 后端**: 大部分已实现
- **GLFW 窗口系统**: 完全可用
- **HelloTriangle 示例**: 可工作（编译通过，尚未在硬件上测试）
- **测试基础设施**: Google Test 已集成到 CI/CD

## 缺失的内容（v1.0 关键）

### 🔴 P0 - 关键: 着色器编译库

**状态**: 尚未添加  
**阻塞**: 阶段 5-6 实现  
**工作量**: 1-2 周

**所需库**:
1. **SPIRV-Headers** - https://github.com/KhronosGroup/SPIRV-Headers (仅头文件)
2. **SPIRV-Tools** - https://github.com/KhronosGroup/SPIRV-Tools (~12 MB)
3. **glslang** - https://github.com/KhronosGroup/glslang (~15 MB)
4. **SPIRV-Cross** - https://github.com/KhronosGroup/SPIRV-Cross (~8 MB)

**总大小影响**: ~35 MB 源码, ~12 MB 编译后 (Release)

**为什么关键**:
- GLSL → SPIR-V 编译
- SPIR-V → 后端特定的 GLSL 转换
- 着色器反射（uniforms、textures、bindings）
- OpenGL 和 Vulkan 后端都需要
- 支持纹理渲染示例

### 🟡 P1 - 高: 特性检测

**状态**: 接口已定义，实现待完成  
**阻塞**: 后端能力查询  
**工作量**: 1 周

### 🟡 P1 - 高: Vulkan 后端

**状态**: 未开始  
**阻塞**: 多后端演示  
**工作量**: 2-3 周

### 🟢 P2 - 中: 完成资源管理

**状态**: 部分实现  
**阻塞**: 高级示例  
**工作量**: 1-2 周

## 详细建议

查看综合文档:
- **English**: [docs/NEXT_STEPS.md](../NEXT_STEPS.md)
- **中文**: [docs/zh-CN/NEXT_STEPS.md](NEXT_STEPS.md)

## 即时行动计划

### 第 1 周: 添加着色器库
```bash
cd external/
# 添加 SPIRV-Headers
git clone --depth 1 --branch vulkan-sdk-1.3.275.0 \
  https://github.com/KhronosGroup/SPIRV-Headers.git

# 添加 SPIRV-Tools
git clone --depth 1 --branch vulkan-sdk-1.3.275.0 \
  https://github.com/KhronosGroup/SPIRV-Tools.git

# 添加 glslang
git clone --depth 1 --branch 14.0.0 \
  https://github.com/KhronosGroup/glslang.git

# 添加 SPIRV-Cross
git clone --depth 1 --branch vulkan-sdk-1.3.275.0 \
  https://github.com/KhronosGroup/SPIRV-Cross.git
```

### 第 2 周: 与 VRHI 集成
1. 更新 `external/CMakeLists.txt` 以包含着色器库
2. 实现支持 SPIR-V 的 `CreateShader()`
3. 为 OpenGL 后端添加 SPIR-V → GLSL 转换
4. 更新 HelloTriangle 使用编译的着色器
5. 在硬件上测试

### 第 3-4 周: 特性检测与完善
1. 实现 OpenGL 特性检测
2. 完成资源管理
3. 添加更多示例
4. 编写集成测试

### 第 5-7 周: Vulkan 后端
1. 实现 VulkanBackend 骨架
2. 基础三角形渲染
3. 多后端切换演示

## 仓库链接

所有推荐的库都是官方 Khronos 项目:

| 库 | 仓库 | 许可证 | 状态 |
|---------|-----------|---------|--------|
| SPIRV-Headers | https://github.com/KhronosGroup/SPIRV-Headers | MIT | ⬅️ 需要添加 |
| SPIRV-Tools | https://github.com/KhronosGroup/SPIRV-Tools | Apache 2.0 | ⬅️ 需要添加 |
| glslang | https://github.com/KhronosGroup/glslang | BSD 3-Clause | ⬅️ 需要添加 |
| SPIRV-Cross | https://github.com/KhronosGroup/SPIRV-Cross | Apache 2.0 | ⬅️ 需要添加 |
| GLFW | https://github.com/glfw/glfw | Zlib | ✅ 已添加 |
| GoogleTest | https://github.com/google/googletest | BSD 3-Clause | ✅ 已添加 |

**未来 (v2.0)**:
- VulkanMemoryAllocator: https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator
- Slang: https://github.com/shader-slang/slang

## 进度跟踪

在以下位置跟踪详细进度:
- [docs/KANBAN.md](../KANBAN.md) - 详细任务板
- [docs/ROADMAP_v1.0.md](ROADMAP_v1.0.md) - 逐阶段路线图

**当前阶段**: 阶段 3-4（后端实现）  
**当前冲刺**: 着色器编译集成  
**下一个里程碑**: M2 - 完成 OpenGL 后端（第 11 周）

## 风险评估

✅ **低风险**: 所有库都是业界标准，积极维护  
⚠️ **中等风险**: 构建复杂度增加，需要版本管理  
❌ **无风险**: 所有库从源码编译，仍支持离线构建

## 问题与讨论

请使用:
- GitHub Issues: https://github.com/zzxzzk115/VRHI/issues
- GitHub Discussions: https://github.com/zzxzzk115/VRHI/discussions

---

**最后更新**: 2026-01-18  
**状态**: 准备好进行着色器库集成
