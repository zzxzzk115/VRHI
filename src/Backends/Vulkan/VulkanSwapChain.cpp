// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#include "VulkanSwapChain.hpp"
#include "VulkanDevice.hpp"

namespace VRHI {

std::expected<std::unique_ptr<SwapChain>, Error>
VulkanSwapChain::Create(VulkanDevice& device, const SwapChainDesc& desc) {
    return std::unique_ptr<SwapChain>(new VulkanSwapChain(device));
}

} // namespace VRHI
