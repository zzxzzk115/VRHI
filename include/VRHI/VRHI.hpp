// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#pragma once

#include <cstdint>
#include <expected>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace VRHI {

// Forward declarations
class Device;
class Buffer;
class Texture;
class Sampler;
class Shader;
class Pipeline;
class CommandBuffer;
class RenderPass;
class Framebuffer;
class Fence;
class Semaphore;
class SwapChain;

// ============================================================================
// Enumerations
// ============================================================================

enum class BackendType {
    // Modern APIs
    Vulkan,
    Direct3D12,    // Planned: Windows high-performance backend
    Metal,         // Planned: macOS/iOS high-performance backend
    
    // OpenGL Desktop
    OpenGL46,
    OpenGL41,
    OpenGL33,
    
    // OpenGL ES Mobile
    OpenGLES31,
    OpenGLES30,
    
    // Future/Experimental
    WebGPU,        // Planned: Web and cross-platform backend
    
    Auto,          // Automatically select the best backend
};

enum class Feature {
    // Core features
    Compute,
    Geometry,
    Tessellation,
    
    // Texture features
    Texture1D,
    Texture2D,
    Texture3D,
    TextureCube,
    Texture2DArray,
    TextureCompression,
    AnisotropicFiltering,
    
    // Advanced features
    MultiDrawIndirect,
    RayTracing,
    MeshShading,
    AsyncCompute,
    BindlessResources,
    
    // Debug features
    DebugMarkers,
    GPUValidation,
};

enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error,
    Off,
};

// ============================================================================
// Error Handling
// ============================================================================

struct Error {
    enum class Code {
        Success = 0,
        InvalidConfig,
        NoCompatibleBackend,
        InitializationFailed,
        DeviceLost,
        OutOfMemory,
        ValidationError,
        CompilationError,
        UnsupportedFeature,
    };
    
    Code code = Code::Success;
    std::string message;
    
    constexpr operator bool() const noexcept { return code == Code::Success; }
};

// ============================================================================
// Feature Detection
// ============================================================================

struct FeatureRequirements {
    std::vector<Feature> required;
    std::vector<Feature> optional;
};

struct FeatureSet {
    // Core features
    struct {
        bool vertexShader = false;
        bool fragmentShader = false;
        bool geometryShader = false;
        bool tessellationShader = false;
        bool computeShader = false;
        bool meshShader = false;
        
        bool uniformBuffers = false;
        bool storageBuffers = false;
        bool vertexBuffers = false;
        bool indexBuffers = false;
        bool indirectBuffers = false;
        
        bool instancing = false;
        bool multiDrawIndirect = false;
    } core;
    
    // Texture features
    struct {
        bool texture1D = false;
        bool texture2D = false;
        bool texture3D = false;
        bool textureCube = false;
        bool texture2DArray = false;
        
        bool floatTextures = false;
        bool depthTextures = false;
        bool compressedTextures = false;
        
        bool dxt = false;
        bool etc2 = false;
        bool astc = false;
        
        bool anisotropicFiltering = false;
        
        uint32_t maxTextureSize = 0;
        uint32_t max3DTextureSize = 0;
        uint32_t maxArrayLayers = 0;
        float maxAnisotropy = 0.0f;
    } texture;
    
    // Rendering features
    struct {
        bool multipleRenderTargets = false;
        uint32_t maxColorAttachments = 0;
        bool independentBlend = false;
        
        bool depthClamp = false;
        bool multisample = false;
        uint32_t maxSamples = 0;
    } rendering;
    
    // Compute features
    struct {
        bool computeShader = false;
        
        uint32_t maxWorkGroupSizeX = 0;
        uint32_t maxWorkGroupSizeY = 0;
        uint32_t maxWorkGroupSizeZ = 0;
        uint32_t maxWorkGroupInvocations = 0;
        
        uint32_t maxComputeSharedMemorySize = 0;
    } compute;
    
    // Advanced features
    struct {
        bool rayTracing = false;
        bool meshShading = false;
        bool variableRateShading = false;
        bool bindlessResources = false;
        bool asyncCompute = false;
    } advanced;
    
    // Memory features
    struct {
        uint64_t deviceLocalMemory = 0;
        uint64_t hostVisibleMemory = 0;
        
        uint32_t minUniformBufferAlignment = 0;
        uint32_t minStorageBufferAlignment = 0;
        
        bool unifiedMemory = false;
    } memory;
};

// ============================================================================
// Backend Information
// ============================================================================

struct BackendInfo {
    BackendType type = BackendType::Auto;
    std::string name;
    std::string version;
    
    float score = 0.0f;
    struct ScoreDetail {
        std::string category;
        float score = 0.0f;
        float weight = 0.0f;
    };
    std::vector<ScoreDetail> scoreDetails;
    
    std::string deviceName;
    std::string vendorName;
    std::string driverVersion;
    
    FeatureSet features;
};

struct DeviceProperties {
    std::string deviceName;
    std::string vendorName;
    uint32_t vendorId = 0;
    uint32_t deviceId = 0;
    
