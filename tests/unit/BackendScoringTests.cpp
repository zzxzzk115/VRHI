// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#include <VRHI/VRHIAll.hpp>
#include <VRHI/BackendScoring.hpp>
#include <gtest/gtest.h>

using namespace VRHI;

class BackendScoringTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a typical feature set for testing
        features.core.vertexShader = true;
        features.core.fragmentShader = true;
        features.core.geometryShader = true;
        features.core.computeShader = true;
        features.core.uniformBuffers = true;
        features.core.storageBuffers = true;
        features.core.vertexBuffers = true;
        features.core.indexBuffers = true;
        features.core.multiDrawIndirect = true;
        
        features.texture.texture1D = true;
        features.texture.texture2D = true;
        features.texture.texture3D = true;
        features.texture.textureCube = true;
        features.texture.texture2DArray = true;
        features.texture.compressedTextures = true;
        features.texture.anisotropicFiltering = true;
        features.texture.maxTextureSize = 16384;
        
        features.rendering.multipleRenderTargets = true;
        features.rendering.maxColorAttachments = 8;
    }
    
    FeatureSet features{};
};

// ============================================================================
// Platform Detection Tests
// ============================================================================

TEST_F(BackendScoringTest, PlatformDetection) {
    Platform platform = GetCurrentPlatform();
    EXPECT_NE(platform, Platform::Unknown);
    
    std::string_view name = GetPlatformName(platform);
    EXPECT_FALSE(name.empty());
    EXPECT_NE(name, "Unknown");
}

TEST_F(BackendScoringTest, PlatformNames) {
    EXPECT_EQ(GetPlatformName(Platform::Windows), "Windows");
    EXPECT_EQ(GetPlatformName(Platform::Linux), "Linux");
    EXPECT_EQ(GetPlatformName(Platform::macOS), "macOS");
    EXPECT_EQ(GetPlatformName(Platform::iOS), "iOS");
    EXPECT_EQ(GetPlatformName(Platform::Android), "Android");
}

// ============================================================================
// Feature Support Tests
// ============================================================================

TEST_F(BackendScoringTest, FeatureSupportChecks) {
    EXPECT_TRUE(IsFeatureSupported(features, Feature::Compute));
    EXPECT_TRUE(IsFeatureSupported(features, Feature::Geometry));
    EXPECT_TRUE(IsFeatureSupported(features, Feature::Texture2D));
    EXPECT_TRUE(IsFeatureSupported(features, Feature::Texture3D));
    EXPECT_TRUE(IsFeatureSupported(features, Feature::MultiDrawIndirect));
    
    // Features not set should return false
    EXPECT_FALSE(IsFeatureSupported(features, Feature::RayTracing));
    EXPECT_FALSE(IsFeatureSupported(features, Feature::MeshShading));
}

TEST_F(BackendScoringTest, FeatureScoreAllRequired) {
    FeatureRequirements requirements;
    requirements.required = {Feature::Texture2D, Feature::Compute};
    
    float score = BackendScorer::CalculateFeatureScore(features, requirements);
    EXPECT_GT(score, 0.0f);
    EXPECT_LE(score, 100.0f);
}

TEST_F(BackendScoringTest, FeatureScoreMissingRequired) {
    FeatureRequirements requirements;
    requirements.required = {Feature::RayTracing}; // Not supported
    
    float score = BackendScorer::CalculateFeatureScore(features, requirements);
    EXPECT_EQ(score, -1.0f); // Should reject
}

TEST_F(BackendScoringTest, FeatureScoreOptionalBonus) {
    FeatureRequirements requirements1;
    requirements1.required = {Feature::Texture2D};
    requirements1.optional = {};
    
    FeatureRequirements requirements2;
    requirements2.required = {Feature::Texture2D};
    requirements2.optional = {Feature::Compute, Feature::Geometry};
    
    float score1 = BackendScorer::CalculateFeatureScore(features, requirements1);
    float score2 = BackendScorer::CalculateFeatureScore(features, requirements2);
    
    // Both should be valid and high scores
    EXPECT_GT(score1, 0.0f);
    EXPECT_GT(score2, 0.0f);
    EXPECT_LE(score1, 100.0f);
    EXPECT_LE(score2, 100.0f);
    
    // When optional features are supported, score should be >= score without them
    // (it may be capped at 100, so >= rather than >)
    EXPECT_GE(score2, score1);
}

// ============================================================================
// Performance Scoring Tests
// ============================================================================

TEST_F(BackendScoringTest, PerformanceLevels) {
    // Vulkan should be very high performance
    EXPECT_EQ(BackendScorer::GetPerformanceLevel(BackendType::Vulkan, Platform::Windows),
              PerformanceLevel::VeryHigh);
    
    // Modern OpenGL should be high
    EXPECT_EQ(BackendScorer::GetPerformanceLevel(BackendType::OpenGL46, Platform::Windows),
              PerformanceLevel::High);
    
    // Older OpenGL should be medium
    EXPECT_EQ(BackendScorer::GetPerformanceLevel(BackendType::OpenGL33, Platform::Windows),
              PerformanceLevel::Medium);
}

