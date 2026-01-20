// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#include "VulkanFramebuffer.hpp"
#include "VulkanDevice.hpp"

namespace VRHI {

std::expected<std::unique_ptr<Framebuffer>, Error>
VulkanFramebuffer::Create(VulkanDevice& device, const FramebufferDesc& desc) {
    return std::unique_ptr<Framebuffer>(new VulkanFramebuffer(device));
}

} // namespace VRHI
