// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#include "VulkanShader.hpp"
#include "VulkanDevice.hpp"

namespace VRHI {

std::expected<std::unique_ptr<Shader>, Error>
VulkanShader::Create(VulkanDevice& device, const ShaderDesc& desc) {
    return std::unique_ptr<Shader>(new VulkanShader(device, desc));
}

} // namespace VRHI
