// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#include <VRHI/VRHI.hpp>

namespace VRHI {

// Global log level
static LogLevel g_logLevel = LogLevel::Info;

void SetLogLevel(LogLevel level) noexcept {
    g_logLevel = level;
}

std::expected<std::unique_ptr<Device>, Error> 
CreateDevice(const DeviceConfig& config) {
    // Placeholder implementation
    // TODO: Implement actual device creation with backend selection
    Error error;
    error.code = Error::Code::InitializationFailed;
    error.message = "Device creation not yet implemented";
    return std::unexpected(error);
}

std::vector<BackendInfo> EnumerateBackends() {
    // Placeholder implementation
    // TODO: Implement actual backend enumeration
    return {};
}

} // namespace VRHI
