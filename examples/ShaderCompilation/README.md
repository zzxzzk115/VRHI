# Shader Compilation Example

This example demonstrates the VRHI shader compilation system using glslang and SPIRV-Cross.

## What This Example Shows

1. **GLSL to SPIR-V Compilation**: Compile GLSL shader source to SPIR-V bytecode
2. **SPIR-V to GLSL Conversion**: Convert SPIR-V to different GLSL versions (e.g., 330, 410, 460)
3. **Shader Reflection**: Extract metadata from compiled shaders (inputs, outputs, uniforms, samplers)

## Building

```bash
mkdir build && cd build
cmake .. -DVRHI_BUILD_EXAMPLES=ON
cmake --build .
```

## Running

```bash
./bin/ShaderCompilation
```

## How It Works

The example demonstrates the complete shader compilation pipeline:

```cpp
// Step 1: Compile GLSL to SPIR-V
auto spirv = ShaderCompiler::CompileGLSLToSPIRV(
    glslSource, ShaderStage::Vertex, "main");

// Step 2: Convert SPIR-V to target GLSL version
auto glsl330 = ShaderCompiler::ConvertSPIRVToGLSL(*spirv, 330);

// Step 3: Reflect shader metadata
auto reflection = ShaderCompiler::ReflectSPIRV(*spirv);
```

## Key Features Demonstrated

### Version Compatibility

The example shows how GLSL 450 shaders can be converted to GLSL 330 for OpenGL 3.3 compatibility:

```glsl
// Input: GLSL 450
#version 450
layout(location = 0) in vec3 inPosition;
// ...

// Output: GLSL 330 (automatically converted)
#version 330
in vec3 inPosition;
// ...
```

### Shader Reflection

The reflection system extracts:
- **Inputs**: Vertex attributes (positions, colors, normals, etc.)
- **Outputs**: Fragment outputs, vertex shader outputs
- **Uniform Buffers**: UBO declarations
- **Samplers**: Texture samplers

## Integration with OpenGL Backend

The OpenGL backend automatically uses this pipeline:

```cpp
ShaderDesc desc{};
desc.stage = ShaderStage::Vertex;
desc.language = ShaderLanguage::GLSL;
desc.code = glslSource;
desc.codeSize = strlen(glslSource);

// Automatically compiles: GLSL -> SPIR-V -> GLSL 3.30
auto shader = device->CreateShader(desc);
```

This ensures that modern GLSL code (version 460) works seamlessly with OpenGL 3.3.

## See Also

- [Shader System Design](../../docs/design/shader_system.md)
- [HelloTriangle Example](../HelloTriangle/) - Uses GLSL 460 with OpenGL 3.3
- [ShaderCompiler API](../../include/VRHI/ShaderCompiler.hpp)
