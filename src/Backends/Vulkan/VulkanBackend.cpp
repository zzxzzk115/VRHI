// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#include "VulkanBackend.hpp"
#include "VulkanDevice.hpp"
#include <VRHI/BackendScoring.hpp>
#include <VRHI/Logging.hpp>

namespace VRHI {

VulkanBackend::VulkanBackend() {
    // Detect instance-level features (no device needed)
    DetectInstanceExtensions();
    DetectFeatures();
}

VulkanBackend::~VulkanBackend() = default;

BackendType VulkanBackend::GetType() const noexcept {
    return BackendType::Vulkan;
}

std::string_view VulkanBackend::GetName() const noexcept {
    return "Vulkan";
}

Version VulkanBackend::GetVersion() const noexcept {
    // Return Vulkan 1.3 as the target version
    return Version{1, 3, 0, "1.3"};
}

void VulkanBackend::DetectInstanceExtensions() {
    try {
        m_instanceExtensions = vk::enumerateInstanceExtensionProperties();
        LogInfo("Vulkan: Found {} instance extensions", m_instanceExtensions.size());
    } catch (const vk::SystemError& e) {
        LogError("Failed to enumerate Vulkan instance extensions: {}", e.what());
    }
}

void VulkanBackend::DetectFeatures() {
    if (m_featuresDetected) {
        return;
    }
    
    // We can detect some features at instance level, but most require a device
    // For now, we'll set conservative defaults and update when device is created
    
    // Core shader stages - Vulkan always supports these
    m_features.core.vertexShader = true;
    m_features.core.fragmentShader = true;
    m_features.core.geometryShader = true;  // Core in Vulkan 1.0
    m_features.core.tessellationShader = true;  // Core in Vulkan 1.0
    m_features.core.computeShader = true;  // Core in Vulkan 1.0
    m_features.core.meshShader = false;  // Requires extension
    
    // Buffer features - all core in Vulkan
    m_features.core.vertexBuffers = true;
    m_features.core.indexBuffers = true;
    m_features.core.uniformBuffers = true;
    m_features.core.storageBuffers = true;
    m_features.core.indirectBuffers = true;
    
    // Drawing features
    m_features.core.instancing = true;
    m_features.core.multiDrawIndirect = true;
    
    // Texture features - all core in Vulkan
    m_features.texture.texture1D = true;
    m_features.texture.texture2D = true;
    m_features.texture.texture3D = true;
    m_features.texture.textureCube = true;
    m_features.texture.texture2DArray = true;
    
    m_features.texture.floatTextures = true;
    m_features.texture.depthTextures = true;
    m_features.texture.compressedTextures = true;
    
    // Compression formats - most are optional extensions
    m_features.texture.dxt = false;  // Requires BC extension
    m_features.texture.etc2 = true;  // Core in Vulkan
    m_features.texture.astc = false;  // Optional
    
    // Render target features
    m_features.texture.renderToTexture = true;
    m_features.texture.multipleRenderTargets = true;
    m_features.texture.depthStencilTarget = true;
    
    // Sampling features
    m_features.texture.mipmapping = true;
    m_features.texture.anisotropicFiltering = true;  // Usually supported
    m_features.texture.shadowSamplers = true;
    
    // Advanced features - require extensions or higher versions
    m_features.advanced.rayTracing = false;  // Requires ray tracing extensions
    m_features.advanced.asyncCompute = true;  // Multiple queue families
    m_features.advanced.bindlessResources = false;  // Requires descriptor indexing
    
    // Debug features
    m_features.debug.debugMarkers = true;  // VK_EXT_debug_utils
    m_features.debug.gpuValidation = true;  // Validation layers
    
    m_featuresDetected = true;
    LogInfo("Vulkan backend features detected");
}

std::expected<FeatureSet, Error> VulkanBackend::GetSupportedFeatures() const {
    if (!m_featuresDetected) {
        return std::unexpected(Error{
            Error::Code::InitializationFailed,
            "Vulkan features not detected yet"
        });
    }
    return m_features;
}

bool VulkanBackend::IsFeatureSupported(Feature feature) const noexcept {
    if (!m_featuresDetected) {
        return false;
    }
    
    switch (feature) {
        case Feature::Compute: return m_features.core.computeShader;
        case Feature::Geometry: return m_features.core.geometryShader;
        case Feature::Tessellation: return m_features.core.tessellationShader;
        case Feature::Texture1D: return m_features.texture.texture1D;
        case Feature::Texture2D: return m_features.texture.texture2D;
        case Feature::Texture3D: return m_features.texture.texture3D;
        case Feature::TextureCube: return m_features.texture.textureCube;
        case Feature::Texture2DArray: return m_features.texture.texture2DArray;
        case Feature::TextureCompression: return m_features.texture.compressedTextures;
        case Feature::AnisotropicFiltering: return m_features.texture.anisotropicFiltering;
        case Feature::MultiDrawIndirect: return m_features.core.multiDrawIndirect;
        case Feature::RayTracing: return m_features.advanced.rayTracing;
        case Feature::MeshShading: return m_features.core.meshShader;
        case Feature::AsyncCompute: return m_features.advanced.asyncCompute;
        case Feature::BindlessResources: return m_features.advanced.bindlessResources;
        case Feature::DebugMarkers: return m_features.debug.debugMarkers;
        case Feature::GPUValidation: return m_features.debug.gpuValidation;
        default: return false;
    }
}

float VulkanBackend::CalculateScore(const FeatureRequirements& requirements) const {
    if (!m_featuresDetected) {
        return -1.0f;
    }
    
    // Use the standard scoring algorithm from BackendScoring
    return CalculateBackendScore(*this, m_features, requirements);
}

std::expected<std::unique_ptr<Device>, Error>
VulkanBackend::CreateDevice(const DeviceConfig& config) {
    // Create Vulkan device
    auto device = std::make_unique<VulkanDevice>(config, this);
    
    // Initialize the device
    auto initResult = device->Initialize();
    if (!initResult) {
        return std::unexpected(initResult.error());
    }
    
    // Update features with device-specific capabilities
    auto deviceFeatures = device->GetFeatures();
    m_features = deviceFeatures;
    
    return device;
}

// Backend registration - automatically register on library load
VRHI_REGISTER_BACKEND(Vulkan, []() {
    return std::make_unique<VulkanBackend>();
});

} // namespace VRHI
