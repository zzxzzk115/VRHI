// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#include <VRHI/Logging.hpp>
#include <VRHI/VRHI.hpp>
#include <gtest/gtest.h>
#include <cstdarg>
#include <string>
#include <vector>

// ============================================================================
// Test Fixtures and Helpers
// ============================================================================

// Approach 1: Custom Logger Class Implementation
class TestLogger : public VRHI::ILogger {
public:
    struct LogEntry {
        VRHI::LogLevel level;
        std::string message;
    };
    
    std::vector<LogEntry> entries;
    
    void Log(VRHI::LogLevel level, std::string_view message) override {
        entries.push_back({level, std::string(message)});
    }
    
    void LogFormatted(VRHI::LogLevel level, const char* format, va_list args) override {
        char buffer[4096];
        int written = vsnprintf(buffer, sizeof(buffer), format, args);
        // Handle truncation - vsnprintf returns the number of chars that would have been written
        if (written < 0) {
            entries.push_back({level, "[Formatting error]"});
        } else if (written >= static_cast<int>(sizeof(buffer))) {
            entries.push_back({level, std::string(buffer) + "...[truncated]"});
        } else {
            entries.push_back({level, std::string(buffer)});
        }
    }
    
    void Clear() {
        entries.clear();
    }
};

// Approach 2: Function Pointer Implementation
namespace {
    std::vector<std::pair<VRHI::LogLevel, std::string>> g_functionPointerLogs;
    
    void TestLogFunction(VRHI::LogLevel level, std::string_view message) {
        g_functionPointerLogs.push_back({level, std::string(message)});
    }
    
    void TestLogFormattedFunction(VRHI::LogLevel level, const char* format, va_list args) {
        char buffer[4096];
        int written = vsnprintf(buffer, sizeof(buffer), format, args);
        if (written >= 0 && written < static_cast<int>(sizeof(buffer))) {
            g_functionPointerLogs.push_back({level, std::string(buffer)});
        } else {
            g_functionPointerLogs.push_back({level, "[Formatting error or truncated]"});
        }
    }
}

// ============================================================================
// Log Level Tests
// ============================================================================

TEST(LoggingTests, DefaultLogLevel) {
    // Default should be Info
    VRHI::SetLogLevel(VRHI::LogLevel::Info);
    EXPECT_EQ(VRHI::GetLogLevel(), VRHI::LogLevel::Info);
}

TEST(LoggingTests, SetAndGetLogLevel) {
    VRHI::SetLogLevel(VRHI::LogLevel::Debug);
    EXPECT_EQ(VRHI::GetLogLevel(), VRHI::LogLevel::Debug);
    
    VRHI::SetLogLevel(VRHI::LogLevel::Warning);
    EXPECT_EQ(VRHI::GetLogLevel(), VRHI::LogLevel::Warning);
    
    VRHI::SetLogLevel(VRHI::LogLevel::Error);
    EXPECT_EQ(VRHI::GetLogLevel(), VRHI::LogLevel::Error);
    
    VRHI::SetLogLevel(VRHI::LogLevel::Off);
    EXPECT_EQ(VRHI::GetLogLevel(), VRHI::LogLevel::Off);
    
    // Reset to default
    VRHI::SetLogLevel(VRHI::LogLevel::Info);
}

// ============================================================================
// Approach 1: Interface Class Logger Tests
// ============================================================================

TEST(LoggingTests, CustomLoggerInterface_BasicLog) {
    TestLogger testLogger;
    VRHI::SetLogger(&testLogger);
    VRHI::SetLogLevel(VRHI::LogLevel::Debug);
    
    // Test basic logging
    VRHI::Internal::Log(VRHI::LogLevel::Info, "Test message");
    
    ASSERT_EQ(testLogger.entries.size(), 1);
    EXPECT_EQ(testLogger.entries[0].level, VRHI::LogLevel::Info);
    EXPECT_EQ(testLogger.entries[0].message, "Test message");
    
    // Cleanup
    VRHI::SetLogger(nullptr);
    VRHI::SetLogLevel(VRHI::LogLevel::Info);
}

