# Shader System Design

## Overview

VRHI's shader system must support multiple graphics backends, each with different shader format requirements. This document details the design decisions for shader compilation, conversion, and management.

## Design Approach

### SPIR-V-Centric Shader System

```
GLSL/HLSL/Slang (Source)
    ↓
SPIR-V (Intermediate Representation)
    ↓
SPIRV-Cross (Conversion Tool)
    ↓
Backend-Specific Formats (GLSL, MSL, HLSL)
```

### Recommended Workflow

**v1.0 (Simple & Reliable)**:
- Write shaders in GLSL 4.5+
- Compile to SPIR-V using glslangValidator
- Use SPIRV-Cross to convert to target formats

**v2.0+ (Advanced)**:
- Write shaders in Slang (modern syntax)
- Compile to SPIR-V using Slang compiler
- Use SPIRV-Cross for backend conversion

## Technical Decisions

### 1. GLAD Multi-Version Strategy

Generate separate GLAD loaders for each OpenGL and OpenGL ES version:

```
external/glad/
├── glad_gl33/      # OpenGL 3.3 Core
├── glad_gl41/      # OpenGL 4.1 Core
├── glad_gl46/      # OpenGL 4.6 Core
├── glad_gles30/    # OpenGL ES 3.0
└── glad_gles31/    # OpenGL ES 3.1
```

**Runtime Selection**:
```cpp
switch (backendType) {
    case BackendType::OpenGL33:
        gladLoadGL33();      // Load GL 3.3 functions
        break;
    case BackendType::OpenGL41:
        gladLoadGL41();      // Load GL 4.1 functions
        break;
    case BackendType::OpenGL46:
        gladLoadGL46();      // Load GL 4.6 functions
        break;
    case BackendType::OpenGLES30:
        gladLoadGLES30();    // Load GLES 3.0 functions
        break;
    case BackendType::OpenGLES31:
        gladLoadGLES31();    // Load GLES 3.1 functions
        break;
}
```

**Advantages**:
- ✅ Precise control over function sets per version
- ✅ Smaller binary size per backend
- ✅ Compile-time type safety
- ✅ Avoid unnecessary extension checks
- ✅ OpenGL and OpenGL ES fully separated (avoid symbol conflicts)

**Why GLES needs separate loaders**:
- OpenGL ES is an independent API specification with different function sets from Desktop OpenGL
- GLES 3.0 and GLES 3.1 have different capabilities (e.g., GLES 3.1 adds compute shaders)
- Mobile platforms (Android/iOS) and embedded devices require dedicated GLES loaders
- Separation avoids symbol naming conflicts with Desktop GL

### 2. Shader Language Selection

**v1.0: GLSL** (Recommended)
- Mature and stable
- Native OpenGL/Vulkan support
- Extensive documentation and resources

**v2.0+: Slang** (Recommended)
- Modern syntax (similar to HLSL/C++)
- Write-once, deploy-everywhere
- Covers ALL planned platforms ✅
- Better optimization and module system

**Platform Coverage with Slang**:

| Backend | Slang Support | Output Format |
|---------|--------------|---------------|
| Vulkan | ✅ | SPIR-V |
| OpenGL 3.3+ | ✅ | GLSL (via SPIRV-Cross) |
| OpenGL ES 3.0+ | ✅ | GLSL ES (via SPIRV-Cross) |
| Direct3D 12 | ✅ | DXIL/HLSL |
| Metal | ✅ | MSL (via SPIRV-Cross) |
| WebGPU | ✅ | WGSL/SPIR-V |

### 3. SPIRV-Cross Integration

**Purpose**:
- Convert SPIR-V to backend-specific shader languages
- Reflect shader metadata (uniforms, attributes, etc.)

**Platform Coverage**: SPIRV-Cross covers ALL planned platforms ✅

## Required Dependencies

### v1.0 Essential

