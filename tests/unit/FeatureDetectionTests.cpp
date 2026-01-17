// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#include <VRHI/VRHIAll.hpp>
#include <VRHI/BackendScoring.hpp>
#include <gtest/gtest.h>

using namespace VRHI;

// ============================================================================
// Feature Support Mapping Tests (B2.1)
// ============================================================================

TEST(FeatureDetectionTests, IsFeatureSupportedCoreFeatures) {
    FeatureSet features{};
    
    // Test compute shader feature
    features.core.computeShader = true;
    EXPECT_TRUE(IsFeatureSupported(features, Feature::Compute));
    
    features.core.computeShader = false;
    EXPECT_FALSE(IsFeatureSupported(features, Feature::Compute));
    
    // Test geometry shader feature
    features.core.geometryShader = true;
    EXPECT_TRUE(IsFeatureSupported(features, Feature::Geometry));
    
    // Test tessellation shader feature
    features.core.tessellationShader = true;
    EXPECT_TRUE(IsFeatureSupported(features, Feature::Tessellation));
}

TEST(FeatureDetectionTests, IsFeatureSupportedTextureFeatures) {
    FeatureSet features{};
    
    // Test various texture features
    features.texture.texture1D = true;
    EXPECT_TRUE(IsFeatureSupported(features, Feature::Texture1D));
    
    features.texture.texture2D = true;
    EXPECT_TRUE(IsFeatureSupported(features, Feature::Texture2D));
    
    features.texture.texture3D = true;
    EXPECT_TRUE(IsFeatureSupported(features, Feature::Texture3D));
    
    features.texture.textureCube = true;
    EXPECT_TRUE(IsFeatureSupported(features, Feature::TextureCube));
    
    features.texture.texture2DArray = true;
    EXPECT_TRUE(IsFeatureSupported(features, Feature::Texture2DArray));
    
    features.texture.compressedTextures = true;
    EXPECT_TRUE(IsFeatureSupported(features, Feature::TextureCompression));
    
    features.texture.anisotropicFiltering = true;
    EXPECT_TRUE(IsFeatureSupported(features, Feature::AnisotropicFiltering));
}

TEST(FeatureDetectionTests, IsFeatureSupportedAdvancedFeatures) {
    FeatureSet features{};
    
    // Test advanced features
    features.core.multiDrawIndirect = true;
    EXPECT_TRUE(IsFeatureSupported(features, Feature::MultiDrawIndirect));
    
    features.advanced.rayTracing = true;
    EXPECT_TRUE(IsFeatureSupported(features, Feature::RayTracing));
    
    features.advanced.meshShading = true;
    EXPECT_TRUE(IsFeatureSupported(features, Feature::MeshShading));
    
    features.advanced.asyncCompute = true;
    EXPECT_TRUE(IsFeatureSupported(features, Feature::AsyncCompute));
    
    features.advanced.bindlessResources = true;
    EXPECT_TRUE(IsFeatureSupported(features, Feature::BindlessResources));
}

TEST(FeatureDetectionTests, IsFeatureSupportedDebugFeatures) {
    FeatureSet features{};
    
    // Debug features should always be supported (as per BackendScoring.cpp)
    EXPECT_TRUE(IsFeatureSupported(features, Feature::DebugMarkers));
    EXPECT_TRUE(IsFeatureSupported(features, Feature::GPUValidation));
}

// ============================================================================
// Required Feature Validation Tests (B2.2)
// ============================================================================

TEST(FeatureDetectionTests, RequiredFeatureValidationSuccess) {
    FeatureSet features{};
    features.core.computeShader = true;
    features.texture.texture3D = true;
    
    FeatureRequirements requirements{};
    requirements.required = {Feature::Compute, Feature::Texture3D};
    
    // Feature scoring should succeed when all requirements are met
    float score = BackendScorer::CalculateFeatureScore(features, requirements);
    EXPECT_GE(score, 0.0f);
}

TEST(FeatureDetectionTests, RequiredFeatureValidationFailure) {
    FeatureSet features{};
    features.core.computeShader = false;  // Compute not supported
    features.texture.texture3D = true;
    
    FeatureRequirements requirements{};
    requirements.required = {Feature::Compute, Feature::Texture3D};
    
    // Feature scoring should fail when required features are missing
    float score = BackendScorer::CalculateFeatureScore(features, requirements);
    EXPECT_EQ(score, -1.0f);
}

TEST(FeatureDetectionTests, OptionalFeatureScoring) {
    FeatureSet features{};
    features.core.computeShader = true;
    features.advanced.rayTracing = false;
    
    FeatureRequirements requirements{};
    requirements.required = {Feature::Compute};
    requirements.optional = {Feature::RayTracing};
    
    // Should succeed even without optional features
    float score = BackendScorer::CalculateFeatureScore(features, requirements);
    EXPECT_GE(score, 0.0f);
    
    // Now add the optional feature
    features.advanced.rayTracing = true;
    float scoreWithOptional = BackendScorer::CalculateFeatureScore(features, requirements);
    
    // Score should be higher or equal with optional feature
    // (may be capped at 100, so >= instead of >)
    EXPECT_GE(scoreWithOptional, score);
}

