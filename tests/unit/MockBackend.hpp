// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#pragma once

#include <VRHI/VRHIAll.hpp>
#include <VRHI/BackendScoring.hpp>
#include <map>
#include <cstring>

namespace VRHI {
namespace Mock {

// ============================================================================
// Mock Resource Implementations
// ============================================================================

class MockBuffer : public Buffer {
public:
    explicit MockBuffer(const BufferDesc& desc) 
        : m_desc(desc)
        , m_mappedMemory(desc.size > 0 ? desc.size : 1024) // Use actual size or 1KB minimum
    {}
    
    size_t GetSize() const noexcept override { return m_desc.size; }
    BufferUsage GetUsage() const noexcept override { return m_desc.usage; }
    
    void* Map() override { return m_mappedMemory.data(); }
    void* Map(size_t offset, size_t size) override { 
        return m_mappedMemory.data() + offset; 
    }
    void Unmap() override {}
    
    void Update(const void* data, size_t size, size_t offset) override {
        if (offset + size <= m_mappedMemory.size() && data) {
            std::memcpy(m_mappedMemory.data() + offset, data, size);
        }
    }
    
    void Read(void* data, size_t size, size_t offset) override {
        if (offset + size <= m_mappedMemory.size() && data) {
            std::memcpy(data, m_mappedMemory.data() + offset, size);
        }
    }
    
private:
    BufferDesc m_desc{};
    std::vector<uint8_t> m_mappedMemory;
};

class MockTexture : public Texture {
public:
    explicit MockTexture(const TextureDesc& desc) : m_desc(desc) {}
    
    TextureType GetType() const noexcept override { return m_desc.type; }
    TextureFormat GetFormat() const noexcept override { return m_desc.format; }
    uint32_t GetWidth() const noexcept override { return m_desc.width; }
    uint32_t GetHeight() const noexcept override { return m_desc.height; }
    uint32_t GetDepth() const noexcept override { return m_desc.depth; }
    uint32_t GetMipLevels() const noexcept override { return m_desc.mipLevels; }
    uint32_t GetArrayLayers() const noexcept override { return m_desc.arrayLayers; }
    
    void Update(const void*, size_t, uint32_t, uint32_t) override {}
    void UpdateRegion(const void*, uint32_t, uint32_t, uint32_t,
                     uint32_t, uint32_t, uint32_t, uint32_t, uint32_t) override {}
    void GenerateMipmaps(CommandBuffer*) override {}
    void Read(void*, size_t, uint32_t, uint32_t) override {}
    
private:
    TextureDesc m_desc{};
};

class MockSampler : public Sampler {
public:
    explicit MockSampler(const SamplerDesc& desc) : m_desc(desc) {}
    
private:
    SamplerDesc m_desc{};
};

class MockShader : public Shader {
public:
    explicit MockShader(const ShaderDesc& desc) : m_desc(desc) {}
    
    ShaderStage GetStage() const noexcept override { return m_desc.stage; }
    ShaderLanguage GetLanguage() const noexcept override { return m_desc.language; }
    std::string_view GetEntryPoint() const noexcept override { 
        return m_desc.entryPoint ? m_desc.entryPoint : "main"; 
    }
    
private:
    ShaderDesc m_desc{};
};

class MockPipeline : public Pipeline {
public:
    explicit MockPipeline(const PipelineDesc& desc) : m_type(desc.type) {}
    
    PipelineType GetType() const noexcept override { return m_type; }
    
private:
    PipelineType m_type{};
};

class MockRenderPass : public RenderPass {
public:
    explicit MockRenderPass(const RenderPassDesc&) {}
};

class MockFramebuffer : public Framebuffer {
public:
    explicit MockFramebuffer(const FramebufferDesc& desc) : m_desc(desc) {}
    
