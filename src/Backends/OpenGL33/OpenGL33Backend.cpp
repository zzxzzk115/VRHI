// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#include "OpenGL33Backend.hpp"
#include "OpenGL33Device.hpp"
#include <VRHI/BackendScoring.hpp>
#include <VRHI/Logging.hpp>
#include <glad/glad.h>
#include <cstring>

namespace VRHI {

OpenGL33Backend::OpenGL33Backend() {
    // Features will be detected when creating device (after context creation)
}

OpenGL33Backend::~OpenGL33Backend() = default;

BackendType OpenGL33Backend::GetType() const noexcept {
    return BackendType::OpenGL33;
}

std::string_view OpenGL33Backend::GetName() const noexcept {
    return "OpenGL 3.3";
}

Version OpenGL33Backend::GetVersion() const noexcept {
    return Version{3, 3, 0, "3.3"};
}

void OpenGL33Backend::DetectFeatures() {
    if (m_featuresDetected) {
        return;
    }
    
    // Initialize GLAD to load OpenGL functions
    // Note: This requires an active OpenGL context
    if (!gladLoadGL()) {
        LogError("Failed to initialize GLAD for OpenGL 3.3");
        return;
    }
    
    // Core shader stages
    m_features.core.vertexShader = true;
    m_features.core.fragmentShader = true;
    m_features.core.geometryShader = true;  // GL 3.2+
    m_features.core.tessellationShader = false;  // GL 4.0+
    m_features.core.computeShader = false;  // GL 4.3+
    m_features.core.meshShader = false;  // Not available
    
    // Buffer features
    m_features.core.vertexBuffers = true;
    m_features.core.indexBuffers = true;
    m_features.core.uniformBuffers = true;  // GL 3.1+
    m_features.core.storageBuffers = false;  // GL 4.3+ (SSBO)
    m_features.core.indirectBuffers = false;  // GL 4.0+
    
    // Drawing features
    m_features.core.instancing = true;  // GL 3.3
    m_features.core.multiDrawIndirect = false;  // GL 4.3+
    
    // Texture features
    m_features.texture.texture1D = true;
    m_features.texture.texture2D = true;
    m_features.texture.texture3D = true;
    m_features.texture.textureCube = true;
    m_features.texture.texture2DArray = true;  // GL 3.0+
    
    m_features.texture.floatTextures = true;
    m_features.texture.depthTextures = true;
    m_features.texture.compressedTextures = true;
    
    // Compression formats
    m_features.texture.dxt = true;  // BC1-BC3, widely supported
    m_features.texture.etc2 = false;  // GL 4.3+ / GLES 3.0+
    m_features.texture.astc = false;  // Extension only
    
    m_features.texture.anisotropicFiltering = true;  // Extension, but widely available
    
    // Query texture limits
    GLint maxTextureSize = 0;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
    m_features.texture.maxTextureSize = static_cast<uint32_t>(maxTextureSize);
    
    GLint max3DTextureSize = 0;
    glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &max3DTextureSize);
    m_features.texture.max3DTextureSize = static_cast<uint32_t>(max3DTextureSize);
    
    GLint maxArrayLayers = 0;
    glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &maxArrayLayers);
    m_features.texture.maxArrayLayers = static_cast<uint32_t>(maxArrayLayers);
    
    // Check for anisotropic filtering extension
    // Note: GLAD may not define extension macros for all extensions
    // We can check for the extension manually if needed
    GLint numExtensions = 0;
    glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
    bool hasAnisotropic = false;
    for (GLint i = 0; i < numExtensions; ++i) {
        const char* ext = reinterpret_cast<const char*>(glGetStringi(GL_EXTENSIONS, i));
        if (std::strcmp(ext, "GL_EXT_texture_filter_anisotropic") == 0) {
            hasAnisotropic = true;
            break;
        }
    }
    
    if (hasAnisotropic) {
        GLfloat maxAnisotropy = 0.0f;
        glGetFloatv(0x84FF, &maxAnisotropy);  // GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT value
        m_features.texture.maxAnisotropy = maxAnisotropy;
    } else {
        m_features.texture.maxAnisotropy = 1.0f;
    }
    
    // Rendering features
    m_features.rendering.multipleRenderTargets = true;
    
    GLint maxColorAttachments = 0;
    glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAttachments);
    m_features.rendering.maxColorAttachments = static_cast<uint32_t>(maxColorAttachments);
    
    m_features.rendering.independentBlend = false;  // GL 4.0+
    m_features.rendering.depthClamp = true;  // GL 3.2+
    m_features.rendering.multisample = true;
    
    GLint maxSamples = 0;
    glGetIntegerv(GL_MAX_SAMPLES, &maxSamples);
    m_features.rendering.maxSamples = static_cast<uint32_t>(maxSamples);
    
    // Compute features (not available in GL 3.3)
    m_features.compute.computeShader = false;
    m_features.compute.maxWorkGroupSizeX = 0;
    m_features.compute.maxWorkGroupSizeY = 0;
    m_features.compute.maxWorkGroupSizeZ = 0;
    m_features.compute.maxWorkGroupInvocations = 0;
    m_features.compute.maxComputeSharedMemorySize = 0;
    
    // Advanced features (not available in GL 3.3)
    m_features.advanced.rayTracing = false;
    m_features.advanced.meshShading = false;
    m_features.advanced.variableRateShading = false;
    m_features.advanced.bindlessResources = false;
    m_features.advanced.asyncCompute = false;
    
    // Memory features (OpenGL doesn't expose detailed memory info)
    m_features.memory.deviceLocalMemory = 0;  // Unknown
    m_features.memory.hostVisibleMemory = 0;  // Unknown
    m_features.memory.minUniformBufferAlignment = 256;  // Conservative estimate
    m_features.memory.minStorageBufferAlignment = 0;  // Not available
    m_features.memory.unifiedMemory = false;  // Typically discrete GPU
    
    m_featuresDetected = true;
}

