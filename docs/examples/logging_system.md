# VRHI Logging System

VRHI provides a flexible logging system that supports two approaches for custom logging:

1. **Interface Class Approach** (OOP style, more flexible)
2. **Function Pointer Approach** (C-style, simpler, better for C interop)

Both approaches allow external applications to redirect VRHI's internal logging to their own logging systems. A simple default logger is provided as a fallback when no custom logger is set.

## Table of Contents

- [Quick Start](#quick-start)
- [Approach 1: Interface Class](#approach-1-interface-class)
- [Approach 2: Function Pointers](#approach-2-function-pointers)
- [Priority System](#priority-system)
- [Default Fallback Logger](#default-fallback-logger)
- [Log Level Filtering](#log-level-filtering)
- [Thread Safety](#thread-safety)

## Quick Start

### Using Default Logger

```cpp
#include <VRHI/VRHI.hpp>
#include <VRHI/Logging.hpp>

int main() {
    // Set log level (default is Info)
    VRHI::SetLogLevel(VRHI::LogLevel::Debug);
    
    // VRHI will use the default logger automatically
    auto device = VRHI::CreateDevice();
    
    // Internal VRHI operations will log to stdout/stderr
}
```

## Approach 1: Interface Class

The interface class approach is recommended for C++ applications as it provides better encapsulation and flexibility.

### Step 1: Implement ILogger Interface

```cpp
#include <VRHI/Logging.hpp>
#include <iostream>
#include <cstdarg>

class MyCustomLogger : public VRHI::ILogger {
public:
    void Log(VRHI::LogLevel level, std::string_view message) override {
        // Redirect to your logging system
        std::cout << "[MyApp] [" << LevelToString(level) << "] " 
                  << message << std::endl;
    }
    
    void LogFormatted(VRHI::LogLevel level, const char* format, va_list args) override {
        char buffer[4096];
        vsnprintf(buffer, sizeof(buffer), format, args);
        Log(level, buffer);
    }

private:
    const char* LevelToString(VRHI::LogLevel level) {
        switch (level) {
            case VRHI::LogLevel::Debug:   return "DEBUG";
            case VRHI::LogLevel::Info:    return "INFO";
            case VRHI::LogLevel::Warning: return "WARN";
            case VRHI::LogLevel::Error:   return "ERROR";
            default:                       return "UNKNOWN";
        }
    }
};
```

### Step 2: Set Your Custom Logger

```cpp
int main() {
    MyCustomLogger logger;
    
    // Set custom logger
    VRHI::SetLogger(&logger);
    VRHI::SetLogLevel(VRHI::LogLevel::Debug);
    
    // All VRHI logs will now go through your logger
    auto device = VRHI::CreateDevice();
    
    // Restore default logger when done
    VRHI::SetLogger(nullptr);
}
```

### Integration with Popular Logging Libraries

#### spdlog Example

```cpp
#include <spdlog/spdlog.h>

class SpdlogLogger : public VRHI::ILogger {
public:
    void Log(VRHI::LogLevel level, std::string_view message) override {
        switch (level) {
            case VRHI::LogLevel::Debug:
                spdlog::debug("{}", message);
                break;
            case VRHI::LogLevel::Info:
                spdlog::info("{}", message);
                break;
            case VRHI::LogLevel::Warning:
                spdlog::warn("{}", message);
                break;
            case VRHI::LogLevel::Error:
                spdlog::error("{}", message);
                break;
            default:
                break;
        }
    }
    
    void LogFormatted(VRHI::LogLevel level, const char* format, va_list args) override {
        char buffer[4096];
        vsnprintf(buffer, sizeof(buffer), format, args);
        Log(level, buffer);
    }
};
```

## Approach 2: Function Pointers

The function pointer approach is simpler and better suited for C interoperability or when you need a lightweight solution.

### Step 1: Define Logging Functions

```cpp
#include <VRHI/Logging.hpp>
#include <cstdio>

void MyLogFunction(VRHI::LogLevel level, std::string_view message) {
    const char* levelStr = "INFO";
    switch (level) {
        case VRHI::LogLevel::Debug:   levelStr = "DEBUG"; break;
        case VRHI::LogLevel::Warning: levelStr = "WARN"; break;
        case VRHI::LogLevel::Error:   levelStr = "ERROR"; break;
        default: break;
    }
    
    printf("[MyApp] [%s] %.*s\n", levelStr, 
           (int)message.size(), message.data());
}

void MyLogFormattedFunction(VRHI::LogLevel level, const char* format, va_list args) {
    printf("[MyApp] ");
    vprintf(format, args);
    printf("\n");
}
```

### Step 2: Set Function Pointers

```cpp
int main() {
    // Set custom logging functions
    VRHI::SetLogFunction(MyLogFunction);
    VRHI::SetLogFormattedFunction(MyLogFormattedFunction);
    VRHI::SetLogLevel(VRHI::LogLevel::Debug);
    
    // All VRHI logs will now go through your functions
    auto device = VRHI::CreateDevice();
    
    // Restore default functions when done
    VRHI::SetLogFunction(nullptr);
    VRHI::SetLogFormattedFunction(nullptr);
}
```

### C Interoperability Example

```c
// In C code
#include <VRHI/Logging.h>  // C-compatible header

void my_c_log_function(VRHILogLevel level, const char* message, size_t length) {
    printf("[C Logger] %.*s\n", (int)length, message);
}

void setup_vrhi_logging() {
    vrhi_set_log_function(my_c_log_function);
    vrhi_set_log_level(VRHI_LOG_LEVEL_DEBUG);
}
```

## Priority System

When multiple logging methods are configured, VRHI uses the following priority order:

1. **ILogger interface** (highest priority)
2. **Function pointers**
3. **Default logger** (fallback)

```cpp
MyCustomLogger logger;
VRHI::SetLogger(&logger);           // Priority 1
VRHI::SetLogFunction(MyLogFunc);    // Priority 2 (won't be used)

// All logs will go through 'logger' (ILogger interface)

VRHI::SetLogger(nullptr);           // Disable interface logger
// Now logs will go through MyLogFunc (function pointer)

VRHI::SetLogFunction(nullptr);      // Disable function pointer
// Now logs will go through default logger
```

## Default Fallback Logger

VRHI includes a simple default logger that:

- Prints to **stdout** for Debug/Info messages
- Prints to **stderr** for Warning/Error messages
- Includes timestamps in the format: `[YYYY-MM-DD HH:MM:SS]`
- Includes log level: `[DEBUG]`, `[INFO]`, `[WARNING]`, `[ERROR]`
- Thread-safe

Example output:
```
[2024-01-17 15:30:42] [VRHI] [INFO] Initializing Vulkan backend
[2024-01-17 15:30:42] [VRHI] [DEBUG] Found 2 physical devices
[2024-01-17 15:30:42] [VRHI] [WARNING] Feature 'RayTracing' not supported
```

## Log Level Filtering

VRHI supports five log levels with filtering:

```cpp
enum class LogLevel {
    Debug,      // Detailed debug information
    Info,       // Informational messages (default)
    Warning,    // Warning messages
    Error,      // Error messages
    Off,        // Disable all logging
};
```

### Setting Log Level

```cpp
// Show all messages including debug
VRHI::SetLogLevel(VRHI::LogLevel::Debug);

// Show only warnings and errors
VRHI::SetLogLevel(VRHI::LogLevel::Warning);

// Disable all logging
VRHI::SetLogLevel(VRHI::LogLevel::Off);

// Get current log level
VRHI::LogLevel current = VRHI::GetLogLevel();
```

Messages below the configured level are filtered out before reaching your logger.

## Thread Safety

All logging functions are **thread-safe**:

- Internal mutex protects concurrent access
- Safe to call from multiple threads
- Safe to change loggers at runtime

```cpp
// Thread 1
VRHI::Internal::Log(VRHI::LogLevel::Info, "Message from thread 1");

// Thread 2
VRHI::Internal::LogFormatted(VRHI::LogLevel::Debug, "Message from thread %d", 2);

// Thread 3
VRHI::SetLogLevel(VRHI::LogLevel::Warning);  // Safe to change at runtime
```

## Best Practices

1. **Set logger early**: Configure your logger before calling any VRHI functions
2. **Don't delete logger while in use**: Keep the ILogger instance alive while VRHI might use it
3. **Use appropriate log levels**: Debug for development, Info for production
4. **Handle formatted logs**: Implement both `Log()` and `LogFormatted()` in your ILogger
5. **Avoid logging in logger**: Don't call VRHI logging functions from within your logger to avoid recursion

## Examples

See the following examples for complete integration:

- `examples/01_logging_interface/` - Using ILogger interface
- `examples/02_logging_functions/` - Using function pointers
- `examples/03_logging_spdlog/` - Integration with spdlog
- `examples/04_logging_c_api/` - C API usage

## API Reference

### Functions

```cpp
// Set custom logger (interface approach)
void SetLogger(ILogger* logger) noexcept;

// Set custom log functions (function pointer approach)
void SetLogFunction(LogFunction func) noexcept;
void SetLogFormattedFunction(LogFormattedFunction func) noexcept;

// Configure log level
void SetLogLevel(LogLevel level) noexcept;
LogLevel GetLogLevel() noexcept;

// Internal logging (used by VRHI, can also be used by applications)
namespace Internal {
    void Log(LogLevel level, std::string_view message);
    void LogFormatted(LogLevel level, const char* format, ...);
}
```

### Interface

```cpp
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void Log(LogLevel level, std::string_view message) = 0;
    virtual void LogFormatted(LogLevel level, const char* format, va_list args) = 0;
};
```

---

For more information, see the [API documentation](../api/core.md).
