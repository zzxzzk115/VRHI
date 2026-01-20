// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#pragma once

#include <cstdint>
#include <cstddef>
#include <expected>
#include <memory>

namespace VRHI {

// Forward declarations
class CommandBuffer;
struct Error;

// ============================================================================
// Buffer
// ============================================================================

enum class BufferUsage : uint32_t {
    Vertex       = 1 << 0,  // Vertex buffer
    Index        = 1 << 1,  // Index buffer
    Uniform      = 1 << 2,  // Uniform buffer
    Storage      = 1 << 3,  // Storage buffer (SSBO)
    Indirect     = 1 << 4,  // Indirect draw buffer
    TransferSrc  = 1 << 5,  // Transfer source
    TransferDst  = 1 << 6,  // Transfer destination
};

// Bitwise operators for BufferUsage
constexpr BufferUsage operator|(BufferUsage lhs, BufferUsage rhs) noexcept {
    return static_cast<BufferUsage>(
        static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs)
    );
}

constexpr BufferUsage operator&(BufferUsage lhs, BufferUsage rhs) noexcept {
    return static_cast<BufferUsage>(
        static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs)
    );
}

constexpr BufferUsage& operator|=(BufferUsage& lhs, BufferUsage rhs) noexcept {
    lhs = lhs | rhs;
    return lhs;
}

constexpr BufferUsage& operator&=(BufferUsage& lhs, BufferUsage rhs) noexcept {
    lhs = lhs & rhs;
    return lhs;
}

enum class MemoryAccess {
    GpuOnly,      // GPU only access
    CpuToGpu,     // CPU write, GPU read
    GpuToCpu,     // GPU write, CPU read
    CpuOnly,      // CPU only access
};

struct BufferDesc {
    size_t size = 0;
    BufferUsage usage = BufferUsage::Vertex;
    MemoryAccess memoryAccess = MemoryAccess::GpuOnly;
    const void* initialData = nullptr;
    const char* debugName = nullptr;
};

class Buffer {
public:
    virtual ~Buffer() = default;
    
    // Buffer cannot be copied, only moved
    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;
    Buffer(Buffer&&) noexcept = default;
    Buffer& operator=(Buffer&&) noexcept = default;
    
    /// Get buffer size in bytes
    virtual size_t GetSize() const noexcept = 0;
    
    /// Get buffer usage flags
    virtual BufferUsage GetUsage() const noexcept = 0;
    
    /// Map buffer memory for CPU access
    /// @return Pointer to mapped memory
    virtual void* Map() = 0;
    
    /// Map buffer memory range for CPU access
    /// @param offset Offset in bytes
    /// @param size Size in bytes
    /// @return Pointer to mapped memory
    virtual void* Map(size_t offset, size_t size) = 0;
    
    /// Unmap buffer memory
    virtual void Unmap() = 0;
    
    /// Update buffer data
    /// @param data Source data
    /// @param size Size in bytes
    /// @param offset Offset in bytes
    virtual void Update(const void* data, size_t size, size_t offset = 0) = 0;
    
    /// Read buffer data (only supported for CpuToGpu or GpuToCpu)
    /// @param data Destination buffer
    /// @param size Size in bytes
    /// @param offset Offset in bytes
    virtual void Read(void* data, size_t size, size_t offset = 0) = 0;
    
protected:
    Buffer() = default;
};

// ============================================================================
// Texture
// ============================================================================

enum class TextureType {
    Texture1D,
    Texture2D,
    Texture3D,
    TextureCube,
    Texture1DArray,
    Texture2DArray,
    TextureCubeArray,
};

enum class TextureFormat {
    // 8-bit formats
    R8_UNorm,
    RG8_UNorm,
    RGBA8_UNorm,
    RGBA8_SRGB,
    
    // 16-bit formats
    R16_Float,
    RG16_Float,
    RGBA16_Float,
    
    // 32-bit formats
    R32_Float,
    RG32_Float,
    RGB32_Float,
    RGBA32_Float,
    
    R32_UInt,
    RG32_UInt,
    RGB32_UInt,
    RGBA32_UInt,
    
    // Depth/Stencil
    Depth16,
    Depth24Stencil8,
    Depth32F,
    Depth32FStencil8,
    
    // Compressed formats
    BC1_UNorm,      // DXT1
    BC3_UNorm,      // DXT5
    BC7_UNorm,      // BC7
    ETC2_RGB8,      // ETC2
    ASTC_4x4,       // ASTC
};