    uint32_t GetWidth() const noexcept override { return m_desc.width; }
    uint32_t GetHeight() const noexcept override { return m_desc.height; }
    uint32_t GetLayers() const noexcept override { return m_desc.layers; }
    
private:
    FramebufferDesc m_desc{};
};

class MockFence : public Fence {
public:
    explicit MockFence(bool signaled) : m_signaled(signaled) {}
    
    bool Wait(uint64_t) override { return m_signaled; }
    void Reset() override { m_signaled = false; }
    bool IsSignaled() const noexcept override { return m_signaled; }
    
private:
    bool m_signaled;
};

class MockSemaphore : public Semaphore {
public:
    MockSemaphore() = default;
};

class MockSwapChain : public SwapChain {
public:
    explicit MockSwapChain(const SwapChainDesc& desc) : m_desc(desc) {
        // Create mock images
        TextureDesc texDesc{};
        texDesc.type = TextureType::Texture2D;
        texDesc.format = desc.format;
        texDesc.width = desc.width;
        texDesc.height = desc.height;
        texDesc.usage = TextureUsage::RenderTarget;
        
        for (uint32_t i = 0; i < desc.bufferCount; ++i) {
            m_images.push_back(std::make_unique<MockTexture>(texDesc));
        }
    }
    
    uint32_t AcquireNextImage(uint64_t, Semaphore*, Fence*) override {
        m_currentImage = (m_currentImage + 1) % m_images.size();
        return m_currentImage;
    }
    
    void Present(Semaphore* const*, uint32_t) override {}
    void Resize(uint32_t width, uint32_t height) override {
        m_desc.width = width;
        m_desc.height = height;
    }
    
    uint32_t GetWidth() const noexcept override { return m_desc.width; }
    uint32_t GetHeight() const noexcept override { return m_desc.height; }
    TextureFormat GetFormat() const noexcept override { return m_desc.format; }
    uint32_t GetImageCount() const noexcept override { return static_cast<uint32_t>(m_images.size()); }
    Texture* GetImage(uint32_t index) override { 
        return index < m_images.size() ? m_images[index].get() : nullptr; 
    }
    uint32_t GetCurrentImageIndex() const noexcept override { return m_currentImage; }
    
private:
    SwapChainDesc m_desc{};
    std::vector<std::unique_ptr<MockTexture>> m_images;
    uint32_t m_currentImage = 0;
};

class MockCommandBuffer : public CommandBuffer {
public:
    MockCommandBuffer() = default;
    
    void Begin() override { m_state = CommandBufferState::Recording; }
    void End() override { m_state = CommandBufferState::Executable; }
    void Reset() override { m_state = CommandBufferState::Initial; }
    CommandBufferState GetState() const noexcept override { return m_state; }
    
    void BeginRenderPass(RenderPass*, Framebuffer*, const Rect2D&) override {}
    void EndRenderPass() override {}
    
    void BindPipeline(Pipeline*) override {}
    void BindVertexBuffers(uint32_t, std::span<Buffer* const>, std::span<const uint64_t>) override {}
    void BindIndexBuffer(Buffer*, uint64_t, bool) override {}
    
    void SetViewport(const Viewport&) override {}
    void SetViewports(std::span<const Viewport>) override {}
    void SetScissor(const Rect2D&) override {}
    void SetScissors(std::span<const Rect2D>) override {}
    void SetLineWidth(float) override {}
    void SetBlendConstants(const float[4]) override {}
    void SetDepthBias(float, float, float) override {}
    void SetDepthBounds(float, float) override {}
    void SetStencilCompareMask(bool, uint32_t) override {}
    void SetStencilWriteMask(bool, uint32_t) override {}
    void SetStencilReference(bool, uint32_t) override {}
    
    void Draw(const DrawParams&) override {}
    void Draw(uint32_t, uint32_t, uint32_t, uint32_t) override {}
    void DrawIndexed(const DrawIndexedParams&) override {}
    void DrawIndexed(uint32_t, uint32_t, uint32_t, int32_t, uint32_t) override {}
    void DrawIndirect(Buffer*, uint64_t, uint32_t, uint32_t) override {}
    void DrawIndexedIndirect(Buffer*, uint64_t, uint32_t, uint32_t) override {}
    
