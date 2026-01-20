// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#pragma once

#include <VRHI/RenderPass.hpp>
#include <vulkan/vulkan.hpp>

namespace VRHI {

class VulkanDevice;

class VulkanFramebuffer : public Framebuffer {
public:
    ~VulkanFramebuffer() override = default;
    
    static std::expected<std::unique_ptr<Framebuffer>, Error>
    Create(VulkanDevice& device, const FramebufferDesc& desc);
    
    uint32_t GetWidth() const noexcept override { return 0; }  // TODO
    uint32_t GetHeight() const noexcept override { return 0; }  // TODO
    uint32_t GetLayers() const noexcept override { return 1; }  // TODO
    
private:
    VulkanFramebuffer(VulkanDevice& device) : m_device(device) {}
    VulkanDevice& m_device;
};

} // namespace VRHI
