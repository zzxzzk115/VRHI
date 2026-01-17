// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#pragma once

#include "Resources.hpp"
#include <cstdint>
#include <span>

namespace VRHI {

// Forward declarations
class Pipeline;
class RenderPass;
class Framebuffer;
class Buffer;
class Texture;

// ============================================================================
// Command Buffer State
// ============================================================================

enum class CommandBufferState {
    Initial,      // Ready to record commands
    Recording,    // Currently recording commands
    Executable,   // Recording finished, ready to submit
    Submitted,    // Submitted to GPU
    Invalid,      // Invalid state (error occurred)
};

// ============================================================================
// Draw Parameters
// ============================================================================

struct DrawParams {
    uint32_t vertexCount = 0;
    uint32_t instanceCount = 1;
    uint32_t firstVertex = 0;
    uint32_t firstInstance = 0;
};

struct DrawIndexedParams {
    uint32_t indexCount = 0;
    uint32_t instanceCount = 1;
    uint32_t firstIndex = 0;
    int32_t vertexOffset = 0;
    uint32_t firstInstance = 0;
};

struct DispatchParams {
    uint32_t groupCountX = 1;
    uint32_t groupCountY = 1;
    uint32_t groupCountZ = 1;
};

// ============================================================================
// Viewport and Scissor
// ============================================================================

struct Viewport {
    float x = 0.0f;
    float y = 0.0f;
    float width = 0.0f;
    float height = 0.0f;
    float minDepth = 0.0f;
    float maxDepth = 1.0f;
};

struct Rect2D {
    int32_t x = 0;
    int32_t y = 0;
    uint32_t width = 0;
    uint32_t height = 0;
};

// ============================================================================
// Clear Values
// ============================================================================

struct ClearColorValue {
    union {
        float float32[4];
        int32_t int32[4];
        uint32_t uint32[4];
    };
    
    ClearColorValue() : float32{0.0f, 0.0f, 0.0f, 0.0f} {}
    ClearColorValue(float r, float g, float b, float a) : float32{r, g, b, a} {}
};

struct ClearDepthStencilValue {
    float depth = 1.0f;
    uint32_t stencil = 0;
};

// ============================================================================
// Command Buffer Interface
// ============================================================================

/// Command buffer for recording and submitting rendering commands
class ICommandBuffer {
public:
    virtual ~ICommandBuffer() = default;
    
    // CommandBuffer cannot be copied, only moved
    ICommandBuffer(const ICommandBuffer&) = delete;
    ICommandBuffer& operator=(const ICommandBuffer&) = delete;
    ICommandBuffer(ICommandBuffer&&) noexcept = default;
    ICommandBuffer& operator=(ICommandBuffer&&) noexcept = default;
    
    // ========================================================================
    // Command Buffer Lifecycle
    // ========================================================================
    
    /// Begin recording commands
    virtual void Begin() = 0;
    
    /// End recording commands
    virtual void End() = 0;
    
    /// Reset command buffer to initial state
    virtual void Reset() = 0;
    
    /// Get current command buffer state
    virtual CommandBufferState GetState() const noexcept = 0;
    
    // ========================================================================
    // Render Pass
    // ========================================================================
    
    /// Begin a render pass
    virtual void BeginRenderPass(RenderPass* renderPass, 
                                Framebuffer* framebuffer,
                                const Rect2D& renderArea) = 0;
    
    /// End current render pass
    virtual void EndRenderPass() = 0;
    
    // ========================================================================
    // Pipeline Binding
    // ========================================================================
    
    /// Bind a graphics or compute pipeline
    virtual void BindPipeline(Pipeline* pipeline) = 0;
    
    // ========================================================================
    // Resource Binding
    // ========================================================================
    
    /// Bind vertex buffers
    virtual void BindVertexBuffers(uint32_t firstBinding,
                                  std::span<Buffer* const> buffers,
                                  std::span<const uint64_t> offsets = {}) = 0;
    
    /// Bind index buffer
    virtual void BindIndexBuffer(Buffer* buffer, uint64_t offset = 0,
                                bool use16BitIndices = false) = 0;
    
    // ========================================================================
    // Dynamic State
    // ========================================================================
    
    /// Set viewport
    virtual void SetViewport(const Viewport& viewport) = 0;
    
    /// Set multiple viewports
    virtual void SetViewports(std::span<const Viewport> viewports) = 0;
    
    /// Set scissor rectangle
    virtual void SetScissor(const Rect2D& scissor) = 0;
    
    /// Set multiple scissor rectangles
    virtual void SetScissors(std::span<const Rect2D> scissors) = 0;
    
