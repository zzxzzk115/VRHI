// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#pragma once
#include <expected>
#include <memory>

#include <VRHI/VRHI.hpp>
#include <VRHI/Resources.hpp>
#include <glad/glad.h>

namespace VRHI {

class OpenGL33Texture : public Texture {
public:
    ~OpenGL33Texture() override;
    
    static std::expected<std::unique_ptr<Texture>, Error>
    Create(const TextureDesc& desc);
    
    // Texture interface
    TextureType GetType() const noexcept override;
    TextureFormat GetFormat() const noexcept override;
    uint32_t GetWidth() const noexcept override;
    uint32_t GetHeight() const noexcept override;
    uint32_t GetDepth() const noexcept override;
    uint32_t GetMipLevels() const noexcept override;
    uint32_t GetArrayLayers() const noexcept override;
    
    void Update(const void* data, size_t size, uint32_t mipLevel = 0, uint32_t arrayLayer = 0) override;
    void UpdateRegion(const void* data, uint32_t x, uint32_t y, uint32_t z,
                     uint32_t width, uint32_t height, uint32_t depth,
                     uint32_t mipLevel = 0, uint32_t arrayLayer = 0) override;
    void GenerateMipmaps(CommandBuffer* cmd) override;
    void Read(void* data, size_t size, uint32_t mipLevel = 0, uint32_t arrayLayer = 0) override;
    
    GLuint GetHandle() const noexcept { return m_texture; }
    
private:
    OpenGL33Texture(const TextureDesc& desc, GLuint texture);
    
    TextureDesc m_desc;
    GLuint m_texture = 0;
};

} // namespace VRHI
