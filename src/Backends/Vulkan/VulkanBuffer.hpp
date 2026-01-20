// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#pragma once

#include <VRHI/Resources.hpp>
#include <vulkan/vulkan.hpp>

namespace VRHI {

class VulkanDevice;

class VulkanBuffer : public Buffer {
public:
    ~VulkanBuffer() override;
    
    static std::expected<std::unique_ptr<Buffer>, Error>
    Create(VulkanDevice& device, const BufferDesc& desc);
    
    // Buffer interface
    size_t GetSize() const noexcept override { return m_desc.size; }
    BufferUsage GetUsage() const noexcept override { return m_desc.usage; }
    
    void* Map() override;
    void* Map(size_t offset, size_t size) override;
    void Unmap() override;
    
    void Update(const void* data, size_t size, size_t offset = 0) override;
    void Read(void* data, size_t size, size_t offset = 0) override;
    
    vk::Buffer GetVulkanBuffer() const noexcept { return m_buffer.get(); }
    
private:
    VulkanBuffer(VulkanDevice& device, const BufferDesc& desc, vk::UniqueBuffer buffer, vk::UniqueDeviceMemory memory);
    
    VulkanDevice& m_device;
    BufferDesc m_desc;
    vk::UniqueBuffer m_buffer;
    vk::UniqueDeviceMemory m_memory;
    void* m_mappedPtr = nullptr;
};

} // namespace VRHI