    /// Set line width
    virtual void SetLineWidth(float width) = 0;
    
    /// Set blend constants
    virtual void SetBlendConstants(const float blendConstants[4]) = 0;
    
    /// Set depth bias
    virtual void SetDepthBias(float constantFactor, float clamp, float slopeFactor) = 0;
    
    /// Set depth bounds
    virtual void SetDepthBounds(float minDepth, float maxDepth) = 0;
    
    /// Set stencil compare mask
    virtual void SetStencilCompareMask(bool frontFace, uint32_t compareMask) = 0;
    
    /// Set stencil write mask
    virtual void SetStencilWriteMask(bool frontFace, uint32_t writeMask) = 0;
    
    /// Set stencil reference
    virtual void SetStencilReference(bool frontFace, uint32_t reference) = 0;
    
    // ========================================================================
    // Drawing Commands
    // ========================================================================
    
    /// Draw primitives
    virtual void Draw(const DrawParams& params) = 0;
    
    /// Draw primitives (convenience overload)
    virtual void Draw(uint32_t vertexCount, uint32_t instanceCount = 1,
                     uint32_t firstVertex = 0, uint32_t firstInstance = 0) = 0;
    
    /// Draw indexed primitives
    virtual void DrawIndexed(const DrawIndexedParams& params) = 0;
    
    /// Draw indexed primitives (convenience overload)
    virtual void DrawIndexed(uint32_t indexCount, uint32_t instanceCount = 1,
                            uint32_t firstIndex = 0, int32_t vertexOffset = 0,
                            uint32_t firstInstance = 0) = 0;
    
    /// Draw indirect
    virtual void DrawIndirect(Buffer* buffer, uint64_t offset,
                             uint32_t drawCount, uint32_t stride) = 0;
    
    /// Draw indexed indirect
    virtual void DrawIndexedIndirect(Buffer* buffer, uint64_t offset,
                                    uint32_t drawCount, uint32_t stride) = 0;
    
    // ========================================================================
    // Compute Commands
    // ========================================================================
    
    /// Dispatch compute shader
    virtual void Dispatch(const DispatchParams& params) = 0;
    
    /// Dispatch compute shader (convenience overload)
    virtual void Dispatch(uint32_t groupCountX, uint32_t groupCountY = 1,
                         uint32_t groupCountZ = 1) = 0;
    
    /// Dispatch compute indirect
    virtual void DispatchIndirect(Buffer* buffer, uint64_t offset) = 0;
    
    // ========================================================================
    // Clear Commands
    // ========================================================================
    
    /// Clear color attachment
    virtual void ClearColorAttachment(uint32_t attachment,
                                     const ClearColorValue& color,
                                     const Rect2D& rect) = 0;
    
    /// Clear depth/stencil attachment
    virtual void ClearDepthStencilAttachment(const ClearDepthStencilValue& value,
                                            const Rect2D& rect) = 0;
    
    // ========================================================================
    // Copy Commands
    // ========================================================================
    
    /// Copy buffer to buffer
    virtual void CopyBuffer(Buffer* src, Buffer* dst,
                           uint64_t srcOffset, uint64_t dstOffset,
                           uint64_t size) = 0;
    
    /// Copy buffer to texture
    virtual void CopyBufferToTexture(Buffer* src, Texture* dst,
                                    uint32_t mipLevel = 0,
                                    uint32_t arrayLayer = 0) = 0;
    
    /// Copy texture to buffer
    virtual void CopyTextureToBuffer(Texture* src, Buffer* dst,
                                    uint32_t mipLevel = 0,
                                    uint32_t arrayLayer = 0) = 0;
    
    /// Copy texture to texture
    virtual void CopyTexture(Texture* src, Texture* dst,
                            uint32_t srcMipLevel = 0, uint32_t srcArrayLayer = 0,
                            uint32_t dstMipLevel = 0, uint32_t dstArrayLayer = 0) = 0;
    
    // ========================================================================
    // Synchronization
    // ========================================================================
    
    /// Insert pipeline barrier for resource synchronization
    virtual void PipelineBarrier() = 0;
    
    // ========================================================================
    // Debug Markers
    // ========================================================================
    
    /// Begin a debug marker region
    virtual void BeginDebugMarker(const char* name, const float color[4] = nullptr) = 0;
    
    /// End current debug marker region
    virtual void EndDebugMarker() = 0;
    
    /// Insert a debug marker
    virtual void InsertDebugMarker(const char* name, const float color[4] = nullptr) = 0;
    
protected:
    ICommandBuffer() = default;
};

// Convenience alias for the public API
using CommandBuffer = ICommandBuffer;

} // namespace VRHI
