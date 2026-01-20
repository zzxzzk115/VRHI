// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#include "VulkanSwapChain.hpp"
#include "VulkanDevice.hpp"
#include "VulkanSync.hpp"
#include "VulkanTexture.hpp"
#include <VRHI/Logging.hpp>
#include <algorithm>

namespace VRHI {

VulkanSwapChain::VulkanSwapChain(VulkanDevice& device)
    : m_device(device)
{
}

VulkanSwapChain::~VulkanSwapChain() {
    CleanupSwapChain();
}

std::unique_ptr<SwapChain>
VulkanSwapChain::Create(VulkanDevice& device) {
    auto swapChain = std::unique_ptr<VulkanSwapChain>(new VulkanSwapChain(device));
    
    try {
        swapChain->CreateSwapChain();
        swapChain->CreateImageViews();
        LogInfo("VulkanSwapChain created successfully");
        return swapChain;
        
    } catch (const std::exception& e) {
        LogError("Failed to create VulkanSwapChain: {}", e.what());
        return nullptr;
    }
}

void VulkanSwapChain::CreateSwapChain() {
    auto swapChainSupport = QuerySwapChainSupport();
    
    auto surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
    auto presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
    auto extent = ChooseSwapExtent(swapChainSupport.capabilities);
    
    // Request one more image than the minimum to avoid waiting
    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && 
        imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }
    
    vk::SwapchainCreateInfoKHR createInfo;
    createInfo.surface = m_device.GetSurface();
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
    
    uint32_t queueFamilyIndices[] = {
        m_device.GetGraphicsQueueFamily(),
        m_device.GetPresentQueueFamily()
    };
    
    if (m_device.GetGraphicsQueueFamily() != m_device.GetPresentQueueFamily()) {
        createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = vk::SharingMode::eExclusive;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }
    
    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = nullptr;
    
    m_swapChain = m_device.GetVulkanDevice().createSwapchainKHRUnique(createInfo);
    
    // Get swapchain images
    m_images = m_device.GetVulkanDevice().getSwapchainImagesKHR(m_swapChain.get());
    
    // Store format and extent
    m_vkFormat = surfaceFormat.format;
    m_extent = extent;
    m_width = extent.width;
    m_height = extent.height;
    
    // Convert Vulkan format to VRHI format
    switch (surfaceFormat.format) {
        case vk::Format::eB8G8R8A8Srgb:
        case vk::Format::eR8G8B8A8Srgb:
            m_format = TextureFormat::RGBA8_SRGB;
            break;
        case vk::Format::eB8G8R8A8Unorm:
        case vk::Format::eR8G8B8A8Unorm:
            m_format = TextureFormat::RGBA8_UNorm;
            break;
        default:
            // Default to SRGB for unknown formats
            m_format = TextureFormat::RGBA8_SRGB;
            LogWarning("Unknown swapchain format, defaulting to RGBA8_SRGB");
            break;
    }
    
    LogInfo("Swapchain created: {}x{}, {} images", m_width, m_height, m_images.size());
}

void VulkanSwapChain::CreateImageViews() {
    m_imageViews.resize(m_images.size());
    
    for (size_t i = 0; i < m_images.size(); i++) {
        vk::ImageViewCreateInfo createInfo;
        createInfo.image = m_images[i];
        createInfo.viewType = vk::ImageViewType::e2D;
        createInfo.format = m_vkFormat;
        createInfo.components.r = vk::ComponentSwizzle::eIdentity;
        createInfo.components.g = vk::ComponentSwizzle::eIdentity;
        createInfo.components.b = vk::ComponentSwizzle::eIdentity;
        createInfo.components.a = vk::ComponentSwizzle::eIdentity;
        createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;
        
        m_imageViews[i] = m_device.GetVulkanDevice().createImageViewUnique(createInfo);
    }
    
    LogInfo("Created {} swapchain image views", m_imageViews.size());
}

void VulkanSwapChain::CleanupSwapChain() {
    m_imageViews.clear();
    m_swapChain.reset();
    m_images.clear();
}

VulkanSwapChain::SwapChainSupportDetails VulkanSwapChain::QuerySwapChainSupport() {
    SwapChainSupportDetails details;
    
    auto physicalDevice = m_device.GetPhysicalDevice();
    auto surface = m_device.GetSurface();
    
    details.capabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);
    details.formats = physicalDevice.getSurfaceFormatsKHR(surface);
    details.presentModes = physicalDevice.getSurfacePresentModesKHR(surface);
    
    return details;
}

vk::SurfaceFormatKHR VulkanSwapChain::ChooseSwapSurfaceFormat(
    const std::vector<vk::SurfaceFormatKHR>& availableFormats) {
    
    if (availableFormats.empty()) {
        throw std::runtime_error("Failed to query surface formats: no formats available for the selected device and surface");
    }
    
    // Prefer SRGB format
    for (const auto& format : availableFormats) {
        if (format.format == vk::Format::eB8G8R8A8Srgb &&
            format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            return format;
        }
    }
    
    // Fallback to first available format
    return availableFormats[0];
}

