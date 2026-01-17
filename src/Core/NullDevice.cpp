// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#include "NullDevice.hpp"
#include <VRHI/Resources.hpp>
#include <VRHI/Shader.hpp>
#include <VRHI/Pipeline.hpp>
#include <VRHI/RenderPass.hpp>
#include <VRHI/CommandBuffer.hpp>
#include <VRHI/Sync.hpp>

namespace VRHI {

NullDevice::NullDevice() {
    // Initialize with empty features
    m_features = FeatureSet{};
    
    // Initialize device properties
    m_properties.deviceName = "Null Device";
    m_properties.vendorName = "VRHI";
    m_properties.vendorId = 0;
    m_properties.deviceId = 0;
    m_properties.driverVersion = "0.0.0";
    m_properties.apiVersion = "0.0.0";
    m_properties.totalMemory = 0;
    m_properties.availableMemory = 0;
    m_properties.computeUnits = 0;
    m_properties.maxThreadsPerGroup = 0;
}

BackendType NullDevice::GetBackendType() const noexcept {
    return BackendType::Auto;
}

BackendInfo NullDevice::GetBackendInfo() const {
    BackendInfo info{};
    info.type = BackendType::Auto;
    info.name = "Null Backend";
    info.version = "0.0.0";
    info.features = m_features;
    info.score = -1.0f;
    info.deviceName = m_properties.deviceName;
    info.vendorName = m_properties.vendorName;
    info.driverVersion = m_properties.driverVersion;
    return info;
}

const FeatureSet& NullDevice::GetFeatures() const noexcept {
    return m_features;
}

bool NullDevice::IsFeatureSupported(Feature feature) const noexcept {
    return false;
}

const DeviceProperties& NullDevice::GetProperties() const noexcept {
    return m_properties;
}

// Resource creation stubs - all return errors
std::expected<std::unique_ptr<Buffer>, Error>
NullDevice::CreateBuffer(const struct BufferDesc& desc) {
    Error error{};
    error.code = Error::Code::UnsupportedFeature;
    error.message = "NullDevice: Buffer creation not supported";
    return std::unexpected(error);
}

std::expected<std::unique_ptr<Texture>, Error>
NullDevice::CreateTexture(const struct TextureDesc& desc) {
    Error error{};
    error.code = Error::Code::UnsupportedFeature;
    error.message = "NullDevice: Texture creation not supported";
    return std::unexpected(error);
}

std::expected<std::unique_ptr<Sampler>, Error>
NullDevice::CreateSampler(const struct SamplerDesc& desc) {
    Error error{};
    error.code = Error::Code::UnsupportedFeature;
    error.message = "NullDevice: Sampler creation not supported";
    return std::unexpected(error);
}

std::expected<std::unique_ptr<Shader>, Error>
NullDevice::CreateShader(const struct ShaderDesc& desc) {
    Error error{};
    error.code = Error::Code::UnsupportedFeature;
    error.message = "NullDevice: Shader creation not supported";
    return std::unexpected(error);
}

std::expected<std::unique_ptr<Pipeline>, Error>
NullDevice::CreatePipeline(const struct PipelineDesc& desc) {
    Error error{};
    error.code = Error::Code::UnsupportedFeature;
    error.message = "NullDevice: Pipeline creation not supported";
    return std::unexpected(error);
}

std::expected<std::unique_ptr<RenderPass>, Error>
NullDevice::CreateRenderPass(const struct RenderPassDesc& desc) {
    Error error{};
    error.code = Error::Code::UnsupportedFeature;
    error.message = "NullDevice: RenderPass creation not supported";
    return std::unexpected(error);
}

std::expected<std::unique_ptr<Framebuffer>, Error>
NullDevice::CreateFramebuffer(const struct FramebufferDesc& desc) {
    Error error{};
    error.code = Error::Code::UnsupportedFeature;
    error.message = "NullDevice: Framebuffer creation not supported";
    return std::unexpected(error);
}

// Command execution stubs
std::unique_ptr<CommandBuffer> NullDevice::CreateCommandBuffer() {
    return nullptr;
}

void NullDevice::Submit(std::unique_ptr<CommandBuffer> cmd) {
    // Do nothing
}

void NullDevice::Submit(std::span<std::unique_ptr<CommandBuffer>> cmds) {
    // Do nothing
}

void NullDevice::WaitIdle() {
    // Do nothing
}

// Synchronization stubs
std::unique_ptr<Fence> NullDevice::CreateFence(bool signaled) {
    return nullptr;
}

std::unique_ptr<Semaphore> NullDevice::CreateSemaphore() {
    return nullptr;
}

void NullDevice::Flush() {
    // Do nothing
}

// Swap chain stubs
SwapChain* NullDevice::GetSwapChain() noexcept {
    return nullptr;
}

void NullDevice::Present() {
    // Do nothing
}

void NullDevice::Resize(uint32_t width, uint32_t height) {
    // Do nothing
}

} // namespace VRHI
