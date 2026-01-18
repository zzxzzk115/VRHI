// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#include "OpenGL33CommandBuffer.hpp"
#include "OpenGL33Buffer.hpp"
#include "OpenGL33Pipeline.hpp"
#include <VRHI/Logging.hpp>
#include <glad/glad.h>

namespace VRHI {

namespace {
    // Helper function to convert VertexFormat to OpenGL type info
    struct VertexFormatInfo {
        GLint componentCount;
        GLenum type;
        GLboolean normalized;
    };
    
    VertexFormatInfo GetVertexFormatInfo(VertexFormat format) {
        switch (format) {
            case VertexFormat::Float:    return {1, GL_FLOAT, GL_FALSE};
            case VertexFormat::Float2:   return {2, GL_FLOAT, GL_FALSE};
            case VertexFormat::Float3:   return {3, GL_FLOAT, GL_FALSE};
            case VertexFormat::Float4:   return {4, GL_FLOAT, GL_FALSE};
            case VertexFormat::Int:      return {1, GL_INT, GL_FALSE};
            case VertexFormat::Int2:     return {2, GL_INT, GL_FALSE};
            case VertexFormat::Int3:     return {3, GL_INT, GL_FALSE};
            case VertexFormat::Int4:     return {4, GL_INT, GL_FALSE};
            case VertexFormat::UInt:     return {1, GL_UNSIGNED_INT, GL_FALSE};
            case VertexFormat::UInt2:    return {2, GL_UNSIGNED_INT, GL_FALSE};
            case VertexFormat::UInt3:    return {3, GL_UNSIGNED_INT, GL_FALSE};
            case VertexFormat::UInt4:    return {4, GL_UNSIGNED_INT, GL_FALSE};
            default:                     return {3, GL_FLOAT, GL_FALSE};
        }
    }
} // anonymous namespace

void OpenGL33CommandBuffer::Begin() {
    m_state = CommandBufferState::Recording;
}

void OpenGL33CommandBuffer::End() {
    m_state = CommandBufferState::Executable;
}

void OpenGL33CommandBuffer::Reset() {
    m_state = CommandBufferState::Initial;
}

CommandBufferState OpenGL33CommandBuffer::GetState() const noexcept {
    return m_state;
}

void OpenGL33CommandBuffer::BeginRenderPass(RenderPass* renderPass, Framebuffer* framebuffer, const Rect2D& renderArea) {
    // In OpenGL, we would bind the framebuffer here
}

void OpenGL33CommandBuffer::EndRenderPass() {
    // Unbind framebuffer
}

void OpenGL33CommandBuffer::BindPipeline(Pipeline* pipeline) {
    // Bind shader program
    if (pipeline) {
        auto* glPipeline = static_cast<OpenGL33Pipeline*>(pipeline);
        glUseProgram(glPipeline->GetHandle());
        m_currentPipeline = pipeline;  // Track for vertex layout
    }
}

void OpenGL33CommandBuffer::BindVertexBuffers(uint32_t firstBinding, std::span<Buffer* const> buffers, std::span<const uint64_t> offsets) {
    // Get vertex layout from the currently bound pipeline
    if (!m_currentPipeline) {
        LogWarning("BindVertexBuffers called without a bound pipeline");
        return;
    }
    
    auto* glPipeline = static_cast<OpenGL33Pipeline*>(m_currentPipeline);
    const auto& vertexInput = glPipeline->GetVertexInputState();
    
    if (vertexInput.attributes.empty() || vertexInput.bindings.empty()) {
        LogWarning("Pipeline has no vertex input layout defined");
        return;
    }
    
    // Bind vertex buffers and set up attributes according to the pipeline's vertex layout
    for (const auto& binding : vertexInput.bindings) {
        if (binding.binding < firstBinding || binding.binding >= firstBinding + buffers.size()) {
            continue;
        }
        
        uint32_t bufferIndex = binding.binding - firstBinding;
        if (!buffers[bufferIndex]) {
            continue;
        }
        
        auto* glBuffer = static_cast<OpenGL33Buffer*>(buffers[bufferIndex]);
        glBindBuffer(GL_ARRAY_BUFFER, glBuffer->GetHandle());
        
        // Set up all attributes that use this binding
        for (const auto& attr : vertexInput.attributes) {
            if (attr.binding == binding.binding) {
                auto formatInfo = GetVertexFormatInfo(attr.format);
                
                glEnableVertexAttribArray(attr.location);
                glVertexAttribPointer(
                    attr.location,
                    formatInfo.componentCount,
                    formatInfo.type,
                    formatInfo.normalized,
                    binding.stride,
                    reinterpret_cast<const void*>(static_cast<uintptr_t>(attr.offset))
                );
            }
        }
    }
}

void OpenGL33CommandBuffer::BindIndexBuffer(Buffer* buffer, uint64_t offset, bool use16BitIndices) {
    // Store the index type for later use in DrawIndexed
    m_indexType = use16BitIndices ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
    // Bind index buffer would be implemented here
}

void OpenGL33CommandBuffer::SetViewport(const Viewport& viewport) {
    glViewport(static_cast<GLint>(viewport.x), static_cast<GLint>(viewport.y),
              static_cast<GLsizei>(viewport.width), static_cast<GLsizei>(viewport.height));
    glDepthRange(viewport.minDepth, viewport.maxDepth);
}

void OpenGL33CommandBuffer::SetViewports(std::span<const Viewport> viewports) {
    if (!viewports.empty()) {
        SetViewport(viewports[0]);
    }
}

void OpenGL33CommandBuffer::SetScissor(const Rect2D& scissor) {
    glScissor(scissor.x, scissor.y, scissor.width, scissor.height);
}

void OpenGL33CommandBuffer::SetScissors(std::span<const Rect2D> scissors) {
    if (!scissors.empty()) {
        SetScissor(scissors[0]);
    }
}

void OpenGL33CommandBuffer::SetLineWidth(float width) {
    glLineWidth(width);
}

void OpenGL33CommandBuffer::SetBlendConstants(const float blendConstants[4]) {
    glBlendColor(blendConstants[0], blendConstants[1], blendConstants[2], blendConstants[3]);
}

void OpenGL33CommandBuffer::SetDepthBias(float constantFactor, float clamp, float slopeFactor) {
    glPolygonOffset(slopeFactor, constantFactor);
}

void OpenGL33CommandBuffer::SetDepthBounds(float minDepth, float maxDepth) {
    // Not directly supported in GL 3.3
}

void OpenGL33CommandBuffer::SetStencilCompareMask(bool frontFace, uint32_t compareMask) {
    glStencilMaskSeparate(frontFace ? GL_FRONT : GL_BACK, compareMask);
}

void OpenGL33CommandBuffer::SetStencilWriteMask(bool frontFace, uint32_t writeMask) {
    glStencilMaskSeparate(frontFace ? GL_FRONT : GL_BACK, writeMask);
}

void OpenGL33CommandBuffer::SetStencilReference(bool frontFace, uint32_t reference) {
    // Store for later use with glStencilFunc
}

void OpenGL33CommandBuffer::Draw(const DrawParams& params) {
    Draw(params.vertexCount, params.instanceCount, params.firstVertex, params.firstInstance);
}

void OpenGL33CommandBuffer::Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) {
    if (instanceCount > 1) {
        glDrawArraysInstanced(GL_TRIANGLES, firstVertex, vertexCount, instanceCount);
    } else {
        glDrawArrays(GL_TRIANGLES, firstVertex, vertexCount);
    }
}

