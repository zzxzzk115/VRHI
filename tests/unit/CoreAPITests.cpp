// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#include "MockBackend.hpp"
#include <VRHI/VRHIAll.hpp>
#include <gtest/gtest.h>

using namespace VRHI;

// ============================================================================
// Initialization/Shutdown Tests
// ============================================================================

class InitializationTests : public ::testing::Test {
protected:
    void TearDown() override {
        // Always shutdown after each test
        if (IsInitialized()) {
            Shutdown();
        }
    }
};

TEST_F(InitializationTests, InitializeAndShutdown) {
    EXPECT_FALSE(IsInitialized());
    
    Initialize();
    EXPECT_TRUE(IsInitialized());
    
    Shutdown();
    EXPECT_FALSE(IsInitialized());
}

TEST_F(InitializationTests, MultipleInitializeCallsSafe) {
    EXPECT_FALSE(IsInitialized());
    
    Initialize();
    EXPECT_TRUE(IsInitialized());
    
    // Second initialize should be safe (logs warning but doesn't break)
    Initialize();
    EXPECT_TRUE(IsInitialized());
    
    Shutdown();
    EXPECT_FALSE(IsInitialized());
}

TEST_F(InitializationTests, MultipleShutdownCallsSafe) {
    Initialize();
    EXPECT_TRUE(IsInitialized());
    
    Shutdown();
    EXPECT_FALSE(IsInitialized());
    
    // Second shutdown should be safe (logs warning but doesn't break)
    Shutdown();
    EXPECT_FALSE(IsInitialized());
}

TEST_F(InitializationTests, AutoInitializeOnCreateDevice) {
    EXPECT_FALSE(IsInitialized());
    
    // Register mock backend first
    BackendFactory::RegisterBackend(BackendType::Auto, 
        []() { return std::make_unique<Mock::MockBackend>(); });
    
    // CreateDevice should auto-initialize
    DeviceConfig config{};
    auto result = CreateDevice(config);
    
    // Should be initialized now
    EXPECT_TRUE(IsInitialized());
    
    // Clean up
    Shutdown();
}

TEST_F(InitializationTests, AutoInitializeOnEnumerateBackends) {
    EXPECT_FALSE(IsInitialized());
    
    // EnumerateBackends should auto-initialize
    auto backends = EnumerateBackends();
    
    // Should be initialized now
    EXPECT_TRUE(IsInitialized());
    
    // Clean up
    Shutdown();
}

// ============================================================================
// EnumerateBackends Tests
// ============================================================================

class EnumerateBackendsTests : public ::testing::Test {
protected:
    void SetUp() override {
        // Register mock backend
        BackendFactory::RegisterBackend(BackendType::Auto, 
            []() { return std::make_unique<Mock::MockBackend>(); });
    }
    
    void TearDown() override {
        if (IsInitialized()) {
            Shutdown();
        }
    }
};

TEST_F(EnumerateBackendsTests, ReturnsBackendList) {
    auto backends = EnumerateBackends();
    
    // Should have at least the mock backend
    EXPECT_GE(backends.size(), 1);
}

TEST_F(EnumerateBackendsTests, BackendInfoContainsCorrectData) {
    auto backends = EnumerateBackends();
    
    ASSERT_GE(backends.size(), 1);
    
    // Check first backend has valid info
    const auto& info = backends[0];
    EXPECT_FALSE(info.name.empty());
    EXPECT_FALSE(info.version.empty());
    EXPECT_GE(info.score, 0.0f);
}

TEST_F(EnumerateBackendsTests, BackendsSortedByScore) {
    auto backends = EnumerateBackends();
    
    if (backends.size() > 1) {
        // Check that backends are sorted by score (highest first)
        for (size_t i = 1; i < backends.size(); ++i) {
            EXPECT_GE(backends[i-1].score, backends[i].score);
        }
    }
}

// ============================================================================
// CreateDevice Tests
// ============================================================================

class CreateDeviceTests : public ::testing::Test {
protected:
    void SetUp() override {
        // Register mock backend
        BackendFactory::RegisterBackend(BackendType::Auto, 
            []() { return std::make_unique<Mock::MockBackend>(); });
    }
    
    void TearDown() override {
        if (IsInitialized()) {
            Shutdown();
        }
    }
};

TEST_F(CreateDeviceTests, CreateDeviceWithDefaultConfig) {
    DeviceConfig config{};
    auto result = CreateDevice(config);
    
    ASSERT_TRUE(result.has_value());
    auto& device = result.value();
    EXPECT_NE(device, nullptr);
}

