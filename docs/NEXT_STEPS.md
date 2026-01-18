# VRHI v1.0 Next Steps and Recommendations

**Last Updated**: 2026-01-18

## Executive Summary

Based on a comprehensive review of all documentation and the KANBAN board, this document provides recommendations for the next phase of VRHI development. The project has successfully completed Phase 1-2 (Foundation & Core Interface) and has made significant progress on backend implementation.

## Current Status Review

### ✅ Completed (Phase 1-2)
All 27 tasks in Phase 1-2 are marked as **Done**:

#### Build System & Project Structure (9/9 Done)
- ✅ Root CMakeLists.txt with C++23 configuration
- ✅ `external/` directory structure established
- ✅ Vulkan and OpenGL detection configured
- ✅ Modular CMakeLists for subdirectories
- ✅ Build options and platform detection
- ✅ Install targets configured

#### Core Types & Error Handling (8/8 Done)
- ✅ Error class with error codes
- ✅ BackendType and Feature enums
- ✅ DeviceConfig and FeatureRequirements structs
- ✅ Base types and std::expected aliases
- ✅ Unit tests for error handling

#### Device Abstraction (8/8 Done)
- ✅ IBackend, IDevice, ICommandBuffer interfaces defined
- ✅ IBuffer, ITexture, IPipeline, IShader interfaces defined
- ✅ Mock backend for testing implemented

#### Core API Functions (5/5 Done)
- ✅ EnumerateBackends() and CreateDevice() signatures
- ✅ Backend registry and factory pattern
- ✅ Initialization/shutdown functions

#### Backend Scoring & Selection (8/8 Done)
- ✅ Backend scoring algorithm (40% features, 30% performance, 20% stability, 10% compatibility)
- ✅ Platform-native API bonus logic
- ✅ All scoring components implemented
- ✅ Unit tests for scoring system

#### Testing Infrastructure (3/3 Done)
- ✅ Google Test framework set up
- ✅ Tests for core types and error handling
- ✅ CI/CD pipeline functional

### 🚧 In Progress
- **OpenGL 3.3 Backend**: Substantially implemented (device, resources, command buffers)
- **GLFW Window System**: Fully implemented with example
- **HelloTriangle Example**: Implemented and compiles

### ❌ Not Started (High Priority)
The following critical components are still in **Backlog** status:

1. **Shader Compilation System** (Tasks R4.1-R4.7) - **CRITICAL**
2. **Feature Detection** (Tasks B2.1-B2.7)
3. **Vulkan Backend** (Tasks R6.1-R6.8)
4. **Complete Resource Management** (Tasks R1.1-R1.8, R2.1-R2.7, R3.1-R3.7)

## Critical Recommendation: Add Shader Compilation Libraries

### Priority: **P0 (Critical)**

The shader compilation system is essential for VRHI v1.0. According to the [Shader System Design](zh-CN/design/shader_system.md), the following libraries are required:

### 1. glslang (GLSL → SPIR-V Compiler)

**Repository**: https://github.com/KhronosGroup/glslang

**Version**: Latest stable (recommend v14.x or later)

**License**: BSD 3-Clause (Compatible with MIT)

**Purpose**: 
- Compile GLSL 4.5+ shaders to SPIR-V intermediate representation
- Official Khronos reference compiler
- Essential for both Vulkan and OpenGL backends

**Integration Steps**:
```bash
# Add to external/
cd external/
git clone --depth 1 --branch 14.0.0 https://github.com/KhronosGroup/glslang.git
```

**CMake Integration**:
```cmake
# In external/CMakeLists.txt
add_subdirectory(glslang)
# Link targets: glslang, SPIRV
```

**Size**: ~15 MB (source), ~5 MB (compiled)

### 2. SPIRV-Cross (SPIR-V → GLSL/MSL/HLSL Converter)

**Repository**: https://github.com/KhronosGroup/SPIRV-Cross

**Version**: Latest stable (recommend v2024-01-04 or later)

**License**: Apache 2.0 (Compatible with MIT)

**Purpose**:
- Convert SPIR-V to backend-specific shader languages
- Vulkan → Direct SPIR-V usage
- OpenGL → Convert to GLSL 3.30/4.10/4.60
- Metal (v2.0) → Convert to MSL
- D3D12 (v2.0) → Convert to HLSL
- Shader reflection (uniforms, attributes, bindings)

**Integration Steps**:
```bash
# Add to external/
cd external/
git clone --depth 1 --branch vulkan-sdk-1.3.275.0 https://github.com/KhronosGroup/SPIRV-Cross.git
```

**CMake Integration**:
```cmake
# In external/CMakeLists.txt
add_subdirectory(SPIRV-Cross)
# Link targets: spirv-cross-core, spirv-cross-glsl, spirv-cross-msl, spirv-cross-hlsl
```

**Size**: ~8 MB (source), ~3 MB (compiled)

### 3. SPIRV-Tools (SPIR-V Optimization & Validation)

**Repository**: https://github.com/KhronosGroup/SPIRV-Tools

**Version**: Latest stable matching glslang version