void OpenGL33CommandBuffer::DrawIndexed(const DrawIndexedParams& params) {
    DrawIndexed(params.indexCount, params.instanceCount, params.firstIndex, params.vertexOffset, params.firstInstance);
}

void OpenGL33CommandBuffer::DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) {
    // Calculate index offset based on the actual index type size
    size_t indexSize = (m_indexType == GL_UNSIGNED_SHORT) ? sizeof(uint16_t) : sizeof(uint32_t);
    const void* indices = reinterpret_cast<const void*>(static_cast<uintptr_t>(firstIndex * indexSize));
    
    if (instanceCount > 1) {
        glDrawElementsInstanced(GL_TRIANGLES, indexCount, m_indexType, indices, instanceCount);
    } else {
        glDrawElements(GL_TRIANGLES, indexCount, m_indexType, indices);
    }
}

void OpenGL33CommandBuffer::DrawIndirect(Buffer* buffer, uint64_t offset, uint32_t drawCount, uint32_t stride) {
    // GL 4.0+ feature
    LogWarning("DrawIndirect not supported in OpenGL 3.3");
}

void OpenGL33CommandBuffer::DrawIndexedIndirect(Buffer* buffer, uint64_t offset, uint32_t drawCount, uint32_t stride) {
    // GL 4.0+ feature
    LogWarning("DrawIndexedIndirect not supported in OpenGL 3.3");
}

