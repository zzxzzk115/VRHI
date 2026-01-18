// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#include "GLFWWindow.hpp"
#include <VRHI/VRHI.hpp>
#include <GLFW/glfw3.h>
#include <stdexcept>

namespace VRHI {

namespace {
    // Global GLFW initialization counter
    int g_glfwInitCount = 0;
    
    void InitializeGLFW() {
        if (g_glfwInitCount == 0) {
            if (!glfwInit()) {
                throw std::runtime_error("Failed to initialize GLFW");
            }
        }
        ++g_glfwInitCount;
    }
    
    void ShutdownGLFW() {
        --g_glfwInitCount;
        if (g_glfwInitCount == 0) {
            glfwTerminate();
        }
    }
}

GLFWWindow::GLFWWindow(const WindowConfig& config)
    : m_config(config)
    , m_width(config.width)
    , m_height(config.height)
{
}

GLFWWindow::~GLFWWindow() {
    if (m_window) {
        glfwDestroyWindow(m_window);
        ShutdownGLFW();
    }
}

std::expected<void, Error> GLFWWindow::Initialize() {
    if (m_initialized) {
        return std::unexpected(Error{
            Error::Code::ValidationError,
            "Window already initialized"
        });
    }
    
    try {
        InitializeGLFW();
    } catch (const std::exception& e) {
        return std::unexpected(Error{
            Error::Code::InitializationFailed,
            std::string("GLFW initialization failed: ") + e.what()
        });
    }
    
    // Set OpenGL context hints
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, m_config.opengl.majorVersion);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, m_config.opengl.minorVersion);
    
    if (m_config.opengl.coreProfile) {
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    }
    
    if (m_config.opengl.forwardCompatible) {
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    }
    
    // Window hints
    glfwWindowHint(GLFW_RESIZABLE, m_config.resizable ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_VISIBLE, m_config.visible ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_DECORATED, m_config.decorated ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_FOCUSED, m_config.focused ? GLFW_TRUE : GLFW_FALSE);
    
    if (m_config.opengl.samples > 0) {
        glfwWindowHint(GLFW_SAMPLES, m_config.opengl.samples);
    }
    
    // Create window
    m_window = glfwCreateWindow(
        static_cast<int>(m_config.width),
        static_cast<int>(m_config.height),
        m_config.title.c_str(),
        nullptr,
        nullptr
    );
    
    if (!m_window) {
        ShutdownGLFW();
        return std::unexpected(Error{
            Error::Code::InitializationFailed,
            "Failed to create GLFW window"
        });
    }
    
    // Store this pointer for callbacks
    glfwSetWindowUserPointer(m_window, this);
    
    // Register callbacks
    glfwSetKeyCallback(m_window, KeyCallbackImpl);
    glfwSetMouseButtonCallback(m_window, MouseButtonCallbackImpl);
    glfwSetCursorPosCallback(m_window, CursorPosCallbackImpl);
    glfwSetScrollCallback(m_window, ScrollCallbackImpl);
    glfwSetFramebufferSizeCallback(m_window, FramebufferSizeCallbackImpl);
    glfwSetWindowCloseCallback(m_window, WindowCloseCallbackImpl);
    
    // Make OpenGL context current
    glfwMakeContextCurrent(m_window);
    
    m_initialized = true;
    return {};
}

uint32_t GLFWWindow::GetWidth() const noexcept {
    return m_width;
}

uint32_t GLFWWindow::GetHeight() const noexcept {
    return m_height;
}

bool GLFWWindow::ShouldClose() const noexcept {
    if (!m_window) return true;
    return glfwWindowShouldClose(m_window);
}

WindowSystemType GLFWWindow::GetType() const noexcept {
    return WindowSystemType::GLFW;
}

void GLFWWindow::SetTitle(std::string_view title) {
    if (m_window) {
        glfwSetWindowTitle(m_window, std::string(title).c_str());
    }
}

void GLFWWindow::SetSize(uint32_t width, uint32_t height) {
    if (m_window) {
        glfwSetWindowSize(m_window, static_cast<int>(width), static_cast<int>(height));
        m_width = width;
        m_height = height;
    }
}

void GLFWWindow::Show() {
    if (m_window) {
        glfwShowWindow(m_window);
    }
}

void GLFWWindow::Hide() {
    if (m_window) {
        glfwHideWindow(m_window);
    }
}

void GLFWWindow::Close() {
    if (m_window) {
        glfwSetWindowShouldClose(m_window, GLFW_TRUE);
    }
}

void GLFWWindow::PollEvents() {
    glfwPollEvents();
}

void GLFWWindow::WaitEvents() {
    glfwWaitEvents();
}

void GLFWWindow::SwapBuffers() {
    if (m_window) {
        glfwSwapBuffers(m_window);
    }
}

void* GLFWWindow::GetNativeHandle() const noexcept {
    return m_window;
}

void GLFWWindow::SetKeyCallback(KeyCallback callback) {
    m_keyCallback = std::move(callback);
}

void GLFWWindow::SetMouseButtonCallback(MouseButtonCallback callback) {
    m_mouseButtonCallback = std::move(callback);
}

