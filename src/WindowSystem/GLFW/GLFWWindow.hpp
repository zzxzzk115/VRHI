// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#pragma once

#include <VRHI/Window.hpp>

struct GLFWwindow;

namespace VRHI {

/// GLFW window system implementation
class GLFWWindow : public IWindow {
public:
    GLFWWindow(const WindowConfig& config);
    ~GLFWWindow() override;
    
    // Prevent copying
    GLFWWindow(const GLFWWindow&) = delete;
    GLFWWindow& operator=(const GLFWWindow&) = delete;
    
    // Initialize the window
    std::expected<void, Error> Initialize();
    
    // IWindow implementation
    uint32_t GetWidth() const noexcept override;
    uint32_t GetHeight() const noexcept override;
    bool ShouldClose() const noexcept override;
    WindowSystemType GetType() const noexcept override;
    
    void SetTitle(std::string_view title) override;
    void SetSize(uint32_t width, uint32_t height) override;
    void Show() override;
    void Hide() override;
    void Close() override;
    
    void PollEvents() override;
    void WaitEvents() override;
    
    void* GetNativeHandle() const noexcept override;
    
    void SetKeyCallback(KeyCallback callback) override;
    void SetMouseButtonCallback(MouseButtonCallback callback) override;
    void SetCursorPosCallback(CursorPosCallback callback) override;
    void SetScrollCallback(ScrollCallback callback) override;
    void SetResizeCallback(ResizeCallback callback) override;
    void SetCloseCallback(CloseCallback callback) override;
    
    // GLFW-specific
    GLFWwindow* GetGLFWHandle() const noexcept { return m_window; }
    
private:
    // GLFW callbacks (static)
    static void KeyCallbackImpl(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void MouseButtonCallbackImpl(GLFWwindow* window, int button, int action, int mods);
    static void CursorPosCallbackImpl(GLFWwindow* window, double xpos, double ypos);
    static void ScrollCallbackImpl(GLFWwindow* window, double xoffset, double yoffset);
    static void FramebufferSizeCallbackImpl(GLFWwindow* window, int width, int height);
    static void WindowCloseCallbackImpl(GLFWwindow* window);
    
    // Key code conversion
    static KeyCode GLFWKeyToKeyCode(int glfwKey);
    static KeyAction GLFWActionToKeyAction(int glfwAction);
    static KeyMods GLFWModsToKeyMods(int glfwMods);
    static MouseButton GLFWButtonToMouseButton(int glfwButton);
    
    WindowConfig m_config;
    GLFWwindow* m_window = nullptr;
    bool m_initialized = false;
    
    // User callbacks
    KeyCallback m_keyCallback;
    MouseButtonCallback m_mouseButtonCallback;
    CursorPosCallback m_cursorPosCallback;
    ScrollCallback m_scrollCallback;
    ResizeCallback m_resizeCallback;
    CloseCallback m_closeCallback;
    
    // Window size tracking
    uint32_t m_width;
    uint32_t m_height;
};

} // namespace VRHI