TEST_F(CreateDeviceTests, CreateDeviceAutoSelectsBackend) {
    DeviceConfig config{};
    config.preferredBackend = BackendType::Auto;
    
    auto result = CreateDevice(config);
    
    ASSERT_TRUE(result.has_value());
    auto& device = result.value();
    EXPECT_NE(device, nullptr);
}

TEST_F(CreateDeviceTests, CreateDeviceWithSpecificBackend) {
    DeviceConfig config{};
    config.preferredBackend = BackendType::Auto;
    
    auto result = CreateDevice(config);
    
    ASSERT_TRUE(result.has_value());
    auto& device = result.value();
    EXPECT_NE(device, nullptr);
    EXPECT_EQ(device->GetBackendType(), BackendType::Auto);
}

TEST_F(CreateDeviceTests, CreateDeviceWithRequiredFeatures) {
    DeviceConfig config{};
    config.features.required = {Feature::Texture2D};
    
    auto result = CreateDevice(config);
    
    ASSERT_TRUE(result.has_value());
    auto& device = result.value();
    EXPECT_NE(device, nullptr);
    EXPECT_TRUE(device->IsFeatureSupported(Feature::Texture2D));
}

TEST_F(CreateDeviceTests, CreateDeviceWithOptionalFeatures) {
    DeviceConfig config{};
    config.features.optional = {Feature::TextureCube};
    
    auto result = CreateDevice(config);
    
    ASSERT_TRUE(result.has_value());
    auto& device = result.value();
    EXPECT_NE(device, nullptr);
}

TEST_F(CreateDeviceTests, CreateDeviceFailsWithUnsupportedRequiredFeature) {
    DeviceConfig config{};
    config.preferredBackend = BackendType::Auto;
    config.features.required = {Feature::RayTracing}; // Not supported by mock
    
    auto result = CreateDevice(config);
    
    // Should fail because mock backend doesn't support ray tracing
    EXPECT_FALSE(result.has_value());
    if (!result.has_value()) {
        EXPECT_EQ(result.error().code, Error::Code::NoCompatibleBackend);
    }
}

TEST_F(CreateDeviceTests, CreateDeviceWithMixedFeatures) {
    DeviceConfig config{};
    config.features.required = {Feature::Texture2D};
    config.features.optional = {Feature::TextureCube, Feature::RayTracing};
    
    auto result = CreateDevice(config);
    
    ASSERT_TRUE(result.has_value());
    auto& device = result.value();
    EXPECT_NE(device, nullptr);
    EXPECT_TRUE(device->IsFeatureSupported(Feature::Texture2D));
}

TEST_F(CreateDeviceTests, MultipleDeviceCreation) {
    DeviceConfig config{};
    
    auto result1 = CreateDevice(config);
    ASSERT_TRUE(result1.has_value());
    
    auto result2 = CreateDevice(config);
    ASSERT_TRUE(result2.has_value());
    
    // Should be able to create multiple devices
    EXPECT_NE(result1.value(), nullptr);
    EXPECT_NE(result2.value(), nullptr);
}

// ============================================================================
// Integration Tests
// ============================================================================

class CoreAPIIntegrationTests : public ::testing::Test {
protected:
    void SetUp() override {
        // Register mock backend
        BackendFactory::RegisterBackend(BackendType::Auto, 
            []() { return std::make_unique<Mock::MockBackend>(); });
    }
    
    void TearDown() override {
        if (IsInitialized()) {
            Shutdown();
        }
    }
};

TEST_F(CoreAPIIntegrationTests, EnumerateAndCreateDevice) {
    // First enumerate backends
    auto backends = EnumerateBackends();
    ASSERT_GE(backends.size(), 1);
    
    // Then create device with auto-selection
    DeviceConfig config{};
    auto result = CreateDevice(config);
    ASSERT_TRUE(result.has_value());
    
    auto& device = result.value();
    EXPECT_NE(device, nullptr);
}

TEST_F(CoreAPIIntegrationTests, FullWorkflow) {
    // Initialize
    Initialize();
    EXPECT_TRUE(IsInitialized());
    
    // Enumerate backends
    auto backends = EnumerateBackends();
    EXPECT_GE(backends.size(), 1);
    
    // Create device
    DeviceConfig config{};
    auto result = CreateDevice(config);
    ASSERT_TRUE(result.has_value());
    
    auto& device = result.value();
    EXPECT_NE(device, nullptr);
    
    // Get device info
    auto backendInfo = device->GetBackendInfo();
    EXPECT_FALSE(backendInfo.name.empty());
    
    // Shutdown
    Shutdown();
    EXPECT_FALSE(IsInitialized());
}

// ============================================================================
// Main
// ============================================================================

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
