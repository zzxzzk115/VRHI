// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#pragma once

#include <VRHI/Sync.hpp>
#include <vulkan/vulkan.hpp>

namespace VRHI {

class VulkanDevice;

class VulkanFence : public Fence {
public:
    ~VulkanFence() override = default;
    
    static std::expected<std::unique_ptr<Fence>, Error>
    Create(VulkanDevice& device, bool signaled);
    
    void Wait() override {}
    void Reset() override {}
    bool IsSignaled() const override { return false; }
    
private:
    VulkanFence(VulkanDevice& device) : m_device(device) {}
    VulkanDevice& m_device;
};

class VulkanSemaphore : public Semaphore {
public:
    ~VulkanSemaphore() override = default;
    
    static std::expected<std::unique_ptr<Semaphore>, Error>
    Create(VulkanDevice& device);
    
private:
    VulkanSemaphore(VulkanDevice& device) : m_device(device) {}
    VulkanDevice& m_device;
};

} // namespace VRHI
