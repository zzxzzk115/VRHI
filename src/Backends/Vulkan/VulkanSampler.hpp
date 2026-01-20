// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#pragma once

#include <VRHI/Resources.hpp>
#include <vulkan/vulkan.hpp>

namespace VRHI {

class VulkanDevice;

class VulkanSampler : public Sampler {
public:
    ~VulkanSampler() override = default;
    
    static std::expected<std::unique_ptr<Sampler>, Error>
    Create(VulkanDevice& device, const SamplerDesc& desc);
    
private:
    VulkanSampler(VulkanDevice& device, const SamplerDesc& desc)
        : m_device(device), m_desc(desc) {}
    
    VulkanDevice& m_device;
    SamplerDesc m_desc;
};

} // namespace VRHI
