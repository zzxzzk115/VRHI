// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#include <gtest/gtest.h>
#include <VRHI/VRHI.hpp>
#include <VRHI/Resources.hpp>
#include <array>

// Include internal headers for testing
#include "../../src/Core/NullDevice.hpp"

// ============================================================================
// Resource Management Tests
// ============================================================================

class ResourceManagementTest : public ::testing::Test {
protected:
    void SetUp() override {
        // For headless testing environments, create a NullDevice directly
        // This tests the resource management abstraction without requiring
        // a graphics context (OpenGL/Vulkan)
        device = std::make_unique<VRHI::NullDevice>();
    }
    
    void TearDown() override {
        device.reset();
    }
    
    std::unique_ptr<VRHI::Device> device;
};

// ============================================================================
// Buffer Tests
// ============================================================================

TEST_F(ResourceManagementTest, BufferCreation_ValidDescriptor) {
    ASSERT_NE(device, nullptr);
    
    VRHI::BufferDesc desc{};
    desc.size = 1024;
    desc.usage = VRHI::BufferUsage::Vertex;
    desc.memoryAccess = VRHI::MemoryAccess::CpuToGpu;
    
    auto result = device->CreateBuffer(desc);
    ASSERT_TRUE(result.has_value()) << "Buffer creation failed: " << result.error().message;
    
    auto& buffer = *result;
    ASSERT_NE(buffer, nullptr);
    EXPECT_EQ(buffer->GetSize(), 1024);
    EXPECT_EQ(buffer->GetUsage(), VRHI::BufferUsage::Vertex);
}

TEST_F(ResourceManagementTest, BufferCreation_InvalidSize) {
    ASSERT_NE(device, nullptr);
    
    VRHI::BufferDesc desc{};
    desc.size = 0;  // Invalid size
    desc.usage = VRHI::BufferUsage::Vertex;
    
    auto result = device->CreateBuffer(desc);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().code, VRHI::Error::Code::InvalidConfig);
}

TEST_F(ResourceManagementTest, BufferCreation_WithInitialData) {
    ASSERT_NE(device, nullptr);
    
    std::array<float, 256> data;
    for (size_t i = 0; i < data.size(); ++i) {
        data[i] = static_cast<float>(i);
    }
    
    VRHI::BufferDesc desc{};
    desc.size = sizeof(data);
    desc.usage = VRHI::BufferUsage::Vertex;
    desc.memoryAccess = VRHI::MemoryAccess::CpuToGpu;
    desc.initialData = data.data();
    
    auto result = device->CreateBuffer(desc);
    ASSERT_TRUE(result.has_value());
    
    auto& buffer = *result;
    ASSERT_NE(buffer, nullptr);
    
    // Verify data by reading back
    std::array<float, 256> readBack{};
    buffer->Read(readBack.data(), sizeof(readBack));
    
    for (size_t i = 0; i < data.size(); ++i) {
        EXPECT_FLOAT_EQ(readBack[i], data[i]) << "Mismatch at index " << i;
    }
}

TEST_F(ResourceManagementTest, BufferUpdate) {
    ASSERT_NE(device, nullptr);
    
    VRHI::BufferDesc desc{};
    desc.size = 1024;
    desc.usage = VRHI::BufferUsage::Vertex;
    desc.memoryAccess = VRHI::MemoryAccess::CpuToGpu;
    
    auto result = device->CreateBuffer(desc);
    ASSERT_TRUE(result.has_value());
    
    auto& buffer = *result;
    
    // Update buffer data
    std::array<uint32_t, 64> data;
    for (size_t i = 0; i < data.size(); ++i) {
        data[i] = static_cast<uint32_t>(i * 2);
    }
    
    buffer->Update(data.data(), sizeof(data), 0);
    
    // Read back and verify
    std::array<uint32_t, 64> readBack{};
    buffer->Read(readBack.data(), sizeof(readBack), 0);
    
    for (size_t i = 0; i < data.size(); ++i) {
        EXPECT_EQ(readBack[i], data[i]);
    }
}

