// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#include "MockBackend.hpp"
#include <VRHI/VRHIAll.hpp>
#include <gtest/gtest.h>
#include <cstring>

using namespace VRHI;

class BackendInterfaceTest : public ::testing::Test {
protected:
    void SetUp() override {
        backend = std::make_unique<Mock::MockBackend>();
    }
    
    std::unique_ptr<IBackend> backend;
};

TEST_F(BackendInterfaceTest, GetBasicInfo) {
    EXPECT_EQ(backend->GetType(), BackendType::Auto);
    EXPECT_EQ(backend->GetName(), "Mock");
    
    auto version = backend->GetVersion();
    EXPECT_EQ(version.major, 1);
    EXPECT_EQ(version.minor, 0);
    EXPECT_EQ(version.patch, 0);
}

TEST_F(BackendInterfaceTest, FeatureSupport) {
    EXPECT_TRUE(backend->IsFeatureSupported(Feature::Texture2D));
    EXPECT_TRUE(backend->IsFeatureSupported(Feature::TextureCube));
    EXPECT_FALSE(backend->IsFeatureSupported(Feature::RayTracing));
    EXPECT_FALSE(backend->IsFeatureSupported(Feature::MeshShading));
}

TEST_F(BackendInterfaceTest, GetSupportedFeatures) {
    auto features = backend->GetSupportedFeatures();
    
    EXPECT_TRUE(features.core.vertexShader);
    EXPECT_TRUE(features.core.fragmentShader);
    EXPECT_TRUE(features.core.uniformBuffers);
    EXPECT_TRUE(features.texture.texture2D);
    EXPECT_EQ(features.texture.maxTextureSize, 16384);
    EXPECT_EQ(features.rendering.maxColorAttachments, 8);
}

TEST_F(BackendInterfaceTest, ScoreCalculationWithRequirements) {
    FeatureRequirements requirements{};
    requirements.required = {Feature::Texture2D};
    requirements.optional = {Feature::TextureCube};
    
    float score = backend->CalculateScore(requirements);
    EXPECT_GT(score, 0.0f);
}

TEST_F(BackendInterfaceTest, ScoreCalculationUnsupportedRequirement) {
    FeatureRequirements requirements{};
    requirements.required = {Feature::RayTracing};
    
    float score = backend->CalculateScore(requirements);
    EXPECT_EQ(score, -1.0f);
}

// Device creation is now handled by VRHI::CreateDevice() using DeviceImpl
// Backend interface no longer exposes CreateDevice directly

// The following tests are disabled pending device implementation refactoring
// TODO: Re-enable these tests with proper Device implementation

