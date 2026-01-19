// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#include "VulkanTexture.hpp"
#include "VulkanDevice.hpp"

namespace VRHI {

std::expected<std::unique_ptr<Texture>, Error>
VulkanTexture::Create(VulkanDevice& device, const TextureDesc& desc) {
    // TODO: Implement Vulkan texture creation
    return std::unique_ptr<Texture>(new VulkanTexture(device, desc));
}

} // namespace VRHI
