// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#include "MockBackend.hpp"
#include "DeviceImpl.hpp"
#include <VRHI/BackendScoring.hpp>

namespace VRHI {

MockBackend::MockBackend() {
    // Initialize with basic features
    m_features = FeatureSet{};
    
    // Set some default features for a basic mock backend
    m_features.core.vertexShader = true;
    m_features.core.fragmentShader = true;
    m_features.core.uniformBuffers = true;
    m_features.core.vertexBuffers = true;
    m_features.core.indexBuffers = true;
    m_features.core.instancing = true;
    
    m_features.texture.texture2D = true;
    m_features.texture.textureCube = true;
    m_features.texture.depthTextures = true;
    m_features.texture.maxTextureSize = 4096;
    m_features.texture.maxAnisotropy = 16.0f;
    
    m_features.rendering.multipleRenderTargets = true;
    m_features.rendering.maxColorAttachments = 4;
    m_features.rendering.multisample = true;
    m_features.rendering.maxSamples = 4;
    
    m_features.memory.deviceLocalMemory = 1024 * 1024 * 1024; // 1GB
    m_features.memory.hostVisibleMemory = 512 * 1024 * 1024;  // 512MB
    m_features.memory.minUniformBufferAlignment = 256;
    m_features.memory.minStorageBufferAlignment = 256;
}

BackendType MockBackend::GetType() const noexcept {
    return BackendType::OpenGL33; // Pretend to be OpenGL 3.3
}

std::string_view MockBackend::GetName() const noexcept {
    return "Mock Backend";
}

Version MockBackend::GetVersion() const noexcept {
    return Version{1, 0, 0, "1.0.0"};
}

FeatureSet MockBackend::GetSupportedFeatures() const {
    return m_features;
}

bool MockBackend::IsFeatureSupported(Feature feature) const noexcept {
    return VRHI::IsFeatureSupported(m_features, feature);
}

float MockBackend::CalculateScore(const FeatureRequirements& requirements) const {
    return BackendScorer::CalculateScore(
        GetType(),
        m_features,
        requirements
    );
}

} // namespace VRHI