    std::string driverVersion;
    std::string apiVersion;
    
    uint64_t totalMemory = 0;
    uint64_t availableMemory = 0;
    
    uint32_t computeUnits = 0;
    uint32_t maxThreadsPerGroup = 0;
};

// ============================================================================
// Configuration
// ============================================================================

struct DeviceConfig {
    BackendType preferredBackend = BackendType::Auto;
    FeatureRequirements features;
    
    bool enableValidation = false;
    bool enableDebugMarkers = false;
    
    void* windowHandle = nullptr;
    uint32_t width = 1280;
    uint32_t height = 720;
    
    bool vsync = true;
    uint32_t backBufferCount = 2;
    
    LogLevel logLevel = LogLevel::Info;
};

// ============================================================================
// Version
// ============================================================================

struct Version {
    uint32_t major;
    uint32_t minor;
    uint32_t patch;
    std::string_view string;
};

// ============================================================================
// Global Functions
// ============================================================================

/// Create a rendering device
/// @param config Device configuration
/// @return Device instance on success, error on failure
std::expected<std::unique_ptr<Device>, Error> 
CreateDevice(const DeviceConfig& config = {});

/// Enumerate all available backends
/// @return List of available backend information
std::vector<BackendInfo> EnumerateBackends();

/// Get VRHI version
/// @return Version information
Version GetVersion() noexcept;

/// Set global log level
/// @param level Log level
void SetLogLevel(LogLevel level) noexcept;

// ============================================================================
// Device Interface
// ============================================================================

class Device {
public:
    virtual ~Device() = default;
    
    // Device cannot be copied, only moved
    Device(const Device&) = delete;
    Device& operator=(const Device&) = delete;
    Device(Device&&) noexcept = default;
    Device& operator=(Device&&) noexcept = default;
    
    // ========================================================================
    // Device Information
    // ========================================================================
    
    /// Get backend type
    virtual BackendType GetBackendType() const noexcept = 0;
    
    /// Get backend information
    virtual BackendInfo GetBackendInfo() const = 0;
    
    /// Get supported features
    virtual const FeatureSet& GetFeatures() const noexcept = 0;
    
    /// Check if a feature is supported
    virtual bool IsFeatureSupported(Feature feature) const noexcept = 0;
    
    /// Get device properties
    virtual const DeviceProperties& GetProperties() const noexcept = 0;
    
    // ========================================================================
    // Resource Creation
    // ========================================================================
    
    /// Create a buffer
    virtual std::expected<std::unique_ptr<Buffer>, Error>
    CreateBuffer(const struct BufferDesc& desc) = 0;
    
    /// Create a texture
    virtual std::expected<std::unique_ptr<Texture>, Error>
    CreateTexture(const struct TextureDesc& desc) = 0;
    
    /// Create a sampler
    virtual std::expected<std::unique_ptr<Sampler>, Error>
    CreateSampler(const struct SamplerDesc& desc) = 0;
    
    /// Create a shader
    virtual std::expected<std::unique_ptr<Shader>, Error>
    CreateShader(const struct ShaderDesc& desc) = 0;
    
    /// Create a graphics pipeline
    virtual std::expected<std::unique_ptr<Pipeline>, Error>
    CreatePipeline(const struct PipelineDesc& desc) = 0;
    
    /// Create a render pass
    virtual std::expected<std::unique_ptr<RenderPass>, Error>
    CreateRenderPass(const struct RenderPassDesc& desc) = 0;
    
    /// Create a framebuffer
    virtual std::expected<std::unique_ptr<Framebuffer>, Error>
    CreateFramebuffer(const struct FramebufferDesc& desc) = 0;
    
    // ========================================================================
    // Command Execution
    // ========================================================================
    
    /// Create a command buffer
    virtual std::unique_ptr<CommandBuffer> CreateCommandBuffer() = 0;
    
    /// Submit a command buffer
    virtual void Submit(std::unique_ptr<CommandBuffer> cmd) = 0;
    
    /// Submit multiple command buffers
    virtual void Submit(std::span<std::unique_ptr<CommandBuffer>> cmds) = 0;
    
    /// Wait for device to be idle
    virtual void WaitIdle() = 0;
    
    // ========================================================================
    // Synchronization
    // ========================================================================
    
    /// Create a fence
    virtual std::unique_ptr<Fence> CreateFence(bool signaled = false) = 0;
    
    /// Create a semaphore
    virtual std::unique_ptr<Semaphore> CreateSemaphore() = 0;
    
    /// Flush pending commands
    virtual void Flush() = 0;
    
    // ========================================================================
    // Swap Chain
    // ========================================================================
    
    /// Get swap chain
    virtual SwapChain* GetSwapChain() noexcept = 0;
    
    /// Present the current frame
    virtual void Present() = 0;
    
    /// Resize swap chain
    virtual void Resize(uint32_t width, uint32_t height) = 0;
    
protected:
    Device() = default;
};

} // namespace VRHI
