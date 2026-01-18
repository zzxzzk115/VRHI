// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#pragma once

#include <VRHI/Backend.hpp>

namespace VRHI {

/// OpenGL 3.3 backend implementation
class OpenGL33Backend : public IBackend {
public:
    OpenGL33Backend();
    ~OpenGL33Backend() override;
    
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
    
    FeatureSet m_features;
    bool m_featuresDetected = false;
};

} // namespace VRHI
