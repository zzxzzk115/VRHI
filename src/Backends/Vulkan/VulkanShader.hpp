// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#pragma once

#include <VRHI/VRHI.hpp>
#include <VRHI/Shader.hpp>
#include <vulkan/vulkan.hpp>

namespace VRHI {

class VulkanDevice;

class VulkanShader : public Shader {
public:
    ~VulkanShader() override = default;
    
    static std::expected<std::unique_ptr<Shader>, Error>
    Create(VulkanDevice& device, const ShaderDesc& desc);
    
    ShaderStage GetStage() const noexcept override { return m_desc.stage; }
    ShaderLanguage GetLanguage() const noexcept override { return ShaderLanguage::SPIRV; }  // Vulkan uses SPIR-V
    std::string_view GetEntryPoint() const noexcept override { return "main"; }  // TODO: from desc
    
private:
    VulkanShader(VulkanDevice& device, const ShaderDesc& desc)
        : m_device(device), m_desc(desc) {}
    
    VulkanDevice& m_device;
    ShaderDesc m_desc;
};

} // namespace VRHI