void OpenGL33CommandBuffer::Dispatch(const DispatchParams& params) {
    LogWarning("Compute shaders not supported in OpenGL 3.3");
}

void OpenGL33CommandBuffer::Dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) {
    LogWarning("Compute shaders not supported in OpenGL 3.3");
}

void OpenGL33CommandBuffer::DispatchIndirect(Buffer* buffer, uint64_t offset) {
    LogWarning("Compute shaders not supported in OpenGL 3.3");
}

void OpenGL33CommandBuffer::ClearColorAttachment(uint32_t attachment, const ClearColorValue& color, const Rect2D& rect) {
    glClearColor(color.float32[0], color.float32[1], color.float32[2], color.float32[3]);
    glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGL33CommandBuffer::ClearDepthStencilAttachment(const ClearDepthStencilValue& value, const Rect2D& rect) {
    glClearDepth(value.depth);
    glClearStencil(value.stencil);
    glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void OpenGL33CommandBuffer::CopyBuffer(Buffer* src, Buffer* dst, uint64_t srcOffset, uint64_t dstOffset, uint64_t size) {
    // Use glCopyBufferSubData (GL 3.1+)
    auto* glSrc = static_cast<OpenGL33Buffer*>(src);
    auto* glDst = static_cast<OpenGL33Buffer*>(dst);
    
    glBindBuffer(GL_COPY_READ_BUFFER, glSrc->GetHandle());
    glBindBuffer(GL_COPY_WRITE_BUFFER, glDst->GetHandle());
    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, srcOffset, dstOffset, size);
    glBindBuffer(GL_COPY_READ_BUFFER, 0);
    glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
}

void OpenGL33CommandBuffer::CopyBufferToTexture(Buffer* src, Texture* dst, uint32_t mipLevel, uint32_t arrayLayer) {
    // Implementation would use glBindBuffer + glTexSubImage
}

void OpenGL33CommandBuffer::CopyTextureToBuffer(Texture* src, Buffer* dst, uint32_t mipLevel, uint32_t arrayLayer) {
    // Implementation would use glGetTexImage
}

void OpenGL33CommandBuffer::CopyTexture(Texture* src, Texture* dst, uint32_t srcMipLevel, uint32_t srcArrayLayer, uint32_t dstMipLevel, uint32_t dstArrayLayer) {
    // Implementation would use glCopyImageSubData (GL 4.3+) or FBO blitting
}

void OpenGL33CommandBuffer::PipelineBarrier() {
    // OpenGL has implicit barriers
}

void OpenGL33CommandBuffer::BeginDebugMarker(const char* name, const float color[4]) {
    // Use GL_KHR_debug extension if available
}

void OpenGL33CommandBuffer::EndDebugMarker() {
    // Use GL_KHR_debug extension if available
}

void OpenGL33CommandBuffer::InsertDebugMarker(const char* name, const float color[4]) {
    // Use GL_KHR_debug extension if available
}

void OpenGL33CommandBuffer::Execute() {
    // In OpenGL, commands are executed immediately
    // This is a no-op for compatibility with the API
    m_state = CommandBufferState::Submitted;
}

} // namespace VRHI
