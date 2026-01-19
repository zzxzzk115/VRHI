// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#include "VulkanCommandBuffer.hpp"
#include "VulkanDevice.hpp"

namespace VRHI {

std::unique_ptr<CommandBuffer>
VulkanCommandBuffer::Create(VulkanDevice& device) {
    return std::unique_ptr<CommandBuffer>(new VulkanCommandBuffer(device));
}

} // namespace VRHI