**License**: Apache 2.0 (Compatible with MIT)

**Purpose**:
- Validate SPIR-V bytecode correctness
- Optimize SPIR-V for performance
- Required by glslang

**Integration Steps**:
```bash
# Add to external/
cd external/
git clone --depth 1 --branch vulkan-sdk-1.3.275.0 https://github.com/KhronosGroup/SPIRV-Tools.git
```

**CMake Integration**:
```cmake
# In external/CMakeLists.txt
# Note: SPIRV-Tools requires SPIRV-Headers as a dependency
add_subdirectory(SPIRV-Tools)
```

**Size**: ~12 MB (source), ~4 MB (compiled)

### 4. SPIRV-Headers (SPIR-V Definitions)

**Repository**: https://github.com/KhronosGroup/SPIRV-Headers

**Version**: Latest stable matching SPIRV-Tools

**License**: MIT (Same as VRHI)

**Purpose**:
- Header-only library with SPIR-V definitions
- Required by SPIRV-Tools and glslang

**Integration Steps**:
```bash
# Add to external/
cd external/
git clone --depth 1 --branch vulkan-sdk-1.3.275.0 https://github.com/KhronosGroup/SPIRV-Headers.git
```

**CMake Integration**:
```cmake
# In external/CMakeLists.txt
add_subdirectory(SPIRV-Headers)
```

**Size**: ~500 KB (header-only)

## Recommended Implementation Order

### Phase 1: Shader Compilation Infrastructure (Week 1-2)

**Tasks** (Currently R4.1-R4.7 in Backlog):

1. **Add SPIRV-Headers** (Task R4.1 - effort: S)
   - Clone to `external/SPIRV-Headers/`
   - Add to `external/CMakeLists.txt`
   - Verify compilation

2. **Add SPIRV-Tools** (Task R4.2 - effort: M)
   - Clone to `external/SPIRV-Tools/`
   - Configure CMake options (disable tests)
   - Link with SPIRV-Headers
   - Verify compilation

3. **Add glslang** (Task R4.3 - effort: M)
   - Clone to `external/glslang/`
   - Link with SPIRV-Tools and SPIRV-Headers
   - Verify GLSL → SPIR-V compilation works

4. **Add SPIRV-Cross** (Task R4.4 - effort: M)
   - Clone to `external/SPIRV-Cross/`
   - Configure CMake (enable GLSL backend, disable others for now)
   - Verify SPIR-V → GLSL conversion works

5. **Integrate with Shader API** (Task R4.5 - effort: M)
   - Implement `CreateShader()` with SPIR-V input
   - Implement runtime SPIR-V → GLSL conversion for OpenGL backend
   - Add shader compilation example

6. **Implement Shader Reflection** (Task R4.6 - effort: L)
   - Use SPIRV-Cross reflection API
   - Extract uniform buffers, textures, attributes
   - Populate pipeline layout automatically

7. **Add Shader Caching** (Task R4.7 - effort: M)
   - Cache compiled SPIR-V bytecode
   - Cache converted GLSL source
   - Implement cache invalidation

**Acceptance Criteria**:
- ✅ All libraries compile on Windows, Linux, macOS
- ✅ Can compile GLSL 4.5 → SPIR-V
- ✅ Can convert SPIR-V → GLSL 3.30 for OpenGL 3.3
- ✅ Shader reflection extracts uniform/texture bindings
- ✅ HelloTriangle example uses compiled shaders

### Phase 2: Feature Detection (Week 3)

**Tasks** (Currently B2.1-B2.7 in Backlog):

1. Implement `IsFeatureSupported()` for OpenGL backend
2. Query OpenGL extensions and capabilities
3. Map OpenGL features to VRHI feature enum
4. Add feature validation before device creation
5. Write tests for feature detection

### Phase 3: Complete Resource Management (Week 4-5)

**Tasks** (Currently R1.1-R3.7 in Backlog):

1. Finalize Buffer implementation with all usage flags
2. Finalize Texture implementation with all formats
3. Complete Shader and Pipeline management
4. Add resource state tracking
5. Implement proper destruction order
6. Test resource lifecycle with ASAN/Valgrind

### Phase 4: Vulkan Backend Skeleton (Week 6-7)

**Tasks** (Currently R6.1-R6.8 in Backlog):

1. Link Vulkan SDK
2. Implement VulkanBackend and VulkanDevice classes
3. Create Vulkan instance and logical device
4. Implement basic resource creation (buffer, texture, shader)
5. Verify triangle rendering with Vulkan
6. Test multi-backend switching

## Additional Dependencies (Lower Priority)

### For v1.x (Optional)

#### 1. Vulkan Memory Allocator (VMA)

**Repository**: https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator

**Purpose**: Efficient Vulkan memory allocation and management

**Priority**: P1 (when implementing Vulkan backend)

#### 2. vk-bootstrap

**Repository**: https://github.com/charles-lunarg/vk-bootstrap

**Purpose**: Simplify Vulkan initialization

**Priority**: P2 (nice-to-have, reduces boilerplate)

### For v2.0 (Future)

