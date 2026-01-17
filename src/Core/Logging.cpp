// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#include <VRHI/Logging.hpp>
#include <VRHI/VRHI.hpp>

#include <cstdio>
#include <cstdarg>
#include <iostream>
#include <mutex>
#include <memory>

namespace VRHI {

// ============================================================================
// Default Logger Implementation (Fallback)
// ============================================================================

namespace {

class DefaultLogger : public ILogger {
public:
    void Log(LogLevel level, std::string_view message) override {
        // Use simple output - stderr for warnings and errors, stdout for others
        if (level >= LogLevel::Warning) {
            std::cerr << "[VRHI] [" << LogLevelToString(level) << "] " << message << '\n';
        } else {
            std::cout << "[VRHI] [" << LogLevelToString(level) << "] " << message << '\n';
        }
    }
    
    void LogFormatted(LogLevel level, const char* format, va_list args) override {
        // Format the message first
        char buffer[4096];
        int written = vsnprintf(buffer, sizeof(buffer), format, args);
        
        if (written < 0) {
            // Formatting error
            Log(level, "[Formatting error]");
            return;
        }
        
        if (written >= static_cast<int>(sizeof(buffer))) {
            // Message was truncated
            std::string truncated(buffer);
            truncated += "...[truncated]";
            Log(level, truncated);
        } else {
            // Normal case
            Log(level, std::string_view(buffer, written));
        }
    }

private:
    static const char* LogLevelToString(LogLevel level) {
        switch (level) {
            case LogLevel::Debug:   return "DEBUG";
            case LogLevel::Info:    return "INFO";
            case LogLevel::Warning: return "WARNING";
            case LogLevel::Error:   return "ERROR";
            case LogLevel::Off:     return "OFF";
            default:                return "UNKNOWN";
        }
    }
};

} // anonymous namespace

// ============================================================================
// Global State
// ============================================================================

namespace {

// Current log level
LogLevel g_logLevel = LogLevel::Info;

// User-provided logger instance (or nullptr to use default)
ILogger* g_userLogger = nullptr;

// Default logger instance (fallback)
DefaultLogger g_defaultLogger;

// Mutex for thread-safe logging
std::mutex g_logMutex;

} // anonymous namespace

// ============================================================================
// Public API Implementation
// ============================================================================

void SetLogger(ILogger* logger) noexcept {
    std::lock_guard<std::mutex> lock(g_logMutex);
    g_userLogger = logger;
}

void SetLogLevel(LogLevel level) noexcept {
    std::lock_guard<std::mutex> lock(g_logMutex);
    g_logLevel = level;
}

LogLevel GetLogLevel() noexcept {
    std::lock_guard<std::mutex> lock(g_logMutex);
    return g_logLevel;
}

// ============================================================================
// Internal Logging Implementation
// ============================================================================

namespace Internal {

void Log(LogLevel level, std::string_view message) {
    std::lock_guard<std::mutex> lock(g_logMutex);
    
    // Check log level filter
    if (level < g_logLevel || g_logLevel == LogLevel::Off) {
        return;
    }
    
    // Use custom logger if provided, otherwise use default logger
    if (g_userLogger) {
        g_userLogger->Log(level, message);
    } else {
        g_defaultLogger.Log(level, message);
    }
}

void LogFormatted(LogLevel level, const char* format, ...) {
    std::lock_guard<std::mutex> lock(g_logMutex);
    
    // Check log level filter
    if (level < g_logLevel || g_logLevel == LogLevel::Off) {
        return;
    }
    
    va_list args;
    va_start(args, format);
    
    // Use custom logger if provided, otherwise use default logger
    if (g_userLogger) {
        g_userLogger->LogFormatted(level, format, args);
    } else {
        g_defaultLogger.LogFormatted(level, format, args);
    }
    
    va_end(args);
}

} // namespace Internal

} // namespace VRHI

