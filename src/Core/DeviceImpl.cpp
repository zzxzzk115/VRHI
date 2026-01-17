// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#include "DeviceImpl.hpp"
#include <VRHI/Logging.hpp>
#include <VRHI/BackendScoring.hpp>
#include <VRHI/Resources.hpp>
#include <VRHI/Shader.hpp>
#include <VRHI/Pipeline.hpp>
#include <VRHI/RenderPass.hpp>
#include <VRHI/CommandBuffer.hpp>
#include <VRHI/Sync.hpp>

namespace VRHI {

DeviceImpl::DeviceImpl(std::unique_ptr<IBackend> backend, const DeviceConfig& config)
    : m_backend(std::move(backend))
    , m_config(config)
{
    // Get features from backend
    m_features = m_backend->GetSupportedFeatures();
    
    // Initialize device properties (placeholder values for now)
    m_properties.deviceName = std::string(m_backend->GetName());
    m_properties.vendorName = "Unknown";
    m_properties.vendorId = 0;
    m_properties.deviceId = 0;
    m_properties.driverVersion = std::string(m_backend->GetVersion().string);
    m_properties.apiVersion = std::string(m_backend->GetVersion().string);
    m_properties.totalMemory = 0;
    m_properties.availableMemory = 0;
    m_properties.computeUnits = 0;
    m_properties.maxThreadsPerGroup = 0;
}

BackendType DeviceImpl::GetBackendType() const noexcept {
    return m_backend->GetType();
}

BackendInfo DeviceImpl::GetBackendInfo() const {
    BackendInfo info{};
    info.type = m_backend->GetType();
    info.name = std::string(m_backend->GetName());
    
    auto version = m_backend->GetVersion();
    info.version = std::string(version.string);
    
    info.features = m_features;
    info.score = m_backend->CalculateScore(m_config.features);
    
    info.deviceName = m_properties.deviceName;
    info.vendorName = m_properties.vendorName;
    info.driverVersion = m_properties.driverVersion;
    
    return info;
}

const FeatureSet& DeviceImpl::GetFeatures() const noexcept {
    return m_features;
}

bool DeviceImpl::IsFeatureSupported(Feature feature) const noexcept {
    return m_backend->IsFeatureSupported(feature);
}

const DeviceProperties& DeviceImpl::GetProperties() const noexcept {
    return m_properties;
}

// Resource creation stubs
std::expected<std::unique_ptr<Buffer>, Error>
DeviceImpl::CreateBuffer(const struct BufferDesc& desc) {
    Error error{};
    error.code = Error::Code::UnsupportedFeature;
    error.message = "Buffer creation not yet implemented";
    return std::unexpected(error);
}

std::expected<std::unique_ptr<Texture>, Error>
DeviceImpl::CreateTexture(const struct TextureDesc& desc) {
    Error error{};
    error.code = Error::Code::UnsupportedFeature;
    error.message = "Texture creation not yet implemented";
    return std::unexpected(error);
}

std::expected<std::unique_ptr<Sampler>, Error>
DeviceImpl::CreateSampler(const struct SamplerDesc& desc) {
    Error error{};
    error.code = Error::Code::UnsupportedFeature;
    error.message = "Sampler creation not yet implemented";
    return std::unexpected(error);
}

std::expected<std::unique_ptr<Shader>, Error>
DeviceImpl::CreateShader(const struct ShaderDesc& desc) {
    Error error{};
    error.code = Error::Code::UnsupportedFeature;
    error.message = "Shader creation not yet implemented";
    return std::unexpected(error);
}

std::expected<std::unique_ptr<Pipeline>, Error>
DeviceImpl::CreatePipeline(const struct PipelineDesc& desc) {
    Error error{};
    error.code = Error::Code::UnsupportedFeature;
    error.message = "Pipeline creation not yet implemented";
    return std::unexpected(error);
}

std::expected<std::unique_ptr<RenderPass>, Error>
DeviceImpl::CreateRenderPass(const struct RenderPassDesc& desc) {
    Error error{};
    error.code = Error::Code::UnsupportedFeature;
    error.message = "RenderPass creation not yet implemented";
    return std::unexpected(error);
}

std::expected<std::unique_ptr<Framebuffer>, Error>
DeviceImpl::CreateFramebuffer(const struct FramebufferDesc& desc) {
    Error error{};
    error.code = Error::Code::UnsupportedFeature;
    error.message = "Framebuffer creation not yet implemented";
    return std::unexpected(error);
}

// Command execution stubs
std::unique_ptr<CommandBuffer> DeviceImpl::CreateCommandBuffer() {
    return nullptr;
}

void DeviceImpl::Submit(std::unique_ptr<CommandBuffer> cmd) {
    // Not implemented
}

void DeviceImpl::Submit(std::span<std::unique_ptr<CommandBuffer>> cmds) {
    // Not implemented
}

void DeviceImpl::WaitIdle() {
    // Not implemented
}

// Synchronization stubs
std::unique_ptr<Fence> DeviceImpl::CreateFence(bool signaled) {
    return nullptr;
}

std::unique_ptr<Semaphore> DeviceImpl::CreateSemaphore() {
    return nullptr;
}

void DeviceImpl::Flush() {
    // Not implemented
}

// Swap chain stubs
SwapChain* DeviceImpl::GetSwapChain() noexcept {
    return nullptr;
}

void DeviceImpl::Present() {
    // Not implemented
}

void DeviceImpl::Resize(uint32_t width, uint32_t height) {
    // Not implemented
}

} // namespace VRHI
