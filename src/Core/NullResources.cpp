// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#include "NullResources.hpp"
#include <VRHI/Logging.hpp>
#include <algorithm>

namespace VRHI {

// ============================================================================
// NullBuffer Implementation
// ============================================================================

NullBuffer::NullBuffer(const BufferDesc& desc)
    : m_desc(desc)
    , m_data(desc.size)
{
    // Copy initial data if provided
    if (desc.initialData) {
        std::memcpy(m_data.data(), desc.initialData, desc.size);
    }
}

std::expected<std::unique_ptr<Buffer>, Error>
NullBuffer::Create(const BufferDesc& desc) {
    // Validate descriptor
    if (desc.size == 0) {
        return std::unexpected(Error{
            Error::Code::InvalidConfig,
            "Buffer size must be greater than 0"
        });
    }
    
    try {
        auto buffer = std::unique_ptr<Buffer>(new NullBuffer(desc));
        return buffer;
    } catch (const std::bad_alloc&) {
        return std::unexpected(Error{
            Error::Code::OutOfMemory,
            "Failed to allocate buffer memory"
        });
    }
}

size_t NullBuffer::GetSize() const noexcept {
    return m_desc.size;
}

BufferUsage NullBuffer::GetUsage() const noexcept {
    return m_desc.usage;
}

void* NullBuffer::Map() {
    return Map(0, m_desc.size);
}

void* NullBuffer::Map(size_t offset, size_t size) {
    if (m_mappedPtr != nullptr) {
        LogWarning("NullBuffer: Buffer already mapped");
        return m_mappedPtr;
    }
    
    if (offset + size > m_desc.size) {
        LogError("NullBuffer: Map range out of bounds");
        return nullptr;
    }
    
    m_mappedPtr = m_data.data() + offset;
    return m_mappedPtr;
}

void NullBuffer::Unmap() {
    if (m_mappedPtr == nullptr) {
        LogWarning("NullBuffer: Buffer not mapped");
        return;
    }
    
    m_mappedPtr = nullptr;
}

void NullBuffer::Update(const void* data, size_t size, size_t offset) {
    if (offset + size > m_desc.size) {
        LogError("NullBuffer: Update out of bounds");
        return;
    }
    
    std::memcpy(m_data.data() + offset, data, size);
}

void NullBuffer::Read(void* data, size_t size, size_t offset) {
    if (offset + size > m_desc.size) {
        LogError("NullBuffer: Read out of bounds");
        return;
    }
    
    std::memcpy(data, m_data.data() + offset, size);
}

// ============================================================================
// NullTexture Implementation
// ============================================================================

namespace {
    // Helper to calculate texture data size
    size_t CalculateTextureSize(const TextureDesc& desc) {
        // Simplified calculation - in a real implementation, this would
        // account for format, mip levels, alignment, etc.
        size_t bytesPerPixel = 4;  // Assume RGBA8 for simplicity
        
        size_t totalSize = 0;
        for (uint32_t mip = 0; mip < desc.mipLevels; ++mip) {
            uint32_t mipWidth = std::max(1u, desc.width >> mip);
            uint32_t mipHeight = std::max(1u, desc.height >> mip);
            uint32_t mipDepth = std::max(1u, desc.depth >> mip);
            
            totalSize += mipWidth * mipHeight * mipDepth * bytesPerPixel;
        }
        
        return totalSize * desc.arrayLayers;
    }
}

NullTexture::NullTexture(const TextureDesc& desc)
    : m_desc(desc)
{
    size_t dataSize = CalculateTextureSize(desc);
    m_data.resize(dataSize);
    
    // Copy initial data if provided
    if (desc.initialData) {
        std::memcpy(m_data.data(), desc.initialData, 
                   std::min(dataSize, static_cast<size_t>(desc.width * desc.height * desc.depth * 4)));
    }
}

std::expected<std::unique_ptr<Texture>, Error>
NullTexture::Create(const TextureDesc& desc) {
    // Validate descriptor
    if (desc.width == 0 || desc.height == 0 || desc.depth == 0) {
        return std::unexpected(Error{
            Error::Code::InvalidConfig,
            "Texture dimensions must be greater than 0"
        });
    }
    
    if (desc.mipLevels == 0) {
        return std::unexpected(Error{
            Error::Code::InvalidConfig,
            "Texture mip levels must be at least 1"
        });
    }
    
    if (desc.arrayLayers == 0) {
        return std::unexpected(Error{
            Error::Code::InvalidConfig,
            "Texture array layers must be at least 1"
        });
    }
    
    try {
        auto texture = std::unique_ptr<Texture>(new NullTexture(desc));
        return texture;
    } catch (const std::bad_alloc&) {
        return std::unexpected(Error{
            Error::Code::OutOfMemory,
            "Failed to allocate texture memory"
        });
    }
}

TextureType NullTexture::GetType() const noexcept {
    return m_desc.type;
}

TextureFormat NullTexture::GetFormat() const noexcept {
    return m_desc.format;
}

uint32_t NullTexture::GetWidth() const noexcept {
    return m_desc.width;
}

uint32_t NullTexture::GetHeight() const noexcept {
    return m_desc.height;
}

uint32_t NullTexture::GetDepth() const noexcept {
    return m_desc.depth;
}

uint32_t NullTexture::GetMipLevels() const noexcept {
    return m_desc.mipLevels;
}

uint32_t NullTexture::GetArrayLayers() const noexcept {
    return m_desc.arrayLayers;
}

void NullTexture::Update(const void* data, size_t size,
                         uint32_t mipLevel, uint32_t arrayLayer) {
    if (mipLevel >= m_desc.mipLevels) {
        LogError("NullTexture: Mip level out of range");
        return;
    }
    
    if (arrayLayer >= m_desc.arrayLayers) {
        LogError("NullTexture: Array layer out of range");
        return;
    }
    
    // Simplified - just copy to the beginning for demonstration
    size_t copySize = std::min(size, m_data.size());
    std::memcpy(m_data.data(), data, copySize);
}

void NullTexture::UpdateRegion(const void* data,
                               uint32_t x, uint32_t y, uint32_t z,
                               uint32_t width, uint32_t height, uint32_t depth,
                               uint32_t mipLevel, uint32_t arrayLayer) {
    if (mipLevel >= m_desc.mipLevels) {
        LogError("NullTexture: Mip level out of range");
        return;
    }
    
    if (arrayLayer >= m_desc.arrayLayers) {
        LogError("NullTexture: Array layer out of range");
        return;
    }
    
    if (x + width > m_desc.width || y + height > m_desc.height || z + depth > m_desc.depth) {
        LogError("NullTexture: Update region out of bounds");
        return;
    }
    
    // Simplified implementation for demonstration
    LogDebug("NullTexture: UpdateRegion called (demonstration only)");
}

void NullTexture::GenerateMipmaps(CommandBuffer* cmd) {
    // Demonstration implementation - would generate mipmap chain in real backend
    LogDebug("NullTexture: GenerateMipmaps called (demonstration only)");
}

void NullTexture::Read(void* data, size_t size,
                      uint32_t mipLevel, uint32_t arrayLayer) {
    if (mipLevel >= m_desc.mipLevels) {
        LogError("NullTexture: Mip level out of range");
        return;
    }
    
    if (arrayLayer >= m_desc.arrayLayers) {
        LogError("NullTexture: Array layer out of range");
        return;
    }
    
    // Simplified - just copy from the beginning for demonstration
    size_t copySize = std::min(size, m_data.size());
    std::memcpy(data, m_data.data(), copySize);
}

// ============================================================================
// NullSampler Implementation
// ============================================================================

NullSampler::NullSampler(const SamplerDesc& desc)
    : m_desc(desc)
{
    // Nothing to initialize for demonstration
}

std::expected<std::unique_ptr<Sampler>, Error>
NullSampler::Create(const SamplerDesc& desc) {
    // Validate descriptor (basic validation)
    if (desc.maxAnisotropy < 1.0f) {
        return std::unexpected(Error{
            Error::Code::InvalidConfig,
            "Sampler max anisotropy must be at least 1.0"
        });
    }
    
    if (desc.minLod > desc.maxLod) {
        return std::unexpected(Error{
            Error::Code::InvalidConfig,
            "Sampler minLod cannot be greater than maxLod"
        });
    }
    
    try {
        auto sampler = std::unique_ptr<Sampler>(new NullSampler(desc));
        return sampler;
    } catch (const std::bad_alloc&) {
        return std::unexpected(Error{
            Error::Code::OutOfMemory,
            "Failed to allocate sampler"
        });
    }
}

} // namespace VRHI
