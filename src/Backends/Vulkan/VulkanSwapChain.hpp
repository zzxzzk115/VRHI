// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#pragma once

#include <VRHI/Sync.hpp>
#include <vulkan/vulkan.hpp>
#include <vector>

namespace VRHI {

class VulkanDevice;

class VulkanSwapChain : public SwapChain {
public:
    ~VulkanSwapChain() override;
    
    static std::unique_ptr<SwapChain>
    Create(VulkanDevice& device);
    
    // SwapChain interface
    uint32_t AcquireNextImage(uint64_t timeout = UINT64_MAX,
                             Semaphore* semaphore = nullptr,
                             Fence* fence = nullptr) override;
    
    void Present(Semaphore* const* waitSemaphores = nullptr,
                uint32_t waitSemaphoreCount = 0) override;
    
    void Resize(uint32_t width, uint32_t height) override;
    
    uint32_t GetWidth() const noexcept override { return m_width; }
    uint32_t GetHeight() const noexcept override { return m_height; }
    TextureFormat GetFormat() const noexcept override { return m_format; }
    uint32_t GetImageCount() const noexcept override { return static_cast<uint32_t>(m_images.size()); }
    Texture* GetImage(uint32_t index) override;
    uint32_t GetCurrentImageIndex() const noexcept override { return m_currentImageIndex; }
    
    void* GetNativeHandle() const noexcept override { return (void*)m_swapChain.get(); }
    
    // Vulkan-specific accessors
    vk::SwapchainKHR GetVulkanSwapChain() const noexcept { return m_swapChain.get(); }
    vk::Format GetVulkanFormat() const noexcept { return m_vkFormat; }
    vk::Extent2D GetExtent() const noexcept { return m_extent; }
    
private:
    VulkanSwapChain(VulkanDevice& device);
    
    void CreateSwapChain();
    void CreateImageViews();
    void CleanupSwapChain();
    
    struct SwapChainSupportDetails {
        vk::SurfaceCapabilitiesKHR capabilities;
        std::vector<vk::SurfaceFormatKHR> formats;
        std::vector<vk::PresentModeKHR> presentModes;
    };
    
    SwapChainSupportDetails QuerySwapChainSupport();
    vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
    vk::PresentModeKHR ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);
    vk::Extent2D ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities);
    
    VulkanDevice& m_device;
    vk::UniqueSwapchainKHR m_swapChain;
    std::vector<vk::Image> m_images;
    std::vector<vk::UniqueImageView> m_imageViews;
    
    vk::Format m_vkFormat;
    vk::Extent2D m_extent;
    uint32_t m_width = 0;
    uint32_t m_height = 0;
    TextureFormat m_format = TextureFormat::RGBA8_SRGB;
    uint32_t m_currentImageIndex = 0;
};

} // namespace VRHI
