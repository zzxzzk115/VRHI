// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#include "VulkanDevice.hpp"
#include "VulkanBackend.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanTexture.hpp"
#include "VulkanSampler.hpp"
#include "VulkanShader.hpp"
#include "VulkanPipeline.hpp"
#include "VulkanRenderPass.hpp"
#include "VulkanFramebuffer.hpp"
#include "VulkanCommandBuffer.hpp"
#include "VulkanSync.hpp"
#include "VulkanSwapChain.hpp"
#include <VRHI/Logging.hpp>
#include <VRHI/BackendScoring.hpp>
#include <set>

// Forward declare GLFW types and function
struct GLFWwindow;
extern "C" {
    VkResult glfwCreateWindowSurface(VkInstance instance, GLFWwindow* window,
                                     const VkAllocationCallbacks* allocator,
                                     VkSurfaceKHR* surface);
}

namespace VRHI {

VulkanDevice::VulkanDevice(const DeviceConfig& config, VulkanBackend* backend)
    : m_config(config)
    , m_backend(backend)
{
#ifdef VRHI_ENABLE_VALIDATION
    m_enableValidationLayers = true;
#else
    m_enableValidationLayers = false;
#endif
}

VulkanDevice::~VulkanDevice() {
    if (m_initialized && m_device) {
        WaitIdle();
    }
}

std::expected<void, Error> VulkanDevice::Initialize() {
    if (m_initialized) {
        return {};
    }
    
    try {
        CreateInstance();
        CreateSurface();
        SetupDebugMessenger();
        PickPhysicalDevice();
        CreateLogicalDevice();
        DetectDeviceFeatures();
        
        // Create swapchain if we have a surface
        if (m_surface) {
            m_swapChain = VulkanSwapChain::Create(*this);
            if (!m_swapChain) {
                LogWarning("Failed to create swapchain");
            }
        }
        
        m_initialized = true;
        LogInfo("Vulkan device initialized successfully");
        return {};
        
    } catch (const vk::SystemError& e) {
        return std::unexpected(Error{
            Error::Code::InitializationFailed,
            std::string("Vulkan initialization failed: ") + e.what()
        });
    } catch (const std::exception& e) {
        return std::unexpected(Error{
            Error::Code::InitializationFailed,
            std::string("Vulkan initialization failed: ") + e.what()
        });
    }
}

void VulkanDevice::CreateInstance() {
    // Application info
    vk::ApplicationInfo appInfo;
    appInfo.pApplicationName = "VRHI Application";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "VRHI";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;
    
    // Get required extensions
    std::vector<const char*> extensions;
    
    // Add surface extensions if we have a window handle
    if (m_config.windowHandle) {
        extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
        
        // Add platform-specific surface extensions
#if defined(_WIN32)
        extensions.push_back("VK_KHR_win32_surface");
#elif defined(__APPLE__)
        extensions.push_back("VK_EXT_metal_surface");
#elif defined(__linux__)
        extensions.push_back("VK_KHR_xcb_surface");
        extensions.push_back("VK_KHR_xlib_surface");
        extensions.push_back("VK_KHR_wayland_surface");
#endif
    }
    
    // Add validation layer extensions if enabled
    if (m_enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
    
    // Instance create info
    vk::InstanceCreateInfo createInfo;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();
    
    // Enable validation layers if requested
    if (m_enableValidationLayers) {
        // Check if validation layers are available
        auto availableLayers = vk::enumerateInstanceLayerProperties();
        bool validationLayersSupported = false;
        
        for (const auto& layerName : s_validationLayers) {
            for (const auto& layerProps : availableLayers) {
                if (std::strcmp(layerName, layerProps.layerName) == 0) {
                    validationLayersSupported = true;
                    break;
                }
            }
        }
        
        if (validationLayersSupported) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(std::size(s_validationLayers));
            createInfo.ppEnabledLayerNames = s_validationLayers;
            LogInfo("Vulkan validation layers enabled");
        } else {
            LogWarning("Vulkan validation layers requested but not available");
            m_enableValidationLayers = false;
        }
    }
    
    m_instance = vk::createInstanceUnique(createInfo);
    LogInfo("Vulkan instance created");
}

void VulkanDevice::CreateSurface() {
    // Skip surface creation if no window handle is provided
    if (!m_config.windowHandle) {
        LogInfo("No window handle provided, skipping surface creation");
        return;
    }
    
    VkSurfaceKHR surface;
    GLFWwindow* window = static_cast<GLFWwindow*>(m_config.windowHandle);
    VkResult result = glfwCreateWindowSurface(
        static_cast<VkInstance>(m_instance.get()),
        window,
        nullptr,
        &surface
    );
    
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create window surface");
    }
    
