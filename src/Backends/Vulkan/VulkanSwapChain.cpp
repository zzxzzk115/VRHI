// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#include "VulkanSwapChain.hpp"
#include "VulkanDevice.hpp"
#include "VulkanSync.hpp"
#include <VRHI/Logging.hpp>

namespace VRHI {

VulkanSwapChain::VulkanSwapChain(VulkanDevice& device)
    : m_device(device)
{
}

VulkanSwapChain::~VulkanSwapChain() = default;

std::unique_ptr<SwapChain>
VulkanSwapChain::Create(VulkanDevice& device) {
    // For now, return a stub implementation
    // Full swapchain creation requires surface creation which needs window integration
    LogWarning("VulkanSwapChain::Create - stub implementation, swap chain not functional yet");
    return std::unique_ptr<SwapChain>(new VulkanSwapChain(device));
}

uint32_t VulkanSwapChain::AcquireNextImage(uint64_t timeout,
                                           Semaphore* semaphore,
                                           Fence* fence) {
    // TODO: Implement when swapchain is fully functional
    return 0;
}

void VulkanSwapChain::Present(Semaphore* const* waitSemaphores,
                             uint32_t waitSemaphoreCount) {
    // TODO: Implement when swapchain is fully functional
}

void VulkanSwapChain::Resize(uint32_t width, uint32_t height) {
    m_width = width;
    m_height = height;
    // TODO: Recreate swapchain
}

} // namespace VRHI
