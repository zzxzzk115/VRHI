// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#pragma once

#include <VRHI/Backend.hpp>
#include <VRHI/BackendScoring.hpp>

namespace VRHI {

/// Mock backend for testing
class MockBackend : public IBackend {
public:
    MockBackend();
    ~MockBackend() override = default;
    
    // Set custom features for testing
    void SetFeatures(const FeatureSet& features) { m_features = features; }
    
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
    FeatureSet m_features;
};

} // namespace VRHI
