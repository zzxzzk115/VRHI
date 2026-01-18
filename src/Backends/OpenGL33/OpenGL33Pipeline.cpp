// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#include "OpenGL33Pipeline.hpp"
#include "OpenGL33Shader.hpp"
#include <VRHI/Logging.hpp>

namespace VRHI {

std::expected<std::unique_ptr<Pipeline>, Error>
OpenGL33Pipeline::Create(const PipelineDesc& desc) {
    // For OpenGL 3.3, we need to create a shader program
    GLuint program = glCreateProgram();
    
    if (program == 0) {
        return std::unexpected(Error{
            Error::Code::InitializationFailed,
            "Failed to create OpenGL program"
        });
    }
    
    // Attach shaders based on pipeline type
    if (desc.type == PipelineType::Graphics) {
        // Attach vertex shader (required)
        if (desc.graphics.vertexShader) {
            auto* glShader = static_cast<OpenGL33Shader*>(desc.graphics.vertexShader);
            glAttachShader(program, glShader->GetHandle());
        } else {
            glDeleteProgram(program);
            return std::unexpected(Error{
                Error::Code::ValidationError,
                "Graphics pipeline requires a vertex shader"
            });
        }
        
        // Attach fragment shader (required)
        if (desc.graphics.fragmentShader) {
            auto* glShader = static_cast<OpenGL33Shader*>(desc.graphics.fragmentShader);
            glAttachShader(program, glShader->GetHandle());
        } else {
            glDeleteProgram(program);
            return std::unexpected(Error{
                Error::Code::ValidationError,
                "Graphics pipeline requires a fragment shader"
            });
        }
        
        // Attach optional shaders
        if (desc.graphics.geometryShader) {
            auto* glShader = static_cast<OpenGL33Shader*>(desc.graphics.geometryShader);
            glAttachShader(program, glShader->GetHandle());
        }
        
        if (desc.graphics.tessControlShader) {
            auto* glShader = static_cast<OpenGL33Shader*>(desc.graphics.tessControlShader);
            glAttachShader(program, glShader->GetHandle());
        }
        
        if (desc.graphics.tessEvalShader) {
            auto* glShader = static_cast<OpenGL33Shader*>(desc.graphics.tessEvalShader);
            glAttachShader(program, glShader->GetHandle());
        }
    } else if (desc.type == PipelineType::Compute) {
        // Compute shaders not supported in OpenGL 3.3
        glDeleteProgram(program);
        return std::unexpected(Error{
            Error::Code::UnsupportedFeature,
            "Compute shaders not supported in OpenGL 3.3"
        });
    }
    
    // Link the program
    glLinkProgram(program);
    
    // Check link status
    GLint linked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    
    if (!linked) {
        GLint logLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
        
        std::string errorLog;
        if (logLength > 0) {
            errorLog.resize(logLength);
            glGetProgramInfoLog(program, logLength, nullptr, errorLog.data());
        }
        
        glDeleteProgram(program);
        
        return std::unexpected(Error{
            Error::Code::CompilationError,
            "Program linking failed: " + errorLog
        });
    }
    
    // Detach shaders after linking (they're no longer needed)
    if (desc.type == PipelineType::Graphics) {
        if (desc.graphics.vertexShader) {
            auto* glShader = static_cast<OpenGL33Shader*>(desc.graphics.vertexShader);
            glDetachShader(program, glShader->GetHandle());
        }
        if (desc.graphics.fragmentShader) {
            auto* glShader = static_cast<OpenGL33Shader*>(desc.graphics.fragmentShader);
            glDetachShader(program, glShader->GetHandle());
        }
        if (desc.graphics.geometryShader) {
            auto* glShader = static_cast<OpenGL33Shader*>(desc.graphics.geometryShader);
            glDetachShader(program, glShader->GetHandle());
        }
        if (desc.graphics.tessControlShader) {
            auto* glShader = static_cast<OpenGL33Shader*>(desc.graphics.tessControlShader);
            glDetachShader(program, glShader->GetHandle());
        }
        if (desc.graphics.tessEvalShader) {
            auto* glShader = static_cast<OpenGL33Shader*>(desc.graphics.tessEvalShader);
            glDetachShader(program, glShader->GetHandle());
        }
    }
    
    // Copy vertex input state for graphics pipelines
    if (desc.type == PipelineType::Graphics) {
        return std::unique_ptr<Pipeline>(new OpenGL33Pipeline(program, desc.type, desc.graphics));
    } else {
        return std::unique_ptr<Pipeline>(new OpenGL33Pipeline(program, desc.type, {}));
    }
}

OpenGL33Pipeline::OpenGL33Pipeline(GLuint program, PipelineType type, const GraphicsPipelineDesc& desc)
    : m_program(program)
    , m_type(type)
{
    // Copy vertex attributes and bindings to internal storage
    if (!desc.vertexInput.attributes.empty()) {
        m_vertexAttributes.assign(desc.vertexInput.attributes.begin(), desc.vertexInput.attributes.end());
        m_vertexInputState.attributes = m_vertexAttributes;
    }
    
    if (!desc.vertexInput.bindings.empty()) {
        m_vertexBindings.assign(desc.vertexInput.bindings.begin(), desc.vertexInput.bindings.end());
        m_vertexInputState.bindings = m_vertexBindings;
    }
    
    // Store pipeline state
    if (type == PipelineType::Graphics) {
        m_depthStencilState = desc.depthStencil;
        m_rasterizationState = desc.rasterization;
        m_colorBlendState = desc.colorBlend;
        
        // Copy color blend attachments
        if (!desc.colorBlend.attachments.empty()) {
            m_colorBlendAttachments.assign(desc.colorBlend.attachments.begin(), desc.colorBlend.attachments.end());
            m_colorBlendState.attachments = m_colorBlendAttachments;
        }
    }
}

OpenGL33Pipeline::~OpenGL33Pipeline() {
    if (m_program != 0) {
        glDeleteProgram(m_program);
        m_program = 0;
    }
}

} // namespace VRHI
