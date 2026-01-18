// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#pragma once

#include "VRHI.hpp"
#include <cstdarg>
#include <string_view>

namespace VRHI {

// ============================================================================
// Logging Interface
// ============================================================================

/// Abstract logger interface for custom logging implementations.
/// Users can inherit from this class to integrate their own logging systems
/// (e.g., spdlog, custom file loggers, etc.)
class ILogger {
public:
    virtual ~ILogger() = default;
    
    /// Log a simple message
    /// @param level The log level
    /// @param message The log message
    virtual void Log(LogLevel level, std::string_view message) = 0;
    
    /// Log a formatted message (printf-style)
    /// @param level The log level
    /// @param format Printf-style format string
    /// @param args Variable arguments list
    virtual void LogFormatted(LogLevel level, const char* format, va_list args) = 0;
};

// ============================================================================
// Global Logging Configuration
// ============================================================================

/// Set custom logger instance
/// @param logger Custom logger instance, or nullptr to use default logger
/// @note The caller retains ownership of the logger pointer
void SetLogger(ILogger* logger) noexcept;

/// Set global log level (filters messages below this level)
/// @param level Minimum log level to display
void SetLogLevel(LogLevel level) noexcept;

/// Get current log level
/// @return Current log level
LogLevel GetLogLevel() noexcept;

// ============================================================================
// Internal Logging Functions (Used by VRHI internally)
// ============================================================================

namespace Internal {

/// Log a message (used internally by VRHI)
/// This will use the custom logger/function or fall back to default
/// @param level The log level
/// @param message The log message
void Log(LogLevel level, std::string_view message);

/// Log a formatted message (printf-style, used internally by VRHI)
/// This will use the custom logger/function or fall back to default
/// @param level The log level
/// @param format Printf-style format string
/// @param ... Variable arguments
void LogFormatted(LogLevel level, const char* format, ...) 
#ifdef __GNUC__
    __attribute__((format(printf, 2, 3)))
#endif
;

} // namespace Internal

// ============================================================================
// Convenience Logging Functions
// ============================================================================

/// Log a debug message
inline void LogDebug(std::string_view message) {
    Internal::Log(LogLevel::Debug, message);
}

/// Log a debug message with printf-style formatting (requires at least one argument)
template<typename... Args>
    requires (sizeof...(Args) > 0)
inline void LogDebug(const char* format, Args&&... args) {
    Internal::LogFormatted(LogLevel::Debug, format, std::forward<Args>(args)...);
}

/// Log an info message
inline void LogInfo(std::string_view message) {
    Internal::Log(LogLevel::Info, message);
}

/// Log an info message with printf-style formatting (requires at least one argument)
template<typename... Args>
    requires (sizeof...(Args) > 0)
inline void LogInfo(const char* format, Args&&... args) {
    Internal::LogFormatted(LogLevel::Info, format, std::forward<Args>(args)...);
}

/// Log a warning message
inline void LogWarning(std::string_view message) {
    Internal::Log(LogLevel::Warning, message);
}

/// Log a warning message with printf-style formatting (requires at least one argument)
template<typename... Args>
    requires (sizeof...(Args) > 0)
inline void LogWarning(const char* format, Args&&... args) {
    Internal::LogFormatted(LogLevel::Warning, format, std::forward<Args>(args)...);
}

/// Log an error message
inline void LogError(std::string_view message) {
    Internal::Log(LogLevel::Error, message);
}

/// Log an error message with printf-style formatting (requires at least one argument)
template<typename... Args>
    requires (sizeof...(Args) > 0)
inline void LogError(const char* format, Args&&... args) {
    Internal::LogFormatted(LogLevel::Error, format, std::forward<Args>(args)...);
}

} // namespace VRHI

