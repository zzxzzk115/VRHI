// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#pragma once

#include <VRHI/RenderPass.hpp>
#include <vulkan/vulkan.hpp>

namespace VRHI {

class VulkanDevice;

class VulkanRenderPass : public RenderPass {
public:
    ~VulkanRenderPass() override = default;
    
    static std::expected<std::unique_ptr<RenderPass>, Error>
    Create(VulkanDevice& device, const RenderPassDesc& desc);
    
private:
    VulkanRenderPass(VulkanDevice& device) : m_device(device) {}
    VulkanDevice& m_device;
};

} // namespace VRHI
