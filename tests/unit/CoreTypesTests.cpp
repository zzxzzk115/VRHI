// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#include <VRHI/VRHI.hpp>
#include <gtest/gtest.h>

// ============================================================================
// Error Class Tests
// ============================================================================

TEST(ErrorTests, DefaultConstructor) {
    VRHI::Error error{};
    EXPECT_EQ(error.code, VRHI::Error::Code::Success);
    EXPECT_TRUE(error.message.empty());
    EXPECT_TRUE(error);  // Success should evaluate to true
}

TEST(ErrorTests, SuccessCode) {
    VRHI::Error error{};
    error.code = VRHI::Error::Code::Success;
    EXPECT_TRUE(error);
}

TEST(ErrorTests, ErrorCodes) {
    VRHI::Error error{};
    
    error.code = VRHI::Error::Code::InvalidConfig;
    EXPECT_FALSE(error);
    
    error.code = VRHI::Error::Code::NoCompatibleBackend;
    EXPECT_FALSE(error);
    
    error.code = VRHI::Error::Code::InitializationFailed;
    EXPECT_FALSE(error);
    
    error.code = VRHI::Error::Code::DeviceLost;
    EXPECT_FALSE(error);
    
    error.code = VRHI::Error::Code::OutOfMemory;
    EXPECT_FALSE(error);
    
    error.code = VRHI::Error::Code::ValidationError;
    EXPECT_FALSE(error);
    
    error.code = VRHI::Error::Code::CompilationError;
    EXPECT_FALSE(error);
    
    error.code = VRHI::Error::Code::UnsupportedFeature;
    EXPECT_FALSE(error);
}

TEST(ErrorTests, ErrorMessage) {
    VRHI::Error error{};
    error.code = VRHI::Error::Code::InvalidConfig;
    error.message = "Test error message";
    
    EXPECT_FALSE(error);
    EXPECT_EQ(error.message, "Test error message");
}

// ============================================================================
// BackendType Enum Tests
// ============================================================================

TEST(BackendTypeTests, AllBackendTypes) {
    // Modern APIs
    VRHI::BackendType vulkan = VRHI::BackendType::Vulkan;
    VRHI::BackendType d3d12 = VRHI::BackendType::Direct3D12;
    VRHI::BackendType metal = VRHI::BackendType::Metal;
    
    // OpenGL Desktop
    VRHI::BackendType gl46 = VRHI::BackendType::OpenGL46;
    VRHI::BackendType gl41 = VRHI::BackendType::OpenGL41;
    VRHI::BackendType gl33 = VRHI::BackendType::OpenGL33;
    
    // OpenGL ES Mobile
    VRHI::BackendType gles31 = VRHI::BackendType::OpenGLES31;
    VRHI::BackendType gles30 = VRHI::BackendType::OpenGLES30;
    
    // Future/Experimental
    VRHI::BackendType webgpu = VRHI::BackendType::WebGPU;
    
    // Auto
    VRHI::BackendType autoBackend = VRHI::BackendType::Auto;
    
    // Ensure they compile and are distinct
    EXPECT_NE(vulkan, d3d12);
    EXPECT_NE(vulkan, metal);
    EXPECT_NE(gl46, gl41);
    EXPECT_NE(gl41, gl33);
    EXPECT_NE(gles31, gles30);
    EXPECT_NE(webgpu, autoBackend);
}

// ============================================================================
// Feature Enum Tests
// ============================================================================

TEST(FeatureTests, CoreFeatures) {
    VRHI::Feature compute = VRHI::Feature::Compute;
    VRHI::Feature geometry = VRHI::Feature::Geometry;
    VRHI::Feature tessellation = VRHI::Feature::Tessellation;
    
    EXPECT_NE(compute, geometry);
    EXPECT_NE(geometry, tessellation);
}

TEST(FeatureTests, TextureFeatures) {
    VRHI::Feature tex1d = VRHI::Feature::Texture1D;
    VRHI::Feature tex2d = VRHI::Feature::Texture2D;
    VRHI::Feature tex3d = VRHI::Feature::Texture3D;
    VRHI::Feature texCube = VRHI::Feature::TextureCube;
    VRHI::Feature tex2dArray = VRHI::Feature::Texture2DArray;
    VRHI::Feature texCompression = VRHI::Feature::TextureCompression;
    VRHI::Feature anisotropic = VRHI::Feature::AnisotropicFiltering;
    
    EXPECT_NE(tex1d, tex2d);
    EXPECT_NE(tex2d, tex3d);
    EXPECT_NE(tex3d, texCube);
    EXPECT_NE(texCube, tex2dArray);
    EXPECT_NE(tex2dArray, texCompression);
    EXPECT_NE(texCompression, anisotropic);
}

