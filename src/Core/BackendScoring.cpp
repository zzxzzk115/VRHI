// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#include <VRHI/BackendScoring.hpp>
#include <VRHI/Logging.hpp>
#include <algorithm>

// Platform-specific includes
#if defined(__APPLE__)
    #include <TargetConditionals.h>
#endif

namespace VRHI {

// ============================================================================
// Platform Detection
// ============================================================================

Platform GetCurrentPlatform() noexcept {
#if defined(_WIN32) || defined(_WIN64)
    return Platform::Windows;
#elif defined(__APPLE__)
    #if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
        return Platform::iOS;
    #else
        return Platform::macOS;
    #endif
#elif defined(__ANDROID__)
    return Platform::Android;
#elif defined(__linux__)
    // Simple heuristic: check for Raspberry Pi
    // In production, you'd check /proc/cpuinfo or similar
    return Platform::Linux;
#else
    return Platform::Unknown;
#endif
}

std::string_view GetPlatformName(Platform platform) noexcept {
    switch (platform) {
        case Platform::Windows:     return "Windows";
        case Platform::Linux:       return "Linux";
        case Platform::macOS:       return "macOS";
        case Platform::iOS:         return "iOS";
        case Platform::Android:     return "Android";
        case Platform::RaspberryPi: return "Raspberry Pi";
        case Platform::Unknown:
        default:                    return "Unknown";
    }
}

// ============================================================================
// Feature Support Helper
// ============================================================================

bool IsFeatureSupported(const FeatureSet& features, Feature feature) noexcept {
    switch (feature) {
        // Core features
        case Feature::Compute:
            return features.core.computeShader;
        case Feature::Geometry:
            return features.core.geometryShader;
        case Feature::Tessellation:
            return features.core.tessellationShader;
            
        // Texture features
        case Feature::Texture1D:
            return features.texture.texture1D;
        case Feature::Texture2D:
            return features.texture.texture2D;
        case Feature::Texture3D:
            return features.texture.texture3D;
        case Feature::TextureCube:
            return features.texture.textureCube;
        case Feature::Texture2DArray:
            return features.texture.texture2DArray;
        case Feature::TextureCompression:
            return features.texture.compressedTextures;
        case Feature::AnisotropicFiltering:
            return features.texture.anisotropicFiltering;
            
        // Advanced features
        case Feature::MultiDrawIndirect:
            return features.core.multiDrawIndirect;
        case Feature::RayTracing:
            return features.advanced.rayTracing;
        case Feature::MeshShading:
            return features.advanced.meshShading;
        case Feature::AsyncCompute:
            return features.advanced.asyncCompute;
        case Feature::BindlessResources:
            return features.advanced.bindlessResources;
            
        // Debug features - assume supported for now
        case Feature::DebugMarkers:
        case Feature::GPUValidation:
            return true;
            
        default:
            return false;
    }
}

// ============================================================================
// Backend Scoring Implementation
// ============================================================================

float BackendScorer::CalculateFeatureScore(
    const FeatureSet& features,
    const FeatureRequirements& requirements
) {
    // Check all required features
    for (const auto& feature : requirements.required) {
        if (!IsFeatureSupported(features, feature)) {
            // Required feature not supported - reject this backend
            return -1.0f;
        }
    }
    
    // All required features are supported, start with base score
    float score = 100.0f; // Base score for meeting all requirements
    
    // Count optional features supported
    int optionalSupported = 0;
    int optionalTotal = static_cast<int>(requirements.optional.size());
    
    for (const auto& feature : requirements.optional) {
        if (IsFeatureSupported(features, feature)) {
            optionalSupported++;
        }
    }
    
    // Add bonus for optional features (up to +50 points)
    if (optionalTotal > 0) {
        float optionalRatio = static_cast<float>(optionalSupported) / optionalTotal;
        score += optionalRatio * 50.0f;
    }
    
    // Cap at 100
    return std::min(score, 100.0f);
}

PerformanceLevel BackendScorer::GetPerformanceLevel(
    BackendType backendType,
    Platform platform
) {
    // Modern low-overhead APIs
    switch (backendType) {
        case BackendType::Vulkan:
            return PerformanceLevel::VeryHigh;
            
        case BackendType::Direct3D12:
            // D3D12 is native on Windows, very high performance
            return (platform == Platform::Windows) 
                ? PerformanceLevel::VeryHigh 
                : PerformanceLevel::High;
            
        case BackendType::Metal:
            // Metal is native on Apple platforms
            return (platform == Platform::macOS || platform == Platform::iOS)
                ? PerformanceLevel::VeryHigh
                : PerformanceLevel::High;
            
        // Modern OpenGL Desktop
        case BackendType::OpenGL46:
            return PerformanceLevel::High;
        case BackendType::OpenGL41:
            return PerformanceLevel::High;
        case BackendType::OpenGL33:
            return PerformanceLevel::Medium;
            
        // OpenGL ES Mobile
        case BackendType::OpenGLES31:
            return PerformanceLevel::High;
        case BackendType::OpenGLES30:
            return PerformanceLevel::Medium;
            
        // Future
        case BackendType::WebGPU:
            return PerformanceLevel::Medium;
            
        default:
            return PerformanceLevel::Low;
    }
}

float BackendScorer::CalculatePerformanceScore(
    BackendType backendType,
    Platform platform
) {
    // Get base performance level
    auto level = GetPerformanceLevel(backendType, platform);
    float score = static_cast<float>(level);
    
    // Add platform-native bonus
    score += GetPlatformNativeBonus(backendType, platform);
    
    return std::min(score, 100.0f);
}

StabilityLevel BackendScorer::GetStabilityLevel(
    BackendType backendType,
    Platform platform
) {
    switch (backendType) {
        // Mature, production-ready backends
        case BackendType::OpenGL33:
        case BackendType::OpenGL41:
            return StabilityLevel::Production;
            
        case BackendType::OpenGL46:
            // Very stable on Windows/Linux, good on others
            return (platform == Platform::Windows || platform == Platform::Linux)
                ? StabilityLevel::Production
                : StabilityLevel::Stable;
            
        case BackendType::Vulkan:
            // Mature on desktop, still improving on mobile
            return (platform == Platform::Windows || platform == Platform::Linux)
                ? StabilityLevel::Production
                : StabilityLevel::Stable;
            
        case BackendType::OpenGLES30:
            return StabilityLevel::Production;
            
        case BackendType::OpenGLES31:
            return StabilityLevel::Stable;
            
        // Planned backends - assume stable when implemented
        case BackendType::Direct3D12:
            return (platform == Platform::Windows)
                ? StabilityLevel::Production
                : StabilityLevel::Experimental;
                
        case BackendType::Metal:
            return (platform == Platform::macOS || platform == Platform::iOS)
                ? StabilityLevel::Production
                : StabilityLevel::Experimental;
                
        case BackendType::WebGPU:
            return StabilityLevel::Beta;
            
        default:
            return StabilityLevel::Experimental;
    }
}

float BackendScorer::CalculateStabilityScore(
    BackendType backendType,
    Platform platform
) {
    auto level = GetStabilityLevel(backendType, platform);
    return static_cast<float>(level);
}

float BackendScorer::CalculateCompatibilityScore(
    BackendType backendType,
    Platform platform
) {
    float score = 70.0f; // Base compatibility score
    
    // Platform-specific compatibility adjustments
    switch (platform) {
        case Platform::Windows:
            if (backendType == BackendType::Direct3D12) score = 100.0f;
            else if (backendType == BackendType::Vulkan) score = 95.0f;
            else if (backendType == BackendType::OpenGL46) score = 90.0f;
            else if (backendType == BackendType::OpenGL41) score = 85.0f;
            else if (backendType == BackendType::OpenGL33) score = 95.0f; // Wide compatibility
            break;
            
        case Platform::Linux:
            if (backendType == BackendType::Vulkan) score = 95.0f;
            else if (backendType == BackendType::OpenGL46) score = 90.0f;
            else if (backendType == BackendType::OpenGL41) score = 85.0f;
            else if (backendType == BackendType::OpenGL33) score = 95.0f;
            break;
            
        case Platform::macOS:
            if (backendType == BackendType::Metal) score = 100.0f;
            else if (backendType == BackendType::OpenGL41) score = 90.0f; // Max GL version on macOS
            else if (backendType == BackendType::OpenGL33) score = 85.0f;
            else if (backendType == BackendType::Vulkan) score = 60.0f; // Via MoltenVK
            break;
            
        case Platform::iOS:
            if (backendType == BackendType::Metal) score = 100.0f;
            else if (backendType == BackendType::OpenGLES31) score = 80.0f;
            else if (backendType == BackendType::OpenGLES30) score = 90.0f;
            break;
            
        case Platform::Android:
            if (backendType == BackendType::Vulkan) score = 85.0f;
            else if (backendType == BackendType::OpenGLES31) score = 90.0f;
            else if (backendType == BackendType::OpenGLES30) score = 95.0f; // Most compatible
            break;
            
        case Platform::RaspberryPi:
            if (backendType == BackendType::OpenGLES31) score = 80.0f;
            else if (backendType == BackendType::OpenGLES30) score = 95.0f;
            else if (backendType == BackendType::Vulkan) score = 60.0f; // Experimental
            break;
            
        default:
            score = 50.0f;
            break;
    }
    
    return score;
}

bool BackendScorer::IsNativeBackend(BackendType backendType, Platform platform) {
    switch (platform) {
        case Platform::Windows:
            return backendType == BackendType::Direct3D12;
            
        case Platform::macOS:
        case Platform::iOS:
            return backendType == BackendType::Metal;
            
        // Linux and Android don't have a single "native" API
        // Vulkan is the modern standard cross-platform API
        case Platform::Linux:
        case Platform::Android:
            return backendType == BackendType::Vulkan;
            
        default:
            return false;
    }
}

float BackendScorer::GetPlatformNativeBonus(
    BackendType backendType,
    Platform platform
) {
    if (IsNativeBackend(backendType, platform)) {
        return 10.0f; // +10 for native API
    }
    
    // Cross-platform modern APIs get a smaller bonus
    if (backendType == BackendType::Vulkan) {
        return 5.0f; // +5 for Vulkan (widely supported modern API)
    }
    
    return 0.0f; // No bonus for others
}

float BackendScorer::CalculateScore(
    BackendType backendType,
    const FeatureSet& features,
    const FeatureRequirements& requirements,
    const ScoringWeights& weights
) {
    // Calculate feature score first
    float featureScore = CalculateFeatureScore(features, requirements);
    
    // If required features not met, reject immediately
    if (featureScore < 0.0f) {
        return -1.0f;
    }
    
    // Get current platform
    Platform platform = GetCurrentPlatform();
    
    // Calculate other scores
    float performanceScore = CalculatePerformanceScore(backendType, platform);
    float stabilityScore = CalculateStabilityScore(backendType, platform);
    float compatibilityScore = CalculateCompatibilityScore(backendType, platform);
    
    // Calculate weighted total
    float totalScore = 
        (featureScore * weights.featureWeight) +
        (performanceScore * weights.performanceWeight) +
        (stabilityScore * weights.stabilityWeight) +
        (compatibilityScore * weights.compatibilityWeight);
    
    // Log detailed scoring if debug enabled
    LogDebug("Backend scoring for " + std::string(GetPlatformName(platform)) + ":");
    LogDebug("  Feature Score:       " + std::to_string(featureScore) + 
             " (weight: " + std::to_string(weights.featureWeight) + ")");
    LogDebug("  Performance Score:   " + std::to_string(performanceScore) + 
             " (weight: " + std::to_string(weights.performanceWeight) + ")");
    LogDebug("  Stability Score:     " + std::to_string(stabilityScore) + 
             " (weight: " + std::to_string(weights.stabilityWeight) + ")");
    LogDebug("  Compatibility Score: " + std::to_string(compatibilityScore) + 
             " (weight: " + std::to_string(weights.compatibilityWeight) + ")");
    LogDebug("  Total Score:         " + std::to_string(totalScore));
    
    return totalScore;
}

std::vector<BackendType> BackendScorer::GetBackendPriority(Platform platform) {
    std::vector<BackendType> priority;
    
    switch (platform) {
        case Platform::Windows:
            priority = {
                BackendType::Direct3D12,  // Native, highest performance
                BackendType::Vulkan,      // Cross-platform, very high performance
                BackendType::OpenGL46,    // Modern GL
                BackendType::OpenGL41,
                BackendType::OpenGL33,    // Fallback for compatibility
            };
            break;
            
        case Platform::macOS:
            priority = {
                BackendType::Metal,       // Native, highest performance
                BackendType::OpenGL41,    // Maximum GL version on macOS
                BackendType::OpenGL33,
                BackendType::Vulkan,      // Via MoltenVK (experimental)
            };
            break;
            
        case Platform::iOS:
            priority = {
                BackendType::Metal,       // Native, highest performance
                BackendType::OpenGLES31,
                BackendType::OpenGLES30,
            };
            break;
            
        case Platform::Linux:
            priority = {
                BackendType::Vulkan,      // Modern standard for Linux
                BackendType::OpenGL46,
                BackendType::OpenGL41,
                BackendType::OpenGL33,
            };
            break;
            
        case Platform::Android:
            priority = {
                BackendType::Vulkan,      // Modern standard for Android
                BackendType::OpenGLES31,
                BackendType::OpenGLES30,
            };
            break;
            
        case Platform::RaspberryPi:
            priority = {
                BackendType::OpenGLES31,
                BackendType::OpenGLES30,
                BackendType::Vulkan,      // Experimental on RPi
            };
            break;
            
        default:
            // Unknown platform - provide a reasonable default
            priority = {
                BackendType::Vulkan,
                BackendType::OpenGL33,
            };
            break;
    }
    
    return priority;
}

} // namespace VRHI