FeatureSet OpenGL33Backend::GetSupportedFeatures() const {
    // For const correctness, we need to return a copy
    // Features should be detected during device creation
    if (!m_featuresDetected) {
        // Return empty feature set if not yet detected
        // This is a limitation - features can only be detected with an active context
        return FeatureSet{};
    }
    return m_features;
}

bool OpenGL33Backend::IsFeatureSupported(Feature feature) const noexcept {
    if (!m_featuresDetected) {
        return false;
    }
    
    switch (feature) {
        case Feature::Compute: return false;  // GL 4.3+
        case Feature::Geometry: return m_features.core.geometryShader;
        case Feature::Tessellation: return false;  // GL 4.0+
        
        case Feature::Texture1D: return m_features.texture.texture1D;
        case Feature::Texture2D: return m_features.texture.texture2D;
        case Feature::Texture3D: return m_features.texture.texture3D;
        case Feature::TextureCube: return m_features.texture.textureCube;
        case Feature::Texture2DArray: return m_features.texture.texture2DArray;
        case Feature::TextureCompression: return m_features.texture.compressedTextures;
        case Feature::AnisotropicFiltering: return m_features.texture.anisotropicFiltering;
        
        case Feature::MultiDrawIndirect: return false;  // GL 4.3+
        case Feature::RayTracing: return false;
        case Feature::MeshShading: return false;
        case Feature::AsyncCompute: return false;
        case Feature::BindlessResources: return false;
        
        case Feature::DebugMarkers: return true;  // Can be implemented via extensions
        case Feature::GPUValidation: return false;  // No native support
        
        default: return false;
    }
}

float OpenGL33Backend::CalculateScore(const FeatureRequirements& requirements) const {
    // Check if all required features are supported
    for (const auto& feature : requirements.required) {
        if (!IsFeatureSupported(feature)) {
            return -1.0f;  // Cannot meet requirements
        }
    }
    
    // Use the scoring system from BackendScorer
    return BackendScorer::CalculateScore(BackendType::OpenGL33, m_features, requirements);
}

std::expected<std::unique_ptr<Device>, Error>
OpenGL33Backend::CreateDevice(const DeviceConfig& config) {
    // Create the OpenGL33Device
    // The device will handle context creation
    auto device = std::make_unique<OpenGL33Device>(config, this);
    
    // Initialize the device (creates context and detects features)
    auto initResult = device->Initialize();
    if (!initResult) {
        return std::unexpected(initResult.error());
    }
    
    // Detect features now that we have a context
    const_cast<OpenGL33Backend*>(this)->DetectFeatures();
    
    return device;
}

} // namespace VRHI

// Register the backend with the factory
namespace {
    struct OpenGL33BackendRegistrar {
        OpenGL33BackendRegistrar() {
            VRHI::BackendFactory::RegisterBackend(
                VRHI::BackendType::OpenGL33,
                []() -> std::unique_ptr<VRHI::IBackend> {
                    return std::make_unique<VRHI::OpenGL33Backend>();
                }
            );
        }
    };
    static OpenGL33BackendRegistrar s_opengl33Registrar;
}
