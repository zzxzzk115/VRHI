// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#include "OpenGL33Device.hpp"
#include "OpenGL33Backend.hpp"
#include "OpenGL33Buffer.hpp"
#include "OpenGL33Texture.hpp"
#include "OpenGL33Sampler.hpp"
#include "OpenGL33Shader.hpp"
#include "OpenGL33Pipeline.hpp"
#include "OpenGL33RenderPass.hpp"
#include "OpenGL33Framebuffer.hpp"
#include "OpenGL33CommandBuffer.hpp"
#include "OpenGL33Sync.hpp"
#include "OpenGL33SwapChain.hpp"
#include <VRHI/Logging.hpp>
#include <glad/glad.h>

namespace VRHI {

OpenGL33Device::OpenGL33Device(const DeviceConfig& config, OpenGL33Backend* backend)
    : m_config(config)
    , m_backend(backend)
{
}

OpenGL33Device::~OpenGL33Device() {
    if (m_initialized) {
        WaitIdle();
        
        // Clean up default VAO
        if (m_defaultVAO != 0) {
            glDeleteVertexArrays(1, &m_defaultVAO);
            m_defaultVAO = 0;
        }
        
        // Context cleanup will happen here
        // For now, we're not managing the context ourselves
    }
}

std::expected<void, Error> OpenGL33Device::Initialize() {
    if (m_initialized) {
        return {};
    }
    
    // Note: For a real implementation, we would need a window/context creation system
    // For now, we assume the context is already created externally
    // This is acceptable for the initial implementation since window system
    // abstraction is a separate task (Phase 7-8 in KANBAN)
    
    // Initialize GLAD
    if (!gladLoadGL()) {
        return std::unexpected(Error{
            Error::Code::InitializationFailed,
            "Failed to initialize GLAD for OpenGL 3.3"
        });
    }
    
    // Verify OpenGL version
    GLint majorVersion = 0, minorVersion = 0;
    glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
    glGetIntegerv(GL_MINOR_VERSION, &minorVersion);
    
    if (majorVersion < 3 || (majorVersion == 3 && minorVersion < 3)) {
        return std::unexpected(Error{
            Error::Code::InitializationFailed,
            "OpenGL 3.3 or higher is required"
        });
    }
    
    // Get device properties
    const GLubyte* vendor = glGetString(GL_VENDOR);
    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);
    
    if (vendor) m_properties.vendorName = reinterpret_cast<const char*>(vendor);
    if (renderer) m_properties.deviceName = reinterpret_cast<const char*>(renderer);
    if (version) m_properties.driverVersion = reinterpret_cast<const char*>(version);
    
    m_properties.apiVersion = "OpenGL 3.3";
    
    // Get features from backend
    m_features = m_backend->GetSupportedFeatures();
    
    // Create and bind a default VAO (required for OpenGL 3.3 core profile)
    glGenVertexArrays(1, &m_defaultVAO);
    glBindVertexArray(m_defaultVAO);
    
    m_initialized = true;
    
    LogInfo("OpenGL 3.3 Device initialized");
    if (!m_properties.deviceName.empty()) {
        LogInfo(m_properties.deviceName);
    }
    
    return {};
}

BackendType OpenGL33Device::GetBackendType() const noexcept {
    return BackendType::OpenGL33;
}

BackendInfo OpenGL33Device::GetBackendInfo() const {
    BackendInfo info;
    info.type = BackendType::OpenGL33;
    info.name = "OpenGL 3.3";
    info.version = "3.3";
    info.deviceName = m_properties.deviceName;
    info.vendorName = m_properties.vendorName;
    info.driverVersion = m_properties.driverVersion;
    info.features = m_features;
    return info;
}

const FeatureSet& OpenGL33Device::GetFeatures() const noexcept {
    return m_features;
}

bool OpenGL33Device::IsFeatureSupported(Feature feature) const noexcept {
    return m_backend->IsFeatureSupported(feature);
}

const DeviceProperties& OpenGL33Device::GetProperties() const noexcept {
    return m_properties;
}

std::expected<std::unique_ptr<Buffer>, Error>
OpenGL33Device::CreateBuffer(const BufferDesc& desc) {
    return OpenGL33Buffer::Create(desc);
}

std::expected<std::unique_ptr<Texture>, Error>
OpenGL33Device::CreateTexture(const TextureDesc& desc) {
    return OpenGL33Texture::Create(desc);
}

std::expected<std::unique_ptr<Sampler>, Error>
OpenGL33Device::CreateSampler(const SamplerDesc& desc) {
    return OpenGL33Sampler::Create(desc);
}

std::expected<std::unique_ptr<Shader>, Error>
OpenGL33Device::CreateShader(const ShaderDesc& desc) {
    return OpenGL33Shader::Create(desc);
}

std::expected<std::unique_ptr<Pipeline>, Error>
OpenGL33Device::CreatePipeline(const PipelineDesc& desc) {
    return OpenGL33Pipeline::Create(desc);
}

std::expected<std::unique_ptr<RenderPass>, Error>
OpenGL33Device::CreateRenderPass(const RenderPassDesc& desc) {
    return OpenGL33RenderPass::Create(desc);
}

std::expected<std::unique_ptr<Framebuffer>, Error>
OpenGL33Device::CreateFramebuffer(const FramebufferDesc& desc) {
    return OpenGL33Framebuffer::Create(desc);
}

std::unique_ptr<CommandBuffer> OpenGL33Device::CreateCommandBuffer() {
    return std::make_unique<OpenGL33CommandBuffer>();
}

void OpenGL33Device::Submit(std::unique_ptr<CommandBuffer> cmd) {
    // In OpenGL, commands are typically executed immediately
    // The command buffer pattern is less relevant, but we maintain it for API consistency
    auto* glCmd = static_cast<OpenGL33CommandBuffer*>(cmd.get());
    glCmd->Execute();
}

void OpenGL33Device::Submit(std::span<std::unique_ptr<CommandBuffer>> cmds) {
    for (auto& cmd : cmds) {
        Submit(std::move(cmd));
    }
}

void OpenGL33Device::WaitIdle() {
    // In OpenGL, glFinish waits for all commands to complete
    glFinish();
}

std::unique_ptr<Fence> OpenGL33Device::CreateFence(bool signaled) {
    return std::make_unique<OpenGL33Fence>(signaled);
}

std::unique_ptr<Semaphore> OpenGL33Device::CreateSemaphore() {
    return std::make_unique<OpenGL33Semaphore>();
}

void OpenGL33Device::Flush() {
    // glFlush suggests the GPU should execute pending commands
    glFlush();
}

SwapChain* OpenGL33Device::GetSwapChain() noexcept {
    // Swap chain management would be handled by window system abstraction
    // For now, return nullptr
    return nullptr;
}

void OpenGL33Device::Present() {
    // Present would be handled by the swap chain/window system
    // For now, we just flush
    glFlush();
}

void OpenGL33Device::Resize(uint32_t width, uint32_t height) {
    // Resize would be handled by the swap chain/window system
    m_config.width = width;
    m_config.height = height;
}

} // namespace VRHI
