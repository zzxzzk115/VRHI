# 窗口系统抽象设计

## 概述

VRHI 提供统一的窗口系统抽象层，支持多种主流窗口库：

- **SDL2**: 成熟稳定，广泛使用
- **SDL3**: 下一代 SDL，现代 API
- **GLFW**: 轻量级，专注游戏开发
- **EGL**: 无窗口渲染，嵌入式设备

窗口系统抽象层负责：

1. **窗口创建和管理**：跨平台的窗口生命周期
2. **Surface 创建**：为图形 API 提供渲染表面
3. **输入处理**：统一的键盘、鼠标、触摸事件
4. **KeyCode 转换**：不同库的键码映射到 VRHI 标准键码
5. **事件分发**：支持回调和轮询两种模式

## 设计原则

### 1. 后端无关性

窗口系统抽象与渲染后端完全解耦：

```cpp
// 应用只需关心窗口抽象，不关心具体实现
auto window = WindowSystem::Create(WindowConfig{
    .width = 1280,
    .height = 720,
    .title = "VRHI Application",
});

// 从窗口创建渲染 Surface（后端自动适配）
auto surface = device->CreateSurface(window.get());
```

### 2. 统一接口

所有窗口系统实现相同的接口 `IWindow`：

```cpp
namespace VRHI {

class IWindow {
public:
    virtual ~IWindow() = default;
    
    // 窗口属性
    virtual uint32_t GetWidth() const = 0;
    virtual uint32_t GetHeight() const = 0;
    virtual std::string_view GetTitle() const = 0;
    virtual bool ShouldClose() const = 0;
    
    // 窗口操作
    virtual void SetTitle(std::string_view title) = 0;
    virtual void SetSize(uint32_t width, uint32_t height) = 0;
    virtual void Show() = 0;
    virtual void Hide() = 0;
    virtual void Close() = 0;
    
    // 事件处理
    virtual void PollEvents() = 0;
    virtual void WaitEvents() = 0;
    
    // Surface 信息（供渲染后端使用）
    virtual void* GetNativeHandle() const = 0;
    virtual WindowSystemType GetType() const = 0;
};

enum class WindowSystemType : uint8_t {
    SDL2,
    SDL3,
    GLFW,
    EGL,
    Win32,    // 原生 Windows
    Cocoa,    // 原生 macOS
    X11,      // 原生 Linux
    Wayland,  // 原生 Linux
};

} // namespace VRHI
```

### 3. 多实现支持

通过工厂模式选择窗口系统：

```cpp
enum class WindowBackend {
    Auto,      // 自动选择最佳实现
    SDL2,
    SDL3,
    GLFW,
    EGL,
    Native,    // 使用平台原生 API
};

struct WindowConfig {
    uint32_t width = 800;
    uint32_t height = 600;
    std::string title = "VRHI Window";
    WindowBackend backend = WindowBackend::Auto;
    bool resizable = true;
    bool fullscreen = false;
    bool vsync = true;
};

// 工厂函数
std::expected<std::unique_ptr<IWindow>, Error> 
CreateWindow(const WindowConfig& config);
```

## 键码抽象

### VRHI 统一键码

定义与平台无关的键码枚举：