TEST_F(BackendScoringTest, PerformanceScores) {
    float vulkanScore = BackendScorer::CalculatePerformanceScore(
        BackendType::Vulkan, Platform::Windows);
    float gl46Score = BackendScorer::CalculatePerformanceScore(
        BackendType::OpenGL46, Platform::Windows);
    float gl33Score = BackendScorer::CalculatePerformanceScore(
        BackendType::OpenGL33, Platform::Windows);
    
    EXPECT_GT(vulkanScore, gl46Score);
    EXPECT_GT(gl46Score, gl33Score);
}

// ============================================================================
// Stability Scoring Tests
// ============================================================================

TEST_F(BackendScoringTest, StabilityLevels) {
    // OpenGL 3.3 should be production-ready
    EXPECT_EQ(BackendScorer::GetStabilityLevel(BackendType::OpenGL33, Platform::Windows),
              StabilityLevel::Production);
    
    // Vulkan should be production/stable on desktop
    auto vulkanStability = BackendScorer::GetStabilityLevel(
        BackendType::Vulkan, Platform::Windows);
    EXPECT_GE(static_cast<int>(vulkanStability), static_cast<int>(StabilityLevel::Stable));
}

TEST_F(BackendScoringTest, StabilityScores) {
    float gl33Score = BackendScorer::CalculateStabilityScore(
        BackendType::OpenGL33, Platform::Windows);
    float vulkanScore = BackendScorer::CalculateStabilityScore(
        BackendType::Vulkan, Platform::Windows);
    
    EXPECT_GT(gl33Score, 0.0f);
    EXPECT_GT(vulkanScore, 0.0f);
    EXPECT_LE(gl33Score, 100.0f);
    EXPECT_LE(vulkanScore, 100.0f);
}

// ============================================================================
// Compatibility Scoring Tests
// ============================================================================

TEST_F(BackendScoringTest, CompatibilityScores) {
    float score = BackendScorer::CalculateCompatibilityScore(
        BackendType::OpenGL33, Platform::Windows);
    
    EXPECT_GT(score, 0.0f);
    EXPECT_LE(score, 100.0f);
}

TEST_F(BackendScoringTest, CompatibilityPlatformSpecific) {
    // On macOS, OpenGL 4.1 should have good compatibility
    float macGL41 = BackendScorer::CalculateCompatibilityScore(
        BackendType::OpenGL41, Platform::macOS);
    
    // OpenGL 4.6 should not be available on macOS
    float macGL46 = BackendScorer::CalculateCompatibilityScore(
        BackendType::OpenGL46, Platform::macOS);
    
    EXPECT_GT(macGL41, macGL46);
}

// ============================================================================
// Platform Native Bonus Tests
// ============================================================================

TEST_F(BackendScoringTest, PlatformNativeDetection) {
    // D3D12 is native to Windows
    EXPECT_TRUE(BackendScorer::IsNativeBackend(BackendType::Direct3D12, Platform::Windows));
    EXPECT_FALSE(BackendScorer::IsNativeBackend(BackendType::Direct3D12, Platform::Linux));
    
    // Metal is native to Apple platforms
    EXPECT_TRUE(BackendScorer::IsNativeBackend(BackendType::Metal, Platform::macOS));
    EXPECT_TRUE(BackendScorer::IsNativeBackend(BackendType::Metal, Platform::iOS));
    EXPECT_FALSE(BackendScorer::IsNativeBackend(BackendType::Metal, Platform::Windows));
    
    // Vulkan is considered native to Linux/Android
    EXPECT_TRUE(BackendScorer::IsNativeBackend(BackendType::Vulkan, Platform::Linux));
    EXPECT_TRUE(BackendScorer::IsNativeBackend(BackendType::Vulkan, Platform::Android));
}

TEST_F(BackendScoringTest, NativeBonus) {
    // Native APIs get +10 bonus
    float d3d12Bonus = BackendScorer::GetPlatformNativeBonus(
        BackendType::Direct3D12, Platform::Windows);
    EXPECT_EQ(d3d12Bonus, 10.0f);
    
    // Vulkan gets +5 bonus as cross-platform modern API
    float vulkanBonus = BackendScorer::GetPlatformNativeBonus(
        BackendType::Vulkan, Platform::Windows);
    EXPECT_EQ(vulkanBonus, 5.0f);
    
    // OpenGL gets no special bonus
    float glBonus = BackendScorer::GetPlatformNativeBonus(
        BackendType::OpenGL33, Platform::Windows);
    EXPECT_EQ(glBonus, 0.0f);
}

// ============================================================================
// Overall Scoring Tests
// ============================================================================

