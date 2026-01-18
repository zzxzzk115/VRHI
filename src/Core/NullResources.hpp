// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#pragma once

#include <VRHI/Resources.hpp>
#include <VRHI/VRHI.hpp>
#include <cstring>
#include <memory>
#include <vector>

namespace VRHI {

// ============================================================================
// NullBuffer - Demonstration of backend-agnostic buffer implementation
// ============================================================================

/// Null buffer implementation - stores data in CPU memory for demonstration
/// This shows how a backend would implement the Buffer interface
class NullBuffer : public Buffer {
public:
    ~NullBuffer() override = default;
    
    static std::expected<std::unique_ptr<Buffer>, Error>
    Create(const BufferDesc& desc);
    
    // Buffer interface implementation
    size_t GetSize() const noexcept override;
    BufferUsage GetUsage() const noexcept override;
    
    void* Map() override;
    void* Map(size_t offset, size_t size) override;
    void Unmap() override;
    
    void Update(const void* data, size_t size, size_t offset = 0) override;
    void Read(void* data, size_t size, size_t offset = 0) override;
    
private:
    NullBuffer(const BufferDesc& desc);
    
    BufferDesc m_desc;
    std::vector<uint8_t> m_data;  // CPU-side storage
    void* m_mappedPtr = nullptr;
};

// ============================================================================
// NullTexture - Demonstration of backend-agnostic texture implementation
// ============================================================================

/// Null texture implementation - stores data in CPU memory for demonstration
/// This shows how a backend would implement the Texture interface
class NullTexture : public Texture {
public:
    ~NullTexture() override = default;
    
    static std::expected<std::unique_ptr<Texture>, Error>
    Create(const TextureDesc& desc);
    
    // Texture interface implementation
    TextureType GetType() const noexcept override;
    TextureFormat GetFormat() const noexcept override;
    uint32_t GetWidth() const noexcept override;
    uint32_t GetHeight() const noexcept override;
    uint32_t GetDepth() const noexcept override;
    uint32_t GetMipLevels() const noexcept override;
    uint32_t GetArrayLayers() const noexcept override;
    
    void Update(const void* data, size_t size,
               uint32_t mipLevel = 0, uint32_t arrayLayer = 0) override;
    
    void UpdateRegion(const void* data,
                     uint32_t x, uint32_t y, uint32_t z,
                     uint32_t width, uint32_t height, uint32_t depth,
                     uint32_t mipLevel = 0, uint32_t arrayLayer = 0) override;
    
    void GenerateMipmaps(CommandBuffer* cmd) override;
    
    void Read(void* data, size_t size,
             uint32_t mipLevel = 0, uint32_t arrayLayer = 0) override;
    
private:
    NullTexture(const TextureDesc& desc);
    
    TextureDesc m_desc;
    std::vector<uint8_t> m_data;  // CPU-side storage
};

// ============================================================================
// NullSampler - Demonstration of backend-agnostic sampler implementation
// ============================================================================

/// Null sampler implementation
/// This shows how a backend would implement the Sampler interface
class NullSampler : public Sampler {
public:
    ~NullSampler() override = default;
    
    static std::expected<std::unique_ptr<Sampler>, Error>
    Create(const SamplerDesc& desc);
    
private:
    NullSampler(const SamplerDesc& desc);
    
    SamplerDesc m_desc;
};

} // namespace VRHI