vk::PresentModeKHR VulkanSwapChain::ChooseSwapPresentMode(
    const std::vector<vk::PresentModeKHR>& availablePresentModes) {
    
    // Prefer mailbox mode for triple buffering
    for (const auto& mode : availablePresentModes) {
        if (mode == vk::PresentModeKHR::eMailbox) {
            return mode;
        }
    }
    
    // FIFO is always available (vsync)
    return vk::PresentModeKHR::eFifo;
}

vk::Extent2D VulkanSwapChain::ChooseSwapExtent(
    const vk::SurfaceCapabilitiesKHR& capabilities) {
    
    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    } else {
        // Use current dimensions or fallback to minimum if not initialized
        uint32_t width = (m_width > 0) ? m_width : capabilities.minImageExtent.width;
        uint32_t height = (m_height > 0) ? m_height : capabilities.minImageExtent.height;
        
        vk::Extent2D actualExtent = {width, height};
        
        actualExtent.width = std::clamp(actualExtent.width,
                                       capabilities.minImageExtent.width,
                                       capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height,
                                        capabilities.minImageExtent.height,
                                        capabilities.maxImageExtent.height);
        
        return actualExtent;
    }
}

uint32_t VulkanSwapChain::AcquireNextImage(uint64_t timeout,
                                           Semaphore* semaphore,
                                           Fence* fence) {
    vk::Semaphore vkSemaphore;
    vk::Fence vkFence;
    
    if (semaphore) {
        auto* vulkanSemaphore = static_cast<VulkanSemaphore*>(semaphore);
        vkSemaphore = vulkanSemaphore->GetVulkanSemaphore();
    }
    
    if (fence) {
        vkFence = *static_cast<vk::Fence*>(fence->GetNativeHandle());
    }
    
    try {
        auto result = m_device.GetVulkanDevice().acquireNextImageKHR(
            m_swapChain.get(),
            timeout,
            vkSemaphore,
            vkFence
        );
        
        m_currentImageIndex = result.value;
        return m_currentImageIndex;
        
    } catch (const vk::OutOfDateKHRError&) {
        // Swapchain is out of date, needs to be recreated
        LogWarning("Swapchain out of date during acquire");
        return UINT32_MAX;
    } catch (const vk::SystemError& e) {
        LogError("Failed to acquire next swapchain image: {}", e.what());
        return UINT32_MAX;
    }
}

void VulkanSwapChain::Present(Semaphore* const* waitSemaphores,
                             uint32_t waitSemaphoreCount) {
    std::vector<vk::Semaphore> vkWaitSemaphores;
    
    if (waitSemaphores && waitSemaphoreCount > 0) {
        vkWaitSemaphores.reserve(waitSemaphoreCount);
        for (uint32_t i = 0; i < waitSemaphoreCount; i++) {
            auto* vulkanSemaphore = static_cast<VulkanSemaphore*>(waitSemaphores[i]);
            vkWaitSemaphores.push_back(vulkanSemaphore->GetVulkanSemaphore());
        }
    }
    
    vk::PresentInfoKHR presentInfo;
    presentInfo.waitSemaphoreCount = static_cast<uint32_t>(vkWaitSemaphores.size());
    presentInfo.pWaitSemaphores = vkWaitSemaphores.data();
    
    vk::SwapchainKHR swapChains[] = {m_swapChain.get()};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &m_currentImageIndex;
    presentInfo.pResults = nullptr;
    
    try {
        auto result = m_device.GetPresentQueue().presentKHR(presentInfo);
        
        if (result == vk::Result::eSuboptimalKHR) {
            LogWarning("Swapchain suboptimal during present");
        }
        
    } catch (const vk::OutOfDateKHRError&) {
        LogWarning("Swapchain out of date during present");
    } catch (const vk::SystemError& e) {
        LogError("Failed to present swapchain image: {}", e.what());
    }
}

void VulkanSwapChain::Resize(uint32_t width, uint32_t height) {
    m_width = width;
    m_height = height;
    
    // Wait for device to be idle before recreating swapchain
    m_device.WaitIdle();
    
    // Cleanup old swapchain
    CleanupSwapChain();
    
    // Recreate swapchain
    try {
        CreateSwapChain();
        CreateImageViews();
        LogInfo("Swapchain resized to {}x{}", width, height);
    } catch (const std::exception& e) {
        LogError("Failed to resize swapchain: {}", e.what());
    }
}

Texture* VulkanSwapChain::GetImage(uint32_t index) {
    if (index >= m_images.size()) {
        return nullptr;
    }
    
    // Note: Returning nullptr for now as creating VulkanTexture wrappers for swapchain images
    // requires additional implementation. Swapchain images are primarily accessed via
    // GetCurrentImageIndex() in command buffer recording, so this limitation doesn't
    // affect basic rendering functionality.
    // TODO: Implement VulkanTexture wrappers for swapchain images when needed for advanced use cases
    return nullptr;
}

} // namespace VRHI
