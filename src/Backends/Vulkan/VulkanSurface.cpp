// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#include "VulkanSurface.hpp"
#include <VRHI/Logging.hpp>

// Platform-specific Vulkan extension names (if not defined by vulkan.h)
#ifndef VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#define VK_KHR_WIN32_SURFACE_EXTENSION_NAME "VK_KHR_win32_surface"
#endif

#ifndef VK_KHR_XLIB_SURFACE_EXTENSION_NAME
#define VK_KHR_XLIB_SURFACE_EXTENSION_NAME "VK_KHR_xlib_surface"
#endif

#ifndef VK_KHR_XCB_SURFACE_EXTENSION_NAME
#define VK_KHR_XCB_SURFACE_EXTENSION_NAME "VK_KHR_xcb_surface"
#endif

#ifndef VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME
#define VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME "VK_KHR_wayland_surface"
#endif

#ifndef VK_KHR_ANDROID_SURFACE_EXTENSION_NAME
#define VK_KHR_ANDROID_SURFACE_EXTENSION_NAME "VK_KHR_android_surface"
#endif

#ifndef VK_EXT_METAL_SURFACE_EXTENSION_NAME
#define VK_EXT_METAL_SURFACE_EXTENSION_NAME "VK_EXT_metal_surface"
#endif

// Forward declarations for different window systems
// Each window system provides its own surface creation function

#ifdef VRHI_WINDOW_GLFW
struct GLFWwindow;
extern "C" {
    VkResult glfwCreateWindowSurface(VkInstance instance, GLFWwindow* window,
                                     const VkAllocationCallbacks* allocator,
                                     VkSurfaceKHR* surface);
    const char** glfwGetRequiredInstanceExtensions(uint32_t* count);
}
#endif

#ifdef VRHI_WINDOW_SDL2
#include <SDL2/SDL_vulkan.h>
#endif

#ifdef VRHI_WINDOW_SDL3
// SDL3 would have similar includes
#endif

