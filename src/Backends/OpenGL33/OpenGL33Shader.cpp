// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#include "OpenGL33Shader.hpp"
#include <VRHI/Logging.hpp>
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
    
    // For GLSL, code should be null-terminated string
    const char* source = static_cast<const char*>(desc.code);
    GLint length = static_cast<GLint>(desc.codeSize);
    
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
