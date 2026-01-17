// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#include "NullBackend.hpp"
#include "NullDevice.hpp"
#include <VRHI/BackendScoring.hpp>

namespace VRHI {

BackendType NullBackend::GetType() const noexcept {
    return BackendType::Auto;
}

std::string_view NullBackend::GetName() const noexcept {
    return "Null Backend";
}

Version NullBackend::GetVersion() const noexcept {
    return Version{0, 0, 0, "0.0.0"};
}

FeatureSet NullBackend::GetSupportedFeatures() const {
    // Return empty feature set - null backend supports nothing
    return FeatureSet{};
}

bool NullBackend::IsFeatureSupported(Feature feature) const noexcept {
    // Null backend supports no features
    return false;
}

float NullBackend::CalculateScore(const FeatureRequirements& requirements) const {
    // Null backend never meets requirements
    return -1.0f;
}

std::expected<std::unique_ptr<Device>, Error>
NullBackend::CreateDevice(const DeviceConfig& config) {
    // Return a null device implementation
    return std::make_unique<NullDevice>();
}

} // namespace VRHI
