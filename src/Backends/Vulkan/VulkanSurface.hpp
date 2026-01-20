// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#pragma once

#include <VRHI/VRHI.hpp>
#include <vulkan/vulkan.hpp>
#include <expected>

namespace VRHI {

/// Helper class for creating Vulkan surfaces from different window systems
class VulkanSurfaceCreator {
public:
    /// Create a Vulkan surface based on the window system type
    static std::expected<vk::UniqueSurfaceKHR, Error>
    CreateSurface(vk::Instance instance, const DeviceConfig& config);
    
    /// Get required instance extensions for the window system
    static std::vector<const char*>
    GetRequiredExtensions(DeviceConfig::WindowSystem windowSystem);
    
private:
    static std::expected<vk::UniqueSurfaceKHR, Error>
    CreateGLFWSurface(vk::Instance instance, void* windowHandle);
    
    static std::expected<vk::UniqueSurfaceKHR, Error>
    CreateSDL2Surface(vk::Instance instance, void* windowHandle);
    
    static std::expected<vk::UniqueSurfaceKHR, Error>
    CreateSDL3Surface(vk::Instance instance, void* windowHandle);
    
    static std::expected<vk::UniqueSurfaceKHR, Error>
    CreateWin32Surface(vk::Instance instance, void* windowHandle, void* displayHandle);
    
    static std::expected<vk::UniqueSurfaceKHR, Error>
    CreateXlibSurface(vk::Instance instance, void* windowHandle, void* displayHandle);
    
    static std::expected<vk::UniqueSurfaceKHR, Error>
    CreateXcbSurface(vk::Instance instance, void* windowHandle, void* displayHandle);
    
    static std::expected<vk::UniqueSurfaceKHR, Error>
    CreateWaylandSurface(vk::Instance instance, void* windowHandle, void* displayHandle);
    
    static std::expected<vk::UniqueSurfaceKHR, Error>
    CreateAndroidSurface(vk::Instance instance, void* windowHandle);
    
    static std::expected<vk::UniqueSurfaceKHR, Error>
    CreateMetalSurface(vk::Instance instance, void* windowHandle);
};

} // namespace VRHI