TEST(FeatureDetectionTests, MultipleRequiredFeaturesValidation) {
    FeatureSet features{};
    features.core.computeShader = true;
    features.texture.texture3D = true;
    features.core.multiDrawIndirect = true;
    features.texture.anisotropicFiltering = true;
    
    FeatureRequirements requirements{};
    requirements.required = {
        Feature::Compute,
        Feature::Texture3D,
        Feature::MultiDrawIndirect,
        Feature::AnisotropicFiltering
    };
    
    // All required features are supported
    float score = BackendScorer::CalculateFeatureScore(features, requirements);
    EXPECT_GE(score, 0.0f);
    
    // Remove one required feature
    features.texture.texture3D = false;
    score = BackendScorer::CalculateFeatureScore(features, requirements);
    EXPECT_EQ(score, -1.0f);
}

// ============================================================================
// Feature Set Structure Tests
// ============================================================================

TEST(FeatureDetectionTests, FeatureSetDefaultState) {
    FeatureSet features{};
    
    // All features should default to false/0
    EXPECT_FALSE(features.core.computeShader);
    EXPECT_FALSE(features.core.geometryShader);
    EXPECT_FALSE(features.texture.texture1D);
    EXPECT_FALSE(features.advanced.rayTracing);
    EXPECT_EQ(features.texture.maxTextureSize, 0);
    EXPECT_EQ(features.compute.maxWorkGroupSizeX, 0);
    EXPECT_EQ(features.memory.deviceLocalMemory, 0);
}

TEST(FeatureDetectionTests, FeatureSetLimits) {
    FeatureSet features{};
    
    // Set various limits
    features.texture.maxTextureSize = 16384;
    features.texture.maxAnisotropy = 16.0f;
    features.compute.maxWorkGroupSizeX = 1024;
    features.compute.maxWorkGroupSizeY = 1024;
    features.compute.maxWorkGroupSizeZ = 64;
    features.rendering.maxColorAttachments = 8;
    features.memory.deviceLocalMemory = 4ULL * 1024 * 1024 * 1024; // 4GB
    
    EXPECT_EQ(features.texture.maxTextureSize, 16384);
    EXPECT_FLOAT_EQ(features.texture.maxAnisotropy, 16.0f);
    EXPECT_EQ(features.compute.maxWorkGroupSizeX, 1024);
    EXPECT_EQ(features.rendering.maxColorAttachments, 8);
    EXPECT_EQ(features.memory.deviceLocalMemory, 4ULL * 1024 * 1024 * 1024);
}

// ============================================================================
// Device Feature Queries (B2.6 - Compute Shader Detection)
// ============================================================================

TEST(FeatureDetectionTests, ComputeShaderDetection) {
    FeatureSet features{};
    
    // Enable compute shader support
    features.core.computeShader = true;
    features.core.storageBuffers = true;
    features.compute.maxWorkGroupSizeX = 1024;
    features.compute.maxWorkGroupSizeY = 1024;
    features.compute.maxWorkGroupSizeZ = 64;
    features.compute.maxWorkGroupInvocations = 1024;
    features.compute.maxComputeSharedMemorySize = 49152; // 48KB typical
    
    // Verify compute shader is detected
    EXPECT_TRUE(features.core.computeShader);
    EXPECT_TRUE(IsFeatureSupported(features, Feature::Compute));
    
    // Verify compute limits are set
    EXPECT_GT(features.compute.maxWorkGroupSizeX, 0);
    EXPECT_GT(features.compute.maxWorkGroupSizeY, 0);
    EXPECT_GT(features.compute.maxWorkGroupSizeZ, 0);
    EXPECT_GT(features.compute.maxWorkGroupInvocations, 0);
}

TEST(FeatureDetectionTests, ComputeShaderNotSupported) {
    FeatureSet features{};
    
    // Explicitly disable compute shader
    features.core.computeShader = false;
    
    // Verify compute shader is not detected
    EXPECT_FALSE(features.core.computeShader);
    EXPECT_FALSE(IsFeatureSupported(features, Feature::Compute));
    
    // Limits should be zero
    EXPECT_EQ(features.compute.maxWorkGroupSizeX, 0);
    EXPECT_EQ(features.compute.maxWorkGroupSizeY, 0);
    EXPECT_EQ(features.compute.maxWorkGroupSizeZ, 0);
}

// ============================================================================
// Texture Format Capability Tests (B2.5)
// ============================================================================

TEST(FeatureDetectionTests, TextureFormatCapabilities) {
    FeatureSet features{};
    
    // Set various texture format capabilities
    features.texture.floatTextures = true;
    features.texture.depthTextures = true;
    features.texture.compressedTextures = true;
    
    // Compression format support
    features.texture.dxt = true;
    features.texture.etc2 = true;
    features.texture.astc = true;
    
    // Verify detection
    EXPECT_TRUE(features.texture.floatTextures);
    EXPECT_TRUE(features.texture.depthTextures);
    EXPECT_TRUE(features.texture.compressedTextures);
    EXPECT_TRUE(features.texture.dxt);
    EXPECT_TRUE(features.texture.etc2);
    EXPECT_TRUE(features.texture.astc);
}

TEST(FeatureDetectionTests, TextureSizeLimits) {
    FeatureSet features{};
    
    // Set texture size limits
    features.texture.maxTextureSize = 16384;
    features.texture.max3DTextureSize = 2048;
    features.texture.maxArrayLayers = 2048;
    
    EXPECT_EQ(features.texture.maxTextureSize, 16384);
    EXPECT_EQ(features.texture.max3DTextureSize, 2048);
    EXPECT_EQ(features.texture.maxArrayLayers, 2048);
}

// ============================================================================
// Main
// ============================================================================

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
