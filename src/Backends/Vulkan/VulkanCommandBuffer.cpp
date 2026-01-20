// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#include "VulkanCommandBuffer.hpp"
#include "VulkanDevice.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanPipeline.hpp"
#include "VulkanTexture.hpp"
#include "VulkanSampler.hpp"
#include "VulkanRenderPass.hpp"
#include "VulkanFramebuffer.hpp"
#include <VRHI/Logging.hpp>

namespace VRHI {

VulkanCommandBuffer::VulkanCommandBuffer(VulkanDevice& device, vk::UniqueCommandBuffer commandBuffer)
    : m_device(device)
    , m_commandBuffer(std::move(commandBuffer))
{
}

VulkanCommandBuffer::~VulkanCommandBuffer() {
    if (m_commandBuffer && m_state == CommandBufferState::Recording) {
        try {
            m_commandBuffer->end();
        } catch (...) {
            // Ignore errors during destruction
        }
    }
}

std::unique_ptr<CommandBuffer>
VulkanCommandBuffer::Create(VulkanDevice& device) {
    vk::CommandBufferAllocateInfo allocInfo;
    allocInfo.commandPool = device.GetCommandPool();
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandBufferCount = 1;
    
    auto commandBuffers = device.GetVulkanDevice().allocateCommandBuffersUnique(allocInfo);
    if (commandBuffers.empty()) {
        LogError("Failed to allocate command buffer");
        return nullptr;
    }
    
    return std::unique_ptr<CommandBuffer>(
        new VulkanCommandBuffer(device, std::move(commandBuffers[0]))
    );
}

void VulkanCommandBuffer::Begin() {
    if (m_state == CommandBufferState::Recording) {
        LogWarning("Command buffer already in recording state");
        return;
    }
    
    if (m_state == CommandBufferState::Executable) {
        Reset();
    }
    
    vk::CommandBufferBeginInfo beginInfo;
    beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
    
    m_commandBuffer->begin(beginInfo);
    m_state = CommandBufferState::Recording;
    m_insideRenderPass = false;
}

void VulkanCommandBuffer::End() {
    if (m_state != CommandBufferState::Recording) {
        LogWarning("Command buffer not in recording state");
        return;
    }
    
    if (m_insideRenderPass) {
        LogWarning("Ending command buffer while still inside render pass");
        EndRenderPass();
    }
    
    m_commandBuffer->end();
    m_state = CommandBufferState::Executable;
}

void VulkanCommandBuffer::Reset() {
    m_commandBuffer->reset(vk::CommandBufferResetFlagBits::eReleaseResources);
    m_state = CommandBufferState::Initial;
    m_insideRenderPass = false;
}

void VulkanCommandBuffer::BeginRenderPass(RenderPass* renderPass, Framebuffer* framebuffer, const Rect2D& renderArea) {
    if (!renderPass || !framebuffer) {
        LogWarning("BeginRenderPass called with null renderPass or framebuffer");
        return;
    }
    
    // TODO: Implement render pass begin when VulkanRenderPass and VulkanFramebuffer are implemented
    // For now, this is a placeholder
    m_insideRenderPass = true;
}

void VulkanCommandBuffer::EndRenderPass() {
    if (!m_insideRenderPass) {
        LogWarning("EndRenderPass called but not inside render pass");
        return;
    }
    
    // TODO: Implement render pass end when VulkanRenderPass is implemented
    m_insideRenderPass = false;
}

void VulkanCommandBuffer::BindPipeline(Pipeline* pipeline) {
    if (!pipeline) {
        LogWarning("BindPipeline called with null pipeline");
        return;
    }
    
    // TODO: Implement when VulkanPipeline has the actual vk::Pipeline handle
    // For now, this is a placeholder
}

void VulkanCommandBuffer::BindVertexBuffers(uint32_t firstBinding, std::span<Buffer* const> buffers, std::span<const uint64_t> offsets) {
    if (buffers.empty()) {
        return;
    }
    
    std::vector<vk::Buffer> vkBuffers;
    std::vector<vk::DeviceSize> vkOffsets;
    
    vkBuffers.reserve(buffers.size());
    vkOffsets.reserve(buffers.size());
    
    for (size_t i = 0; i < buffers.size(); ++i) {
        if (!buffers[i]) {
            LogWarning("BindVertexBuffers: null buffer at index {}", i);
            continue;
        }
        auto* vkBuffer = static_cast<VulkanBuffer*>(buffers[i]);
        vkBuffers.push_back(vkBuffer->GetVulkanBuffer());
        vkOffsets.push_back(i < offsets.size() ? offsets[i] : 0);
    }
    
    if (!vkBuffers.empty()) {
        m_commandBuffer->bindVertexBuffers(firstBinding, vkBuffers, vkOffsets);
    }
}

void VulkanCommandBuffer::BindIndexBuffer(Buffer* buffer, uint64_t offset, bool use16BitIndices) {
    if (!buffer) {
        LogWarning("BindIndexBuffer called with null buffer");
        return;
    }
    
    auto* vkBuffer = static_cast<VulkanBuffer*>(buffer);
    vk::IndexType indexType = use16BitIndices ? vk::IndexType::eUint16 : vk::IndexType::eUint32;
    
    m_commandBuffer->bindIndexBuffer(vkBuffer->GetVulkanBuffer(), offset, indexType);
}

void VulkanCommandBuffer::BindUniformBuffer(uint32_t binding, Buffer* buffer, uint64_t offset, uint64_t size) {
    // TODO: Implement descriptor set binding when descriptor sets are implemented
    LogWarning("BindUniformBuffer not yet implemented");
}

void VulkanCommandBuffer::BindTexture(uint32_t binding, Texture* texture, Sampler* sampler) {
    // TODO: Implement descriptor set binding when descriptor sets are implemented
    LogWarning("BindTexture not yet implemented");
}

void VulkanCommandBuffer::SetViewport(const Viewport& viewport) {
    vk::Viewport vkViewport;
    vkViewport.x = viewport.x;
    vkViewport.y = viewport.y;
    vkViewport.width = viewport.width;
    vkViewport.height = viewport.height;
    vkViewport.minDepth = viewport.minDepth;
    vkViewport.maxDepth = viewport.maxDepth;
    
    m_commandBuffer->setViewport(0, 1, &vkViewport);
}

void VulkanCommandBuffer::SetViewports(std::span<const Viewport> viewports) {
    if (viewports.empty()) {
        return;
    }
    
    std::vector<vk::Viewport> vkViewports;
    vkViewports.reserve(viewports.size());
    
    for (const auto& vp : viewports) {
        vk::Viewport vkViewport;
        vkViewport.x = vp.x;
        vkViewport.y = vp.y;
        vkViewport.width = vp.width;
        vkViewport.height = vp.height;
        vkViewport.minDepth = vp.minDepth;
        vkViewport.maxDepth = vp.maxDepth;
        vkViewports.push_back(vkViewport);
    }
    
    m_commandBuffer->setViewport(0, vkViewports);
}

void VulkanCommandBuffer::SetScissor(const Rect2D& scissor) {
    vk::Rect2D vkScissor;
    vkScissor.offset.x = scissor.x;
    vkScissor.offset.y = scissor.y;
    vkScissor.extent.width = scissor.width;
    vkScissor.extent.height = scissor.height;
    
    m_commandBuffer->setScissor(0, 1, &vkScissor);
}

void VulkanCommandBuffer::SetScissors(std::span<const Rect2D> scissors) {
    if (scissors.empty()) {
        return;
    }
    
    std::vector<vk::Rect2D> vkScissors;
    vkScissors.reserve(scissors.size());
    
    for (const auto& sc : scissors) {
        vk::Rect2D vkScissor;
        vkScissor.offset.x = sc.x;
        vkScissor.offset.y = sc.y;
        vkScissor.extent.width = sc.width;
        vkScissor.extent.height = sc.height;
        vkScissors.push_back(vkScissor);
    }
    
    m_commandBuffer->setScissor(0, vkScissors);
}

void VulkanCommandBuffer::SetLineWidth(float width) {
    m_commandBuffer->setLineWidth(width);
}

void VulkanCommandBuffer::SetBlendConstants(const float blendConstants[4]) {
    m_commandBuffer->setBlendConstants(blendConstants);
}

void VulkanCommandBuffer::SetDepthBias(float constantFactor, float clamp, float slopeFactor) {
    m_commandBuffer->setDepthBias(constantFactor, clamp, slopeFactor);
}

void VulkanCommandBuffer::SetDepthBounds(float minDepth, float maxDepth) {
    m_commandBuffer->setDepthBounds(minDepth, maxDepth);
}

void VulkanCommandBuffer::SetStencilCompareMask(bool frontFace, uint32_t compareMask) {
    vk::StencilFaceFlags faceMask = frontFace ? vk::StencilFaceFlagBits::eFront : vk::StencilFaceFlagBits::eBack;
    m_commandBuffer->setStencilCompareMask(faceMask, compareMask);
}

void VulkanCommandBuffer::SetStencilWriteMask(bool frontFace, uint32_t writeMask) {
    vk::StencilFaceFlags faceMask = frontFace ? vk::StencilFaceFlagBits::eFront : vk::StencilFaceFlagBits::eBack;
    m_commandBuffer->setStencilWriteMask(faceMask, writeMask);
}

void VulkanCommandBuffer::SetStencilReference(bool frontFace, uint32_t reference) {
    vk::StencilFaceFlags faceMask = frontFace ? vk::StencilFaceFlagBits::eFront : vk::StencilFaceFlagBits::eBack;
    m_commandBuffer->setStencilReference(faceMask, reference);
}

void VulkanCommandBuffer::Draw(const DrawParams& params) {
    Draw(params.vertexCount, params.instanceCount, params.firstVertex, params.firstInstance);
}

void VulkanCommandBuffer::Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) {
    m_commandBuffer->draw(vertexCount, instanceCount, firstVertex, firstInstance);
}