TEST(LoggingTests, CustomLoggerInterface_FormattedLog) {
    TestLogger testLogger;
    VRHI::SetLogger(&testLogger);
    VRHI::SetLogLevel(VRHI::LogLevel::Debug);
    
    // Test formatted logging
    VRHI::Internal::LogFormatted(VRHI::LogLevel::Warning, "Value: %d, String: %s", 42, "test");
    
    ASSERT_EQ(testLogger.entries.size(), 1);
    EXPECT_EQ(testLogger.entries[0].level, VRHI::LogLevel::Warning);
    EXPECT_EQ(testLogger.entries[0].message, "Value: 42, String: test");
    
    // Cleanup
    VRHI::SetLogger(nullptr);
    VRHI::SetLogLevel(VRHI::LogLevel::Info);
}

TEST(LoggingTests, CustomLoggerInterface_LogLevelFiltering) {
    TestLogger testLogger;
    VRHI::SetLogger(&testLogger);
    VRHI::SetLogLevel(VRHI::LogLevel::Warning);
    
    // These should be filtered out
    VRHI::Internal::Log(VRHI::LogLevel::Debug, "Debug message");
    VRHI::Internal::Log(VRHI::LogLevel::Info, "Info message");
    
    // These should pass through
    VRHI::Internal::Log(VRHI::LogLevel::Warning, "Warning message");
    VRHI::Internal::Log(VRHI::LogLevel::Error, "Error message");
    
    ASSERT_EQ(testLogger.entries.size(), 2);
    EXPECT_EQ(testLogger.entries[0].message, "Warning message");
    EXPECT_EQ(testLogger.entries[1].message, "Error message");
    
    // Cleanup
    VRHI::SetLogger(nullptr);
    VRHI::SetLogLevel(VRHI::LogLevel::Info);
}

TEST(LoggingTests, CustomLoggerInterface_MultipleMessages) {
    TestLogger testLogger;
    VRHI::SetLogger(&testLogger);
    VRHI::SetLogLevel(VRHI::LogLevel::Debug);
    
    VRHI::Internal::Log(VRHI::LogLevel::Debug, "Message 1");
    VRHI::Internal::Log(VRHI::LogLevel::Info, "Message 2");
    VRHI::Internal::LogFormatted(VRHI::LogLevel::Warning, "Message %d", 3);
    
    ASSERT_EQ(testLogger.entries.size(), 3);
    EXPECT_EQ(testLogger.entries[0].message, "Message 1");
    EXPECT_EQ(testLogger.entries[1].message, "Message 2");
    EXPECT_EQ(testLogger.entries[2].message, "Message 3");
    
    // Cleanup
    VRHI::SetLogger(nullptr);
    VRHI::SetLogLevel(VRHI::LogLevel::Info);
}

// ============================================================================
// Approach 2: Function Pointer Logger Tests
// ============================================================================

TEST(LoggingTests, FunctionPointer_BasicLog) {
    g_functionPointerLogs.clear();
    VRHI::SetLogFunction(TestLogFunction);
    VRHI::SetLogLevel(VRHI::LogLevel::Debug);
    
    VRHI::Internal::Log(VRHI::LogLevel::Info, "Function pointer test");
    
    ASSERT_EQ(g_functionPointerLogs.size(), 1);
    EXPECT_EQ(g_functionPointerLogs[0].first, VRHI::LogLevel::Info);
    EXPECT_EQ(g_functionPointerLogs[0].second, "Function pointer test");
    
    // Cleanup
    VRHI::SetLogFunction(nullptr);
    VRHI::SetLogLevel(VRHI::LogLevel::Info);
    g_functionPointerLogs.clear();
}

TEST(LoggingTests, FunctionPointer_FormattedLog) {
    g_functionPointerLogs.clear();
    VRHI::SetLogFormattedFunction(TestLogFormattedFunction);
    VRHI::SetLogLevel(VRHI::LogLevel::Debug);
    
    VRHI::Internal::LogFormatted(VRHI::LogLevel::Error, "Error code: %d", 404);
    
    ASSERT_EQ(g_functionPointerLogs.size(), 1);
    EXPECT_EQ(g_functionPointerLogs[0].first, VRHI::LogLevel::Error);
    EXPECT_EQ(g_functionPointerLogs[0].second, "Error code: 404");
    
    // Cleanup
    VRHI::SetLogFormattedFunction(nullptr);
    VRHI::SetLogLevel(VRHI::LogLevel::Info);
    g_functionPointerLogs.clear();
}

