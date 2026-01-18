// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#pragma once

#include <VRHI/VRHI.hpp>
#include <expected>

namespace VRHI {

class OpenGL33Backend;

/// OpenGL 3.3 device implementation
class OpenGL33Device : public Device {
public:
    OpenGL33Device(const DeviceConfig& config, OpenGL33Backend* backend);
    ~OpenGL33Device() override;
    
    // Initialize the device (create context, etc.)
    std::expected<void, Error> Initialize();
    
    // Device Information
    BackendType GetBackendType() const noexcept override;
    BackendInfo GetBackendInfo() const override;
    const FeatureSet& GetFeatures() const noexcept override;
    bool IsFeatureSupported(Feature feature) const noexcept override;
    const DeviceProperties& GetProperties() const noexcept override;
    
    // Resource Creation
    std::expected<std::unique_ptr<Buffer>, Error>
    CreateBuffer(const BufferDesc& desc) override;
    
    std::expected<std::unique_ptr<Texture>, Error>
    CreateTexture(const TextureDesc& desc) override;
    
    std::expected<std::unique_ptr<Sampler>, Error>
    CreateSampler(const SamplerDesc& desc) override;
    
    std::expected<std::unique_ptr<Shader>, Error>
    CreateShader(const struct ShaderDesc& desc) override;
    
    std::expected<std::unique_ptr<Pipeline>, Error>
    CreatePipeline(const struct PipelineDesc& desc) override;
    
    std::expected<std::unique_ptr<RenderPass>, Error>
    CreateRenderPass(const struct RenderPassDesc& desc) override;
    
    std::expected<std::unique_ptr<Framebuffer>, Error>
    CreateFramebuffer(const struct FramebufferDesc& desc) override;
    
    // Command Execution
    std::unique_ptr<CommandBuffer> CreateCommandBuffer() override;
    void Submit(std::unique_ptr<CommandBuffer> cmd) override;
    void Submit(std::span<std::unique_ptr<CommandBuffer>> cmds) override;
    void WaitIdle() override;
    
    // Synchronization
    std::unique_ptr<Fence> CreateFence(bool signaled = false) override;
    std::unique_ptr<Semaphore> CreateSemaphore() override;
    void Flush() override;
    
    // Swap Chain
    SwapChain* GetSwapChain() noexcept override;
    void Present() override;
    void Resize(uint32_t width, uint32_t height) override;
    
private:
    DeviceConfig m_config;
    OpenGL33Backend* m_backend;
    DeviceProperties m_properties;
    
    // OpenGL context handle (platform-specific, will be void* for now)
    // Reserved for future window system integration (Phase 7-8)
    // Currently, we assume the context is created externally
    [[maybe_unused]] void* m_context = nullptr;
    
    // Default VAO (required for OpenGL 3.3 core profile)
    unsigned int m_defaultVAO = 0;
    
    bool m_initialized = false;
};

} // namespace VRHI
