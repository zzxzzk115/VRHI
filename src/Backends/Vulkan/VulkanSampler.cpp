// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#include "VulkanSampler.hpp"
#include "VulkanDevice.hpp"

namespace VRHI {

std::expected<std::unique_ptr<Sampler>, Error>
VulkanSampler::Create(VulkanDevice& device, const SamplerDesc& desc) {
    return std::unique_ptr<Sampler>(new VulkanSampler(device, desc));
}

} // namespace VRHI
