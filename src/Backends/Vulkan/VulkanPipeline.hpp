// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#pragma once

#include <VRHI/Pipeline.hpp>
#include <vulkan/vulkan.hpp>

namespace VRHI {

class VulkanDevice;

class VulkanPipeline : public Pipeline {
public:
    ~VulkanPipeline() override = default;
    
    static std::expected<std::unique_ptr<Pipeline>, Error>
    Create(VulkanDevice& device, const PipelineDesc& desc);
    
    PipelineType GetType() const noexcept override { return PipelineType::Graphics; }  // TODO: from desc
    
private:
    VulkanPipeline(VulkanDevice& device) : m_device(device) {}
    VulkanDevice& m_device;
};

} // namespace VRHI