    m_surface = vk::UniqueSurfaceKHR(surface, m_instance.get());
    LogInfo("Vulkan surface created");
}

void VulkanDevice::SetupDebugMessenger() {
    if (!m_enableValidationLayers) {
        return;
    }
    
    // TODO: Setup debug messenger
    // The pfnUserCallback type mismatch between Vk and vk:: types makes this tricky
    // For now, skip this and rely on validation layers printing to stdout
    LogInfo("Vulkan validation layers enabled (debug messenger skipped for now)");
}

void VulkanDevice::PickPhysicalDevice() {
    auto devices = m_instance->enumeratePhysicalDevices();
    
    if (devices.empty()) {
        throw std::runtime_error("Failed to find GPUs with Vulkan support");
    }
    
    // Helper lambda to check if a device is suitable
    auto isDeviceSuitable = [this](const vk::PhysicalDevice& device) -> bool {
        // If we have a surface, check if the device supports presentation
        if (m_surface) {
            auto queueFamilies = device.getQueueFamilyProperties();
            bool hasGraphicsQueue = false;
            bool hasPresentQueue = false;
            
            for (uint32_t i = 0; i < queueFamilies.size(); i++) {
                if (queueFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics) {
                    hasGraphicsQueue = true;
                }
                if (device.getSurfaceSupportKHR(i, m_surface.get())) {
                    hasPresentQueue = true;
                }
                
                // Early exit if we found both
                if (hasGraphicsQueue && hasPresentQueue) {
                    break;
                }
            }
            
            return hasGraphicsQueue && hasPresentQueue;
        }
        return true; // No surface requirements
    };
    
    // For now, pick the first discrete GPU that meets requirements, or fallback to first suitable device
    vk::PhysicalDevice suitableDevice;
    for (const auto& device : devices) {
        if (!isDeviceSuitable(device)) {
            continue;
        }
        
        auto props = device.getProperties();
        if (props.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
            m_physicalDevice = device;
            break;
        }
        
        // Remember first suitable device as fallback
        if (!suitableDevice) {
            suitableDevice = device;
        }
    }
    
    if (!m_physicalDevice) {
        m_physicalDevice = suitableDevice;
    }
    
    if (!m_physicalDevice) {
        throw std::runtime_error("Failed to find a suitable GPU");
    }
    
    auto props = m_physicalDevice.getProperties();
    m_properties.deviceName = std::string(props.deviceName.data());
    m_properties.vendorName = "Unknown";  // Map vendor ID to name
    m_properties.driverVersion = std::to_string(props.driverVersion);
    m_properties.apiVersion = "Vulkan " + std::to_string(VK_VERSION_MAJOR(props.apiVersion)) + 
                             "." + std::to_string(VK_VERSION_MINOR(props.apiVersion)) +
                             "." + std::to_string(VK_VERSION_PATCH(props.apiVersion));
    
    LogInfo("Selected Vulkan device: {}", m_properties.deviceName);
}