TEST(FeatureTests, AdvancedFeatures) {
    VRHI::Feature multiDraw = VRHI::Feature::MultiDrawIndirect;
    VRHI::Feature rayTracing = VRHI::Feature::RayTracing;
    VRHI::Feature meshShading = VRHI::Feature::MeshShading;
    VRHI::Feature asyncCompute = VRHI::Feature::AsyncCompute;
    VRHI::Feature bindless = VRHI::Feature::BindlessResources;
    
    EXPECT_NE(multiDraw, rayTracing);
    EXPECT_NE(rayTracing, meshShading);
    EXPECT_NE(meshShading, asyncCompute);
    EXPECT_NE(asyncCompute, bindless);
}

TEST(FeatureTests, DebugFeatures) {
    VRHI::Feature debugMarkers = VRHI::Feature::DebugMarkers;
    VRHI::Feature gpuValidation = VRHI::Feature::GPUValidation;
    
    EXPECT_NE(debugMarkers, gpuValidation);
}

// ============================================================================
// FeatureRequirements Struct Tests
// ============================================================================

TEST(FeatureRequirementsTests, DefaultConstructor) {
    VRHI::FeatureRequirements requirements{};
    EXPECT_TRUE(requirements.required.empty());
    EXPECT_TRUE(requirements.optional.empty());
}

TEST(FeatureRequirementsTests, RequiredFeatures) {
    VRHI::FeatureRequirements requirements{};
    requirements.required = {
        VRHI::Feature::Compute,
        VRHI::Feature::Texture2D,
        VRHI::Feature::MultiDrawIndirect
    };
    
    EXPECT_EQ(requirements.required.size(), 3);
    EXPECT_EQ(requirements.required[0], VRHI::Feature::Compute);
    EXPECT_EQ(requirements.required[1], VRHI::Feature::Texture2D);
    EXPECT_EQ(requirements.required[2], VRHI::Feature::MultiDrawIndirect);
}

TEST(FeatureRequirementsTests, OptionalFeatures) {
    VRHI::FeatureRequirements requirements{};
    requirements.optional = {
        VRHI::Feature::RayTracing,
        VRHI::Feature::MeshShading
    };
    
    EXPECT_EQ(requirements.optional.size(), 2);
    EXPECT_EQ(requirements.optional[0], VRHI::Feature::RayTracing);
    EXPECT_EQ(requirements.optional[1], VRHI::Feature::MeshShading);
}

TEST(FeatureRequirementsTests, MixedFeatures) {
    VRHI::FeatureRequirements requirements{};
    requirements.required = {VRHI::Feature::Compute};
    requirements.optional = {VRHI::Feature::RayTracing};
    
    EXPECT_EQ(requirements.required.size(), 1);
    EXPECT_EQ(requirements.optional.size(), 1);
}

// ============================================================================
// DeviceConfig Struct Tests
// ============================================================================

TEST(DeviceConfigTests, DefaultValues) {
    VRHI::DeviceConfig config{};
    
    EXPECT_EQ(config.preferredBackend, VRHI::BackendType::Auto);
    EXPECT_TRUE(config.features.required.empty());
    EXPECT_TRUE(config.features.optional.empty());
    EXPECT_FALSE(config.enableValidation);
    EXPECT_FALSE(config.enableDebugMarkers);
    EXPECT_EQ(config.windowHandle, nullptr);
    EXPECT_EQ(config.width, 1280);
    EXPECT_EQ(config.height, 720);
    EXPECT_TRUE(config.vsync);
    EXPECT_EQ(config.backBufferCount, 2);
    EXPECT_EQ(config.logLevel, VRHI::LogLevel::Info);
}

TEST(DeviceConfigTests, CustomBackend) {
    VRHI::DeviceConfig config{};
    config.preferredBackend = VRHI::BackendType::Vulkan;
    
    EXPECT_EQ(config.preferredBackend, VRHI::BackendType::Vulkan);
}

TEST(DeviceConfigTests, CustomFeatures) {
    VRHI::DeviceConfig config{};
    config.features.required = {VRHI::Feature::Compute};
    config.features.optional = {VRHI::Feature::RayTracing};
    
    EXPECT_EQ(config.features.required.size(), 1);
    EXPECT_EQ(config.features.optional.size(), 1);
}

