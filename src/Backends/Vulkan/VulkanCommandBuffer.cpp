// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#include "VulkanCommandBuffer.hpp"
#include "VulkanDevice.hpp"

namespace VRHI {

std::expected<std::unique_ptr<CommandBuffer>, Error>
VulkanCommandBuffer::Create(VulkanDevice& device, const CommandBufferDesc& desc) {
    return std::unique_ptr<CommandBuffer>(new VulkanCommandBuffer(device));
}

} // namespace VRHI
