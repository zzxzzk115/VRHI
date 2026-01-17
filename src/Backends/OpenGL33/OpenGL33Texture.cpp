// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#include "OpenGL33Texture.hpp"
#include <VRHI/Logging.hpp>

namespace VRHI {

namespace {
    GLenum GetGLTextureTarget(TextureType type) {
        switch (type) {
            case TextureType::Texture1D: return GL_TEXTURE_1D;
            case TextureType::Texture2D: return GL_TEXTURE_2D;
            case TextureType::Texture3D: return GL_TEXTURE_3D;
            case TextureType::TextureCube: return GL_TEXTURE_CUBE_MAP;
            case TextureType::Texture1DArray: return GL_TEXTURE_1D_ARRAY;
            case TextureType::Texture2DArray: return GL_TEXTURE_2D_ARRAY;
            default: return GL_TEXTURE_2D;
        }
    }
    
    GLenum GetGLInternalFormat(TextureFormat format) {
        switch (format) {
            case TextureFormat::R8_UNorm: return GL_R8;
            case TextureFormat::RG8_UNorm: return GL_RG8;
            case TextureFormat::RGBA8_UNorm: return GL_RGBA8;
            case TextureFormat::RGBA8_SRGB: return GL_SRGB8_ALPHA8;
            case TextureFormat::R16_Float: return GL_R16F;
            case TextureFormat::RG16_Float: return GL_RG16F;
            case TextureFormat::RGBA16_Float: return GL_RGBA16F;
            case TextureFormat::R32_Float: return GL_R32F;
            case TextureFormat::RG32_Float: return GL_RG32F;
            case TextureFormat::RGB32_Float: return GL_RGB32F;
            case TextureFormat::RGBA32_Float: return GL_RGBA32F;
            case TextureFormat::R32_UInt: return GL_R32UI;
            case TextureFormat::RG32_UInt: return GL_RG32UI;
            case TextureFormat::RGB32_UInt: return GL_RGB32UI;
            case TextureFormat::RGBA32_UInt: return GL_RGBA32UI;
            case TextureFormat::Depth16: return GL_DEPTH_COMPONENT16;
            case TextureFormat::Depth24Stencil8: return GL_DEPTH24_STENCIL8;
            case TextureFormat::Depth32F: return GL_DEPTH_COMPONENT32F;
            case TextureFormat::Depth32FStencil8: return GL_DEPTH32F_STENCIL8;
            case TextureFormat::BC1_UNorm: return 0x83F1;  // GL_COMPRESSED_RGBA_S3TC_DXT1_EXT
            case TextureFormat::BC3_UNorm: return 0x83F3;  // GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
            default: return GL_RGBA8;
        }
    }
}

OpenGL33Texture::OpenGL33Texture(const TextureDesc& desc, GLuint texture)
    : m_desc(desc)
    , m_texture(texture)
{
}

OpenGL33Texture::~OpenGL33Texture() {
    if (m_texture != 0) {
        glDeleteTextures(1, &m_texture);
    }
}

std::expected<std::unique_ptr<Texture>, Error>
OpenGL33Texture::Create(const TextureDesc& desc) {
    GLuint texture = 0;
    glGenTextures(1, &texture);
    
    if (texture == 0) {
        return std::unexpected(Error{
            Error::Code::InitializationFailed,
            "Failed to create OpenGL texture"
        });
    }
    
    GLenum target = GetGLTextureTarget(desc.type);
    GLenum internalFormat = GetGLInternalFormat(desc.format);
    
    glBindTexture(target, texture);
    
    // Allocate storage based on texture type
    switch (desc.type) {
        case TextureType::Texture2D:
            glTexImage2D(target, 0, internalFormat, desc.width, desc.height,
                        0, GL_RGBA, GL_UNSIGNED_BYTE, desc.initialData);
            break;
        case TextureType::Texture3D:
            glTexImage3D(target, 0, internalFormat, desc.width, desc.height, desc.depth,
                        0, GL_RGBA, GL_UNSIGNED_BYTE, desc.initialData);
            break;
        default:
            // Simplified for other types
            glTexImage2D(target, 0, internalFormat, desc.width, desc.height,
                        0, GL_RGBA, GL_UNSIGNED_BYTE, desc.initialData);
            break;
    }
    
    // Set default texture parameters
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glBindTexture(target, 0);
    
    auto textureObj = std::unique_ptr<Texture>(
        new OpenGL33Texture(desc, texture)
    );
    
    return textureObj;
}

TextureType OpenGL33Texture::GetType() const noexcept {
    return m_desc.type;
}

TextureFormat OpenGL33Texture::GetFormat() const noexcept {
    return m_desc.format;
}

uint32_t OpenGL33Texture::GetWidth() const noexcept {
    return m_desc.width;
}

uint32_t OpenGL33Texture::GetHeight() const noexcept {
    return m_desc.height;
}

uint32_t OpenGL33Texture::GetDepth() const noexcept {
    return m_desc.depth;
}

uint32_t OpenGL33Texture::GetMipLevels() const noexcept {
    return m_desc.mipLevels;
}

uint32_t OpenGL33Texture::GetArrayLayers() const noexcept {
    return m_desc.arrayLayers;
}

void OpenGL33Texture::Update(const void* data, size_t size, uint32_t mipLevel, uint32_t arrayLayer) {
    GLenum target = GetGLTextureTarget(m_desc.type);
    glBindTexture(target, m_texture);
    
    // Simplified update - proper implementation would handle all texture types
    glTexSubImage2D(target, mipLevel, 0, 0, m_desc.width, m_desc.height,
                   GL_RGBA, GL_UNSIGNED_BYTE, data);
    
    glBindTexture(target, 0);
}

void OpenGL33Texture::UpdateRegion(const void* data, uint32_t x, uint32_t y, uint32_t z,
                                   uint32_t width, uint32_t height, uint32_t depth,
                                   uint32_t mipLevel, uint32_t arrayLayer) {
    GLenum target = GetGLTextureTarget(m_desc.type);
    glBindTexture(target, m_texture);
    
    glTexSubImage2D(target, mipLevel, x, y, width, height,
                   GL_RGBA, GL_UNSIGNED_BYTE, data);
    
    glBindTexture(target, 0);
}

void OpenGL33Texture::GenerateMipmaps(CommandBuffer* cmd) {
    GLenum target = GetGLTextureTarget(m_desc.type);
    glBindTexture(target, m_texture);
    glGenerateMipmap(target);
    glBindTexture(target, 0);
}

void OpenGL33Texture::Read(void* data, size_t size, uint32_t mipLevel, uint32_t arrayLayer) {
    GLenum target = GetGLTextureTarget(m_desc.type);
    glBindTexture(target, m_texture);
    glGetTexImage(target, mipLevel, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glBindTexture(target, 0);
}

} // namespace VRHI