**external/glad/** (multi-version)
- Desktop OpenGL: Separate loaders for GL 3.3, 4.1, 4.6
- Mobile OpenGL ES: Separate loaders for GLES 3.0, 3.1
- License: MIT/Public Domain

**external/glslang/**
- GLSL compiler to SPIR-V
- License: BSD 3-Clause

**external/SPIRV-Cross/**
- SPIR-V decompiler and converter
- License: Apache 2.0

**external/SPIRV-Tools/**
- SPIR-V optimization and validation
- License: Apache 2.0

### v2.0 Optional

**external/slang/**
- Slang shader compiler
- License: MIT

## Summary

### Key Decisions

1. **GLAD**: Multi-version loaders
   - Desktop: GL 3.3, 4.1, 4.6
   - Mobile: GLES 3.0, 3.1
2. **v1.0 Shader Language**: GLSL 4.5+ (stable, mature)
3. **v2.0 Shader Language**: Slang (modern, powerful)
4. **SPIRV-Cross**: Required for cross-platform conversion
5. **Compilation Strategy**: Runtime GLSL→SPIR-V→GLSL for version compatibility

### Platform Coverage Confirmation

| Tool | All Platforms | Notes |
|------|--------------|-------|
| GLAD | ✅ | OpenGL only |
| glslang | ✅ | Generates SPIR-V |
| SPIRV-Cross | ✅ | Converts to all backends |
| Slang | ✅ | Optional, v2.0+ |

**Answer**: Yes, both SPIRV-Cross and Slang can cover all planned platforms!

---

## Implementation Status (v1.0)

### ✅ Completed Features

The shader compilation system has been fully implemented for VRHI v1.0:

#### 1. ShaderCompiler Utility (`include/VRHI/ShaderCompiler.hpp`)

Static utility class providing:

```cpp
// Compile GLSL source to SPIR-V
static std::expected<std::vector<uint32_t>, Error> 
CompileGLSLToSPIRV(const std::string& source, ShaderStage stage, 
                   const char* entryPoint = "main",
                   IShaderIncluder* includer = nullptr);

// Convert SPIR-V to target GLSL version
static std::expected<std::string, Error>
ConvertSPIRVToGLSL(std::span<const uint32_t> spirv, int targetVersion = 330);

// Reflect shader metadata
static std::expected<ShaderCompilationResult::ReflectionData, Error>
ReflectSPIRV(std::span<const uint32_t> spirv);

// One-step compilation with optional reflection
static std::expected<ShaderCompilationResult, Error>
CompileGLSL(const std::string& source, ShaderStage stage,
            const char* entryPoint = "main", bool enableReflection = false,
            IShaderIncluder* includer = nullptr);
```

#### 2. Custom Include Support

`IShaderIncluder` interface allows custom #include resolution:

```cpp
class IShaderIncluder {
public:
    virtual std::string ResolveInclude(
        const std::string& headerName,
        const std::string& includerName,
        size_t includeDepth
    ) = 0;
};
```

#### 3. OpenGL Backend Integration

`OpenGL33Shader` implements complete shader pipeline:
- **GLSL input**: GLSL → SPIR-V → GLSL 3.30
- **SPIR-V input**: SPIR-V → GLSL 3.30

This ensures modern GLSL (e.g., version 460) is automatically converted to GLSL 3.30 for OpenGL 3.3 compatibility.

#### 4. Ray Tracing Shader Stages

Extended `ShaderStage` enum with ray tracing support:
- `RayGeneration` - Ray generation shader
- `AnyHit` - Any-hit shader
- `ClosestHit` - Closest-hit shader
- `Miss` - Miss shader
- `Intersection` - Intersection shader
- `Callable` - Callable shader

All stages are mapped to corresponding glslang shader types.

#### 5. Shader Reflection

Automatic extraction of shader metadata:
- Input attributes
- Output attributes
- Uniform buffers
- Texture samplers

### Usage Example

```cpp
// Example 1: Compile GLSL to SPIR-V
const char* glslSource = R"(
    #version 460 core
    layout(location = 0) in vec3 position;
    void main() { gl_Position = vec4(position, 1.0); }
)";

auto spirv = ShaderCompiler::CompileGLSLToSPIRV(
    glslSource, ShaderStage::Vertex);

// Example 2: Convert to OpenGL 3.3 compatible GLSL
auto glsl330 = ShaderCompiler::ConvertSPIRVToGLSL(*spirv, 330);

// Example 3: Reflect shader information
auto reflection = ShaderCompiler::ReflectSPIRV(*spirv);
for (const auto& input : reflection->inputs) {
    std::cout << "Input: " << input << "\n";
}

// Example 4: Use in OpenGL backend (automatic)
ShaderDesc desc{};
desc.stage = ShaderStage::Vertex;
desc.language = ShaderLanguage::GLSL;  // Will auto-convert
desc.code = glslSource;
desc.codeSize = strlen(glslSource);

auto shader = device->CreateShader(desc);  // Automatically handles conversion
```

### Integration Notes

**CMake Integration**:
- glslang and SPIRV-Cross are linked privately to VRHI
- Use `CMAKE_SKIP_INSTALL_RULES=ON` for builds
- Recommended: Use `add_subdirectory(vrhi)` in consuming projects

**Logging**:
- Enhanced logging with printf-style format support using C++23 concepts
- Example: `LogInfo("Compiled shader: %zu bytes", size)`

**Performance**:
- Compilation happens at shader creation time
- No caching in v1.0 (planned for v1.1)
- SPIR-V conversion is fast (< 10ms typical)

### Future Enhancements (v1.1+)

- [ ] Shader caching (Task R4.6)
- [ ] Slang language support (v2.0)
- [ ] SPIR-V optimization with SPIRV-Tools
- [ ] Pre-compiled shader bundles
- [ ] Shader hot-reloading for development