enum class TextureUsage : uint32_t {
    Sampled          = 1 << 0,  // Shader sampling
    Storage          = 1 << 1,  // Storage image
    RenderTarget     = 1 << 2,  // Render target
    DepthStencil     = 1 << 3,  // Depth/stencil
    TransferSrc      = 1 << 4,  // Transfer source
    TransferDst      = 1 << 5,  // Transfer destination
};

// Bitwise operators for TextureUsage
constexpr TextureUsage operator|(TextureUsage lhs, TextureUsage rhs) noexcept {
    return static_cast<TextureUsage>(
        static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs)
    );
}

constexpr TextureUsage operator&(TextureUsage lhs, TextureUsage rhs) noexcept {
    return static_cast<TextureUsage>(
        static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs)
    );
}

constexpr TextureUsage& operator|=(TextureUsage& lhs, TextureUsage rhs) noexcept {
    lhs = lhs | rhs;
    return lhs;
}

constexpr TextureUsage& operator&=(TextureUsage& lhs, TextureUsage rhs) noexcept {
    lhs = lhs & rhs;
    return lhs;
}

struct TextureDesc {
    TextureType type = TextureType::Texture2D;
    TextureFormat format = TextureFormat::RGBA8_UNorm;
    TextureUsage usage = TextureUsage::Sampled;
    
    uint32_t width = 1;
    uint32_t height = 1;
    uint32_t depth = 1;
    uint32_t mipLevels = 1;
    uint32_t arrayLayers = 1;
    uint32_t sampleCount = 1;  // MSAA sample count
    
    const void* initialData = nullptr;
    const char* debugName = nullptr;
};

class Texture {
public:
    virtual ~Texture() = default;
    
    // Texture cannot be copied, only moved
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    Texture(Texture&&) noexcept = default;
    Texture& operator=(Texture&&) noexcept = default;
    
    /// Get texture type
    virtual TextureType GetType() const noexcept = 0;
    
    /// Get texture format
    virtual TextureFormat GetFormat() const noexcept = 0;
    
    /// Get texture width
    virtual uint32_t GetWidth() const noexcept = 0;
    
    /// Get texture height
    virtual uint32_t GetHeight() const noexcept = 0;
    
    /// Get texture depth
    virtual uint32_t GetDepth() const noexcept = 0;
    
    /// Get mip level count
    virtual uint32_t GetMipLevels() const noexcept = 0;
    
    /// Get array layer count
    virtual uint32_t GetArrayLayers() const noexcept = 0;
    
    /// Update texture data
    virtual void Update(const void* data, size_t size,
                       uint32_t mipLevel = 0, uint32_t arrayLayer = 0) = 0;
    
    /// Update texture region
    virtual void UpdateRegion(const void* data,
                             uint32_t x, uint32_t y, uint32_t z,
                             uint32_t width, uint32_t height, uint32_t depth,
                             uint32_t mipLevel = 0, uint32_t arrayLayer = 0) = 0;
    
    /// Generate mipmaps
    virtual void GenerateMipmaps(CommandBuffer* cmd) = 0;
    
    /// Read texture data
    virtual void Read(void* data, size_t size,
                     uint32_t mipLevel = 0, uint32_t arrayLayer = 0) = 0;
    
protected:
    Texture() = default;
};

// ============================================================================
// Sampler
// ============================================================================

enum class FilterMode {
    Nearest,
    Linear,
};

enum class AddressMode {
    Repeat,
    MirroredRepeat,
    ClampToEdge,
    ClampToBorder,
};

enum class CompareOp {
    Never,
    Less,
    Equal,
    LessOrEqual,
    Greater,
    NotEqual,
    GreaterOrEqual,
    Always,
};

struct SamplerDesc {
    FilterMode minFilter = FilterMode::Linear;
    FilterMode magFilter = FilterMode::Linear;
    FilterMode mipmapMode = FilterMode::Linear;
    
    AddressMode addressModeU = AddressMode::Repeat;
    AddressMode addressModeV = AddressMode::Repeat;
    AddressMode addressModeW = AddressMode::Repeat;
    
    float mipLodBias = 0.0f;
    float minLod = 0.0f;
    float maxLod = 1000.0f;
    
    bool anisotropyEnable = false;
    float maxAnisotropy = 1.0f;
    
    bool compareEnable = false;
    CompareOp compareOp = CompareOp::Never;
    
    float borderColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    
    const char* debugName = nullptr;
};

class Sampler {
public:
    virtual ~Sampler() = default;
    
    // Sampler cannot be copied, only moved
    Sampler(const Sampler&) = delete;
    Sampler& operator=(const Sampler&) = delete;
    Sampler(Sampler&&) noexcept = default;
    Sampler& operator=(Sampler&&) noexcept = default;
    
protected:
    Sampler() = default;
};

} // namespace VRHI