TEST(DeviceConfigTests, ValidationAndDebug) {
    VRHI::DeviceConfig config{};
    config.enableValidation = true;
    config.enableDebugMarkers = true;
    
    EXPECT_TRUE(config.enableValidation);
    EXPECT_TRUE(config.enableDebugMarkers);
}

TEST(DeviceConfigTests, WindowConfiguration) {
    VRHI::DeviceConfig config{};
    config.width = 1920;
    config.height = 1080;
    config.vsync = false;
    config.backBufferCount = 3;
    
    EXPECT_EQ(config.width, 1920);
    EXPECT_EQ(config.height, 1080);
    EXPECT_FALSE(config.vsync);
    EXPECT_EQ(config.backBufferCount, 3);
}

TEST(DeviceConfigTests, LogLevel) {
    VRHI::DeviceConfig config{};
    config.logLevel = VRHI::LogLevel::Debug;
    EXPECT_EQ(config.logLevel, VRHI::LogLevel::Debug);
    
    config.logLevel = VRHI::LogLevel::Warning;
    EXPECT_EQ(config.logLevel, VRHI::LogLevel::Warning);
    
    config.logLevel = VRHI::LogLevel::Error;
    EXPECT_EQ(config.logLevel, VRHI::LogLevel::Error);
    
    config.logLevel = VRHI::LogLevel::Off;
    EXPECT_EQ(config.logLevel, VRHI::LogLevel::Off);
}

// ============================================================================
// Version Struct Tests
// ============================================================================

TEST(VersionTests, VersionStructure) {
    VRHI::Version version = VRHI::GetVersion();
    
    // Version should have valid components
    EXPECT_EQ(version.major, 1);
    EXPECT_EQ(version.minor, 0);
    EXPECT_EQ(version.patch, 0);
    EXPECT_FALSE(version.string.empty());
}

TEST(VersionTests, VersionString) {
    VRHI::Version version = VRHI::GetVersion();
    
    // Version string should contain version numbers
    std::string versionStr(version.string);
    EXPECT_TRUE(versionStr.find("1.0.0") != std::string::npos);
}

// ============================================================================
// FeatureSet Struct Tests
// ============================================================================

TEST(FeatureSetTests, DefaultConstructor) {
    VRHI::FeatureSet features{};
    
    // Core features should be false by default
    EXPECT_FALSE(features.core.vertexShader);
    EXPECT_FALSE(features.core.fragmentShader);
    EXPECT_FALSE(features.core.geometryShader);
    EXPECT_FALSE(features.core.tessellationShader);
    EXPECT_FALSE(features.core.computeShader);
    EXPECT_FALSE(features.core.meshShader);
    
    // Buffer features
    EXPECT_FALSE(features.core.uniformBuffers);
    EXPECT_FALSE(features.core.storageBuffers);
    EXPECT_FALSE(features.core.vertexBuffers);
    EXPECT_FALSE(features.core.indexBuffers);
    EXPECT_FALSE(features.core.indirectBuffers);
    
    // Other features
    EXPECT_FALSE(features.core.instancing);
    EXPECT_FALSE(features.core.multiDrawIndirect);
}

TEST(FeatureSetTests, TextureFeatures) {
    VRHI::FeatureSet features{};
    
    EXPECT_FALSE(features.texture.texture1D);
    EXPECT_FALSE(features.texture.texture2D);
    EXPECT_FALSE(features.texture.texture3D);
    EXPECT_FALSE(features.texture.textureCube);
    EXPECT_FALSE(features.texture.texture2DArray);
    
    EXPECT_FALSE(features.texture.floatTextures);
    EXPECT_FALSE(features.texture.depthTextures);
    EXPECT_FALSE(features.texture.compressedTextures);
    
    EXPECT_FALSE(features.texture.dxt);
    EXPECT_FALSE(features.texture.etc2);
    EXPECT_FALSE(features.texture.astc);
    
    EXPECT_FALSE(features.texture.anisotropicFiltering);
    
    EXPECT_EQ(features.texture.maxTextureSize, 0);
    EXPECT_EQ(features.texture.max3DTextureSize, 0);
    EXPECT_EQ(features.texture.maxArrayLayers, 0);
    EXPECT_FLOAT_EQ(features.texture.maxAnisotropy, 0.0f);
}

TEST(FeatureSetTests, RenderingFeatures) {
    VRHI::FeatureSet features{};
    
    EXPECT_FALSE(features.rendering.multipleRenderTargets);
    EXPECT_EQ(features.rendering.maxColorAttachments, 0);
    EXPECT_FALSE(features.rendering.independentBlend);
    EXPECT_FALSE(features.rendering.depthClamp);
    EXPECT_FALSE(features.rendering.multisample);
    EXPECT_EQ(features.rendering.maxSamples, 0);
}