```cpp
namespace VRHI {

enum class KeyCode : uint16_t {
    // 字母键
    A = 0, B, C, D, E, F, G, H, I, J, K, L, M,
    N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
    
    // 数字键
    Num0, Num1, Num2, Num3, Num4,
    Num5, Num6, Num7, Num8, Num9,
    
    // 功能键
    F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
    
    // 特殊键
    Escape, Tab, CapsLock, LeftShift, LeftControl, LeftAlt,
    RightShift, RightControl, RightAlt, Space, Enter, Backspace,
    
    // 方向键
    ArrowUp, ArrowDown, ArrowLeft, ArrowRight,
    
    // 编辑键
    Insert, Delete, Home, End, PageUp, PageDown,
    
    // 小键盘
    Numpad0, Numpad1, Numpad2, Numpad3, Numpad4,
    Numpad5, Numpad6, Numpad7, Numpad8, Numpad9,
    NumpadAdd, NumpadSubtract, NumpadMultiply, NumpadDivide,
    NumpadEnter, NumpadDecimal,
    
    // 鼠标按键
    MouseLeft, MouseRight, MouseMiddle,
    MouseButton4, MouseButton5,
    
    // 手柄按键（可选）
    GamepadA, GamepadB, GamepadX, GamepadY,
    GamepadLeftBumper, GamepadRightBumper,
    GamepadStart, GamepadSelect,
    
    Unknown = 0xFFFF,
};

enum class KeyAction : uint8_t {
    Press,
    Release,
    Repeat,
};

enum class KeyModifier : uint8_t {
    None    = 0,
    Shift   = 1 << 0,
    Control = 1 << 1,
    Alt     = 1 << 2,
    Super   = 1 << 3,  // Windows 键 / Command 键
};

} // namespace VRHI
```

### 键码映射器

每个窗口系统实现提供键码转换：

```cpp
class KeyCodeMapper {
public:
    // SDL2 键码转 VRHI 键码
    static KeyCode FromSDL2(int sdlKey);
    
    // SDL3 键码转 VRHI 键码
    static KeyCode FromSDL3(int sdl3Key);
    
    // GLFW 键码转 VRHI 键码
    static KeyCode FromGLFW(int glfwKey);
    
    // 平台原生键码转 VRHI 键码
    static KeyCode FromNative(uint32_t nativeKey);
    
    // 反向转换（用于测试）
    static int ToSDL2(KeyCode key);
    static int ToSDL3(KeyCode key);
    static int ToGLFW(KeyCode key);
};
```

### 映射实现示例

```cpp
// src/WindowSystem/KeyCodeMapper.cpp

KeyCode KeyCodeMapper::FromSDL2(int sdlKey) {
    // SDL_Keycode 映射表
    static const std::unordered_map<int, KeyCode> s_sdl2Map = {
        {SDLK_a, KeyCode::A},
        {SDLK_b, KeyCode::B},
        // ... 完整映射表
        {SDLK_ESCAPE, KeyCode::Escape},
        {SDLK_SPACE, KeyCode::Space},
        {SDLK_UP, KeyCode::ArrowUp},
        {SDLK_DOWN, KeyCode::ArrowDown},
        {SDLK_LEFT, KeyCode::ArrowLeft},
        {SDLK_RIGHT, KeyCode::ArrowRight},
    };
    
    auto it = s_sdl2Map.find(sdlKey);
    return (it != s_sdl2Map.end()) ? it->second : KeyCode::Unknown;
}

KeyCode KeyCodeMapper::FromGLFW(int glfwKey) {
    // GLFW_KEY_* 映射表
    static const std::unordered_map<int, KeyCode> s_glfwMap = {
        {GLFW_KEY_A, KeyCode::A},
        {GLFW_KEY_B, KeyCode::B},
        // ... 完整映射表
        {GLFW_KEY_ESCAPE, KeyCode::Escape},
        {GLFW_KEY_SPACE, KeyCode::Space},
    };
    
    auto it = s_glfwMap.find(glfwKey);
    return (it != s_glfwMap.end()) ? it->second : KeyCode::Unknown;
}
```

## 事件系统

### 事件类型

```cpp
namespace VRHI {

struct KeyEvent {
    KeyCode key;
    KeyAction action;
    KeyModifier modifiers;
    uint32_t timestamp;
};

struct MouseButtonEvent {
    KeyCode button;  // MouseLeft, MouseRight, MouseMiddle
    KeyAction action;
    int32_t x, y;    // 窗口坐标
    uint32_t timestamp;
};

struct MouseMoveEvent {
    int32_t x, y;          // 当前位置
    int32_t deltaX, deltaY; // 相对移动
    uint32_t timestamp;
};

struct MouseScrollEvent {
    float deltaX, deltaY;  // 滚轮增量
    uint32_t timestamp;
};

struct WindowResizeEvent {
    uint32_t width, height;
};

struct WindowCloseEvent {
    // 可以在此设置是否取消关闭
    bool cancelled = false;
};

// 事件联合体
using Event = std::variant<
    KeyEvent,
    MouseButtonEvent,
    MouseMoveEvent,
    MouseScrollEvent,
    WindowResizeEvent,
    WindowCloseEvent
>;

} // namespace VRHI
```