TEST(LoggingTests, FunctionPointer_BothFunctions) {
    g_functionPointerLogs.clear();
    VRHI::SetLogFunction(TestLogFunction);
    VRHI::SetLogFormattedFunction(TestLogFormattedFunction);
    VRHI::SetLogLevel(VRHI::LogLevel::Debug);
    
    VRHI::Internal::Log(VRHI::LogLevel::Info, "Simple message");
    VRHI::Internal::LogFormatted(VRHI::LogLevel::Warning, "Formatted: %s", "message");
    
    ASSERT_EQ(g_functionPointerLogs.size(), 2);
    EXPECT_EQ(g_functionPointerLogs[0].second, "Simple message");
    EXPECT_EQ(g_functionPointerLogs[1].second, "Formatted: message");
    
    // Cleanup
    VRHI::SetLogFunction(nullptr);
    VRHI::SetLogFormattedFunction(nullptr);
    VRHI::SetLogLevel(VRHI::LogLevel::Info);
    g_functionPointerLogs.clear();
}

// ============================================================================
// Priority Tests (ILogger takes priority over function pointers)
// ============================================================================

TEST(LoggingTests, LoggerPriority_InterfaceTakesPrecedence) {
    TestLogger testLogger;
    g_functionPointerLogs.clear();
    
    // Set both interface and function pointers
    VRHI::SetLogger(&testLogger);
    VRHI::SetLogFunction(TestLogFunction);
    VRHI::SetLogLevel(VRHI::LogLevel::Debug);
    
    VRHI::Internal::Log(VRHI::LogLevel::Info, "Priority test");
    
    // Interface should take priority
    ASSERT_EQ(testLogger.entries.size(), 1);
    EXPECT_EQ(testLogger.entries[0].message, "Priority test");
    
    // Function pointer should not be called
    EXPECT_EQ(g_functionPointerLogs.size(), 0);
    
    // Cleanup
    VRHI::SetLogger(nullptr);
    VRHI::SetLogFunction(nullptr);
    VRHI::SetLogLevel(VRHI::LogLevel::Info);
    g_functionPointerLogs.clear();
}

TEST(LoggingTests, LoggerPriority_FallbackToFunctionPointer) {
    TestLogger testLogger;
    g_functionPointerLogs.clear();
    
    // Only set function pointer
    VRHI::SetLogFunction(TestLogFunction);
    VRHI::SetLogLevel(VRHI::LogLevel::Debug);
    
    VRHI::Internal::Log(VRHI::LogLevel::Info, "Fallback test");
    
    // Function pointer should be called
    ASSERT_EQ(g_functionPointerLogs.size(), 1);
    EXPECT_EQ(g_functionPointerLogs[0].second, "Fallback test");
    
    // Cleanup
    VRHI::SetLogFunction(nullptr);
    VRHI::SetLogLevel(VRHI::LogLevel::Info);
    g_functionPointerLogs.clear();
}

// ============================================================================
// Off Log Level Test
// ============================================================================

TEST(LoggingTests, LogLevelOff_NoLogging) {
    TestLogger testLogger;
    VRHI::SetLogger(&testLogger);
    VRHI::SetLogLevel(VRHI::LogLevel::Off);
    
    // All messages should be filtered
    VRHI::Internal::Log(VRHI::LogLevel::Debug, "Debug");
    VRHI::Internal::Log(VRHI::LogLevel::Info, "Info");
    VRHI::Internal::Log(VRHI::LogLevel::Warning, "Warning");
    VRHI::Internal::Log(VRHI::LogLevel::Error, "Error");
    
    EXPECT_EQ(testLogger.entries.size(), 0);
    
    // Cleanup
    VRHI::SetLogger(nullptr);
    VRHI::SetLogLevel(VRHI::LogLevel::Info);
}

// ============================================================================
// Main
// ============================================================================

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