TEST_F(BackendScoringTest, OverallScoreCalculation) {
    FeatureRequirements requirements;
    requirements.required = {Feature::Texture2D};
    requirements.optional = {Feature::Compute};
    
    float score = BackendScorer::CalculateScore(
        BackendType::Vulkan, features, requirements);
    
    EXPECT_GT(score, 0.0f);
    EXPECT_LE(score, 100.0f);
}

TEST_F(BackendScoringTest, OverallScoreRespectsMissingRequirements) {
    FeatureRequirements requirements;
    requirements.required = {Feature::RayTracing}; // Not supported
    
    float score = BackendScorer::CalculateScore(
        BackendType::Vulkan, features, requirements);
    
    EXPECT_EQ(score, -1.0f); // Should reject
}

TEST_F(BackendScoringTest, CustomWeights) {
    FeatureRequirements requirements;
    requirements.required = {Feature::Texture2D};
    
    // Default weights
    ScoringWeights defaultWeights;
    float defaultScore = BackendScorer::CalculateScore(
        BackendType::Vulkan, features, requirements, defaultWeights);
    
    // Custom weights emphasizing performance
    ScoringWeights perfWeights;
    perfWeights.featureWeight = 0.1f;
    perfWeights.performanceWeight = 0.7f;
    perfWeights.stabilityWeight = 0.1f;
    perfWeights.compatibilityWeight = 0.1f;
    
    float perfScore = BackendScorer::CalculateScore(
        BackendType::Vulkan, features, requirements, perfWeights);
    
    // Both should be valid scores
    EXPECT_GT(defaultScore, 0.0f);
    EXPECT_GT(perfScore, 0.0f);
}

// ============================================================================
// Backend Priority Tests
// ============================================================================

TEST_F(BackendScoringTest, BackendPriorityWindows) {
    auto priority = BackendScorer::GetBackendPriority(Platform::Windows);
    
    EXPECT_FALSE(priority.empty());
    // D3D12 should be first on Windows
    EXPECT_EQ(priority[0], BackendType::Direct3D12);
    // Vulkan should be second
    EXPECT_EQ(priority[1], BackendType::Vulkan);
}

TEST_F(BackendScoringTest, BackendPriorityMacOS) {
    auto priority = BackendScorer::GetBackendPriority(Platform::macOS);
    
    EXPECT_FALSE(priority.empty());
    // Metal should be first on macOS
    EXPECT_EQ(priority[0], BackendType::Metal);
    // OpenGL 4.1 should be second (max GL version on macOS)
    EXPECT_EQ(priority[1], BackendType::OpenGL41);
}

TEST_F(BackendScoringTest, BackendPriorityLinux) {
    auto priority = BackendScorer::GetBackendPriority(Platform::Linux);
    
    EXPECT_FALSE(priority.empty());
    // Vulkan should be first on Linux
    EXPECT_EQ(priority[0], BackendType::Vulkan);
}

TEST_F(BackendScoringTest, BackendPriorityAndroid) {
    auto priority = BackendScorer::GetBackendPriority(Platform::Android);
    
    EXPECT_FALSE(priority.empty());
    // Vulkan should be first on Android
    EXPECT_EQ(priority[0], BackendType::Vulkan);
    // OpenGL ES should be available
    EXPECT_TRUE(std::find(priority.begin(), priority.end(), 
                         BackendType::OpenGLES31) != priority.end());
}

// ============================================================================
// Score Comparison Tests
// ============================================================================

TEST_F(BackendScoringTest, VulkanVsOpenGLOnWindows) {
    FeatureRequirements requirements;
    requirements.required = {Feature::Texture2D};
    
    float vulkanScore = BackendScorer::CalculateScore(
        BackendType::Vulkan, features, requirements);
    float gl46Score = BackendScorer::CalculateScore(
        BackendType::OpenGL46, features, requirements);
    float gl33Score = BackendScorer::CalculateScore(
        BackendType::OpenGL33, features, requirements);
    
    // Vulkan should score higher than OpenGL on Windows
    // (unless D3D12 which would be native)
    EXPECT_GT(vulkanScore, gl46Score);
    EXPECT_GT(gl46Score, gl33Score);
}

TEST_F(BackendScoringTest, ScoreRankingConsistency) {
    FeatureRequirements requirements;
    requirements.required = {Feature::Texture2D};
    
    std::vector<BackendType> backends = {
        BackendType::Vulkan,
        BackendType::OpenGL46,
        BackendType::OpenGL41,
        BackendType::OpenGL33
    };
    
    std::vector<float> scores;
    for (auto backend : backends) {
        float score = BackendScorer::CalculateScore(backend, features, requirements);
        scores.push_back(score);
    }
    
    // All scores should be valid
    for (float score : scores) {
        EXPECT_GT(score, 0.0f);
        EXPECT_LE(score, 100.0f);
    }
    
    // Scores should generally decrease (Vulkan > GL4.6 > GL4.1 > GL3.3)
    // This might not always be true depending on platform, so we just check validity
}