### 事件处理模式

#### 模式 1: 回调模式

```cpp
class IWindow {
public:
    // 设置事件回调
    virtual void SetKeyCallback(std::function<void(const KeyEvent&)> callback) = 0;
    virtual void SetMouseButtonCallback(std::function<void(const MouseButtonEvent&)> callback) = 0;
    virtual void SetMouseMoveCallback(std::function<void(const MouseMoveEvent&)> callback) = 0;
    virtual void SetResizeCallback(std::function<void(const WindowResizeEvent&)> callback) = 0;
};

// 使用示例
window->SetKeyCallback([](const KeyEvent& e) {
    if (e.key == KeyCode::Escape && e.action == KeyAction::Press) {
        std::cout << "ESC pressed!\n";
    }
});

window->SetResizeCallback([&](const WindowResizeEvent& e) {
    // 调整渲染目标大小
    device->ResizeSwapchain(e.width, e.height);
});
```

#### 模式 2: 轮询模式

```cpp
class IWindow {
public:
    // 获取事件队列
    virtual std::span<const Event> GetEvents() = 0;
    
    // 清空事件队列
    virtual void ClearEvents() = 0;
};

// 使用示例
while (!window->ShouldClose()) {
    window->PollEvents();
    
    for (const auto& event : window->GetEvents()) {
        std::visit([&](auto&& e) {
            using T = std::decay_t<decltype(e)>;
            
            if constexpr (std::is_same_v<T, KeyEvent>) {
                HandleKeyEvent(e);
            } else if constexpr (std::is_same_v<T, MouseButtonEvent>) {
                HandleMouseButton(e);
            }
            // ... 其他事件类型
        }, event);
    }
    
    window->ClearEvents();
    
    // 渲染
    RenderFrame();
}
```

## 窗口系统实现

### SDL2 实现

```cpp
// src/WindowSystem/SDL2Window.hpp

class SDL2Window : public IWindow {
public:
    explicit SDL2Window(const WindowConfig& config);
    ~SDL2Window() override;
    
    // IWindow 接口实现
    uint32_t GetWidth() const override { return m_width; }
    uint32_t GetHeight() const override { return m_height; }
    std::string_view GetTitle() const override { return m_title; }
    bool ShouldClose() const override { return m_shouldClose; }
    
    void SetTitle(std::string_view title) override;
    void SetSize(uint32_t width, uint32_t height) override;
    void PollEvents() override;
    
    void* GetNativeHandle() const override { return m_window; }
    WindowSystemType GetType() const override { return WindowSystemType::SDL2; }
    
private:
    SDL_Window* m_window = nullptr;
    uint32_t m_width, m_height;
    std::string m_title;
    bool m_shouldClose = false;
    
    std::vector<Event> m_events;
    std::function<void(const KeyEvent&)> m_keyCallback;
    
    void ProcessSDLEvent(const SDL_Event& sdlEvent);
};
```

