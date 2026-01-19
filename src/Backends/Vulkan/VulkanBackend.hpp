// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#pragma once

#include <VRHI/Backend.hpp>
#include <vulkan/vulkan.hpp>

namespace VRHI {

/// Vulkan backend implementation
class VulkanBackend : public IBackend {
public:
    VulkanBackend();
    ~VulkanBackend() override;
    
    // IBackend implementation
    BackendType GetType() const noexcept override;
    std::string_view GetName() const noexcept override;
    Version GetVersion() const noexcept override;
    
    std::expected<FeatureSet, Error> GetSupportedFeatures() const override;
    bool IsFeatureSupported(Feature feature) const noexcept override;
    
    float CalculateScore(const FeatureRequirements& requirements) const override;
    
    std::expected<std::unique_ptr<Device>, Error>
    CreateDevice(const DeviceConfig& config) override;
    
private:
    void DetectFeatures();
    void DetectInstanceExtensions();
    
    FeatureSet m_features;
    bool m_featuresDetected = false;
    std::vector<vk::ExtensionProperties> m_instanceExtensions;
};

} // namespace VRHI