TEST(FeatureSetTests, ComputeFeatures) {
    VRHI::FeatureSet features{};
    
    EXPECT_FALSE(features.compute.computeShader);
    EXPECT_EQ(features.compute.maxWorkGroupSizeX, 0);
    EXPECT_EQ(features.compute.maxWorkGroupSizeY, 0);
    EXPECT_EQ(features.compute.maxWorkGroupSizeZ, 0);
    EXPECT_EQ(features.compute.maxWorkGroupInvocations, 0);
    EXPECT_EQ(features.compute.maxComputeSharedMemorySize, 0);
}

TEST(FeatureSetTests, AdvancedFeatures) {
    VRHI::FeatureSet features{};
    
    EXPECT_FALSE(features.advanced.rayTracing);
    EXPECT_FALSE(features.advanced.meshShading);
    EXPECT_FALSE(features.advanced.variableRateShading);
    EXPECT_FALSE(features.advanced.bindlessResources);
    EXPECT_FALSE(features.advanced.asyncCompute);
}

TEST(FeatureSetTests, MemoryFeatures) {
    VRHI::FeatureSet features{};
    
    EXPECT_EQ(features.memory.deviceLocalMemory, 0);
    EXPECT_EQ(features.memory.hostVisibleMemory, 0);
    EXPECT_EQ(features.memory.minUniformBufferAlignment, 0);
    EXPECT_EQ(features.memory.minStorageBufferAlignment, 0);
    EXPECT_FALSE(features.memory.unifiedMemory);
}

// ============================================================================
// BackendInfo Struct Tests
// ============================================================================

TEST(BackendInfoTests, DefaultConstructor) {
    VRHI::BackendInfo info{};
    
    EXPECT_TRUE(info.name.empty());
    EXPECT_TRUE(info.version.empty());
    EXPECT_FLOAT_EQ(info.score, 0.0f);
    EXPECT_TRUE(info.scoreDetails.empty());
    EXPECT_TRUE(info.deviceName.empty());
    EXPECT_TRUE(info.vendorName.empty());
    EXPECT_TRUE(info.driverVersion.empty());
}

TEST(BackendInfoTests, ScoreDetails) {
    VRHI::BackendInfo info{};
    
    VRHI::BackendInfo::ScoreDetail detail{};
    detail.category = "Performance";
    detail.score = 85.0f;
    detail.weight = 0.3f;
    
    info.scoreDetails.push_back(detail);
    
    EXPECT_EQ(info.scoreDetails.size(), 1);
    EXPECT_EQ(info.scoreDetails[0].category, "Performance");
    EXPECT_FLOAT_EQ(info.scoreDetails[0].score, 85.0f);
    EXPECT_FLOAT_EQ(info.scoreDetails[0].weight, 0.3f);
}

// ============================================================================
// DeviceProperties Struct Tests
// ============================================================================

TEST(DevicePropertiesTests, DefaultConstructor) {
    VRHI::DeviceProperties props{};
    
    EXPECT_TRUE(props.deviceName.empty());
    EXPECT_TRUE(props.vendorName.empty());
    EXPECT_EQ(props.vendorId, 0);
    EXPECT_EQ(props.deviceId, 0);
    EXPECT_TRUE(props.driverVersion.empty());
    EXPECT_TRUE(props.apiVersion.empty());
    EXPECT_EQ(props.totalMemory, 0);
    EXPECT_EQ(props.availableMemory, 0);
    EXPECT_EQ(props.computeUnits, 0);
    EXPECT_EQ(props.maxThreadsPerGroup, 0);
}

// ============================================================================
// LogLevel Enum Tests
// ============================================================================

TEST(LogLevelTests, AllLevels) {
    VRHI::LogLevel debug = VRHI::LogLevel::Debug;
    VRHI::LogLevel info = VRHI::LogLevel::Info;
    VRHI::LogLevel warning = VRHI::LogLevel::Warning;
    VRHI::LogLevel error = VRHI::LogLevel::Error;
    VRHI::LogLevel off = VRHI::LogLevel::Off;
    
    EXPECT_NE(debug, info);
    EXPECT_NE(info, warning);
    EXPECT_NE(warning, error);
    EXPECT_NE(error, off);
}

// ============================================================================
// Main
// ============================================================================

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
