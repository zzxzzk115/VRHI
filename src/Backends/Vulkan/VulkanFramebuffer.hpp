// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#pragma once

#include <VRHI/VRHI.hpp>
#include <vulkan/vulkan.hpp>

namespace VRHI {

class VulkanDevice;

class VulkanFramebuffer : public Framebuffer {
public:
    ~VulkanFramebuffer() override = default;
    
    static std::expected<std::unique_ptr<Framebuffer>, Error>
    Create(VulkanDevice& device, const FramebufferDesc& desc);
    
private:
    VulkanFramebuffer(VulkanDevice& device) : m_device(device) {}
    VulkanDevice& m_device;
};

} // namespace VRHI
