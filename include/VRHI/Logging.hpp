// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#pragma once

#include <cstdarg>
#include <string_view>

namespace VRHI {

// Forward declare LogLevel from main header
enum class LogLevel;

// ============================================================================
// Logging Interface - Two Approaches
// ============================================================================

// Approach 1: Interface Class (More flexible, OOP style)
// Users can inherit from this class to implement custom logging

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

// Approach 2: Function Pointers (C-style, simpler for C interop)
// Users can provide function pointers for logging

/// Basic logging function pointer (without formatting)
/// @param level The log level
/// @param message The log message
using LogFunction = void(*)(LogLevel level, std::string_view message);

/// Formatted logging function pointer (printf-style)
/// @param level The log level
/// @param format Printf-style format string
/// @param args Variable arguments
using LogFormattedFunction = void(*)(LogLevel level, const char* format, va_list args);

// ============================================================================
// Global Logging Configuration
// ============================================================================

/// Set custom logger instance (Approach 1: Interface class)
/// @param logger Custom logger instance, or nullptr to use default
/// @note This takes ownership of the logger pointer
void SetLogger(ILogger* logger) noexcept;

/// Set custom log function (Approach 2: Function pointer, without formatting)
/// @param func Custom log function, or nullptr to use default
void SetLogFunction(LogFunction func) noexcept;

/// Set custom formatted log function (Approach 2: Function pointer, printf-style)
/// @param func Custom formatted log function, or nullptr to use default
void SetLogFormattedFunction(LogFormattedFunction func) noexcept;

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

} // namespace VRHI