    void Dispatch(const DispatchParams&) override {}
    void Dispatch(uint32_t, uint32_t, uint32_t) override {}
    void DispatchIndirect(Buffer*, uint64_t) override {}
    
    void ClearColorAttachment(uint32_t, const ClearColorValue&, const Rect2D&) override {}
    void ClearDepthStencilAttachment(const ClearDepthStencilValue&, const Rect2D&) override {}
    
    void CopyBuffer(Buffer*, Buffer*, uint64_t, uint64_t, uint64_t) override {}
    void CopyBufferToTexture(Buffer*, Texture*, uint32_t, uint32_t) override {}
    void CopyTextureToBuffer(Texture*, Buffer*, uint32_t, uint32_t) override {}
    void CopyTexture(Texture*, Texture*, uint32_t, uint32_t, uint32_t, uint32_t) override {}
    
    void PipelineBarrier() override {}
    
    void BeginDebugMarker(const char*, const float[4]) override {}
    void EndDebugMarker() override {}
    void InsertDebugMarker(const char*, const float[4]) override {}
    
private:
    CommandBufferState m_state = CommandBufferState::Initial;
};

// ============================================================================
// Mock Device Implementation
// ============================================================================

class MockDevice : public Device {
public:
    explicit MockDevice(const DeviceConfig& config) : m_config(config) {
        // Initialize feature set with common features
        m_features.core.vertexShader = true;
        m_features.core.fragmentShader = true;
        m_features.core.uniformBuffers = true;
        m_features.core.vertexBuffers = true;
        m_features.core.indexBuffers = true;
        m_features.texture.texture2D = true;
        m_features.texture.maxTextureSize = 16384;
        m_features.rendering.multipleRenderTargets = true;
        m_features.rendering.maxColorAttachments = 8;
        
        // Initialize properties
        m_properties.deviceName = "Mock Device";
        m_properties.vendorName = "VRHI Mock";
        m_properties.vendorId = 0xFFFF;
        m_properties.deviceId = 0x0001;
        m_properties.driverVersion = "1.0.0";
        m_properties.apiVersion = "1.0.0";
        m_properties.totalMemory = 4ULL * 1024 * 1024 * 1024; // 4GB
        m_properties.availableMemory = 4ULL * 1024 * 1024 * 1024;
        
        // Create swap chain if window handle provided
        if (config.windowHandle) {
            SwapChainDesc swapDesc{};
            swapDesc.windowHandle = config.windowHandle;
            swapDesc.width = config.width;
            swapDesc.height = config.height;
            swapDesc.bufferCount = config.backBufferCount;
            swapDesc.vsync = config.vsync;
            m_swapChain = std::make_unique<MockSwapChain>(swapDesc);
        }
    }
    
    BackendType GetBackendType() const noexcept override { 
        return BackendType::Auto; 
    }
    
    BackendInfo GetBackendInfo() const override {
        BackendInfo info{};
        info.type = BackendType::Auto;
        info.name = "Mock Backend";
        info.version = "1.0.0";
        info.score = 50.0f; // Medium score
        info.deviceName = m_properties.deviceName;
        info.vendorName = m_properties.vendorName;
        info.driverVersion = m_properties.driverVersion;
        info.features = m_features;
        return info;
    }
    
    const FeatureSet& GetFeatures() const noexcept override { 
        return m_features; 
    }
    
    bool IsFeatureSupported(Feature feature) const noexcept override {
        // Mock implementation supports basic features
        switch (feature) {
            case Feature::Texture2D:
            case Feature::TextureCube:
                return true;
            default:
                return false;
        }
    }
    
    const DeviceProperties& GetProperties() const noexcept override { 
        return m_properties; 
    }
    
    std::expected<std::unique_ptr<Buffer>, Error>
    CreateBuffer(const BufferDesc& desc) override {
        return std::make_unique<MockBuffer>(desc);
    }
    