namespace VRHI {

std::expected<vk::UniqueSurfaceKHR, Error>
VulkanSurfaceCreator::CreateSurface(vk::Instance instance, const DeviceConfig& config) {
    if (!config.windowHandle) {
        return std::unexpected(Error{
            Error::Code::InvalidConfig,
            "Window handle is required for Vulkan surface creation"
        });
    }
    
    switch (config.windowSystem) {
        case DeviceConfig::WindowSystem::GLFW:
            return CreateGLFWSurface(instance, config.windowHandle);
        
        case DeviceConfig::WindowSystem::SDL2:
            return CreateSDL2Surface(instance, config.windowHandle);
        
        case DeviceConfig::WindowSystem::SDL3:
            return CreateSDL3Surface(instance, config.windowHandle);
        
        case DeviceConfig::WindowSystem::Win32:
            return CreateWin32Surface(instance, config.windowHandle, config.displayHandle);
        
        case DeviceConfig::WindowSystem::Xlib:
            return CreateXlibSurface(instance, config.windowHandle, config.displayHandle);
        
        case DeviceConfig::WindowSystem::Xcb:
            return CreateXcbSurface(instance, config.windowHandle, config.displayHandle);
        
        case DeviceConfig::WindowSystem::Wayland:
            return CreateWaylandSurface(instance, config.windowHandle, config.displayHandle);
        
        case DeviceConfig::WindowSystem::Android:
            return CreateAndroidSurface(instance, config.windowHandle);
        
        case DeviceConfig::WindowSystem::Metal:
            return CreateMetalSurface(instance, config.windowHandle);
        
        case DeviceConfig::WindowSystem::None:
        default:
            return std::unexpected(Error{
                Error::Code::InvalidConfig,
                "No window system specified for Vulkan surface creation"
            });
    }
}

std::vector<const char*>
VulkanSurfaceCreator::GetRequiredExtensions(DeviceConfig::WindowSystem windowSystem) {
    std::vector<const char*> extensions;
    
    // Base surface extension always required
    extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
    
    switch (windowSystem) {
        case DeviceConfig::WindowSystem::GLFW:
#ifdef VRHI_WINDOW_GLFW
            {
                uint32_t glfwExtensionCount = 0;
                const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
                if (glfwExtensions) {
                    // GLFW already includes VK_KHR_surface, so we skip our manually added one
                    extensions.clear();
                    for (uint32_t i = 0; i < glfwExtensionCount; i++) {
                        extensions.push_back(glfwExtensions[i]);
                    }
                }
            }
#endif
            break;
        
        case DeviceConfig::WindowSystem::SDL2:
        case DeviceConfig::WindowSystem::SDL3:
            // SDL provides its own extension query
            break;
        
        case DeviceConfig::WindowSystem::Win32:
            extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
            break;
        
        case DeviceConfig::WindowSystem::Xlib:
            extensions.push_back(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
            break;
        
        case DeviceConfig::WindowSystem::Xcb:
            extensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
            break;
        
        case DeviceConfig::WindowSystem::Wayland:
            extensions.push_back(VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME);
            break;
        
        case DeviceConfig::WindowSystem::Android:
            extensions.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
            break;
        
        case DeviceConfig::WindowSystem::Metal:
            extensions.push_back(VK_EXT_METAL_SURFACE_EXTENSION_NAME);
            break;
        
        default:
            break;
    }
    
    return extensions;
}

std::expected<vk::UniqueSurfaceKHR, Error>
VulkanSurfaceCreator::CreateGLFWSurface(vk::Instance instance, void* windowHandle) {
#ifdef VRHI_WINDOW_GLFW
    GLFWwindow* window = static_cast<GLFWwindow*>(windowHandle);
    
    VkSurfaceKHR surface;
    VkResult result = glfwCreateWindowSurface(
        static_cast<VkInstance>(instance),
        window,
        nullptr,
        &surface
    );
    
    if (result != VK_SUCCESS) {
        return std::unexpected(Error{
            Error::Code::InitializationFailed,
            "Failed to create GLFW Vulkan surface"
        });
    }
    
    return vk::UniqueSurfaceKHR(surface, instance);
#else
    return std::unexpected(Error{
        Error::Code::Unsupported,
        "GLFW support not compiled in (VRHI_WINDOW_GLFW not defined)"
    });
#endif
}

std::expected<vk::UniqueSurfaceKHR, Error>
VulkanSurfaceCreator::CreateSDL2Surface(vk::Instance instance, void* windowHandle) {
#ifdef VRHI_WINDOW_SDL2
    SDL_Window* window = static_cast<SDL_Window*>(windowHandle);
    
    VkSurfaceKHR surface;
    if (!SDL_Vulkan_CreateSurface(window, static_cast<VkInstance>(instance), &surface)) {
        return std::unexpected(Error{
            Error::Code::InitializationFailed,
            std::string("Failed to create SDL2 Vulkan surface: ") + SDL_GetError()
        });
    }
    
    return vk::UniqueSurfaceKHR(surface, instance);
#else
    return std::unexpected(Error{
        Error::Code::Unsupported,
        "SDL2 support not compiled in (VRHI_WINDOW_SDL2 not defined)"
    });
#endif
}

std::expected<vk::UniqueSurfaceKHR, Error>
VulkanSurfaceCreator::CreateSDL3Surface(vk::Instance instance, void* windowHandle) {
#ifdef VRHI_WINDOW_SDL3
    // SDL3 implementation - similar to SDL2
    SDL_Window* window = static_cast<SDL_Window*>(windowHandle);
    
    VkSurfaceKHR surface;
    if (!SDL_Vulkan_CreateSurface(window, static_cast<VkInstance>(instance), &surface)) {
        return std::unexpected(Error{
            Error::Code::InitializationFailed,
            std::string("Failed to create SDL3 Vulkan surface: ") + SDL_GetError()
        });
    }
    
    return vk::UniqueSurfaceKHR(surface, instance);
#else
    return std::unexpected(Error{
        Error::Code::Unsupported,
        "SDL3 support not compiled in (VRHI_WINDOW_SDL3 not defined)"
    });
#endif
}

std::expected<vk::UniqueSurfaceKHR, Error>
VulkanSurfaceCreator::CreateWin32Surface(vk::Instance instance, void* windowHandle, void* displayHandle) {
#ifdef VK_USE_PLATFORM_WIN32_KHR
    HWND hwnd = static_cast<HWND>(windowHandle);
    HINSTANCE hinstance = static_cast<HINSTANCE>(displayHandle);
    
    if (!hinstance) {
        hinstance = GetModuleHandle(nullptr);
    }
    
    vk::Win32SurfaceCreateInfoKHR createInfo;
    createInfo.hinstance = hinstance;
    createInfo.hwnd = hwnd;
    
    try {
        return instance.createWin32SurfaceKHRUnique(createInfo);
    } catch (const vk::SystemError& e) {
        return std::unexpected(Error{
            Error::Code::InitializationFailed,
            std::string("Failed to create Win32 Vulkan surface: ") + e.what()
        });
    }
#else
    return std::unexpected(Error{
        Error::Code::Unsupported,
        "Win32 surface not supported on this platform"
    });
#endif
}

std::expected<vk::UniqueSurfaceKHR, Error>
VulkanSurfaceCreator::CreateXlibSurface(vk::Instance instance, void* windowHandle, void* displayHandle) {
#ifdef VK_USE_PLATFORM_XLIB_KHR
    Display* display = static_cast<Display*>(displayHandle);
    Window window = reinterpret_cast<Window>(windowHandle);
    
    vk::XlibSurfaceCreateInfoKHR createInfo;
    createInfo.dpy = display;
    createInfo.window = window;
    
    try {
        return instance.createXlibSurfaceKHRUnique(createInfo);
    } catch (const vk::SystemError& e) {
        return std::unexpected(Error{
            Error::Code::InitializationFailed,
            std::string("Failed to create Xlib Vulkan surface: ") + e.what()
        });
    }
#else
    return std::unexpected(Error{
        Error::Code::Unsupported,
        "Xlib surface not supported on this platform"
    });
#endif
}

std::expected<vk::UniqueSurfaceKHR, Error>
VulkanSurfaceCreator::CreateXcbSurface(vk::Instance instance, void* windowHandle, void* displayHandle) {
#ifdef VK_USE_PLATFORM_XCB_KHR
    xcb_connection_t* connection = static_cast<xcb_connection_t*>(displayHandle);
    xcb_window_t window = reinterpret_cast<xcb_window_t>(windowHandle);
    
    vk::XcbSurfaceCreateInfoKHR createInfo;
    createInfo.connection = connection;
    createInfo.window = window;
    
    try {
        return instance.createXcbSurfaceKHRUnique(createInfo);
    } catch (const vk::SystemError& e) {
        return std::unexpected(Error{
            Error::Code::InitializationFailed,
            std::string("Failed to create XCB Vulkan surface: ") + e.what()
        });
    }
#else
    return std::unexpected(Error{
        Error::Code::Unsupported,
        "XCB surface not supported on this platform"
    });
#endif
}

std::expected<vk::UniqueSurfaceKHR, Error>
VulkanSurfaceCreator::CreateWaylandSurface(vk::Instance instance, void* windowHandle, void* displayHandle) {
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
    wl_display* display = static_cast<wl_display*>(displayHandle);
    wl_surface* surface = static_cast<wl_surface*>(windowHandle);
    
    vk::WaylandSurfaceCreateInfoKHR createInfo;
    createInfo.display = display;
    createInfo.surface = surface;
    
    try {
        return instance.createWaylandSurfaceKHRUnique(createInfo);
    } catch (const vk::SystemError& e) {
        return std::unexpected(Error{
            Error::Code::InitializationFailed,
            std::string("Failed to create Wayland Vulkan surface: ") + e.what()
        });
    }
#else
    return std::unexpected(Error{
        Error::Code::Unsupported,
        "Wayland surface not supported on this platform"
    });
#endif
}

std::expected<vk::UniqueSurfaceKHR, Error>
VulkanSurfaceCreator::CreateAndroidSurface(vk::Instance instance, void* windowHandle) {
#ifdef VK_USE_PLATFORM_ANDROID_KHR
    ANativeWindow* window = static_cast<ANativeWindow*>(windowHandle);
    
    vk::AndroidSurfaceCreateInfoKHR createInfo;
    createInfo.window = window;
    
    try {
        return instance.createAndroidSurfaceKHRUnique(createInfo);
    } catch (const vk::SystemError& e) {
        return std::unexpected(Error{
            Error::Code::InitializationFailed,
            std::string("Failed to create Android Vulkan surface: ") + e.what()
        });
    }
#else
    return std::unexpected(Error{
        Error::Code::Unsupported,
        "Android surface not supported on this platform"
    });
#endif
}

std::expected<vk::UniqueSurfaceKHR, Error>
VulkanSurfaceCreator::CreateMetalSurface(vk::Instance instance, void* windowHandle) {
#ifdef VK_USE_PLATFORM_METAL_EXT
    CAMetalLayer* layer = static_cast<CAMetalLayer*>(windowHandle);
    
    vk::MetalSurfaceCreateInfoEXT createInfo;
    createInfo.pLayer = layer;
    
    try {
        return instance.createMetalSurfaceEXTUnique(createInfo);
    } catch (const vk::SystemError& e) {
        return std::unexpected(Error{
            Error::Code::InitializationFailed,
            std::string("Failed to create Metal Vulkan surface: ") + e.what()
        });
    }
#else
    return std::unexpected(Error{
        Error::Code::Unsupported,
        "Metal surface not supported on this platform"
    });
#endif
}

} // namespace VRHI
