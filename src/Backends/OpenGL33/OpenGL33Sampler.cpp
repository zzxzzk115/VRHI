// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#include "OpenGL33Sampler.hpp"
#include <VRHI/Logging.hpp>

namespace VRHI {

namespace {
    GLenum GetGLFilterMode(FilterMode mode) {
        switch (mode) {
            case FilterMode::Nearest: return GL_NEAREST;
            case FilterMode::Linear: return GL_LINEAR;
            default: return GL_LINEAR;
        }
    }
    
    GLenum GetGLAddressMode(AddressMode mode) {
        switch (mode) {
            case AddressMode::Repeat: return GL_REPEAT;
            case AddressMode::MirroredRepeat: return GL_MIRRORED_REPEAT;
            case AddressMode::ClampToEdge: return GL_CLAMP_TO_EDGE;
            case AddressMode::ClampToBorder: return GL_CLAMP_TO_BORDER;
            default: return GL_REPEAT;
        }
    }
    
    GLenum GetGLCompareOp(CompareOp op) {
        switch (op) {
            case CompareOp::Never: return GL_NEVER;
            case CompareOp::Less: return GL_LESS;
            case CompareOp::Equal: return GL_EQUAL;
            case CompareOp::LessOrEqual: return GL_LEQUAL;
            case CompareOp::Greater: return GL_GREATER;
            case CompareOp::NotEqual: return GL_NOTEQUAL;
            case CompareOp::GreaterOrEqual: return GL_GEQUAL;
            case CompareOp::Always: return GL_ALWAYS;
            default: return GL_NEVER;
        }
    }
}

OpenGL33Sampler::OpenGL33Sampler(GLuint sampler)
    : m_sampler(sampler)
{
}

OpenGL33Sampler::~OpenGL33Sampler() {
    if (m_sampler != 0) {
        glDeleteSamplers(1, &m_sampler);
    }
}

std::expected<std::unique_ptr<Sampler>, Error>
OpenGL33Sampler::Create(const SamplerDesc& desc) {
    GLuint sampler = 0;
    glGenSamplers(1, &sampler);
    
    if (sampler == 0) {
        return std::unexpected(Error{
            Error::Code::InitializationFailed,
            "Failed to create OpenGL sampler"
        });
    }
    
    // Set sampler parameters
    GLenum minFilter = GetGLFilterMode(desc.minFilter);
    GLenum magFilter = GetGLFilterMode(desc.magFilter);
    
    // Combine min filter with mipmap mode
    // Note: The texture itself should be created with appropriate filters based on mipLevels
    // This sampler configuration will be overridden by texture's own filter settings
    if (desc.mipmapMode == FilterMode::Linear) {
        if (minFilter == GL_NEAREST) {
            minFilter = GL_NEAREST_MIPMAP_LINEAR;
        } else {
            minFilter = GL_LINEAR_MIPMAP_LINEAR;
        }
    } else {
        if (minFilter == GL_NEAREST) {
            minFilter = GL_NEAREST_MIPMAP_NEAREST;
        } else {
            minFilter = GL_LINEAR_MIPMAP_NEAREST;
        }
    }
    
    glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, minFilter);
    glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, magFilter);
    
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GetGLAddressMode(desc.addressModeU));
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GetGLAddressMode(desc.addressModeV));
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_R, GetGLAddressMode(desc.addressModeW));
    
    glSamplerParameterf(sampler, GL_TEXTURE_LOD_BIAS, desc.mipLodBias);
    glSamplerParameterf(sampler, GL_TEXTURE_MIN_LOD, desc.minLod);
    glSamplerParameterf(sampler, GL_TEXTURE_MAX_LOD, desc.maxLod);
    
    if (desc.anisotropyEnable) {
        // Use GLAD-provided constant
        glSamplerParameterf(sampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, desc.maxAnisotropy);
    }
    
    if (desc.compareEnable) {
        glSamplerParameteri(sampler, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glSamplerParameteri(sampler, GL_TEXTURE_COMPARE_FUNC, GetGLCompareOp(desc.compareOp));
    }
    
    glSamplerParameterfv(sampler, GL_TEXTURE_BORDER_COLOR, desc.borderColor);
    
    auto samplerObj = std::unique_ptr<Sampler>(
        new OpenGL33Sampler(sampler)
    );
    
    return samplerObj;
}

} // namespace VRHI
