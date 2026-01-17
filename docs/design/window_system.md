# Window System Abstraction Design

## Overview

VRHI provides a unified window system abstraction layer supporting multiple mainstream window libraries:

- **SDL2**: Mature and stable, widely used
- **SDL3**: Next-generation SDL with modern API
- **GLFW**: Lightweight, game development focused
- **EGL**: Headless rendering, embedded devices

The window system abstraction layer handles:

1. **Window Creation and Management**: Cross-platform window lifecycle
2. **Surface Creation**: Provides rendering surfaces for graphics APIs
3. **Input Handling**: Unified keyboard, mouse, and touch events
4. **KeyCode Translation**: Maps different library key codes to VRHI standard key codes
5. **Event Dispatch**: Supports both callback and polling modes

## Design Principles

### 1. Backend Agnostic

Window system abstraction is completely decoupled from rendering backends:

```cpp
// Application only deals with window abstraction
auto window = WindowSystem::Create(WindowConfig{
    .width = 1280,
    .height = 720,
    .title = "VRHI Application",
});

// Create rendering surface from window (backend auto-adapts)
auto surface = device->CreateSurface(window.get());
```

### 2. Unified Interface

All window systems implement the same `IWindow` interface:

```cpp
namespace VRHI {

class IWindow {
public:
    virtual ~IWindow() = default;
    
    // Window properties
    virtual uint32_t GetWidth() const = 0;
    virtual uint32_t GetHeight() const = 0;
    virtual bool ShouldClose() const = 0;
    
    // Window operations
    virtual void SetTitle(std::string_view title) = 0;
    virtual void SetSize(uint32_t width, uint32_t height) = 0;
    
    // Event handling
    virtual void PollEvents() = 0;
    
    // Surface information (for rendering backends)
    virtual void* GetNativeHandle() const = 0;
    virtual WindowSystemType GetType() const = 0;
};

} // namespace VRHI
```

### 3. Multi-Implementation Support

Factory pattern to select window system:

```cpp
enum class WindowBackend {
    Auto,      // Auto-select best implementation
    SDL2,
    SDL3,
    GLFW,
    EGL,
    Native,    // Platform native API
};

struct WindowConfig {
    uint32_t width = 800;
    uint32_t height = 600;
    std::string title = "VRHI Window";
    WindowBackend backend = WindowBackend::Auto;
    bool resizable = true;
    bool fullscreen = false;
};

std::expected<std::unique_ptr<IWindow>, Error> 
CreateWindow(const WindowConfig& config);
```

## KeyCode Abstraction

### Unified KeyCode Enum

Platform-independent key code enumeration:

```cpp
enum class KeyCode : uint16_t {
    // Letters
    A, B, C, D, E, F, G, H, I, J, K, L, M,
    N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
    
    // Numbers
    Num0, Num1, Num2, Num3, Num4,
    Num5, Num6, Num7, Num8, Num9,
    
    // Function keys
    F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
    
    // Special keys
    Escape, Tab, Space, Enter, Backspace,
    LeftShift, RightShift, LeftControl, RightControl,
    
    // Arrow keys
    ArrowUp, ArrowDown, ArrowLeft, ArrowRight,
    
    // Mouse buttons
    MouseLeft, MouseRight, MouseMiddle,
    
    Unknown = 0xFFFF,
};

enum class KeyAction : uint8_t {
    Press,
    Release,
    Repeat,
};
```

### KeyCode Mapper

Each window system provides key code translation:

```cpp
class KeyCodeMapper {
public:
    static KeyCode FromSDL2(int sdlKey);
    static KeyCode FromSDL3(int sdl3Key);
    static KeyCode FromGLFW(int glfwKey);
    static KeyCode FromNative(uint32_t nativeKey);
};
```

## Event System

### Event Types

```cpp
struct KeyEvent {
    KeyCode key;
    KeyAction action;
    KeyModifier modifiers;
    uint32_t timestamp;
};

struct MouseButtonEvent {
    KeyCode button;
    KeyAction action;
    int32_t x, y;
    uint32_t timestamp;
};

struct WindowResizeEvent {
    uint32_t width, height;
};

using Event = std::variant<
    KeyEvent,
    MouseButtonEvent,
    MouseMoveEvent,
    MouseScrollEvent,
    WindowResizeEvent,
    WindowCloseEvent
>;
```

### Event Handling Modes

#### Callback Mode

```cpp
window->SetKeyCallback([](const KeyEvent& e) {
    if (e.key == KeyCode::Escape && e.action == KeyAction::Press) {
        std::cout << "ESC pressed!\n";
    }
});

window->SetResizeCallback([&](const WindowResizeEvent& e) {
    device->ResizeSwapchain(e.width, e.height);
});
```

#### Polling Mode

```cpp
while (!window->ShouldClose()) {
    window->PollEvents();
    
    for (const auto& event : window->GetEvents()) {
        std::visit([&](auto&& e) {
            using T = std::decay_t<decltype(e)>;
            if constexpr (std::is_same_v<T, KeyEvent>) {
                HandleKeyEvent(e);
            }
        }, event);
    }
    
    RenderFrame();
}
```

## Implementation Examples

For detailed implementation examples including:

- SDL2/SDL3/GLFW/EGL window implementations
- Surface creation for different rendering backends
- Complete key code mapping tables
- Platform-specific considerations
- Full usage examples

See the [comprehensive Chinese documentation](../zh-CN/design/window_system.md).

## Platform Support

- **Windows**: SDL2/SDL3/GLFW/Native Win32
- **macOS**: SDL2/SDL3/GLFW/Native Cocoa
- **Linux**: SDL2/SDL3/GLFW/X11/Wayland
- **Android**: SDL2/SDL3/EGL
- **Raspberry Pi**: EGL/SDL2

## Summary

VRHI's window system abstraction provides:

1. **Unified Interface**: `IWindow` abstracts all window system differences
2. **Multiple Implementations**: SDL2/SDL3/GLFW/EGL via factory pattern
3. **KeyCode Translation**: Unified `KeyCode` enum with automatic mapping
4. **Flexible Events**: Supports both callback and polling modes
5. **Backend Integration**: Each rendering backend creates surfaces from windows
6. **Cross-Platform**: Single codebase for Windows/Linux/macOS/Android/Raspberry Pi
