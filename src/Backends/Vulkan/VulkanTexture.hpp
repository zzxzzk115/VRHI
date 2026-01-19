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
    uint32_t GetWidth() const noexcept override { return m_desc.width; }
    uint32_t GetHeight() const noexcept override { return m_desc.height; }
    uint32_t GetDepth() const noexcept override { return m_desc.depth; }
    uint32_t GetMipLevels() const noexcept override { return m_desc.mipLevels; }
    
private:
    VulkanTexture(VulkanDevice& device, const TextureDesc& desc) 
        : m_device(device), m_desc(desc) {}
    
    VulkanDevice& m_device;
    TextureDesc m_desc;
};

} // namespace VRHI
