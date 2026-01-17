// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#pragma once

#include "Resources.hpp"
#include <cstdint>
#include <span>

namespace VRHI {

// Forward declarations
class Texture;

// ============================================================================
// Attachment Load/Store Operations
// ============================================================================

enum class AttachmentLoadOp {
    Load,       // Load previous contents
    Clear,      // Clear to specified value
    DontCare,   // Don't care about previous contents
};

enum class AttachmentStoreOp {
    Store,      // Store results
    DontCare,   // Don't care about storing results
};

// ============================================================================
// Attachment Description
// ============================================================================

struct AttachmentDesc {
    TextureFormat format = TextureFormat::RGBA8_UNorm;
    uint32_t samples = 1;
    AttachmentLoadOp loadOp = AttachmentLoadOp::Clear;
    AttachmentStoreOp storeOp = AttachmentStoreOp::Store;
    AttachmentLoadOp stencilLoadOp = AttachmentLoadOp::DontCare;
    AttachmentStoreOp stencilStoreOp = AttachmentStoreOp::DontCare;
};

// ============================================================================
// Attachment Reference
// ============================================================================

struct AttachmentReference {
    uint32_t attachment = 0;
    // TextureLayout layout; // TODO: Add texture layout enum
};

// ============================================================================
// Subpass Description
// ============================================================================

struct SubpassDesc {
    std::span<const AttachmentReference> colorAttachments;
    const AttachmentReference* depthStencilAttachment = nullptr;
    std::span<const AttachmentReference> inputAttachments;
    std::span<const uint32_t> preserveAttachments;
};

// ============================================================================
// Subpass Dependency
// ============================================================================

struct SubpassDependency {
    uint32_t srcSubpass = 0;
    uint32_t dstSubpass = 0;
    // PipelineStageFlags srcStageMask;  // TODO: Add pipeline stage flags
    // PipelineStageFlags dstStageMask;
    // AccessFlags srcAccessMask;         // TODO: Add access flags
    // AccessFlags dstAccessMask;
};

// ============================================================================
// Render Pass Descriptor
// ============================================================================

struct RenderPassDesc {
    std::span<const AttachmentDesc> attachments;
    std::span<const SubpassDesc> subpasses;
    std::span<const SubpassDependency> dependencies;
    
    const char* debugName = nullptr;
};

// ============================================================================
// Render Pass Interface
// ============================================================================

/// Render pass interface
class IRenderPass {
public:
    virtual ~IRenderPass() = default;
    
    // RenderPass cannot be copied, only moved
    IRenderPass(const IRenderPass&) = delete;
    IRenderPass& operator=(const IRenderPass&) = delete;
    IRenderPass(IRenderPass&&) noexcept = default;
    IRenderPass& operator=(IRenderPass&&) noexcept = default;
    
    // ========================================================================
    // Native Handle Access (Optional)
    // ========================================================================
    
    /// Get native render pass handle for interoperability
    /// @return Native handle or nullptr if not supported
    virtual void* GetNativeHandle() const noexcept { return nullptr; }
    
protected:
    IRenderPass() = default;
};

// ============================================================================
// Framebuffer Descriptor
// ============================================================================

struct FramebufferDesc {
    IRenderPass* renderPass = nullptr;
    std::span<Texture* const> attachments;
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t layers = 1;
    
    const char* debugName = nullptr;
};

// ============================================================================
// Framebuffer Interface
// ============================================================================

/// Framebuffer interface
class IFramebuffer {
public:
    virtual ~IFramebuffer() = default;
    
    // Framebuffer cannot be copied, only moved
    IFramebuffer(const IFramebuffer&) = delete;
    IFramebuffer& operator=(const IFramebuffer&) = delete;
    IFramebuffer(IFramebuffer&&) noexcept = default;
    IFramebuffer& operator=(IFramebuffer&&) noexcept = default;
    
    // ========================================================================
    // Framebuffer Information
    // ========================================================================
    
    /// Get framebuffer width
    virtual uint32_t GetWidth() const noexcept = 0;
    
    /// Get framebuffer height
    virtual uint32_t GetHeight() const noexcept = 0;
    
    /// Get framebuffer layers
    virtual uint32_t GetLayers() const noexcept = 0;
    
    // ========================================================================
    // Native Handle Access (Optional)
    // ========================================================================
    
    /// Get native framebuffer handle for interoperability
    /// @return Native handle or nullptr if not supported
    virtual void* GetNativeHandle() const noexcept { return nullptr; }
    
protected:
    IFramebuffer() = default;
};

// Convenience aliases for the public API
using RenderPass = IRenderPass;
using Framebuffer = IFramebuffer;

} // namespace VRHI
