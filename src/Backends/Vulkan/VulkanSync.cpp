// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#include "VulkanSync.hpp"
#include "VulkanDevice.hpp"

namespace VRHI {

std::expected<std::unique_ptr<Fence>, Error>
VulkanFence::Create(VulkanDevice& device, bool signaled) {
    return std::unique_ptr<Fence>(new VulkanFence(device));
}

std::expected<std::unique_ptr<Semaphore>, Error>
VulkanSemaphore::Create(VulkanDevice& device) {
    return std::unique_ptr<Semaphore>(new VulkanSemaphore(device));
}

} // namespace VRHI