```cpp
// src/WindowSystem/SDL2Window.cpp

SDL2Window::SDL2Window(const WindowConfig& config)
    : m_width(config.width)
    , m_height(config.height)
    , m_title(config.title)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        throw std::runtime_error("SDL2 init failed");
    }
    
    uint32_t flags = SDL_WINDOW_SHOWN;
    if (config.resizable) flags |= SDL_WINDOW_RESIZABLE;
    if (config.fullscreen) flags |= SDL_WINDOW_FULLSCREEN;
    
    // 设置 OpenGL 属性（如果使用 OpenGL 后端）
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    
    // 创建窗口
    m_window = SDL_CreateWindow(
        m_title.c_str(),
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        m_width, m_height,
        flags
    );
    
    if (!m_window) {
        throw std::runtime_error("SDL_CreateWindow failed");
    }
}

void SDL2Window::PollEvents() {
    m_events.clear();
    
    SDL_Event sdlEvent;
    while (SDL_PollEvent(&sdlEvent)) {
        ProcessSDLEvent(sdlEvent);
    }
}

void SDL2Window::ProcessSDLEvent(const SDL_Event& sdlEvent) {
    switch (sdlEvent.type) {
        case SDL_QUIT:
            m_shouldClose = true;
            m_events.push_back(WindowCloseEvent{});
            break;
            
        case SDL_KEYDOWN:
        case SDL_KEYUP: {
            KeyEvent e;
            e.key = KeyCodeMapper::FromSDL2(sdlEvent.key.keysym.sym);
            e.action = (sdlEvent.type == SDL_KEYDOWN) ? KeyAction::Press : KeyAction::Release;
            e.modifiers = static_cast<KeyModifier>(0);
            
            if (sdlEvent.key.keysym.mod & KMOD_SHIFT) e.modifiers = e.modifiers | KeyModifier::Shift;
            if (sdlEvent.key.keysym.mod & KMOD_CTRL) e.modifiers = e.modifiers | KeyModifier::Control;
            if (sdlEvent.key.keysym.mod & KMOD_ALT) e.modifiers = e.modifiers | KeyModifier::Alt;
            
            e.timestamp = sdlEvent.key.timestamp;
            
            m_events.push_back(e);
            
            if (m_keyCallback) {
                m_keyCallback(e);
            }
            break;
        }
        
        case SDL_WINDOWEVENT:
            if (sdlEvent.window.event == SDL_WINDOWEVENT_RESIZED) {
                m_width = sdlEvent.window.data1;
                m_height = sdlEvent.window.data2;
                m_events.push_back(WindowResizeEvent{m_width, m_height});
            }
            break;
            
        // ... 其他事件类型
    }
}
```

### GLFW 实现

```cpp
// src/WindowSystem/GLFWWindow.cpp

class GLFWWindow : public IWindow {
public:
    explicit GLFWWindow(const WindowConfig& config);
    ~GLFWWindow() override;
    
    void PollEvents() override {
        m_events.clear();
        glfwPollEvents();
    }
    
private:
    GLFWwindow* m_window = nullptr;
    
    // GLFW 回调函数
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        auto* self = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(window));
        
        KeyEvent e;
        e.key = KeyCodeMapper::FromGLFW(key);
        e.action = (action == GLFW_PRESS) ? KeyAction::Press :
                   (action == GLFW_RELEASE) ? KeyAction::Release : KeyAction::Repeat;
        
        e.modifiers = static_cast<KeyModifier>(0);
        if (mods & GLFW_MOD_SHIFT) e.modifiers = e.modifiers | KeyModifier::Shift;
        if (mods & GLFW_MOD_CONTROL) e.modifiers = e.modifiers | KeyModifier::Control;
        
        self->m_events.push_back(e);
        
        if (self->m_keyCallback) {
            self->m_keyCallback(e);
        }
    }
};

GLFWWindow::GLFWWindow(const WindowConfig& config) {
    if (!glfwInit()) {
        throw std::runtime_error("GLFW init failed");
    }
    
    // 配置 OpenGL 上下文
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    m_window = glfwCreateWindow(config.width, config.height, config.title.c_str(), nullptr, nullptr);
    
    glfwSetWindowUserPointer(m_window, this);
    glfwSetKeyCallback(m_window, KeyCallback);
    // ... 其他回调
}
```

### EGL 实现（无窗口）

