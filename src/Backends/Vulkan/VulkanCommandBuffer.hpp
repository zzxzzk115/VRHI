// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#pragma once

#include <VRHI/CommandBuffer.hpp>
#include <vulkan/vulkan.hpp>

namespace VRHI {

class VulkanDevice;

class VulkanCommandBuffer : public CommandBuffer {
public:
    ~VulkanCommandBuffer() override = default;
    
    static std::expected<std::unique_ptr<CommandBuffer>, Error>
    Create(VulkanDevice& device, const CommandBufferDesc& desc);
    
private:
    VulkanCommandBuffer(VulkanDevice& device) : m_device(device) {}
    VulkanDevice& m_device;
};

} // namespace VRHI
