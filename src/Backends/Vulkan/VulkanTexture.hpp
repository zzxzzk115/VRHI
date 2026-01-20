// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#pragma once

#include <VRHI/Resources.hpp>
#include <vulkan/vulkan.hpp>

namespace VRHI {

class VulkanDevice;

class VulkanTexture : public Texture {
public:
    ~VulkanTexture() override = default;
    
    static std::expected<std::unique_ptr<Texture>, Error>
    Create(VulkanDevice& device, const TextureDesc& desc);
    
    TextureType GetType() const noexcept override { return m_desc.type; }
    TextureFormat GetFormat() const noexcept override { return m_desc.format; }
    uint32_t GetWidth() const noexcept override { return m_desc.width; }
    uint32_t GetHeight() const noexcept override { return m_desc.height; }
    uint32_t GetDepth() const noexcept override { return m_desc.depth; }
    uint32_t GetMipLevels() const noexcept override { return m_desc.mipLevels; }
    uint32_t GetArrayLayers() const noexcept override { return m_desc.arrayLayers; }
    
    void Update(const void* /*data*/, size_t /*size*/,
               uint32_t /*mipLevel*/ = 0, uint32_t /*arrayLayer*/ = 0) override {}  // TODO
    
    void UpdateRegion(const void* /*data*/,
                     uint32_t /*x*/, uint32_t /*y*/, uint32_t /*z*/,
                     uint32_t /*width*/, uint32_t /*height*/, uint32_t /*depth*/,
                     uint32_t /*mipLevel*/ = 0, uint32_t /*arrayLayer*/ = 0) override {}  // TODO
    
    void Read(void* /*data*/, size_t /*size*/,
             uint32_t /*mipLevel*/ = 0, uint32_t /*arrayLayer*/ = 0) override {}  // TODO
    
    void GenerateMipmaps(CommandBuffer* /*cmd*/) override {}  // TODO
    
private:
    VulkanTexture(VulkanDevice& device, const TextureDesc& desc) 
        : m_device(device), m_desc(desc) {}
    
    VulkanDevice& m_device;
    TextureDesc m_desc;
};

} // namespace VRHI