void VulkanCommandBuffer::DrawIndexed(const DrawIndexedParams& params) {
    DrawIndexed(params.indexCount, params.instanceCount, params.firstIndex, params.vertexOffset, params.firstInstance);
}

void VulkanCommandBuffer::DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) {
    m_commandBuffer->drawIndexed(indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

void VulkanCommandBuffer::DrawIndirect(Buffer* buffer, uint64_t offset, uint32_t drawCount, uint32_t stride) {
    if (!buffer) {
        LogWarning("DrawIndirect called with null buffer");
        return;
    }
    
    auto* vkBuffer = static_cast<VulkanBuffer*>(buffer);
    m_commandBuffer->drawIndirect(vkBuffer->GetVulkanBuffer(), offset, drawCount, stride);
}

void VulkanCommandBuffer::DrawIndexedIndirect(Buffer* buffer, uint64_t offset, uint32_t drawCount, uint32_t stride) {
    if (!buffer) {
        LogWarning("DrawIndexedIndirect called with null buffer");
        return;
    }
    
    auto* vkBuffer = static_cast<VulkanBuffer*>(buffer);
    m_commandBuffer->drawIndexedIndirect(vkBuffer->GetVulkanBuffer(), offset, drawCount, stride);
}

void VulkanCommandBuffer::Dispatch(const DispatchParams& params) {
    Dispatch(params.groupCountX, params.groupCountY, params.groupCountZ);
}

void VulkanCommandBuffer::Dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) {
    m_commandBuffer->dispatch(groupCountX, groupCountY, groupCountZ);
}

void VulkanCommandBuffer::DispatchIndirect(Buffer* buffer, uint64_t offset) {
    if (!buffer) {
        LogWarning("DispatchIndirect called with null buffer");
        return;
    }
    
    auto* vkBuffer = static_cast<VulkanBuffer*>(buffer);
    m_commandBuffer->dispatchIndirect(vkBuffer->GetVulkanBuffer(), offset);
}

void VulkanCommandBuffer::ClearColorAttachment(uint32_t attachment, const ClearColorValue& color, const Rect2D& rect) {
    if (!m_insideRenderPass) {
        LogWarning("ClearColorAttachment must be called inside a render pass");
        return;
    }
    
    vk::ClearAttachment clearAttachment;
    clearAttachment.aspectMask = vk::ImageAspectFlagBits::eColor;
    clearAttachment.colorAttachment = attachment;
    clearAttachment.clearValue.color.float32[0] = color.float32[0];
    clearAttachment.clearValue.color.float32[1] = color.float32[1];
    clearAttachment.clearValue.color.float32[2] = color.float32[2];
    clearAttachment.clearValue.color.float32[3] = color.float32[3];
    
    vk::ClearRect clearRect;
    clearRect.rect.offset.x = rect.x;
    clearRect.rect.offset.y = rect.y;
    clearRect.rect.extent.width = rect.width;
    clearRect.rect.extent.height = rect.height;
    clearRect.baseArrayLayer = 0;
    clearRect.layerCount = 1;
    
    m_commandBuffer->clearAttachments(1, &clearAttachment, 1, &clearRect);
}

void VulkanCommandBuffer::ClearDepthStencilAttachment(const ClearDepthStencilValue& value, const Rect2D& rect) {
    if (!m_insideRenderPass) {
        LogWarning("ClearDepthStencilAttachment must be called inside a render pass");
        return;
    }
    
    vk::ClearAttachment clearAttachment;
    clearAttachment.aspectMask = vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;
    clearAttachment.clearValue.depthStencil.depth = value.depth;
    clearAttachment.clearValue.depthStencil.stencil = value.stencil;
    
    vk::ClearRect clearRect;
    clearRect.rect.offset.x = rect.x;
    clearRect.rect.offset.y = rect.y;
    clearRect.rect.extent.width = rect.width;
    clearRect.rect.extent.height = rect.height;
    clearRect.baseArrayLayer = 0;
    clearRect.layerCount = 1;
    
    m_commandBuffer->clearAttachments(1, &clearAttachment, 1, &clearRect);
}

void VulkanCommandBuffer::CopyBuffer(Buffer* src, Buffer* dst, uint64_t srcOffset, uint64_t dstOffset, uint64_t size) {
    if (!src || !dst) {
        LogWarning("CopyBuffer called with null buffer");
        return;
    }
    
    auto* vkSrcBuffer = static_cast<VulkanBuffer*>(src);
    auto* vkDstBuffer = static_cast<VulkanBuffer*>(dst);
    
    vk::BufferCopy copyRegion;
    copyRegion.srcOffset = srcOffset;
    copyRegion.dstOffset = dstOffset;
    copyRegion.size = size;
    
    m_commandBuffer->copyBuffer(vkSrcBuffer->GetVulkanBuffer(), vkDstBuffer->GetVulkanBuffer(), 1, &copyRegion);
}

void VulkanCommandBuffer::CopyBufferToTexture(Buffer* src, Texture* dst, uint32_t mipLevel, uint32_t arrayLayer) {
    // TODO: Implement when VulkanTexture has the actual vk::Image handle
    LogWarning("CopyBufferToTexture not yet implemented");
}

void VulkanCommandBuffer::CopyTextureToBuffer(Texture* src, Buffer* dst, uint32_t mipLevel, uint32_t arrayLayer) {
    // TODO: Implement when VulkanTexture has the actual vk::Image handle
    LogWarning("CopyTextureToBuffer not yet implemented");
}

void VulkanCommandBuffer::CopyTexture(Texture* src, Texture* dst, uint32_t srcMipLevel, uint32_t srcArrayLayer, uint32_t dstMipLevel, uint32_t dstArrayLayer) {
    // TODO: Implement when VulkanTexture has the actual vk::Image handle
    LogWarning("CopyTexture not yet implemented");
}

void VulkanCommandBuffer::PipelineBarrier() {
    // TODO: The VRHI interface should be extended to accept pipeline stage and access mask parameters
    // For now, use a conservative barrier that works for all cases but may be suboptimal
    vk::MemoryBarrier memoryBarrier;
    memoryBarrier.srcAccessMask = vk::AccessFlagBits::eMemoryWrite;
    memoryBarrier.dstAccessMask = vk::AccessFlagBits::eMemoryRead;
    
    m_commandBuffer->pipelineBarrier(
        vk::PipelineStageFlagBits::eAllCommands,
        vk::PipelineStageFlagBits::eAllCommands,
        vk::DependencyFlags{},
        1, &memoryBarrier,
        0, nullptr,
        0, nullptr
    );
}

void VulkanCommandBuffer::BeginDebugMarker(const char* name, const float color[4]) {
    // TODO: Implement debug markers when validation/debug extensions are set up
}

void VulkanCommandBuffer::EndDebugMarker() {
    // TODO: Implement debug markers when validation/debug extensions are set up
}

void VulkanCommandBuffer::InsertDebugMarker(const char* name, const float color[4]) {
    // TODO: Implement debug markers when validation/debug extensions are set up
}

} // namespace VRHI