```cpp
// src/WindowSystem/EGLWindow.cpp

class EGLWindow : public IWindow {
public:
    explicit EGLWindow(const WindowConfig& config);
    
    // EGL 可以无窗口运行（离屏渲染）
    void PollEvents() override {
        // 无事件，或从系统获取最小事件
    }
    
    void* GetNativeHandle() const override {
        return reinterpret_cast<void*>(m_surface);
    }
    
private:
    EGLDisplay m_display = EGL_NO_DISPLAY;
    EGLSurface m_surface = EGL_NO_SURFACE;
    EGLContext m_context = EGL_NO_CONTEXT;
};

EGLWindow::EGLWindow(const WindowConfig& config) {
    m_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(m_display, nullptr, nullptr);
    
    // 创建 Pbuffer surface（离屏）
    EGLint pbufferAttribs[] = {
        EGL_WIDTH, static_cast<EGLint>(config.width),
        EGL_HEIGHT, static_cast<EGLint>(config.height),
        EGL_NONE,
    };
    
    m_surface = eglCreatePbufferSurface(m_display, eglConfig, pbufferAttribs);
    
    // 创建上下文
    EGLint contextAttribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 3,
        EGL_NONE,
    };
    
    m_context = eglCreateContext(m_display, eglConfig, EGL_NO_CONTEXT, contextAttribs);
    eglMakeCurrent(m_display, m_surface, m_surface, m_context);
}
```

## Surface 创建

### 后端集成

每个渲染后端需要从窗口创建 Surface：

```cpp
// Vulkan 后端
VkSurfaceKHR CreateVulkanSurface(IWindow* window, VkInstance instance) {
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    
    switch (window->GetType()) {
        case WindowSystemType::SDL2: {
            auto* sdlWindow = static_cast<SDL_Window*>(window->GetNativeHandle());
            SDL_Vulkan_CreateSurface(sdlWindow, instance, &surface);
            break;
        }
        
        case WindowSystemType::GLFW: {
            auto* glfwWindow = static_cast<GLFWwindow*>(window->GetNativeHandle());
            glfwCreateWindowSurface(instance, glfwWindow, nullptr, &surface);
            break;
        }
        
        case WindowSystemType::Win32: {
            // 原生 Win32 surface
            VkWin32SurfaceCreateInfoKHR createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
            createInfo.hwnd = static_cast<HWND>(window->GetNativeHandle());
            vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &surface);
            break;
        }
        
        // ... 其他平台
    }
    
    return surface;
}

// OpenGL 后端
void MakeGLContextCurrent(IWindow* window) {
    switch (window->GetType()) {
        case WindowSystemType::SDL2: {
            auto* sdlWindow = static_cast<SDL_Window*>(window->GetNativeHandle());
            SDL_GLContext context = SDL_GL_CreateContext(sdlWindow);
            SDL_GL_MakeCurrent(sdlWindow, context);
            break;
        }
        
        case WindowSystemType::GLFW: {
            auto* glfwWindow = static_cast<GLFWwindow*>(window->GetNativeHandle());
            glfwMakeContextCurrent(glfwWindow);
            break;
        }
        
        case WindowSystemType::EGL: {
            // EGL 已在窗口创建时设置
            break;
        }
    }
}
```

## 工厂实现

### 自动选择最佳窗口系统

```cpp
// src/WindowSystem/WindowFactory.cpp

std::expected<std::unique_ptr<IWindow>, Error>
CreateWindow(const WindowConfig& config) {
    WindowBackend backend = config.backend;
    
    // 自动选择
    if (backend == WindowBackend::Auto) {
#if defined(VRHI_WINDOW_SDL3)
        backend = WindowBackend::SDL3;  // 优先 SDL3
#elif defined(VRHI_WINDOW_SDL2)
        backend = WindowBackend::SDL2;
#elif defined(VRHI_WINDOW_GLFW)
        backend = WindowBackend::GLFW;
#elif defined(VRHI_WINDOW_EGL)
        backend = WindowBackend::EGL;
#else
        return std::unexpected(Error{
            Error::Code::NotSupported,
            "No window system available"
        });
#endif
    }
    
    // 创建对应实现
    try {
        switch (backend) {
#if defined(VRHI_WINDOW_SDL2)
            case WindowBackend::SDL2:
                return std::make_unique<SDL2Window>(config);
#endif
                
#if defined(VRHI_WINDOW_SDL3)
            case WindowBackend::SDL3:
                return std::make_unique<SDL3Window>(config);
#endif
                
#if defined(VRHI_WINDOW_GLFW)
            case WindowBackend::GLFW:
                return std::make_unique<GLFWWindow>(config);
#endif
                
#if defined(VRHI_WINDOW_EGL)
            case WindowBackend::EGL:
                return std::make_unique<EGLWindow>(config);
#endif
                
            default:
                return std::unexpected(Error{
                    Error::Code::NotSupported,
                    "Requested window system not compiled in"
                });
        }
    } catch (const std::exception& e) {
        return std::unexpected(Error{
            Error::Code::InitializationFailed,
            std::string("Window creation failed: ") + e.what()
        });
    }
}
```