void VulkanDevice::CreateLogicalDevice() {
    // Find queue families
    auto queueFamilies = m_physicalDevice.getQueueFamilyProperties();
    
    // Find graphics and present queue families
    bool foundGraphics = false;
    bool foundPresent = false;
    
    for (uint32_t i = 0; i < queueFamilies.size(); i++) {
        if (queueFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics) {
            m_graphicsQueueFamily = i;
            foundGraphics = true;
        }
        
        // Check for present support if we have a surface
        if (m_surface && m_physicalDevice.getSurfaceSupportKHR(i, m_surface.get())) {
            m_presentQueueFamily = i;
            foundPresent = true;
        }
        
        // If we found both and they're the same, we're done
        if (foundGraphics && (!m_surface || foundPresent)) {
            if (!m_surface || m_graphicsQueueFamily == m_presentQueueFamily) {
                break;
            }
        }
    }
    
    // If we have a surface but didn't find present queue, verify graphics queue supports presentation
    if (m_surface && !foundPresent) {
        if (m_physicalDevice.getSurfaceSupportKHR(m_graphicsQueueFamily, m_surface.get())) {
            m_presentQueueFamily = m_graphicsQueueFamily;
            LogInfo("Using graphics queue for presentation");
        } else {
            throw std::runtime_error("No queue family supports presentation on this surface");
        }
    }
    
    // Queue create info
    float queuePriority = 1.0f;
    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {m_graphicsQueueFamily};
    
    if (m_surface) {
        uniqueQueueFamilies.insert(m_presentQueueFamily);
    }
    
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        vk::DeviceQueueCreateInfo queueCreateInfo;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }
    
    // Device features
    vk::PhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.geometryShader = VK_TRUE;
    deviceFeatures.tessellationShader = VK_TRUE;
    deviceFeatures.samplerAnisotropy = VK_TRUE;
    
    // Device extensions
    std::vector<const char*> deviceExtensions;
    
    // Add swapchain extension if we have a surface
    if (m_surface) {
        deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    }
    
    // Device create info
    vk::DeviceCreateInfo createInfo;
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();
    createInfo.pEnabledFeatures = &deviceFeatures;
    
    m_device = m_physicalDevice.createDeviceUnique(createInfo);
    m_graphicsQueue = m_device->getQueue(m_graphicsQueueFamily, 0);
    
    if (m_surface) {
        m_presentQueue = m_device->getQueue(m_presentQueueFamily, 0);
    }
    
    LogInfo("Vulkan logical device created");
}

void VulkanDevice::DetectDeviceFeatures() {
    auto physicalFeatures = m_physicalDevice.getFeatures();
    auto props = m_physicalDevice.getProperties();
    
    // Core shader stages
    m_features.core.vertexShader = true;
    m_features.core.fragmentShader = true;
    m_features.core.geometryShader = physicalFeatures.geometryShader;
    m_features.core.tessellationShader = physicalFeatures.tessellationShader;
    m_features.core.computeShader = true;  // Vulkan 1.0 core
    m_features.core.meshShader = false;  // Requires extension check
    
    // Buffer features
    m_features.core.vertexBuffers = true;
    m_features.core.indexBuffers = true;
    m_features.core.uniformBuffers = true;
    m_features.core.storageBuffers = true;
    m_features.core.indirectBuffers = true;
    
    // Drawing features
    m_features.core.instancing = true;
    m_features.core.multiDrawIndirect = physicalFeatures.multiDrawIndirect;
    
    // Texture features
    m_features.texture.texture1D = true;
    m_features.texture.texture2D = true;
    m_features.texture.texture3D = true;
    m_features.texture.textureCube = true;
    m_features.texture.texture2DArray = true;
    m_features.texture.floatTextures = true;
    m_features.texture.depthTextures = true;
    m_features.texture.compressedTextures = true;
    
    // Compression formats
    m_features.texture.dxt = false;  // Check BC extension
    m_features.texture.etc2 = true;  // Vulkan core
    m_features.texture.astc = false;  // Check ASTC extension
    
    // Rendering features
    m_features.rendering.multipleRenderTargets = true;
    m_features.rendering.independentBlend = physicalFeatures.independentBlend;
    m_features.rendering.depthClamp = physicalFeatures.depthClamp;
    m_features.rendering.multisample = true;
    m_features.rendering.maxColorAttachments = props.limits.maxColorAttachments;
    m_features.rendering.maxSamples = static_cast<uint32_t>(props.limits.framebufferColorSampleCounts);
    
    // Sampling features
    m_features.texture.anisotropicFiltering = physicalFeatures.samplerAnisotropy;
    
    // Advanced features
    m_features.advanced.rayTracing = false;  // Check ray tracing extension
    m_features.advanced.asyncCompute = true;
    m_features.advanced.bindlessResources = false;  // Check descriptor indexing
    
    LogInfo("Vulkan device features detected");
}

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDevice::DebugCallbackVk(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        LogWarning("Vulkan validation: {}", pCallbackData->pMessage);
    } else {
        LogDebug("Vulkan validation: {}", pCallbackData->pMessage);
    }
    return VK_FALSE;
}

