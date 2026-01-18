// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

/**
 * Shader Compilation Example
 * 
 * Demonstrates how to use the VRHI ShaderCompiler to:
 * - Compile GLSL source code to SPIR-V
 * - Convert SPIR-V to different GLSL versions
 * - Reflect shader metadata
 */

#include <VRHI/ShaderCompiler.hpp>
#include <VRHI/Logging.hpp>
#include <iostream>

int main() {
    // Set log level to see all messages
    VRHI::SetLogLevel(VRHI::LogLevel::Info);
    
    // Simple vertex shader in GLSL 4.5
    const char* vertexShaderSource = R"(
#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 0) in vec3 inColor;

layout(location = 0) out vec3 outColor;

void main() {
    gl_Position = vec4(inPosition, 1.0);
    outColor = inColor;
}
)";
    
    std::cout << "=== VRHI Shader Compilation Example ===\n\n";
    
    std::cout << "1. Compiling vertex shader to SPIR-V...\n";
    auto vertexSpirv = VRHI::ShaderCompiler::CompileGLSLToSPIRV(
        vertexShaderSource,
        VRHI::ShaderStage::Vertex,
        "main"
    );
    
    if (!vertexSpirv) {
        std::cerr << "Failed: " << vertexSpirv.error().message << "\n";
        return 1;
    }
    
    std::cout << "   Success! SPIR-V size: " << vertexSpirv->size() * 4 << " bytes\n\n";
    
    std::cout << "2. Converting SPIR-V to GLSL 3.30...\n";
    auto glsl330 = VRHI::ShaderCompiler::ConvertSPIRVToGLSL(*vertexSpirv, 330);
    
    if (!glsl330) {
        std::cerr << "Failed: " << glsl330.error().message << "\n";
        return 1;
    }
    
    std::cout << "   Success!\n\n";
    
    std::cout << "3. Reflecting shader metadata...\n";
    auto reflection = VRHI::ShaderCompiler::ReflectSPIRV(*vertexSpirv);
    
    if (reflection) {
        std::cout << "   Inputs: " << reflection->inputs.size() << "\n";
        std::cout << "   Outputs: " << reflection->outputs.size() << "\n";
    }
    
    std::cout << "\n=== All tests passed! ===\n";
    return 0;
}
