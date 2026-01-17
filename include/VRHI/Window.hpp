// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#pragma once

#include <cstdint>
#include <expected>
#include <functional>
#include <memory>
#include <string>
#include <string_view>

namespace VRHI {

// Forward declarations
struct Error;

// ============================================================================
// Window System Enumerations
// ============================================================================

/// Window system backend type
enum class WindowSystemType {
    Auto,      // Automatically select best backend for platform
    GLFW,      // GLFW 3.x - Lightweight, game-focused
    SDL2,      // SDL 2.x - Mature, stable
    SDL3,      // SDL 3.x - Modern SDL API
    EGL,       // EGL - Headless/embedded
};

/// Platform-independent key codes
enum class KeyCode {
    Unknown = -1,
    
    // Printable keys
    Space = 32,
    Apostrophe = 39,
    Comma = 44,
    Minus = 45,
    Period = 46,
    Slash = 47,
    
    Key0 = 48,
    Key1 = 49,
    Key2 = 50,
    Key3 = 51,
    Key4 = 52,
    Key5 = 53,
    Key6 = 54,
    Key7 = 55,
    Key8 = 56,
    Key9 = 57,
    
    Semicolon = 59,
    Equal = 61,
    
    A = 65,
    B = 66,
    C = 67,
    D = 68,
    E = 69,
    F = 70,
    G = 71,
    H = 72,
    I = 73,
    J = 74,
    K = 75,
    L = 76,
    M = 77,
    N = 78,
    O = 79,
    P = 80,
    Q = 81,
    R = 82,
    S = 83,
    T = 84,
    U = 85,
    V = 86,
    W = 87,
    X = 88,
    Y = 89,
    Z = 90,
    
    LeftBracket = 91,
    Backslash = 92,
    RightBracket = 93,
    GraveAccent = 96,
    
    // Function keys
    Escape = 256,
    Enter = 257,
    Tab = 258,
    Backspace = 259,
    Insert = 260,
    Delete = 261,
    Right = 262,
    Left = 263,
    Down = 264,
    Up = 265,
    PageUp = 266,
    PageDown = 267,
    Home = 268,
    End = 269,
    
    CapsLock = 280,
    ScrollLock = 281,
    NumLock = 282,
    PrintScreen = 283,
    Pause = 284,
    
    F1 = 290,
    F2 = 291,
    F3 = 292,
    F4 = 293,
    F5 = 294,
    F6 = 295,
    F7 = 296,
    F8 = 297,
    F9 = 298,
    F10 = 299,
    F11 = 300,
    F12 = 301,
    
    // Keypad
    KP0 = 320,
    KP1 = 321,
    KP2 = 322,
    KP3 = 323,
    KP4 = 324,
    KP5 = 325,
    KP6 = 326,
    KP7 = 327,
    KP8 = 328,
    KP9 = 329,
    KPDecimal = 330,
    KPDivide = 331,
    KPMultiply = 332,
    KPSubtract = 333,
    KPAdd = 334,
    KPEnter = 335,
    KPEqual = 336,
    
    // Modifiers
    LeftShift = 340,
    LeftControl = 341,
    LeftAlt = 342,
    LeftSuper = 343,
    RightShift = 344,
    RightControl = 345,
    RightAlt = 346,
    RightSuper = 347,
    Menu = 348,
};

/// Key/button action
enum class KeyAction {
    Release = 0,
    Press = 1,
    Repeat = 2,
};

/// Mouse button codes
enum class MouseButton {
    Left = 0,
    Right = 1,
    Middle = 2,
    Button4 = 3,
    Button5 = 4,
    Button6 = 5,
    Button7 = 6,
    Button8 = 7,
};

/// Key modifier flags
enum class KeyMods {
    None = 0,
    Shift = 0x0001,
    Control = 0x0002,
    Alt = 0x0004,
    Super = 0x0008,
    CapsLock = 0x0010,
    NumLock = 0x0020,
};

// ============================================================================
// Window Configuration
// ============================================================================

/// Window creation configuration
struct WindowConfig {
    // Window properties
    std::string title = "VRHI Window";
    uint32_t width = 800;
    uint32_t height = 600;
    bool resizable = true;
    bool visible = true;
    bool decorated = true;
    bool focused = true;
    
    // Graphics context hints (for OpenGL)
    struct {
        int majorVersion = 3;
        int minorVersion = 3;
        bool coreProfile = true;
        bool forwardCompatible = false;
        int samples = 0;  // MSAA samples (0 = disabled)
    } opengl;
    
    // Platform-specific settings
    void* nativeWindowHandle = nullptr;  // Use existing native window (optional)
};

// ============================================================================
// Window Interface
// ============================================================================

/// Abstract window interface
class IWindow {
public:
    virtual ~IWindow() = default;
    
    // Window properties
    virtual uint32_t GetWidth() const noexcept = 0;
    virtual uint32_t GetHeight() const noexcept = 0;
    virtual bool ShouldClose() const noexcept = 0;
    virtual WindowSystemType GetType() const noexcept = 0;
    
    // Window operations
    virtual void SetTitle(std::string_view title) = 0;
    virtual void SetSize(uint32_t width, uint32_t height) = 0;
    virtual void Show() = 0;
    virtual void Hide() = 0;
    virtual void Close() = 0;
    
    // Event handling
    virtual void PollEvents() = 0;
    virtual void WaitEvents() = 0;
    
    // Native handle access (for backend integration)
    virtual void* GetNativeHandle() const noexcept = 0;
    
    // Callbacks
    using KeyCallback = std::function<void(KeyCode key, int scancode, KeyAction action, KeyMods mods)>;
    using MouseButtonCallback = std::function<void(MouseButton button, KeyAction action, KeyMods mods)>;
    using CursorPosCallback = std::function<void(double xpos, double ypos)>;
    using ScrollCallback = std::function<void(double xoffset, double yoffset)>;
    using ResizeCallback = std::function<void(uint32_t width, uint32_t height)>;
    using CloseCallback = std::function<void()>;
    
    virtual void SetKeyCallback(KeyCallback callback) = 0;
    virtual void SetMouseButtonCallback(MouseButtonCallback callback) = 0;
    virtual void SetCursorPosCallback(CursorPosCallback callback) = 0;
    virtual void SetScrollCallback(ScrollCallback callback) = 0;
    virtual void SetResizeCallback(ResizeCallback callback) = 0;
    virtual void SetCloseCallback(CloseCallback callback) = 0;
};

// ============================================================================
// Window Factory
// ============================================================================

/// Create a window with the specified configuration
std::expected<std::unique_ptr<IWindow>, Error>
CreateWindow(const WindowConfig& config, WindowSystemType type = WindowSystemType::Auto);

} // namespace VRHI
