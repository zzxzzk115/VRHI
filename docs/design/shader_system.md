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

Generate separate GLAD loaders for each OpenGL version:

```
external/glad/
├── glad_gl33/     # OpenGL 3.3 Core
├── glad_gl41/     # OpenGL 4.1 Core
└── glad_gl46/     # OpenGL 4.6 Core
```

**Advantages**:
- ✅ Precise control over function sets per version
- ✅ Smaller binary size per backend
- ✅ Compile-time type safety
- ✅ Avoid unnecessary extension checks

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
- Separate loaders for GL 3.3, 4.1, 4.6
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

1. **GLAD**: Multi-version loaders (GL 3.3, 4.1, 4.6)
2. **v1.0 Shader Language**: GLSL 4.5+ (stable, mature)
3. **v2.0 Shader Language**: Slang (modern, powerful)
4. **SPIRV-Cross**: Required for cross-platform conversion
5. **Compilation Strategy**: Build-time GLSL→SPIR-V, runtime conversion as needed

### Platform Coverage Confirmation

| Tool | All Platforms | Notes |
|------|--------------|-------|
| GLAD | ✅ | OpenGL only |
| glslang | ✅ | Generates SPIR-V |
| SPIRV-Cross | ✅ | Converts to all backends |
| Slang | ✅ | Optional, v2.0+ |

**Answer**: Yes, both SPIRV-Cross and Slang can cover all planned platforms!
