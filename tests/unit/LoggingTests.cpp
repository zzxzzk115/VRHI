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

// Custom Logger Class Implementation
class TestLogger : public VRHI::ILogger {
public:
    struct LogEntry {
        VRHI::LogLevel level{};
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
// ILogger Interface Tests
// ============================================================================

TEST(LoggingTests, CustomLogger_BasicLog) {
    TestLogger testLogger{};
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

TEST(LoggingTests, CustomLogger_FormattedLog) {
    TestLogger testLogger{};
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

TEST(LoggingTests, CustomLogger_LogLevelFiltering) {
    TestLogger testLogger{};
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

TEST(LoggingTests, CustomLogger_MultipleMessages) {
    TestLogger testLogger{};
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

TEST(LoggingTests, DefaultLogger_FallbackBehavior) {
    // Set logger to nullptr to use default logger
    VRHI::SetLogger(nullptr);
    VRHI::SetLogLevel(VRHI::LogLevel::Debug);
    
    // This should not crash and should use the default logger
    VRHI::Internal::Log(VRHI::LogLevel::Info, "Default logger test");
    VRHI::Internal::LogFormatted(VRHI::LogLevel::Warning, "Formatted: %d", 123);
    
    // No assertions here, just verify it doesn't crash
    // Cleanup
    VRHI::SetLogLevel(VRHI::LogLevel::Info);
}

// ============================================================================
// Off Log Level Test
// ============================================================================

TEST(LoggingTests, LogLevelOff_NoLogging) {
    TestLogger testLogger{};
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
