// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#pragma once

#include "VRHI.hpp"

namespace VRHI {

// ============================================================================
// Platform Detection
// ============================================================================

enum class Platform {
    Windows,
    Linux,
    macOS,
    iOS,
    Android,
    RaspberryPi,
    Unknown
};

/// Get current platform
Platform GetCurrentPlatform() noexcept;

/// Get platform name as string
std::string_view GetPlatformName(Platform platform) noexcept;

// ============================================================================
// Backend Scoring System
// ============================================================================

/// Stability level for backends
enum class StabilityLevel {
    Experimental = 0,   // Experimental, may be unstable
    Beta = 50,          // Beta phase, mostly stable
    Stable = 80,        // Stable version
    Production = 100,   // Production ready, well tested
};

/// Performance level for backends
enum class PerformanceLevel {
    Low = 60,           // Basic performance
    Medium = 75,        // Moderate performance
    High = 90,          // High performance
    VeryHigh = 100,     // Highest performance (native low-level APIs)
};

/// Backend scoring weights
struct ScoringWeights {
    float featureWeight = 0.4f;       // 40% - Feature support
    float performanceWeight = 0.3f;   // 30% - Performance
    float stabilityWeight = 0.2f;     // 20% - Stability
    float compatibilityWeight = 0.1f; // 10% - Compatibility
};

/// Backend scoring calculator
class BackendScorer {
public:
    /// Calculate total score for a backend
    /// @param backendType Backend type to score
    /// @param features Supported features by the backend
    /// @param requirements Application feature requirements
    /// @param weights Custom scoring weights (optional)
    /// @return Total score (0-100), or -1 if requirements not met
    static float CalculateScore(
        BackendType backendType,
        const FeatureSet& features,
        const FeatureRequirements& requirements,
        const ScoringWeights& weights = {}
    );
    
    /// Calculate feature support score (0-100)
    /// @param features Supported features by the backend
    /// @param requirements Application feature requirements
    /// @return Feature score, or -1 if required features not met
    static float CalculateFeatureScore(
        const FeatureSet& features,
        const FeatureRequirements& requirements
    );
    
    /// Calculate performance score (0-100)
    /// @param backendType Backend type
    /// @param platform Current platform
    /// @return Performance score
    static float CalculatePerformanceScore(
        BackendType backendType,
        Platform platform = GetCurrentPlatform()
    );
    
    /// Calculate stability score (0-100)
    /// @param backendType Backend type
    /// @param platform Current platform
    /// @return Stability score
    static float CalculateStabilityScore(
        BackendType backendType,
        Platform platform = GetCurrentPlatform()
    );
    
    /// Calculate compatibility score (0-100)
    /// @param backendType Backend type
    /// @param platform Current platform
    /// @return Compatibility score
    static float CalculateCompatibilityScore(
        BackendType backendType,
        Platform platform = GetCurrentPlatform()
    );
    
    /// Get performance level for a backend
    static PerformanceLevel GetPerformanceLevel(
        BackendType backendType,
        Platform platform = GetCurrentPlatform()
    );
    
    /// Get stability level for a backend
    static StabilityLevel GetStabilityLevel(
        BackendType backendType,
        Platform platform = GetCurrentPlatform()
    );
    
    /// Check if backend is native to the platform
    /// @param backendType Backend type
    /// @param platform Current platform
    /// @return True if native, false otherwise
    static bool IsNativeBackend(
        BackendType backendType,
        Platform platform = GetCurrentPlatform()
    );
    
    /// Get platform-native API bonus
    /// @param backendType Backend type
    /// @param platform Current platform
    /// @return Bonus score (0-10)
    static float GetPlatformNativeBonus(
        BackendType backendType,
        Platform platform = GetCurrentPlatform()
    );
    
    /// Get backend priority for a platform
    /// @param platform Target platform
    /// @return List of backend types in priority order (highest first)
    static std::vector<BackendType> GetBackendPriority(
        Platform platform = GetCurrentPlatform()
    );
};

/// Helper to check if a feature enum matches a feature set field
bool IsFeatureSupported(const FeatureSet& features, Feature feature) noexcept;

} // namespace VRHI
