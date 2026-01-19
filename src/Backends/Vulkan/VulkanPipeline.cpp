// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#include "VulkanPipeline.hpp"
#include "VulkanDevice.hpp"

namespace VRHI {

std::expected<std::unique_ptr<Pipeline>, Error>
VulkanPipeline::Create(VulkanDevice& device, const PipelineDesc& desc) {
    return std::unique_ptr<Pipeline>(new VulkanPipeline(device));
}

} // namespace VRHI
