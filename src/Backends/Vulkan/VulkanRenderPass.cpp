// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#include "VulkanRenderPass.hpp"
#include "VulkanDevice.hpp"

namespace VRHI {

std::expected<std::unique_ptr<RenderPass>, Error>
VulkanRenderPass::Create(VulkanDevice& device, const RenderPassDesc& desc) {
    return std::unique_ptr<RenderPass>(new VulkanRenderPass(device));
}

} // namespace VRHI
