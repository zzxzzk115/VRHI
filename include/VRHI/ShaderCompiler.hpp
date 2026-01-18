// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#pragma once

#include <VRHI/Shader.hpp>
#include <expected>
#include <string>
#include <vector>
#include <span>
#include <optional>

namespace VRHI {

// Forward declare Error from VRHI
class Error;

// ============================================================================
// Shader Compilation Result
// ============================================================================

/// Result of shader compilation
struct ShaderCompilationResult {
    /// SPIR-V bytecode
    std::vector<uint32_t> spirv;
    
    /// Optional reflection data
    struct ReflectionData {
        // Input attributes
        std::vector<std::string> inputs;
        
        // Output attributes
        std::vector<std::string> outputs;
        
        // Uniform buffers
        std::vector<std::string> uniformBuffers;
        
        // Texture samplers
        std::vector<std::string> samplers;
    };
    
    std::optional<ReflectionData> reflection;
};

// ============================================================================
// Shader Compiler
// ============================================================================

/// Shader compilation utility using glslang and SPIRV-Cross
class ShaderCompiler {
public:
    /// Compile GLSL source code to SPIR-V
    /// @param source GLSL source code
    /// @param stage Shader stage
    /// @param entryPoint Entry point function name (default: "main")
    /// @return SPIR-V bytecode or error
    static std::expected<std::vector<uint32_t>, Error> 
    CompileGLSLToSPIRV(
        const std::string& source,
        ShaderStage stage,
        const char* entryPoint = "main"
    );
    
    /// Convert SPIR-V to GLSL
    /// @param spirv SPIR-V bytecode
    /// @param targetVersion Target GLSL version (e.g., 330, 410, 460)
    /// @return GLSL source code or error
    static std::expected<std::string, Error>
    ConvertSPIRVToGLSL(
        std::span<const uint32_t> spirv,
        int targetVersion = 330
    );
    
    /// Reflect shader metadata from SPIR-V
    /// @param spirv SPIR-V bytecode
    /// @return Reflection data or error
    static std::expected<ShaderCompilationResult::ReflectionData, Error>
    ReflectSPIRV(std::span<const uint32_t> spirv);
    
    /// Compile GLSL to SPIR-V with reflection
    /// @param source GLSL source code
    /// @param stage Shader stage
    /// @param entryPoint Entry point function name
    /// @param enableReflection Whether to generate reflection data
    /// @return Compilation result with SPIR-V and optional reflection
    static std::expected<ShaderCompilationResult, Error>
    CompileGLSL(
        const std::string& source,
        ShaderStage stage,
        const char* entryPoint = "main",
        bool enableReflection = false
    );
};

} // namespace VRHI