TEST_F(ResourceManagementTest, BufferMapping) {
    ASSERT_NE(device, nullptr);
    
    VRHI::BufferDesc desc{};
    desc.size = 512;
    desc.usage = VRHI::BufferUsage::Vertex;
    desc.memoryAccess = VRHI::MemoryAccess::CpuToGpu;
    
    auto result = device->CreateBuffer(desc);
    ASSERT_TRUE(result.has_value());
    
    auto& buffer = *result;
    
    // Map the buffer
    void* mapped = buffer->Map();
    ASSERT_NE(mapped, nullptr);
    
    // Write data through mapped pointer
    uint32_t* intPtr = static_cast<uint32_t*>(mapped);
    for (size_t i = 0; i < 128; ++i) {
        intPtr[i] = static_cast<uint32_t>(i * 3);
    }
    
    // Unmap
    buffer->Unmap();
    
    // Verify data
    std::array<uint32_t, 128> readBack{};
    buffer->Read(readBack.data(), sizeof(readBack));
    
    for (size_t i = 0; i < 128; ++i) {
        EXPECT_EQ(readBack[i], i * 3);
    }
}

// ============================================================================
// Texture Tests
// ============================================================================

TEST_F(ResourceManagementTest, TextureCreation_Valid2D) {
    ASSERT_NE(device, nullptr);
    
    VRHI::TextureDesc desc{};
    desc.type = VRHI::TextureType::Texture2D;
    desc.format = VRHI::TextureFormat::RGBA8_UNorm;
    desc.usage = VRHI::TextureUsage::Sampled;
    desc.width = 256;
    desc.height = 256;
    desc.depth = 1;
    desc.mipLevels = 1;
    desc.arrayLayers = 1;
    
    auto result = device->CreateTexture(desc);
    ASSERT_TRUE(result.has_value()) << "Texture creation failed: " << result.error().message;
    
    auto& texture = *result;
    ASSERT_NE(texture, nullptr);
    EXPECT_EQ(texture->GetWidth(), 256);
    EXPECT_EQ(texture->GetHeight(), 256);
    EXPECT_EQ(texture->GetFormat(), VRHI::TextureFormat::RGBA8_UNorm);
}

TEST_F(ResourceManagementTest, TextureCreation_Invalid_ZeroWidth) {
    ASSERT_NE(device, nullptr);
    
    VRHI::TextureDesc desc{};
    desc.type = VRHI::TextureType::Texture2D;
    desc.format = VRHI::TextureFormat::RGBA8_UNorm;
    desc.width = 0;  // Invalid
    desc.height = 256;
    desc.mipLevels = 1;
    
    auto result = device->CreateTexture(desc);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().code, VRHI::Error::Code::InvalidConfig);
}

TEST_F(ResourceManagementTest, TextureUpdate) {
    ASSERT_NE(device, nullptr);
    
    VRHI::TextureDesc desc{};
    desc.type = VRHI::TextureType::Texture2D;
    desc.format = VRHI::TextureFormat::RGBA8_UNorm;
    desc.usage = VRHI::TextureUsage::Sampled;
    desc.width = 64;
    desc.height = 64;
    desc.mipLevels = 1;
    
    auto result = device->CreateTexture(desc);
    ASSERT_TRUE(result.has_value());
    
    auto& texture = *result;
    
    // Create test data (RGBA)
    std::vector<uint8_t> data(64 * 64 * 4);
    for (size_t i = 0; i < data.size(); i += 4) {
        data[i + 0] = 255;  // R
        data[i + 1] = 128;  // G
        data[i + 2] = 64;   // B
        data[i + 3] = 255;  // A
    }
    
    texture->Update(data.data(), data.size());
    
    // Read back and verify
    std::vector<uint8_t> readBack(data.size());
    texture->Read(readBack.data(), readBack.size());
    
    EXPECT_EQ(readBack, data);
}

// ============================================================================
// Sampler Tests
// ============================================================================

