// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#include "OpenGL33Texture.hpp"
#include "GLFormatUtils.hpp"
#include <VRHI/Logging.hpp>

namespace VRHI {

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
    
    GLenum target = GLFormatUtils::GetTextureTarget(desc.type);
    GLenum internalFormat = GLFormatUtils::GetInternalFormat(desc.format);
    
    glBindTexture(target, texture);
    
    // Get the appropriate format and type for this texture format
    GLenum format, type;
    GLFormatUtils::GetFormatAndType(desc.format, format, type);
    
    // Allocate storage based on texture type
    switch (desc.type) {
        case TextureType::Texture2D:
            glTexImage2D(target, 0, internalFormat, desc.width, desc.height,
                        0, format, type, desc.initialData);
            break;
        case TextureType::Texture3D:
            glTexImage3D(target, 0, internalFormat, desc.width, desc.height, desc.depth,
                        0, format, type, desc.initialData);
            break;
        default:
            // Simplified for other types
            glTexImage2D(target, 0, internalFormat, desc.width, desc.height,
                        0, format, type, desc.initialData);
            break;
    }
    
    // Set default texture parameters
    // For textures with only 1 mip level, don't use mipmap filtering
    if (desc.mipLevels > 1) {
        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    } else {
        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    // Set mipmap levels - important for textures without full mipmap chains
    glTexParameteri(target, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, desc.mipLevels - 1);
    
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
    GLenum target = GLFormatUtils::GetTextureTarget(m_desc.type);
    glBindTexture(target, m_texture);
    
    // Get the appropriate format and type for this texture format
    GLenum format, type;
    GLFormatUtils::GetFormatAndType(m_desc.format, format, type);
    
    glTexSubImage2D(target, mipLevel, 0, 0, m_desc.width, m_desc.height,
                   format, type, data);
    
    glBindTexture(target, 0);
}

void OpenGL33Texture::UpdateRegion(const void* data, uint32_t x, uint32_t y, uint32_t z,
                                   uint32_t width, uint32_t height, uint32_t depth,
                                   uint32_t mipLevel, uint32_t arrayLayer) {
    GLenum target = GLFormatUtils::GetTextureTarget(m_desc.type);
    glBindTexture(target, m_texture);
    
    // Get the appropriate format and type for this texture format
    GLenum format, type;
    GLFormatUtils::GetFormatAndType(m_desc.format, format, type);
    
    glTexSubImage2D(target, mipLevel, x, y, width, height,
                   format, type, data);
    
    glBindTexture(target, 0);
}

void OpenGL33Texture::GenerateMipmaps(CommandBuffer* cmd) {
    GLenum target = GLFormatUtils::GetTextureTarget(m_desc.type);
    glBindTexture(target, m_texture);
    glGenerateMipmap(target);
    glBindTexture(target, 0);
}

void OpenGL33Texture::Read(void* data, size_t size, uint32_t mipLevel, uint32_t arrayLayer) {
    GLenum target = GLFormatUtils::GetTextureTarget(m_desc.type);
    glBindTexture(target, m_texture);
    
    // Get the appropriate format and type for this texture format
    GLenum format, type;
    GLFormatUtils::GetFormatAndType(m_desc.format, format, type);
    
    glGetTexImage(target, mipLevel, format, type, data);
    glBindTexture(target, 0);
}

} // namespace VRHI