#### 1. Slang Shader Compiler

**Repository**: https://github.com/shader-slang/slang

**Purpose**: Modern shader language, compile to all backends

**Priority**: P3 (planned for v2.0)

## Directory Structure After Adding Dependencies

```
external/
├── CMakeLists.txt
├── README.md
├── glad_gl33/              # ✅ Already present
├── glad_gl46/              # ✅ Already present
├── glfw-3.4/               # ✅ Already present
├── googletest-1.17.0/      # ✅ Already present
├── SPIRV-Headers/          # ⬅️ ADD THIS (P0)
├── SPIRV-Tools/            # ⬅️ ADD THIS (P0)
├── glslang/                # ⬅️ ADD THIS (P0)
├── SPIRV-Cross/            # ⬅️ ADD THIS (P0)
└── VulkanMemoryAllocator/  # ⬅️ ADD LATER (P1, for Vulkan backend)
```

## CMake Configuration Example

```cmake
# external/CMakeLists.txt

# Disable tests for third-party libraries
set(SPIRV_SKIP_TESTS ON CACHE BOOL "" FORCE)
set(SPIRV_SKIP_EXECUTABLES ON CACHE BOOL "" FORCE)
set(GLSLANG_TESTS OFF CACHE BOOL "" FORCE)
set(ENABLE_GLSLANG_BINARIES OFF CACHE BOOL "" FORCE)

# SPIRV-Headers (header-only)
add_subdirectory(SPIRV-Headers)

# SPIRV-Tools
add_subdirectory(SPIRV-Tools)

# glslang
add_subdirectory(glslang)

# SPIRV-Cross
set(SPIRV_CROSS_CLI OFF CACHE BOOL "" FORCE)
set(SPIRV_CROSS_ENABLE_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory(SPIRV-Cross)

# Create interface library for shader compilation
add_library(VRHI_ShaderCompiler INTERFACE)
target_link_libraries(VRHI_ShaderCompiler INTERFACE
    glslang
    SPIRV
    spirv-cross-core
    spirv-cross-glsl
)
```

## Expected Build Size Impact

| Component | Source Size | Build Size (Debug) | Build Size (Release) |
|-----------|-------------|-------------------|---------------------|
| SPIRV-Headers | ~500 KB | Header-only | Header-only |
| SPIRV-Tools | ~12 MB | ~15 MB | ~4 MB |
| glslang | ~15 MB | ~25 MB | ~5 MB |
| SPIRV-Cross | ~8 MB | ~12 MB | ~3 MB |
| **Total** | **~35 MB** | **~52 MB** | **~12 MB** |

**Note**: These are incremental sizes. The total VRHI project size will increase, but this is acceptable for a graphics API abstraction layer.

## Build Time Impact

- **Initial CMake configuration**: +30-60 seconds
- **Clean build**: +2-4 minutes
- **Incremental build**: Minimal impact (only if shader code changes)

## Next KANBAN Updates

The following tasks should be moved from **Backlog** to **In Progress** once shader libraries are added:

1. **R4.1**: Integrate glslang library → **Done**
2. **R4.2**: Integrate SPIRV-Cross library → **Done**
3. **R4.3**: Implement GLSL → SPIR-V compilation → **In Progress**
4. **R4.4**: Implement SPIR-V → GLSL conversion → **In Progress**
5. **R4.5**: Implement shader reflection → **In Progress**
6. **R4.6**: Add shader caching → **Backlog**
7. **R4.7**: Create shader compilation example → **Backlog**

## Risk Assessment

### Low Risk ✅
- All recommended libraries are official Khronos projects
- Licenses are compatible (Apache 2.0, BSD, MIT)
- Widely used in production (by Vulkan SDK, major engines)
- Actively maintained

### Medium Risk ⚠️
- Build complexity increases (more CMake dependencies)
- Build time increases moderately
- Need to manage multiple library versions

**Mitigation**:
- Pin specific stable versions (use git tags)
- Document version compatibility matrix
- Test on all platforms before committing

### No Risk ❌
- No runtime dependencies on external binaries
- All libraries compile from source
- Offline builds still supported

## Conclusion

**Immediate Action Required**: Add the four shader compilation libraries (SPIRV-Headers, SPIRV-Tools, glslang, SPIRV-Cross) to enable completion of Phase 5-6 of the VRHI v1.0 roadmap.

**Timeline**: Adding these dependencies and implementing basic shader compilation should take 1-2 weeks.

**Impact**: This unblocks:
- Complete OpenGL 3.3 backend implementation
- Vulkan backend implementation  
- Shader-based examples (textured cube, advanced rendering)
- v1.0 feature completeness

**Next Steps**:
1. Review and approve this recommendation
2. Add shader compilation libraries to `external/`
3. Update KANBAN: Mark R4.1-R4.4 as "In Progress"
4. Implement shader compilation integration
5. Update KANBAN: Mark R4.1-R4.4 as "Done"
6. Move to Phase 6: Vulkan Backend Skeleton

---

**Questions? Concerns?** Please discuss in GitHub Issues or Discussions before proceeding with the integration.