/*
class DeviceInterfaceTest : public ::testing::Test {
protected:
    void SetUp() override {
        auto backend = std::make_unique<Mock::MockBackend>();
        DeviceConfig config{};
        auto result = backend->CreateDevice(config);
        ASSERT_TRUE(result.has_value());
        device = std::move(result.value());
    }
    
    std::unique_ptr<Device> device;
};

TEST_F(DeviceInterfaceTest, GetDeviceInfo) {
    EXPECT_EQ(device->GetBackendType(), BackendType::Auto);
    
    auto& props = device->GetProperties();
    EXPECT_EQ(props.deviceName, "Mock Device");
    EXPECT_EQ(props.vendorName, "VRHI Mock");
    EXPECT_EQ(props.vendorId, 0xFFFF);
}

TEST_F(DeviceInterfaceTest, CreateBuffer) {
    BufferDesc desc{};
    desc.size = 1024;
    desc.usage = BufferUsage::Vertex;
    
    auto result = device->CreateBuffer(desc);
    ASSERT_TRUE(result.has_value());
    
    auto& buffer = result.value();
    EXPECT_NE(buffer, nullptr);
    EXPECT_EQ(buffer->GetSize(), 1024);
    EXPECT_EQ(buffer->GetUsage(), BufferUsage::Vertex);
}

TEST_F(DeviceInterfaceTest, CreateTexture) {
    TextureDesc desc{};
    desc.type = TextureType::Texture2D;
    desc.format = TextureFormat::RGBA8_UNorm;
    desc.width = 256;
    desc.height = 256;
    
    auto result = device->CreateTexture(desc);
    ASSERT_TRUE(result.has_value());
    
    auto& texture = result.value();
    EXPECT_NE(texture, nullptr);
    EXPECT_EQ(texture->GetType(), TextureType::Texture2D);
    EXPECT_EQ(texture->GetFormat(), TextureFormat::RGBA8_UNorm);
    EXPECT_EQ(texture->GetWidth(), 256);
    EXPECT_EQ(texture->GetHeight(), 256);
}

TEST_F(DeviceInterfaceTest, CreateSampler) {
    SamplerDesc desc{};
    desc.minFilter = FilterMode::Linear;
    desc.magFilter = FilterMode::Linear;
    
    auto result = device->CreateSampler(desc);
    ASSERT_TRUE(result.has_value());
    EXPECT_NE(result.value(), nullptr);
}

TEST_F(DeviceInterfaceTest, CreateShader) {
    const char* shaderCode = "void main() {}";
    
    ShaderDesc desc{};
    desc.stage = ShaderStage::Vertex;
    desc.language = ShaderLanguage::GLSL;
    desc.code = shaderCode;
    desc.codeSize = std::strlen(shaderCode);
    
    auto result = device->CreateShader(desc);
    ASSERT_TRUE(result.has_value());
    
    auto& shader = result.value();
    EXPECT_NE(shader, nullptr);
    EXPECT_EQ(shader->GetStage(), ShaderStage::Vertex);
    EXPECT_EQ(shader->GetLanguage(), ShaderLanguage::GLSL);
}

TEST_F(DeviceInterfaceTest, CreatePipeline) {
    PipelineDesc desc{};
    desc.type = PipelineType::Graphics;
    
    auto result = device->CreatePipeline(desc);
    ASSERT_TRUE(result.has_value());
    
    auto& pipeline = result.value();
    EXPECT_NE(pipeline, nullptr);
    EXPECT_EQ(pipeline->GetType(), PipelineType::Graphics);
}

TEST_F(DeviceInterfaceTest, CreateCommandBuffer) {
    auto cmd = device->CreateCommandBuffer();
    EXPECT_NE(cmd, nullptr);
    EXPECT_EQ(cmd->GetState(), CommandBufferState::Initial);
}

TEST_F(DeviceInterfaceTest, CreateSyncObjects) {
    auto fence = device->CreateFence(false);
    EXPECT_NE(fence, nullptr);
    EXPECT_FALSE(fence->IsSignaled());
    
    auto semaphore = device->CreateSemaphore();
    EXPECT_NE(semaphore, nullptr);
}

class CommandBufferInterfaceTest : public ::testing::Test {
protected:
    void SetUp() override {
        auto backend = std::make_unique<Mock::MockBackend>();
        DeviceConfig config{};
        auto result = backend->CreateDevice(config);
        ASSERT_TRUE(result.has_value());
        device = std::move(result.value());
        
        cmd = device->CreateCommandBuffer();
    }
    
    std::unique_ptr<Device> device;
    std::unique_ptr<CommandBuffer> cmd;
};

TEST_F(CommandBufferInterfaceTest, Lifecycle) {
    EXPECT_EQ(cmd->GetState(), CommandBufferState::Initial);
    
    cmd->Begin();
    EXPECT_EQ(cmd->GetState(), CommandBufferState::Recording);
    
    cmd->End();
    EXPECT_EQ(cmd->GetState(), CommandBufferState::Executable);
    
    cmd->Reset();
    EXPECT_EQ(cmd->GetState(), CommandBufferState::Initial);
}

TEST_F(CommandBufferInterfaceTest, DrawCommands) {
    cmd->Begin();
    
    // Simple draw
    cmd->Draw(3);
    
    // Draw with parameters
    DrawParams params{};
    params.vertexCount = 6;
    params.instanceCount = 2;
    cmd->Draw(params);
    
    cmd->End();
    EXPECT_EQ(cmd->GetState(), CommandBufferState::Executable);
}

TEST_F(CommandBufferInterfaceTest, ComputeCommands) {
    cmd->Begin();
    
    // Simple dispatch
    cmd->Dispatch(1, 1, 1);
    
    // Dispatch with parameters
    DispatchParams params{};
    params.groupCountX = 8;
    params.groupCountY = 8;
    params.groupCountZ = 1;
    cmd->Dispatch(params);
    
    cmd->End();
    EXPECT_EQ(cmd->GetState(), CommandBufferState::Executable);
}

TEST_F(CommandBufferInterfaceTest, SetDynamicState) {
    cmd->Begin();
    
    Viewport viewport{0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 1.0f};
    cmd->SetViewport(viewport);
    
    Rect2D scissor{0, 0, 1280, 720};
    cmd->SetScissor(scissor);
    
    cmd->SetLineWidth(2.0f);
    
    cmd->End();
}
*/
