// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#pragma once

#include <VRHI/CommandBuffer.hpp>
#include <vulkan/vulkan.hpp>

namespace VRHI {

class VulkanDevice;

class VulkanCommandBuffer : public CommandBuffer {
public:
    ~VulkanCommandBuffer() override;
    
    static std::unique_ptr<CommandBuffer> Create(VulkanDevice& device);
    
    void Begin() override;
    void End() override;
    void Reset() override;
    CommandBufferState GetState() const noexcept override { return m_state; }
    
    void BeginRenderPass(RenderPass*, Framebuffer*, const Rect2D&) override;
    void EndRenderPass() override;
    void BindPipeline(Pipeline*) override;
    void BindVertexBuffers(uint32_t, std::span<Buffer* const>, std::span<const uint64_t>) override;
    void BindIndexBuffer(Buffer*, uint64_t = 0, bool = false) override;
    void BindUniformBuffer(uint32_t, Buffer*, uint64_t = 0, uint64_t = 0) override;
    void BindTexture(uint32_t, Texture*, Sampler* = nullptr) override;
    
    void SetViewport(const Viewport&) override;
    void SetViewports(std::span<const Viewport>) override;
    void SetScissor(const Rect2D&) override;
    void SetScissors(std::span<const Rect2D>) override;
    void SetLineWidth(float) override;
    void SetBlendConstants(const float[4]) override;
    void SetDepthBias(float, float, float) override;
    void SetDepthBounds(float, float) override;
    void SetStencilCompareMask(bool, uint32_t) override;
    void SetStencilWriteMask(bool, uint32_t) override;
    void SetStencilReference(bool, uint32_t) override;
    
    void Draw(const DrawParams&) override;
    void Draw(uint32_t, uint32_t = 1, uint32_t = 0, uint32_t = 0) override;
    void DrawIndexed(const DrawIndexedParams&) override;
    void DrawIndexed(uint32_t, uint32_t = 1, uint32_t = 0, int32_t = 0, uint32_t = 0) override;
    void DrawIndirect(Buffer*, uint64_t, uint32_t, uint32_t) override;
    void DrawIndexedIndirect(Buffer*, uint64_t, uint32_t, uint32_t) override;
    
    void Dispatch(const DispatchParams&) override;
    void Dispatch(uint32_t, uint32_t = 1, uint32_t = 1) override;
    void DispatchIndirect(Buffer*, uint64_t) override;
    
    void ClearColorAttachment(uint32_t, const ClearColorValue&, const Rect2D&) override;
    void ClearDepthStencilAttachment(const ClearDepthStencilValue&, const Rect2D&) override;
    
    void CopyBuffer(Buffer*, Buffer*, uint64_t, uint64_t, uint64_t) override;
    void CopyBufferToTexture(Buffer*, Texture*, uint32_t = 0, uint32_t = 0) override;
    void CopyTextureToBuffer(Texture*, Buffer*, uint32_t = 0, uint32_t = 0) override;
    void CopyTexture(Texture*, Texture*, uint32_t = 0, uint32_t = 0, uint32_t = 0, uint32_t = 0) override;
    
    void PipelineBarrier() override;
    
    void BeginDebugMarker(const char*, const float[4] = nullptr) override;
    void EndDebugMarker() override;
    void InsertDebugMarker(const char*, const float[4] = nullptr) override;
    
    vk::CommandBuffer GetVulkanCommandBuffer() const noexcept { return m_commandBuffer.get(); }

private:
    VulkanCommandBuffer(VulkanDevice& device, vk::UniqueCommandBuffer commandBuffer);
    
    VulkanDevice& m_device;
    vk::UniqueCommandBuffer m_commandBuffer;
    CommandBufferState m_state = CommandBufferState::Initial;
    bool m_insideRenderPass = false;
};

} // namespace VRHI
