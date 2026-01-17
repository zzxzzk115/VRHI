// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#pragma once

#include <VRHI/CommandBuffer.hpp>

namespace VRHI {

class OpenGL33CommandBuffer : public CommandBuffer {
public:
    OpenGL33CommandBuffer() = default;
    ~OpenGL33CommandBuffer() override = default;
    
    // Command buffer lifecycle
    void Begin() override;
    void End() override;
    void Reset() override;
    CommandBufferState GetState() const noexcept override;
    
    // Render pass
    void BeginRenderPass(RenderPass* renderPass, Framebuffer* framebuffer, const Rect2D& renderArea) override;
    void EndRenderPass() override;
    
    // Pipeline binding
    void BindPipeline(Pipeline* pipeline) override;
    
    // Resource binding
    void BindVertexBuffers(uint32_t firstBinding, std::span<Buffer* const> buffers, std::span<const uint64_t> offsets = {}) override;
    void BindIndexBuffer(Buffer* buffer, uint64_t offset = 0, bool use16BitIndices = false) override;
    
    // Dynamic state
    void SetViewport(const Viewport& viewport) override;
    void SetViewports(std::span<const Viewport> viewports) override;
    void SetScissor(const Rect2D& scissor) override;
    void SetScissors(std::span<const Rect2D> scissors) override;
    void SetLineWidth(float width) override;
    void SetBlendConstants(const float blendConstants[4]) override;
    void SetDepthBias(float constantFactor, float clamp, float slopeFactor) override;
    void SetDepthBounds(float minDepth, float maxDepth) override;
    void SetStencilCompareMask(bool frontFace, uint32_t compareMask) override;
    void SetStencilWriteMask(bool frontFace, uint32_t writeMask) override;
    void SetStencilReference(bool frontFace, uint32_t reference) override;
    
    // Drawing commands
    void Draw(const DrawParams& params) override;
    void Draw(uint32_t vertexCount, uint32_t instanceCount = 1, uint32_t firstVertex = 0, uint32_t firstInstance = 0) override;
    void DrawIndexed(const DrawIndexedParams& params) override;
    void DrawIndexed(uint32_t indexCount, uint32_t instanceCount = 1, uint32_t firstIndex = 0, int32_t vertexOffset = 0, uint32_t firstInstance = 0) override;
    void DrawIndirect(Buffer* buffer, uint64_t offset, uint32_t drawCount, uint32_t stride) override;
    void DrawIndexedIndirect(Buffer* buffer, uint64_t offset, uint32_t drawCount, uint32_t stride) override;
    
    // Compute commands
    void Dispatch(const DispatchParams& params) override;
    void Dispatch(uint32_t groupCountX, uint32_t groupCountY = 1, uint32_t groupCountZ = 1) override;
    void DispatchIndirect(Buffer* buffer, uint64_t offset) override;
    
    // Clear commands
    void ClearColorAttachment(uint32_t attachment, const ClearColorValue& color, const Rect2D& rect) override;
    void ClearDepthStencilAttachment(const ClearDepthStencilValue& value, const Rect2D& rect) override;
    
    // Copy commands
    void CopyBuffer(Buffer* src, Buffer* dst, uint64_t srcOffset, uint64_t dstOffset, uint64_t size) override;
    void CopyBufferToTexture(Buffer* src, Texture* dst, uint32_t mipLevel = 0, uint32_t arrayLayer = 0) override;
    void CopyTextureToBuffer(Texture* src, Buffer* dst, uint32_t mipLevel = 0, uint32_t arrayLayer = 0) override;
    void CopyTexture(Texture* src, Texture* dst, uint32_t srcMipLevel = 0, uint32_t srcArrayLayer = 0, uint32_t dstMipLevel = 0, uint32_t dstArrayLayer = 0) override;
    
    // Synchronization
    void PipelineBarrier() override;
    
    // Debug markers
    void BeginDebugMarker(const char* name, const float color[4] = nullptr) override;
    void EndDebugMarker() override;
    void InsertDebugMarker(const char* name, const float color[4] = nullptr) override;
    
    // OpenGL-specific: Execute recorded commands
    void Execute();
    
private:
    CommandBufferState m_state = CommandBufferState::Initial;
};

} // namespace VRHI