BackendType VulkanDevice::GetBackendType() const noexcept {
    return BackendType::Vulkan;
}

BackendInfo VulkanDevice::GetBackendInfo() const {
    BackendInfo info;
    info.type = BackendType::Vulkan;
    info.name = "Vulkan";
    info.version = "1.3";
    info.deviceName = m_properties.deviceName;
    info.vendorName = m_properties.vendorName;
    info.driverVersion = m_properties.driverVersion;
    return info;
}

const FeatureSet& VulkanDevice::GetFeatures() const noexcept {
    return m_features;
}

bool VulkanDevice::IsFeatureSupported(Feature feature) const noexcept {
    return m_backend->IsFeatureSupported(feature);
}

const DeviceProperties& VulkanDevice::GetProperties() const noexcept {
    return m_properties;
}

std::expected<std::unique_ptr<Buffer>, Error>
VulkanDevice::CreateBuffer(const BufferDesc& desc) {
    return VulkanBuffer::Create(*this, desc);
}

std::expected<std::unique_ptr<Texture>, Error>
VulkanDevice::CreateTexture(const TextureDesc& desc) {
    return VulkanTexture::Create(*this, desc);
}

std::expected<std::unique_ptr<Sampler>, Error>
VulkanDevice::CreateSampler(const SamplerDesc& desc) {
    return VulkanSampler::Create(*this, desc);
}

std::expected<std::unique_ptr<Shader>, Error>
VulkanDevice::CreateShader(const ShaderDesc& desc) {
    return VulkanShader::Create(*this, desc);
}

std::expected<std::unique_ptr<Pipeline>, Error>
VulkanDevice::CreatePipeline(const PipelineDesc& desc) {
    return VulkanPipeline::Create(*this, desc);
}

std::expected<std::unique_ptr<RenderPass>, Error>
VulkanDevice::CreateRenderPass(const RenderPassDesc& desc) {
    return VulkanRenderPass::Create(*this, desc);
}

std::expected<std::unique_ptr<Framebuffer>, Error>
VulkanDevice::CreateFramebuffer(const FramebufferDesc& desc) {
    return VulkanFramebuffer::Create(*this, desc);
}

std::unique_ptr<CommandBuffer>
VulkanDevice::CreateCommandBuffer() {
    return VulkanCommandBuffer::Create(*this);
}

std::unique_ptr<Fence>
VulkanDevice::CreateFence(bool signaled) {
    return VulkanFence::Create(*this, signaled);
}

std::unique_ptr<Semaphore>
VulkanDevice::CreateSemaphore() {
    return VulkanSemaphore::Create(*this);
}

void VulkanDevice::Submit(std::span<std::unique_ptr<CommandBuffer>> cmds) {
    // TODO: Implement batch command buffer submission
    LogWarning("VulkanDevice::Submit(span) not yet implemented");
}

void VulkanDevice::Flush() {
    // TODO: Implement command queue flush
    LogWarning("VulkanDevice::Flush not yet implemented");
}

SwapChain* VulkanDevice::GetSwapChain() noexcept {
    return m_swapChain.get();
}

void VulkanDevice::Resize(uint32_t width, uint32_t height) {
    if (m_swapChain) {
        m_swapChain->Resize(width, height);
    }
}

void VulkanDevice::WaitIdle() {
    if (m_device) {
        m_device->waitIdle();
    }
}

void VulkanDevice::Submit(std::unique_ptr<CommandBuffer> cmd) {
    // TODO: Implement command buffer submission
    LogWarning("VulkanDevice::Submit not yet implemented");
}

void VulkanDevice::Present() {
    if (m_swapChain) {
        m_swapChain->Present(nullptr, 0);
    }
}

} // namespace VRHI