    std::expected<std::unique_ptr<Texture>, Error>
    CreateTexture(const TextureDesc& desc) override {
        return std::make_unique<MockTexture>(desc);
    }
    
    std::expected<std::unique_ptr<Sampler>, Error>
    CreateSampler(const SamplerDesc& desc) override {
        return std::make_unique<MockSampler>(desc);
    }
    
    std::expected<std::unique_ptr<Shader>, Error>
    CreateShader(const ShaderDesc& desc) override {
        return std::make_unique<MockShader>(desc);
    }
    
    std::expected<std::unique_ptr<Pipeline>, Error>
    CreatePipeline(const PipelineDesc& desc) override {
        return std::make_unique<MockPipeline>(desc);
    }
    
    std::expected<std::unique_ptr<RenderPass>, Error>
    CreateRenderPass(const RenderPassDesc& desc) override {
        return std::make_unique<MockRenderPass>(desc);
    }
    
    std::expected<std::unique_ptr<Framebuffer>, Error>
    CreateFramebuffer(const FramebufferDesc& desc) override {
        return std::make_unique<MockFramebuffer>(desc);
    }
    
    std::unique_ptr<CommandBuffer> CreateCommandBuffer() override {
        return std::make_unique<MockCommandBuffer>();
    }
    
    void Submit(std::unique_ptr<CommandBuffer>) override {}
    void Submit(std::span<std::unique_ptr<CommandBuffer>>) override {}
    void WaitIdle() override {}
    
    std::unique_ptr<Fence> CreateFence(bool signaled) override {
        return std::make_unique<MockFence>(signaled);
    }
    
    std::unique_ptr<Semaphore> CreateSemaphore() override {
        return std::make_unique<MockSemaphore>();
    }
    
    void Flush() override {}
    
    SwapChain* GetSwapChain() noexcept override { 
        return m_swapChain.get(); 
    }
    
    void Present() override {
        if (m_swapChain) {
            m_swapChain->Present(nullptr, 0);
        }
    }
    
    void Resize(uint32_t width, uint32_t height) override {
        if (m_swapChain) {
            m_swapChain->Resize(width, height);
        }
    }
    
private:
    DeviceConfig m_config{};
    FeatureSet m_features{};
    DeviceProperties m_properties{};
    std::unique_ptr<MockSwapChain> m_swapChain;
};

// ============================================================================
// Mock Backend Implementation
// ============================================================================

class MockBackend : public IBackend {
public:
    MockBackend() = default;
    
    BackendType GetType() const noexcept override { 
        return BackendType::Auto; 
    }
    
    std::string_view GetName() const noexcept override { 
        return "Mock"; 
    }
    
    Version GetVersion() const noexcept override {
        return {1, 0, 0, "1.0.0"};
    }
    
    FeatureSet GetSupportedFeatures() const override {
        FeatureSet features{};
        features.core.vertexShader = true;
        features.core.fragmentShader = true;
        features.core.uniformBuffers = true;
        features.core.vertexBuffers = true;
        features.core.indexBuffers = true;
        features.texture.texture2D = true;
        features.texture.maxTextureSize = 16384;
        features.rendering.multipleRenderTargets = true;
        features.rendering.maxColorAttachments = 8;
        return features;
    }
    
    bool IsFeatureSupported(Feature feature) const noexcept override {
        switch (feature) {
            case Feature::Texture2D:
            case Feature::TextureCube:
                return true;
            default:
                return false;
        }
    }
    
    float CalculateScore(const FeatureRequirements& requirements) const override {
        auto features = GetSupportedFeatures();
        return BackendScorer::CalculateScore(GetType(), features, requirements);
    }
    
    std::expected<std::unique_ptr<Device>, Error>
    CreateDevice(const DeviceConfig& config) override {
        return std::make_unique<MockDevice>(config);
    }
};

} // namespace Mock
} // namespace VRHI
