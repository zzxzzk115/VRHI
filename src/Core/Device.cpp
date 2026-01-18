// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#include <VRHI/VRHI.hpp>
#include <VRHI/Backend.hpp>
#include <VRHI/Logging.hpp>
#include "BackendInit.hpp"
#include <algorithm>

namespace VRHI {

// ============================================================================
// Global Initialization/Shutdown
// ============================================================================

namespace {
    bool g_initialized = false;
}

void Initialize() noexcept {
    if (g_initialized) {
        LogWarning("VRHI already initialized");
        return;
    }
    
    LogInfo("Initializing VRHI v1.0.0");
    
    // Initialize all available backends
    InitializeBackends();
    
    g_initialized = true;
}

void Shutdown() noexcept {
    if (!g_initialized) {
        LogWarning("VRHI not initialized");
        return;
    }
    
    LogInfo("Shutting down VRHI");
    g_initialized = false;
}

bool IsInitialized() noexcept {
    return g_initialized;
}

// ============================================================================
// Device Creation
// ============================================================================

std::expected<std::unique_ptr<Device>, Error> 
CreateDevice(const DeviceConfig& config) {
    LogInfo("Creating device...");
    
    // Auto-initialize if not already initialized
    if (!g_initialized) {
        Initialize();
    }
    
    // Determine which backend to use
    std::expected<std::unique_ptr<IBackend>, Error> backendResult;
    
    if (config.preferredBackend == BackendType::Auto) {
        // First, try to create a backend registered specifically for Auto type
        // This allows tests to override auto-selection by registering a mock backend
        backendResult = BackendFactory::CreateBackend(BackendType::Auto);
        
        // If a backend was registered for Auto, verify it meets requirements
        if (backendResult.has_value()) {
            auto& backend = backendResult.value();
            
            // Check required features
            for (const auto& feature : config.features.required) {
                if (!backend->IsFeatureSupported(feature)) {
                    // Backend doesn't meet requirements, fall back to auto-selection
                    backendResult = std::unexpected(Error{
                        Error::Code::NoCompatibleBackend,
                        "Registered Auto backend does not support all required features"
                    });
                    break;
                }
            }
        }
        
        // If no backend is registered for Auto or it doesn't meet requirements,
        // automatically select best backend
        if (!backendResult.has_value()) {
            LogInfo("Auto-selecting best backend based on requirements");
            backendResult = BackendFactory::CreateBestBackend(config.features);
        }
    } else {
        // Use specified backend
        LogInfo("Creating requested backend");
        backendResult = BackendFactory::CreateBackend(config.preferredBackend);
        
        // If the requested backend succeeded, verify it meets requirements
        if (backendResult.has_value()) {
            auto& backend = backendResult.value();
            
            // Check required features
            for (const auto& feature : config.features.required) {
                if (!backend->IsFeatureSupported(feature)) {
                    Error error{};
                    error.code = Error::Code::NoCompatibleBackend;
                    error.message = "Requested backend does not support all required features";
                    LogError(error.message);
                    return std::unexpected(error);
                }
            }
        }
    }
    
    if (!backendResult.has_value()) {
        LogError("Failed to create backend: " + backendResult.error().message);
        return std::unexpected(backendResult.error());
    }
    
    auto& backend = backendResult.value();
    LogInfo(std::string("Selected backend: ") + std::string(backend->GetName()));
    
    // Create device from backend
    auto deviceResult = backend->CreateDevice(config);
    if (!deviceResult.has_value()) {
        LogError("Failed to create device: " + deviceResult.error().message);
        return std::unexpected(deviceResult.error());
    }
    
    LogInfo("Device created successfully");
    return deviceResult;
}

// ============================================================================
// Backend Enumeration
// ============================================================================

std::vector<BackendInfo> EnumerateBackends() {
    LogInfo("Enumerating available backends");
    
    // Auto-initialize if not already initialized
    if (!g_initialized) {
        Initialize();
    }
    
    std::vector<BackendInfo> backends;
    
    // Get all registered backend types
    auto backendTypes = BackendFactory::EnumerateAvailableBackends();
    
    LogInfo("Found " + std::to_string(backendTypes.size()) + " registered backends");
    
    // Create temporary backend instances to query information
    for (const auto& type : backendTypes) {
        auto backendResult = BackendFactory::CreateBackend(type);
        if (!backendResult.has_value()) {
            LogWarning("Failed to create backend for enumeration");
            continue;
        }
        
        auto& backend = backendResult.value();
        
        // Build backend info
        BackendInfo info{};
        info.type = backend->GetType();
        info.name = std::string(backend->GetName());
        
        auto version = backend->GetVersion();
        info.version = std::string(version.string);
        
        info.features = backend->GetSupportedFeatures();
        
        // Calculate score with empty requirements (baseline score)
        FeatureRequirements emptyReqs{};
        info.score = backend->CalculateScore(emptyReqs);
        
        backends.push_back(std::move(info));
    }
    
    // Sort backends by score (highest first)
    std::sort(backends.begin(), backends.end(),
        [](const BackendInfo& a, const BackendInfo& b) {
            return a.score > b.score;
        });
    
    LogInfo("Backend enumeration complete");
    return backends;
}

} // namespace VRHI