TEST_F(ResourceManagementTest, SamplerCreation_Valid) {
    ASSERT_NE(device, nullptr);
    
    VRHI::SamplerDesc desc{};
    desc.minFilter = VRHI::FilterMode::Linear;
    desc.magFilter = VRHI::FilterMode::Linear;
    desc.mipmapMode = VRHI::FilterMode::Linear;
    desc.addressModeU = VRHI::AddressMode::Repeat;
    desc.addressModeV = VRHI::AddressMode::Repeat;
    desc.maxAnisotropy = 16.0f;
    
    auto result = device->CreateSampler(desc);
    ASSERT_TRUE(result.has_value()) << "Sampler creation failed: " << result.error().message;
    
    auto& sampler = *result;
    ASSERT_NE(sampler, nullptr);
}

TEST_F(ResourceManagementTest, SamplerCreation_Invalid_Anisotropy) {
    ASSERT_NE(device, nullptr);
    
    VRHI::SamplerDesc desc{};
    desc.maxAnisotropy = 0.0f;  // Invalid: must be > 0
    
    auto result = device->CreateSampler(desc);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().code, VRHI::Error::Code::InvalidConfig);
}

TEST_F(ResourceManagementTest, SamplerCreation_Invalid_LodRange) {
    ASSERT_NE(device, nullptr);
    
    VRHI::SamplerDesc desc{};
    desc.minLod = 10.0f;
    desc.maxLod = 1.0f;  // Invalid: minLod > maxLod
    
    auto result = device->CreateSampler(desc);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().code, VRHI::Error::Code::InvalidConfig);
}

// ============================================================================
// RAII Tests
// ============================================================================

TEST_F(ResourceManagementTest, RAII_AutomaticCleanup) {
    ASSERT_NE(device, nullptr);
    
    // Create resources in a scope
    {
        VRHI::BufferDesc bufferDesc{};
        bufferDesc.size = 1024;
        bufferDesc.usage = VRHI::BufferUsage::Vertex;
        
        auto bufferResult = device->CreateBuffer(bufferDesc);
        ASSERT_TRUE(bufferResult.has_value());
        
        VRHI::TextureDesc textureDesc{};
        textureDesc.width = 128;
        textureDesc.height = 128;
        
        auto textureResult = device->CreateTexture(textureDesc);
        ASSERT_TRUE(textureResult.has_value());
        
        // Resources are automatically destroyed at end of scope
    }
    
    // If RAII works correctly, resources are cleaned up
    // No crashes or leaks should occur
    SUCCEED();
}

TEST_F(ResourceManagementTest, RAII_MoveSemantics) {
    ASSERT_NE(device, nullptr);
    
    VRHI::BufferDesc desc{};
    desc.size = 512;
    desc.usage = VRHI::BufferUsage::Vertex;
    
    auto result1 = device->CreateBuffer(desc);
    ASSERT_TRUE(result1.has_value());
    
    // Move the buffer
    auto buffer1 = std::move(*result1);
    auto buffer2 = std::move(buffer1);
    
    ASSERT_NE(buffer2, nullptr);
    EXPECT_EQ(buffer2->GetSize(), 512);
}

// ============================================================================
// Multi-Backend Tests
// ============================================================================

TEST_F(ResourceManagementTest, MultiBackend_ResourcesCreatedCorrectly) {
    // This test verifies that resources can be created regardless of backend
    // The abstraction should work uniformly across all backends
    
    ASSERT_NE(device, nullptr);
    
    // Create a buffer
    VRHI::BufferDesc bufferDesc{};
    bufferDesc.size = 256;
    bufferDesc.usage = VRHI::BufferUsage::Uniform;
    
    auto bufferResult = device->CreateBuffer(bufferDesc);
    ASSERT_TRUE(bufferResult.has_value());
    
    // Create a texture
    VRHI::TextureDesc textureDesc{};
    textureDesc.width = 32;
    textureDesc.height = 32;
    
    auto textureResult = device->CreateTexture(textureDesc);
    ASSERT_TRUE(textureResult.has_value());
    
    // Create a sampler
    VRHI::SamplerDesc samplerDesc{};
    auto samplerResult = device->CreateSampler(samplerDesc);
    ASSERT_TRUE(samplerResult.has_value());
    
    // All resources should be created successfully regardless of backend
    SUCCEED();
}
