// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#include <VRHI/Backend.hpp>
#include <VRHI/Logging.hpp>
#include <map>
#include <mutex>

namespace VRHI {

// ============================================================================
// Backend Factory Implementation
// ============================================================================

namespace {
    // Backend registry storage
    struct BackendRegistry {
        std::map<BackendType, BackendFactory::BackendCreator> creators;
        std::mutex mutex;
    };
    
    BackendRegistry& GetRegistry() {
        static BackendRegistry registry;
        return registry;
    }
}

void BackendFactory::RegisterBackend(BackendType type, BackendCreator creator) {
    auto& registry = GetRegistry();
    std::lock_guard<std::mutex> lock(registry.mutex);
    
    if (registry.creators.find(type) != registry.creators.end()) {
        LogWarning("Backend type already registered, overwriting");
    }
    
    registry.creators[type] = std::move(creator);
    LogInfo("Registered backend: " + std::to_string(static_cast<int>(type)));
}

std::vector<BackendType> BackendFactory::EnumerateAvailableBackends() {
    auto& registry = GetRegistry();
    std::lock_guard<std::mutex> lock(registry.mutex);
    
    std::vector<BackendType> types;
    types.reserve(registry.creators.size());
    
    for (const auto& [type, _] : registry.creators) {
        types.push_back(type);
    }
    
    return types;
}

std::expected<std::unique_ptr<IBackend>, Error>
BackendFactory::CreateBackend(BackendType type) {
    auto& registry = GetRegistry();
    std::lock_guard<std::mutex> lock(registry.mutex);
    
    auto it = registry.creators.find(type);
    if (it == registry.creators.end()) {
        Error error{};
        error.code = Error::Code::NoCompatibleBackend;
        error.message = "Backend type not registered";
        return std::unexpected(error);
    }
    
    try {
        auto backend = it->second();
        if (!backend) {
            Error error{};
            error.code = Error::Code::InitializationFailed;
            error.message = "Backend creator returned null";
            return std::unexpected(error);
        }
        return backend;
    } catch (const std::exception& e) {
        Error error{};
        error.code = Error::Code::InitializationFailed;
        error.message = std::string("Backend creation failed: ") + e.what();
        return std::unexpected(error);
    }
}

std::expected<std::unique_ptr<IBackend>, Error>
BackendFactory::CreateBestBackend(const FeatureRequirements& requirements) {
    auto& registry = GetRegistry();
    std::lock_guard<std::mutex> lock(registry.mutex);
    
    if (registry.creators.empty()) {
        Error error{};
        error.code = Error::Code::NoCompatibleBackend;
        error.message = "No backends registered";
        return std::unexpected(error);
    }
    
    // Score all backends
    struct ScoredBackend {
        BackendType type{};
        float score;
        std::unique_ptr<IBackend> backend;
    };
    
    std::vector<ScoredBackend> scored;
    scored.reserve(registry.creators.size());
    
    for (const auto& [type, creator] : registry.creators) {
        try {
            auto backend = creator();
            if (!backend) {
                continue;
            }
            
            float score = backend->CalculateScore(requirements);
            if (score >= 0.0f) {
                scored.push_back({type, score, std::move(backend)});
            }
        } catch (const std::exception& e) {
            LogWarning(std::string("Failed to create backend for scoring: ") + e.what());
        }
    }
    
    if (scored.empty()) {
        Error error{};
        error.code = Error::Code::NoCompatibleBackend;
        error.message = "No backend meets the required features";
        return std::unexpected(error);
    }
    
    // Find backend with highest score
    auto best = std::max_element(scored.begin(), scored.end(),
        [](const ScoredBackend& a, const ScoredBackend& b) {
            return a.score < b.score;
        });
    
    LogInfo("Selected backend with score: " + std::to_string(best->score));
    return std::move(best->backend);
}

} // namespace VRHI
