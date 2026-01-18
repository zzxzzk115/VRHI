// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#include "OpenGL33Shader.hpp"
#include <VRHI/Logging.hpp>
#include <VRHI/ShaderCompiler.hpp>
#include <glad/glad.h>
#include <cstring>

namespace VRHI {

namespace {
    GLenum GetGLShaderStage(ShaderStage stage) {
        switch (stage) {
            case ShaderStage::Vertex: return GL_VERTEX_SHADER;
            case ShaderStage::Fragment: return GL_FRAGMENT_SHADER;
            case ShaderStage::Geometry: return GL_GEOMETRY_SHADER;
            default:
                LogError("Unsupported shader stage for OpenGL 3.3");
                return GL_VERTEX_SHADER;
        }
    }
}

std::expected<std::unique_ptr<Shader>, Error>
OpenGL33Shader::Create(const ShaderDesc& desc) {
    if (desc.code == nullptr || desc.codeSize == 0) {
        return std::unexpected(Error{
            Error::Code::InvalidConfig,
            "Shader code is required"
        });
    }
    
    GLenum shaderType = GetGLShaderStage(desc.stage);
    GLuint shader = glCreateShader(shaderType);
    
    if (shader == 0) {
        return std::unexpected(Error{
            Error::Code::InitializationFailed,
            "Failed to create OpenGL shader"
        });
    }
    
    // Determine the shader source to compile
    // Always go through GLSL -> SPIR-V -> GLSL 3.30 pipeline to ensure compatibility
    std::string glslSource;
    
    std::vector<uint32_t> spirvData;
    
    // Step 1: Get or compile to SPIR-V
    if (desc.language == ShaderLanguage::SPIRV) {
        // Already have SPIR-V, just wrap it
        const uint32_t* spirvPtr = static_cast<const uint32_t*>(desc.code);
        size_t spirvCount = desc.codeSize / sizeof(uint32_t);
        spirvData.assign(spirvPtr, spirvPtr + spirvCount);
    } 
    else if (desc.language == ShaderLanguage::GLSL) {
        // Compile GLSL to SPIR-V first
        std::string glslInput(static_cast<const char*>(desc.code), desc.codeSize);
        
        auto spirvResult = ShaderCompiler::CompileGLSLToSPIRV(glslInput, desc.stage, desc.entryPoint);
        if (!spirvResult) {
            glDeleteShader(shader);
            return std::unexpected(spirvResult.error());
        }
        
        spirvData = std::move(*spirvResult);
        LogInfo("Compiled GLSL to SPIR-V");
    }
    else {
        glDeleteShader(shader);
        return std::unexpected(Error{
            Error::Code::InvalidConfig,
            "Unsupported shader language for OpenGL 3.3 (only GLSL and SPIRV supported)"
        });
    }
    
    // Step 2: Convert SPIR-V to GLSL 3.30 for OpenGL 3.3 compatibility
    auto glslResult = ShaderCompiler::ConvertSPIRVToGLSL(spirvData, 330);
    if (!glslResult) {
        glDeleteShader(shader);
        return std::unexpected(glslResult.error());
    }
    
    glslSource = std::move(*glslResult);
    LogInfo("Converted SPIR-V to GLSL 3.30 for OpenGL 3.3");
    
    // Compile the GLSL source
    const char* source = glslSource.c_str();
    GLint length = static_cast<GLint>(glslSource.length());
    
    glShaderSource(shader, 1, &source, &length);
    glCompileShader(shader);
    
    // Check compilation status
    GLint compiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    
    if (!compiled) {
        GLint logLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        
        std::string errorLog;
        if (logLength > 0) {
            errorLog.resize(logLength);
            glGetShaderInfoLog(shader, logLength, nullptr, errorLog.data());
        }
        
        glDeleteShader(shader);
        
        return std::unexpected(Error{
            Error::Code::CompilationError,
            "Shader compilation failed: " + errorLog
        });
    }
    
    std::string entryPoint = desc.entryPoint ? desc.entryPoint : "main";
    return std::unique_ptr<Shader>(new OpenGL33Shader(shader, desc.stage, desc.language, std::move(entryPoint)));
}

} // namespace VRHI
