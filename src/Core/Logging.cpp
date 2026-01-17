// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#include <VRHI/Logging.hpp>
#include <VRHI/VRHI.hpp>

#include <cstdio>
#include <cstdarg>
#include <chrono>
#include <iomanip>
#include <sstream>
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
        // Get current time using chrono (thread-safe)
        auto now = std::chrono::system_clock::now();
        auto time_t_now = std::chrono::system_clock::to_time_t(now);
        
        // Format timestamp thread-safely
        std::ostringstream timeStream;
        {
            // Use a local mutex for time formatting to ensure thread safety
            static std::mutex timeMutex;
            std::lock_guard<std::mutex> timeLock(timeMutex);
            std::tm tm_buf;
#ifdef _WIN32
            localtime_s(&tm_buf, &time_t_now);
#else
            localtime_r(&time_t_now, &tm_buf);
#endif
            timeStream << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
        }
        
        // Determine output stream (stderr for warnings and errors, stdout for others)
        FILE* stream = (level >= LogLevel::Warning) ? stderr : stdout;
        
        // Print formatted message
        std::fprintf(stream, "[%s] [VRHI] [%s] %.*s\n", 
                    timeStream.str().c_str(),
                    LogLevelToString(level),
                    static_cast<int>(message.size()),
                    message.data());
        
        std::fflush(stream);
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

// Approach 1: User-provided logger instance (takes priority)
ILogger* g_userLogger = nullptr;

// Approach 2: User-provided function pointers (used if g_userLogger is nullptr)
LogFunction g_userLogFunction = nullptr;
LogFormattedFunction g_userLogFormattedFunction = nullptr;

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

void SetLogFunction(LogFunction func) noexcept {
    std::lock_guard<std::mutex> lock(g_logMutex);
    g_userLogFunction = func;
}

void SetLogFormattedFunction(LogFormattedFunction func) noexcept {
    std::lock_guard<std::mutex> lock(g_logMutex);
    g_userLogFormattedFunction = func;
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
    
    // Priority order:
    // 1. User-provided logger instance (ILogger)
    // 2. User-provided function pointer
    // 3. Default logger
    
    if (g_userLogger) {
        g_userLogger->Log(level, message);
    } else if (g_userLogFunction) {
        g_userLogFunction(level, message);
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
    
    // Priority order:
    // 1. User-provided logger instance (ILogger)
    // 2. User-provided formatted function pointer
    // 3. Default logger
    
    if (g_userLogger) {
        g_userLogger->LogFormatted(level, format, args);
    } else if (g_userLogFormattedFunction) {
        g_userLogFormattedFunction(level, format, args);
    } else {
        g_defaultLogger.LogFormatted(level, format, args);
    }
    
    va_end(args);
}

} // namespace Internal

} // namespace VRHI