## 使用示例

### 完整示例：创建窗口和渲染设备

```cpp
#include <VRHI/VRHI.hpp>
#include <VRHI/WindowSystem.hpp>

int main() {
    using namespace VRHI;
    
    // 1. 创建窗口
    WindowConfig windowConfig;
    windowConfig.width = 1280;
    windowConfig.height = 720;
    windowConfig.title = "VRHI Application";
    windowConfig.backend = WindowBackend::Auto;  // 自动选择
    
    auto windowResult = CreateWindow(windowConfig);
    if (!windowResult) {
        std::cerr << "Window creation failed: " << windowResult.error().message << "\n";
        return 1;
    }
    
    auto window = std::move(*windowResult);
    
    // 2. 创建渲染设备
    DeviceConfig deviceConfig;
    deviceConfig.window = window.get();  // 传递窗口句柄
    deviceConfig.features.required = {Feature::Texture2D};
    
    auto deviceResult = CreateDevice(deviceConfig);
    if (!deviceResult) {
        std::cerr << "Device creation failed: " << deviceResult.error().message << "\n";
        return 1;
    }
    
    auto device = std::move(*deviceResult);
    
    // 3. 设置事件回调
    window->SetKeyCallback([&](const KeyEvent& e) {
        if (e.key == KeyCode::Escape && e.action == KeyAction::Press) {
            window->Close();
        }
    });
    
    window->SetResizeCallback([&](const WindowResizeEvent& e) {
        device->ResizeSwapchain(e.width, e.height);
    });
    
    // 4. 主循环
    while (!window->ShouldClose()) {
        window->PollEvents();
        
        // 渲染
        auto cmd = device->BeginFrame();
        // ... 渲染命令
        device->EndFrame();
    }
    
    return 0;
}
```

## 平台特性

### Windows

- 支持 SDL2/SDL3/GLFW/原生 Win32
- 推荐：GLFW（轻量）或 SDL2（稳定）
- D3D12 后端优先使用原生 Win32 窗口

### macOS

- 支持 SDL2/SDL3/GLFW/原生 Cocoa
- 推荐：GLFW（现代）或 SDL2
- Metal 后端优先使用原生 Cocoa 窗口

### Linux

- 支持 SDL2/SDL3/GLFW/原生 X11/Wayland
- 推荐：SDL2（兼容性）或 GLFW
- 需要安装 X11 或 Wayland 开发库

### Android

- 支持 SDL2/SDL3/EGL
- 推荐：EGL（轻量）或 SDL2（功能完整）
- 使用 NativeActivity 或 SDL_main

### Raspberry Pi

- 支持 EGL/SDL2
- 推荐：EGL（直接渲染）
- OpenGL ES 2.0/3.0 后端

## 总结

VRHI 的窗口系统抽象设计实现了：

1. **统一接口**：`IWindow` 抽象所有窗口系统差异
2. **多实现支持**：SDL2/SDL3/GLFW/EGL，通过工厂模式选择
3. **键码转换**：统一的 `KeyCode` 枚举，自动映射不同库的键码
4. **灵活事件**：支持回调和轮询两种模式
5. **后端集成**：每个渲染后端可从窗口创建对应 Surface
6. **跨平台**：一套代码支持 Windows/Linux/macOS/Android/Raspberry Pi

这种设计确保应用代码无需关心底层窗口系统细节，同时保留了选择最适合平台的窗口库的灵活性。
