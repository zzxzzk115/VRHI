// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#pragma once

#include <VRHI/VRHI.hpp>
#include <VRHI/Backend.hpp>
#include <memory>

namespace VRHI {

/// Concrete Device implementation that wraps a backend
class DeviceImpl : public Device {
public:
    explicit DeviceImpl(std::unique_ptr<IBackend> backend, const DeviceConfig& config);
    ~DeviceImpl() override = default;
    
    // Device Information
    BackendType GetBackendType() const noexcept override;
    BackendInfo GetBackendInfo() const override;
    const FeatureSet& GetFeatures() const noexcept override;
    bool IsFeatureSupported(Feature feature) const noexcept override;
    const DeviceProperties& GetProperties() const noexcept override;
    
    // Resource Creation (stubs for now)
    std::expected<std::unique_ptr<Buffer>, Error>
    CreateBuffer(const struct BufferDesc& desc) override;
    
    std::expected<std::unique_ptr<Texture>, Error>
    CreateTexture(const struct TextureDesc& desc) override;
    
    std::expected<std::unique_ptr<Sampler>, Error>
    CreateSampler(const struct SamplerDesc& desc) override;
    
    std::expected<std::unique_ptr<Shader>, Error>
    CreateShader(const struct ShaderDesc& desc) override;
    
    std::expected<std::unique_ptr<Pipeline>, Error>
    CreatePipeline(const struct PipelineDesc& desc) override;
    
    std::expected<std::unique_ptr<RenderPass>, Error>
    CreateRenderPass(const struct RenderPassDesc& desc) override;
    
    std::expected<std::unique_ptr<Framebuffer>, Error>
    CreateFramebuffer(const struct FramebufferDesc& desc) override;
    
    // Command Execution (stubs for now)
    std::unique_ptr<CommandBuffer> CreateCommandBuffer() override;
    void Submit(std::unique_ptr<CommandBuffer> cmd) override;
    void Submit(std::span<std::unique_ptr<CommandBuffer>> cmds) override;
    void WaitIdle() override;
    
    // Synchronization (stubs for now)
    std::unique_ptr<Fence> CreateFence(bool signaled = false) override;
    std::unique_ptr<Semaphore> CreateSemaphore() override;
    void Flush() override;
    
    // Swap Chain (stubs for now)
    SwapChain* GetSwapChain() noexcept override;
    void Present() override;
    void Resize(uint32_t width, uint32_t height) override;
    
private:
    std::unique_ptr<IBackend> m_backend;
    DeviceConfig m_config;
    FeatureSet m_features;
    DeviceProperties m_properties;
};

} // namespace VRHI
