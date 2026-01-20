// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#pragma once

#include <VRHI/VRHI.hpp>
#include <vulkan/vulkan.hpp>

namespace VRHI {

class VulkanBackend;

/// Vulkan device implementation
class VulkanDevice : public Device {
public:
    VulkanDevice(const DeviceConfig& config, VulkanBackend* backend);
    ~VulkanDevice() override;
    
    // Device initialization
    std::expected<void, Error> Initialize();
    
    // Device information
    BackendType GetBackendType() const noexcept override;
    BackendInfo GetBackendInfo() const override;
    const FeatureSet& GetFeatures() const noexcept override;
    bool IsFeatureSupported(Feature feature) const noexcept override;
    const DeviceProperties& GetProperties() const noexcept override;
    
    // Resource creation
    std::expected<std::unique_ptr<Buffer>, Error>
    CreateBuffer(const BufferDesc& desc) override;
    
    std::expected<std::unique_ptr<Texture>, Error>
    CreateTexture(const TextureDesc& desc) override;
    
    std::expected<std::unique_ptr<Sampler>, Error>
    CreateSampler(const SamplerDesc& desc) override;
    
    std::expected<std::unique_ptr<Shader>, Error>
    CreateShader(const ShaderDesc& desc) override;
    
    std::expected<std::unique_ptr<Pipeline>, Error>
    CreatePipeline(const PipelineDesc& desc) override;
    
    std::expected<std::unique_ptr<RenderPass>, Error>
    CreateRenderPass(const RenderPassDesc& desc) override;
    
    std::expected<std::unique_ptr<Framebuffer>, Error>
    CreateFramebuffer(const FramebufferDesc& desc) override;
    
    // Command execution
    std::unique_ptr<CommandBuffer> CreateCommandBuffer() override;
    void Submit(std::unique_ptr<CommandBuffer> cmd) override;
    void Submit(std::span<std::unique_ptr<CommandBuffer>> cmds) override;
    void WaitIdle() override;
    
    // Synchronization
    std::unique_ptr<Fence> CreateFence(bool signaled = false) override;
    std::unique_ptr<Semaphore> CreateSemaphore() override;
    void Flush() override;
    
    // Swap chain
    SwapChain* GetSwapChain() noexcept override;
    void Present() override;
    void Resize(uint32_t width, uint32_t height) override;
    
    // Vulkan-specific accessors
    vk::Instance GetVulkanInstance() const noexcept { return m_instance.get(); }
    vk::PhysicalDevice GetPhysicalDevice() const noexcept { return m_physicalDevice; }
    vk::Device GetVulkanDevice() const noexcept { return m_device.get(); }
    vk::Queue GetGraphicsQueue() const noexcept { return m_graphicsQueue; }
    vk::Queue GetPresentQueue() const noexcept { return m_presentQueue; }
    uint32_t GetGraphicsQueueFamily() const noexcept { return m_graphicsQueueFamily; }
    uint32_t GetPresentQueueFamily() const noexcept { return m_presentQueueFamily; }
    vk::SurfaceKHR GetSurface() const noexcept { return m_surface.get(); }
    vk::CommandPool GetCommandPool() const noexcept { return m_commandPool.get(); }
    const DeviceConfig& GetConfig() const noexcept { return m_config; }
    
private:
    void CreateInstance();
    void CreateSurface();
    void SetupDebugMessenger();
    void PickPhysicalDevice();
    void CreateLogicalDevice();
    void CreateCommandPool();
    void DetectDeviceFeatures();
    
    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallbackVk(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);
    
    DeviceConfig m_config;
    VulkanBackend* m_backend;
    FeatureSet m_features;
    DeviceProperties m_properties;
    bool m_initialized = false;
    
    // Vulkan handles
    vk::UniqueInstance m_instance;
    vk::UniqueDebugUtilsMessengerEXT m_debugMessenger;
    vk::UniqueSurfaceKHR m_surface;
    vk::PhysicalDevice m_physicalDevice;
    vk::UniqueDevice m_device;
    vk::Queue m_graphicsQueue;
    vk::Queue m_presentQueue;
    uint32_t m_graphicsQueueFamily = 0;
    uint32_t m_presentQueueFamily = 0;
    
    // Swap chain
    std::unique_ptr<SwapChain> m_swapChain;
    
    // Command pool
    vk::UniqueCommandPool m_commandPool;
    
    // Validation layers
    static constexpr const char* s_validationLayers[] = {
        "VK_LAYER_KHRONOS_validation"
    };
    bool m_enableValidationLayers = false;
};

} // namespace VRHI
