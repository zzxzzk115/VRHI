// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#pragma once

#include "VRHI.hpp"
#include <functional>

namespace VRHI {

// Forward declarations
class IDevice;

// ============================================================================
// Backend Interface
// ============================================================================

/// Abstract backend interface that all graphics backends must implement
class IBackend {
public:
    virtual ~IBackend() = default;
    
    // Backend cannot be copied, only moved
    IBackend(const IBackend&) = delete;
    IBackend& operator=(const IBackend&) = delete;
    IBackend(IBackend&&) noexcept = default;
    IBackend& operator=(IBackend&&) noexcept = default;
    
    // ========================================================================
    // Basic Information
    // ========================================================================
    
    /// Get backend type
    virtual BackendType GetType() const noexcept = 0;
    
    /// Get backend name
    virtual std::string_view GetName() const noexcept = 0;
    
    /// Get backend version
    virtual Version GetVersion() const noexcept = 0;
    
    // ========================================================================
    // Feature Detection
    // ========================================================================
    
    /// Get all supported features
    virtual FeatureSet GetSupportedFeatures() const = 0;
    
    /// Check if a specific feature is supported
    virtual bool IsFeatureSupported(Feature feature) const noexcept = 0;
    
    // ========================================================================
    // Backend Scoring
    // ========================================================================
    
    /// Calculate backend score based on feature requirements
    /// @param requirements Feature requirements from application
    /// @return Score value (higher is better, -1 if requirements not met)
    virtual float CalculateScore(const FeatureRequirements& requirements) const = 0;
    
    // ========================================================================
    // Device Management
    // ========================================================================
    
    /// Create a device instance
    /// @param config Device configuration
    /// @return Device instance on success, error on failure
    virtual std::expected<std::unique_ptr<Device>, Error>
    CreateDevice(const DeviceConfig& config) = 0;
    
    // ========================================================================
    // Native Handle Access (Optional)
    // ========================================================================
    
    /// Get native backend handle for interoperability
    /// @return Native handle or nullptr if not supported
    virtual void* GetNativeHandle() const noexcept { return nullptr; }
    
protected:
    IBackend() = default;
};

// ============================================================================
// Backend Factory
// ============================================================================

/// Backend factory for registration and creation
class BackendFactory {
public:
    /// Backend creator function type
    using BackendCreator = std::function<std::unique_ptr<IBackend>()>;
    
    /// Register a backend
    /// @param type Backend type
    /// @param creator Backend creator function
    static void RegisterBackend(BackendType type, BackendCreator creator);
    
    /// Enumerate all available backends
    /// @return List of available backend types
    static std::vector<BackendType> EnumerateAvailableBackends();
    
    /// Create a backend instance
    /// @param type Backend type
    /// @return Backend instance on success, error on failure
    static std::expected<std::unique_ptr<IBackend>, Error>
    CreateBackend(BackendType type);
    
    /// Create the best backend based on requirements
    /// @param requirements Feature requirements
    /// @return Backend instance on success, error on failure
    static std::expected<std::unique_ptr<IBackend>, Error>
    CreateBestBackend(const FeatureRequirements& requirements = {});
};

// ============================================================================
// Backend Registration Macro
// ============================================================================

/// Macro for automatic backend registration
#define VRHI_REGISTER_BACKEND(Type, Creator) \
    namespace { \
        struct BackendRegistrar_##Type { \
            BackendRegistrar_##Type() { \
                ::VRHI::BackendFactory::RegisterBackend( \
                    ::VRHI::BackendType::Type, Creator); \
            } \
        }; \
        static BackendRegistrar_##Type s_registrar_##Type; \
    }

} // namespace VRHI
