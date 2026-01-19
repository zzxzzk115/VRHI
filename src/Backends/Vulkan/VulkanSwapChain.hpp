// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#pragma once

#include <VRHI/VRHI.hpp>
#include <vulkan/vulkan.hpp>

namespace VRHI {

class VulkanDevice;

class VulkanSwapChain : public SwapChain {
public:
    ~VulkanSwapChain() override = default;
    
    static std::expected<std::unique_ptr<SwapChain>, Error>
    Create(VulkanDevice& device, const SwapChainDesc& desc);
    
    void Present() override {}
    void Resize(uint32_t width, uint32_t height) override {}
    
private:
    VulkanSwapChain(VulkanDevice& device) : m_device(device) {}
    VulkanDevice& m_device;
};

} // namespace VRHI
