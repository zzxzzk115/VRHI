// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#pragma once

#include <VRHI/Sync.hpp>
#include <vulkan/vulkan.hpp>

namespace VRHI {

class VulkanDevice;

class VulkanFence : public Fence {
public:
    ~VulkanFence() override;
    
    static std::unique_ptr<Fence>
    Create(VulkanDevice& device, bool signaled);
    
    bool Wait(uint64_t timeout = UINT64_MAX) override;
    void Reset() override;
    bool IsSignaled() const noexcept override;
    
    void* GetNativeHandle() const noexcept override { return (void*)m_fence.get(); }
    
private:
    VulkanFence(VulkanDevice& device, vk::UniqueFence fence);
    
    VulkanDevice& m_device;
    vk::UniqueFence m_fence;
};

class VulkanSemaphore : public Semaphore {
public:
    ~VulkanSemaphore() override;
    
    static std::unique_ptr<Semaphore>
    Create(VulkanDevice& device);
    
    void* GetNativeHandle() const noexcept override { return (void*)m_semaphore.get(); }
    vk::Semaphore GetVulkanSemaphore() const noexcept { return m_semaphore.get(); }
    
private:
    VulkanSemaphore(VulkanDevice& device, vk::UniqueSemaphore semaphore);
    
    VulkanDevice& m_device;
    vk::UniqueSemaphore m_semaphore;
};

} // namespace VRHI
