// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#pragma once

#include <VRHI/Backend.hpp>

namespace VRHI {

/// Null backend - placeholder implementation that does nothing
/// Used as a default/stub backend before real backend implementations are ready
class NullBackend : public IBackend {
public:
    NullBackend() = default;
    ~NullBackend() override = default;
    
    // IBackend implementation
    BackendType GetType() const noexcept override;
    std::string_view GetName() const noexcept override;
    Version GetVersion() const noexcept override;
    
    const FeatureSet& GetSupportedFeatures() const override;
    bool IsFeatureSupported(Feature feature) const noexcept override;
    
    float CalculateScore(const FeatureRequirements& requirements) const override;
    
    std::expected<std::unique_ptr<Device>, Error>
    CreateDevice(const DeviceConfig& config) override;
};

} // namespace VRHI
