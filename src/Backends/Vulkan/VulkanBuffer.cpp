// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#include "VulkanBuffer.hpp"
#include "VulkanDevice.hpp"
#include <VRHI/Logging.hpp>

namespace VRHI {

VulkanBuffer::VulkanBuffer(VulkanDevice& device, const BufferDesc& desc, 
                           vk::UniqueBuffer buffer, vk::UniqueDeviceMemory memory)
    : m_device(device)
    , m_desc(desc)
    , m_buffer(std::move(buffer))
    , m_memory(std::move(memory))
{
}

VulkanBuffer::~VulkanBuffer() {
    if (m_mappedPtr) {
        Unmap();
    }
}

std::expected<std::unique_ptr<Buffer>, Error>
VulkanBuffer::Create(VulkanDevice& device, const BufferDesc& desc) {
    if (desc.size == 0) {
        return std::unexpected(Error{
            Error::Code::InvalidConfig,
            "Buffer size must be greater than 0"
        });
    }
    
    try {
        // Convert usage flags
        vk::BufferUsageFlags usage;
        if (static_cast<uint32_t>(desc.usage) & static_cast<uint32_t>(BufferUsage::Vertex)) {
            usage |= vk::BufferUsageFlagBits::eVertexBuffer;
        }
        if (static_cast<uint32_t>(desc.usage) & static_cast<uint32_t>(BufferUsage::Index)) {
            usage |= vk::BufferUsageFlagBits::eIndexBuffer;
        }
        if (static_cast<uint32_t>(desc.usage) & static_cast<uint32_t>(BufferUsage::Uniform)) {
            usage |= vk::BufferUsageFlagBits::eUniformBuffer;
        }
        if (static_cast<uint32_t>(desc.usage) & static_cast<uint32_t>(BufferUsage::Storage)) {
            usage |= vk::BufferUsageFlagBits::eStorageBuffer;
        }
        usage |= vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eTransferSrc;
        
        // Create buffer
        vk::BufferCreateInfo bufferInfo{
            .size = desc.size,
            .usage = usage,
            .sharingMode = vk::SharingMode::eExclusive
        };
        
        auto buffer = device.GetVulkanDevice().createBufferUnique(bufferInfo);
        
        // Get memory requirements
        auto memRequirements = device.GetVulkanDevice().getBufferMemoryRequirements(buffer.get());
        
        // Allocate memory (simple host-visible allocation for now)
        vk::MemoryAllocateInfo allocInfo{
            .allocationSize = memRequirements.size,
            .memoryTypeIndex = 0  // TODO: Find appropriate memory type
        };
        
        // Find memory type
        auto memProps = device.GetPhysicalDevice().getMemoryProperties();
        vk::MemoryPropertyFlags properties = vk::MemoryPropertyFlagBits::eHostVisible | 
                                            vk::MemoryPropertyFlagBits::eHostCoherent;
        
        for (uint32_t i = 0; i < memProps.memoryTypeCount; i++) {
            if ((memRequirements.memoryTypeBits & (1 << i)) &&
                (memProps.memoryTypes[i].propertyFlags & properties) == properties) {
                allocInfo.memoryTypeIndex = i;
                break;
            }
        }
        
        auto memory = device.GetVulkanDevice().allocateMemoryUnique(allocInfo);
        device.GetVulkanDevice().bindBufferMemory(buffer.get(), memory.get(), 0);
        
        auto vkBuffer = std::unique_ptr<Buffer>(new VulkanBuffer(device, desc, std::move(buffer), std::move(memory)));
        return vkBuffer;
        
    } catch (const vk::SystemError& e) {
        return std::unexpected(Error{
            Error::Code::ResourceCreationFailed,
            std::string("Failed to create Vulkan buffer: ") + e.what()
        });
    }
}

void* VulkanBuffer::Map() {
    return Map(0, m_desc.size);
}

void* VulkanBuffer::Map(size_t offset, size_t size) {
    if (m_mappedPtr) {
        return m_mappedPtr;
    }
    
    try {
        m_mappedPtr = m_device.GetVulkanDevice().mapMemory(m_memory.get(), offset, size);
        return m_mappedPtr;
    } catch (const vk::SystemError& e) {
        LogError("Failed to map Vulkan buffer: {}", e.what());
        return nullptr;
    }
}

void VulkanBuffer::Unmap() {
    if (m_mappedPtr) {
        m_device.GetVulkanDevice().unmapMemory(m_memory.get());
        m_mappedPtr = nullptr;
    }
}

void VulkanBuffer::Update(const void* data, size_t size, size_t offset) {
    if (offset + size > m_desc.size) {
        LogError("Buffer update out of bounds");
        return;
    }
    
    void* mapped = Map(offset, size);
    if (mapped) {
        std::memcpy(mapped, data, size);
        Unmap();
    }
}

void VulkanBuffer::Read(void* data, size_t size, size_t offset) {
    if (offset + size > m_desc.size) {
        LogError("Buffer read out of bounds");
        return;
    }
    
    void* mapped = Map(offset, size);
    if (mapped) {
        std::memcpy(data, mapped, size);
        Unmap();
    }
}

} // namespace VRHI
