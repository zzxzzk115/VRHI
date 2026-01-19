// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#include "VulkanSync.hpp"
#include "VulkanDevice.hpp"
#include <VRHI/Logging.hpp>

namespace VRHI {

// ============================================================================
// VulkanFence
// ============================================================================

VulkanFence::VulkanFence(VulkanDevice& device, vk::UniqueFence fence)
    : m_device(device)
    , m_fence(std::move(fence))
{
}

VulkanFence::~VulkanFence() = default;

std::unique_ptr<Fence>
VulkanFence::Create(VulkanDevice& device, bool signaled) {
    try {
        vk::FenceCreateInfo createInfo;
        if (signaled) {
            createInfo.flags = vk::FenceCreateFlagBits::eSignaled;
        }
        
        auto fence = device.GetVulkanDevice().createFenceUnique(createInfo);
        return std::unique_ptr<Fence>(new VulkanFence(device, std::move(fence)));
        
    } catch (const vk::SystemError& e) {
        LogError("Failed to create Vulkan fence: {}", e.what());
        return nullptr;
    }
}

bool VulkanFence::Wait(uint64_t timeout) {
    try {
        auto result = m_device.GetVulkanDevice().waitForFences(
            m_fence.get(), VK_TRUE, timeout);
        return result == vk::Result::eSuccess;
    } catch (const vk::SystemError& e) {
        LogError("Failed to wait for Vulkan fence: {}", e.what());
        return false;
    }
}

void VulkanFence::Reset() {
    try {
        m_device.GetVulkanDevice().resetFences(m_fence.get());
    } catch (const vk::SystemError& e) {
        LogError("Failed to reset Vulkan fence: {}", e.what());
    }
}

bool VulkanFence::IsSignaled() const noexcept {
    try {
        auto result = m_device.GetVulkanDevice().getFenceStatus(m_fence.get());
        return result == vk::Result::eSuccess;
    } catch (const vk::SystemError&) {
        return false;
    }
}

// ============================================================================
// VulkanSemaphore
// ============================================================================

VulkanSemaphore::VulkanSemaphore(VulkanDevice& device, vk::UniqueSemaphore semaphore)
    : m_device(device)
    , m_semaphore(std::move(semaphore))
{
}

VulkanSemaphore::~VulkanSemaphore() = default;

std::unique_ptr<Semaphore>
VulkanSemaphore::Create(VulkanDevice& device) {
    try {
        vk::SemaphoreCreateInfo createInfo;
        auto semaphore = device.GetVulkanDevice().createSemaphoreUnique(createInfo);
        return std::unique_ptr<Semaphore>(new VulkanSemaphore(device, std::move(semaphore)));
        
    } catch (const vk::SystemError& e) {
        LogError("Failed to create Vulkan semaphore: {}", e.what());
        return nullptr;
    }
}

} // namespace VRHI