void GLFWWindow::SetCursorPosCallback(CursorPosCallback callback) {
    m_cursorPosCallback = std::move(callback);
}

void GLFWWindow::SetScrollCallback(ScrollCallback callback) {
    m_scrollCallback = std::move(callback);
}

void GLFWWindow::SetResizeCallback(ResizeCallback callback) {
    m_resizeCallback = std::move(callback);
}

void GLFWWindow::SetCloseCallback(CloseCallback callback) {
    m_closeCallback = std::move(callback);
}

// ============================================================================
// Static Callbacks
// ============================================================================

void GLFWWindow::KeyCallbackImpl(GLFWwindow* window, int key, int scancode, int action, int mods) {
    auto* self = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(window));
    if (self && self->m_keyCallback) {
        self->m_keyCallback(
            GLFWKeyToKeyCode(key),
            scancode,
            GLFWActionToKeyAction(action),
            GLFWModsToKeyMods(mods)
        );
    }
}

void GLFWWindow::MouseButtonCallbackImpl(GLFWwindow* window, int button, int action, int mods) {
    auto* self = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(window));
    if (self && self->m_mouseButtonCallback) {
        self->m_mouseButtonCallback(
            GLFWButtonToMouseButton(button),
            GLFWActionToKeyAction(action),
            GLFWModsToKeyMods(mods)
        );
    }
}

void GLFWWindow::CursorPosCallbackImpl(GLFWwindow* window, double xpos, double ypos) {
    auto* self = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(window));
    if (self && self->m_cursorPosCallback) {
        self->m_cursorPosCallback(xpos, ypos);
    }
}

void GLFWWindow::ScrollCallbackImpl(GLFWwindow* window, double xoffset, double yoffset) {
    auto* self = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(window));
    if (self && self->m_scrollCallback) {
        self->m_scrollCallback(xoffset, yoffset);
    }
}

void GLFWWindow::FramebufferSizeCallbackImpl(GLFWwindow* window, int width, int height) {
    auto* self = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(window));
    if (self) {
        self->m_width = static_cast<uint32_t>(width);
        self->m_height = static_cast<uint32_t>(height);
        if (self->m_resizeCallback) {
            self->m_resizeCallback(self->m_width, self->m_height);
        }
    }
}

void GLFWWindow::WindowCloseCallbackImpl(GLFWwindow* window) {
    auto* self = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(window));
    if (self && self->m_closeCallback) {
        self->m_closeCallback();
    }
}

// ============================================================================
// Key Code Conversion
// ============================================================================

KeyCode GLFWWindow::GLFWKeyToKeyCode(int glfwKey) {
    // Direct mapping for most keys
    if (glfwKey >= GLFW_KEY_SPACE && glfwKey <= GLFW_KEY_GRAVE_ACCENT) {
        return static_cast<KeyCode>(glfwKey);
    }
    if (glfwKey >= GLFW_KEY_ESCAPE && glfwKey <= GLFW_KEY_MENU) {
        return static_cast<KeyCode>(glfwKey);
    }
    return KeyCode::Unknown;
}

KeyAction GLFWWindow::GLFWActionToKeyAction(int glfwAction) {
    switch (glfwAction) {
        case GLFW_PRESS: return KeyAction::Press;
        case GLFW_RELEASE: return KeyAction::Release;
        case GLFW_REPEAT: return KeyAction::Repeat;
        default: return KeyAction::Release;
    }
}

KeyMods GLFWWindow::GLFWModsToKeyMods(int glfwMods) {
    int result = 0;
    if (glfwMods & GLFW_MOD_SHIFT) result |= static_cast<int>(KeyMods::Shift);
    if (glfwMods & GLFW_MOD_CONTROL) result |= static_cast<int>(KeyMods::Control);
    if (glfwMods & GLFW_MOD_ALT) result |= static_cast<int>(KeyMods::Alt);
    if (glfwMods & GLFW_MOD_SUPER) result |= static_cast<int>(KeyMods::Super);
    if (glfwMods & GLFW_MOD_CAPS_LOCK) result |= static_cast<int>(KeyMods::CapsLock);
    if (glfwMods & GLFW_MOD_NUM_LOCK) result |= static_cast<int>(KeyMods::NumLock);
    return static_cast<KeyMods>(result);
}

MouseButton GLFWWindow::GLFWButtonToMouseButton(int glfwButton) {
    switch (glfwButton) {
        case GLFW_MOUSE_BUTTON_LEFT: return MouseButton::Left;
        case GLFW_MOUSE_BUTTON_RIGHT: return MouseButton::Right;
        case GLFW_MOUSE_BUTTON_MIDDLE: return MouseButton::Middle;
        case GLFW_MOUSE_BUTTON_4: return MouseButton::Button4;
        case GLFW_MOUSE_BUTTON_5: return MouseButton::Button5;
        case GLFW_MOUSE_BUTTON_6: return MouseButton::Button6;
        case GLFW_MOUSE_BUTTON_7: return MouseButton::Button7;
        case GLFW_MOUSE_BUTTON_8: return MouseButton::Button8;
        default: return MouseButton::Left;
    }
}

} // namespace VRHI
