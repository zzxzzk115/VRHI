// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#include "OpenGL33Backend.hpp"
#include "OpenGL33Device.hpp"
#include "GLExtensions.hpp"
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
    
    // Get OpenGL version
    GLint majorVersion = 0, minorVersion = 0;
    glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
    glGetIntegerv(GL_MINOR_VERSION, &minorVersion);
    
    // Helper lambda to check for extension support
    auto hasExtension = [](const char* extensionName) -> bool {
        GLint numExtensions = 0;
        glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
        for (GLint i = 0; i < numExtensions; ++i) {
            const char* ext = reinterpret_cast<const char*>(glGetStringi(GL_EXTENSIONS, i));
            if (std::strcmp(ext, extensionName) == 0) {
                return true;
            }
        }
        return false;
    };
    
    // Core shader stages
    m_features.core.vertexShader = true;
    m_features.core.fragmentShader = true;
    // Geometry shaders: GL 3.2+ or via extension
    m_features.core.geometryShader = (majorVersion > 3 || (majorVersion == 3 && minorVersion >= 2)) ||
                                      hasExtension(GLCommonExtensions::ARB_geometry_shader4);
    m_features.core.tessellationShader = (majorVersion > 4 || (majorVersion == 4 && minorVersion >= 0)) ||
                                          hasExtension(GLCommonExtensions::ARB_tessellation_shader);
    m_features.core.computeShader = (majorVersion > 4 || (majorVersion == 4 && minorVersion >= 3)) ||
                                     hasExtension(GLCommonExtensions::ARB_compute_shader);
    m_features.core.meshShader = false;  // Not available in core OpenGL
    
    // Buffer features
    m_features.core.vertexBuffers = true;
    m_features.core.indexBuffers = true;
    m_features.core.uniformBuffers = true;  // GL 3.1+
    m_features.core.storageBuffers = (majorVersion > 4 || (majorVersion == 4 && minorVersion >= 3)) ||
                                      hasExtension(GLCommonExtensions::ARB_shader_storage_buffer_object);
    m_features.core.indirectBuffers = (majorVersion > 4 || (majorVersion == 4 && minorVersion >= 0));
    
    // Drawing features
    m_features.core.instancing = true;  // GL 3.3
    m_features.core.multiDrawIndirect = (majorVersion > 4 || (majorVersion == 4 && minorVersion >= 3)) ||
                                         hasExtension(GLCommonExtensions::ARB_multi_draw_indirect);
    
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
    m_features.texture.dxt = hasExtension(GLCommonExtensions::EXT_texture_compression_s3tc) ||
                              hasExtension(GLCommonExtensions::ARB_texture_compression_bptc);
    m_features.texture.etc2 = (majorVersion > 4 || (majorVersion == 4 && minorVersion >= 3)) ||
                               hasExtension(GLCommonExtensions::ARB_ES3_compatibility);
    m_features.texture.astc = hasExtension(GLCommonExtensions::KHR_texture_compression_astc_ldr);
    
    // Anisotropic filtering - check via extension
    bool hasAnisotropic = hasExtension(GLCommonExtensions::EXT_texture_filter_anisotropic);
    m_features.texture.anisotropicFiltering = hasAnisotropic;
    
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
    
    // Query max anisotropy if available
    if (hasAnisotropic) {
        GLfloat maxAnisotropy = 0.0f;
        // Use GLAD-provided constant
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
        m_features.texture.maxAnisotropy = maxAnisotropy;
    } else {
        m_features.texture.maxAnisotropy = 1.0f;
    }
    
    // Rendering features
    m_features.rendering.multipleRenderTargets = true;
    
    GLint maxColorAttachments = 0;
    glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAttachments);
    m_features.rendering.maxColorAttachments = static_cast<uint32_t>(maxColorAttachments);
    
    // Independent blend: GL 4.0+ or via extension
    m_features.rendering.independentBlend = (majorVersion > 4 || (majorVersion == 4 && minorVersion >= 0)) ||
                                             hasExtension("GL_ARB_draw_buffers_blend");
    // Depth clamp: GL 3.2+ or via extension
    m_features.rendering.depthClamp = (majorVersion > 3 || (majorVersion == 3 && minorVersion >= 2)) ||
                                       hasExtension("GL_ARB_depth_clamp");
    m_features.rendering.multisample = true;
    
    GLint maxSamples = 0;
    glGetIntegerv(GL_MAX_SAMPLES, &maxSamples);
    m_features.rendering.maxSamples = static_cast<uint32_t>(maxSamples);
    
    // Compute features
    m_features.compute.computeShader = m_features.core.computeShader;
    
    if (m_features.compute.computeShader) {
        // Query compute shader limits
        GLint maxWorkGroupSize[3] = {0, 0, 0};
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &maxWorkGroupSize[0]);
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &maxWorkGroupSize[1]);
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &maxWorkGroupSize[2]);
        m_features.compute.maxWorkGroupSizeX = static_cast<uint32_t>(maxWorkGroupSize[0]);
        m_features.compute.maxWorkGroupSizeY = static_cast<uint32_t>(maxWorkGroupSize[1]);
        m_features.compute.maxWorkGroupSizeZ = static_cast<uint32_t>(maxWorkGroupSize[2]);
        
        GLint maxInvocations = 0;
        glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &maxInvocations);
        m_features.compute.maxWorkGroupInvocations = static_cast<uint32_t>(maxInvocations);
        
        GLint maxSharedMemory = 0;
        glGetIntegerv(GL_MAX_COMPUTE_SHARED_MEMORY_SIZE, &maxSharedMemory);
        m_features.compute.maxComputeSharedMemorySize = static_cast<uint32_t>(maxSharedMemory);
    } else {
        m_features.compute.maxWorkGroupSizeX = 0;
        m_features.compute.maxWorkGroupSizeY = 0;
        m_features.compute.maxWorkGroupSizeZ = 0;
        m_features.compute.maxWorkGroupInvocations = 0;
        m_features.compute.maxComputeSharedMemorySize = 0;
    }
    
    // Advanced features
    m_features.advanced.rayTracing = false;  // Not available in OpenGL
    m_features.advanced.meshShading = false;  // Not available in core OpenGL
    m_features.advanced.variableRateShading = false;  // Not available in OpenGL
    m_features.advanced.bindlessResources = hasExtension(GLCommonExtensions::ARB_bindless_texture);
    m_features.advanced.asyncCompute = false;  // Not available in OpenGL
    
    // Memory features
    m_features.memory.deviceLocalMemory = 0;  // OpenGL doesn't expose this
    m_features.memory.hostVisibleMemory = 0;  // OpenGL doesn't expose this
    
    // Query uniform buffer alignment (instead of hardcoding 256)
    GLint uniformBufferAlignment = 0;
    glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &uniformBufferAlignment);
    m_features.memory.minUniformBufferAlignment = static_cast<uint32_t>(uniformBufferAlignment);
    
    // Storage buffer alignment (if available)
    if (m_features.core.storageBuffers) {
        GLint storageBufferAlignment = 0;
        glGetIntegerv(GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT, &storageBufferAlignment);
        m_features.memory.minStorageBufferAlignment = static_cast<uint32_t>(storageBufferAlignment);
    } else {
        m_features.memory.minStorageBufferAlignment = 0;
    }
    
    m_features.memory.unifiedMemory = false;  // Typically discrete GPU
    
    m_featuresDetected = true;
}

const FeatureSet& OpenGL33Backend::GetSupportedFeatures() const {
    // For const correctness, we return a reference
    // Features should be detected during device creation
    if (!m_featuresDetected) {
        // Return empty feature set if not yet detected
        // This is a limitation - features can only be detected with an active context
        static const FeatureSet emptyFeatureSet{};
        return emptyFeatureSet;
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

// Export registration function for explicit initialization
namespace VRHI {
namespace detail {
    void RegisterOpenGL33Backend() {
        // Force the static registrar to be instantiated
        // This is a dummy function that ensures the static initializer runs
        (void)&s_opengl33Registrar;
    }
}
}
