// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#include <VRHI/Window.hpp>
#include <VRHI/VRHI.hpp>

#ifdef VRHI_WINDOW_GLFW
#include "GLFW/GLFWWindow.hpp"
#endif

namespace VRHI {

std::expected<std::unique_ptr<IWindow>, Error>
CreateWindow(const WindowConfig& config, WindowSystemType type) {
    // Auto-select window system based on availability and platform
    if (type == WindowSystemType::Auto) {
#ifdef VRHI_WINDOW_GLFW
        type = WindowSystemType::GLFW;
#elif defined(VRHI_WINDOW_SDL2)
        type = WindowSystemType::SDL2;
#elif defined(VRHI_WINDOW_SDL3)
        type = WindowSystemType::SDL3;
#elif defined(VRHI_WINDOW_EGL)
        type = WindowSystemType::EGL;
#else
        return std::unexpected(Error{
            Error::Code::NotSupported,
            "No window system backend available"
        });
#endif
    }
    
    // Create window based on selected type
    switch (type) {
#ifdef VRHI_WINDOW_GLFW
        case WindowSystemType::GLFW: {
            auto window = std::make_unique<GLFWWindow>(config);
            auto result = window->Initialize();
            if (!result) {
                return std::unexpected(result.error());
            }
            return window;
        }
#endif
        
#ifdef VRHI_WINDOW_SDL2
        case WindowSystemType::SDL2:
            // TODO: Implement SDL2 window
            return std::unexpected(Error{
                Error::Code::NotSupported,
                "SDL2 window system not yet implemented"
            });
#endif
        
#ifdef VRHI_WINDOW_SDL3
        case WindowSystemType::SDL3:
            // TODO: Implement SDL3 window
            return std::unexpected(Error{
                Error::Code::NotSupported,
                "SDL3 window system not yet implemented"
            });
#endif
        
#ifdef VRHI_WINDOW_EGL
        case WindowSystemType::EGL:
            // TODO: Implement EGL window
            return std::unexpected(Error{
                Error::Code::NotSupported,
                "EGL window system not yet implemented"
            });
#endif
        
        default:
            return std::unexpected(Error{
                Error::Code::NotSupported,
                "Requested window system not available in this build"
            });
    }
}

} // namespace VRHI
